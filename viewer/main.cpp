#include "mk4_assets.h"
#include "renderer.h"
#include "sdl_app.h"
using namespace mk4;
#include <SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr uint32_t gold=0xffd2b273, ink=0xffe9e7e0, muted=0xff8a939f;
constexpr float pi=3.14159265358979323846f;
std::vector<Rect> cards(int w,int h,size_t count) {
    int cols=6,rows=int((count+size_t(cols)-1)/size_t(cols));
    int margin=26,gap=12,top=112,bottom=52;
    int cw=(w-2*margin-gap*(cols-1))/cols,ch=(h-top-bottom-gap*(rows-1))/rows;
    std::vector<Rect> out;
    for(size_t i=0;i<count;++i)out.push_back({margin+(int(i)%cols)*(cw+gap),top+(int(i)/cols)*(ch+gap),cw,ch});
    return out;
}

void draw(Canvas& cv,const mk4::Assets& assets,const std::vector<Bounds>& boxes,double seconds,
          float speed,float yaw,bool paused,bool wire,bool skeleton,int selected) {
    cv.clear();
    cv.rect({26,29,4,44},gold);
    cv.text(44,28,"MORTAL KOMBAT 4",3,ink);
    cv.text(45,62,"CHARACTER GALLERY / ORIGINAL PC ASSETS",1,muted);
    cv.text(cv.w-250,34,paused?"PLAYBACK PAUSED":"IDLE ANIMATIONS",1,gold);
    char metrics[80];std::snprintf(metrics,sizeof metrics,"18 CHARACTERS / %.2fX",double(speed));
    cv.text(cv.w-250,56,metrics,1,muted);
    cv.rect({26,88,cv.w-52,1},0xff343b45);
    auto rs=cards(cv.w,cv.h,assets.characters.size());
    for(size_t i=0;i<assets.characters.size();++i) {
        Rect r=rs[i];auto& ch=assets.characters[i];auto& clip=assets.clips[size_t(ch.idle)];
        cv.rect(r,0xff1c222b);cv.outline(r,int(i)==selected?gold:0xff303944);
        char number[8];std::snprintf(number,sizeof number,"%02zu",i+1);
        cv.text(r.x+12,r.y+11,number,1,muted);
        cv.rect({r.x+12,r.y+r.h-43,r.w-24,1},0xff343b45);
        draw_character(cv,ch,clip,seconds*30.*double(speed),r,boxes[i],yaw,wire,skeleton);
        cv.text(r.x+12,r.y+r.h-32,ch.name,1,ink);
        char info[64];std::snprintf(info,sizeof info,"IDLE %03d / %zu FRAMES",ch.idle,clip.frames.size());
        cv.text(r.x+12,r.y+r.h-16,info,1,muted);
    }
    cv.text(26,cv.h-27,"SPACE PAUSE   DRAG ROTATE   R FRONT   W WIREFRAME   B BONES   -/+ SPEED   ESC QUIT",1,muted);
}

void usage() {
    std::puts("MK4 SDL2 character gallery\n"
        "  --archive PATH     default MK4/FILESYS.DAT\n"
        "  --exe PATH         default MK4/MK-4.EXE\n"
        "  --headless         load/render without a desktop window\n"
        "  --screenshot PATH  save BMP image\n"
        "  --time SECONDS     start at this animation time\n"
        "  --frames N         stop after N rendered frames\n"
        "  --clip ID          inspect this original clip on every compatible rig\n"
        "  --yaw DEGREES      initial view rotation (default front)\n"
        "  --self-test        validate all assets, skeletons and clips\n"
        "  --dump-frames PATH save decoded animation words for independent checks\n");
}
} // namespace

int main(int argc,char** argv) {
    std::string archive="MK4/FILESYS.DAT",exe="MK4/MK-4.EXE",shot,dump;
    bool headless=false,selftest=false;int limit=0,override_clip=-1;double start_time=0;float yaw=0;
    try {
        for(int i=1;i<argc;++i) {
            std::string arg=argv[i];
            auto value=[&](){if(i+1>=argc)throw std::runtime_error("Missing value for "+arg);return std::string(argv[++i]);};
            if(arg=="--archive")archive=value();else if(arg=="--exe")exe=value();
            else if(arg=="--headless")headless=true;else if(arg=="--self-test")selftest=true;
            else if(arg=="--screenshot")shot=value();else if(arg=="--dump-frames")dump=value();
            else if(arg=="--time")start_time=std::stod(value());else if(arg=="--frames")limit=std::stoi(value());
            else if(arg=="--clip")override_clip=std::stoi(value());else if(arg=="--yaw")yaw=std::stof(value())*pi/180;
            else if(arg=="--help"||arg=="-h"){usage();return 0;}
            else throw std::runtime_error("Unknown option "+arg);
        }
        if(limit<0||!std::isfinite(start_time)||start_time<0||!std::isfinite(yaw)||override_clip< -1||override_clip>=612)
            throw std::runtime_error("Invalid command-line value");
        mk4::Assets assets;assets.load(archive,exe);
        size_t triangles=0,frames=0;
        for(auto& c:assets.clips)frames+=c.frames.size();
        std::vector<Bounds> boxes;
        for(auto& ch:assets.characters) {
            int required=0;for(auto& bone:ch.bones)required=std::max(required,bone.channel+1);
            if(override_clip>=0 && assets.clips[size_t(override_clip)].frames[0].angles.size()==size_t(required))ch.idle=override_clip;
            for(auto& mesh:ch.meshes)triangles+=mesh.triangles.size();
            boxes.push_back(bounds(ch,assets.clips[size_t(ch.idle)]));
            const auto& b=boxes.back();
            if(!std::isfinite(b.lo.x)||b.hi.y-b.lo.y<.1f||b.hi.y-b.lo.y>10)
                throw std::runtime_error("Invalid posed bounds for "+ch.name);
            std::printf("%-12s %-14s %2zu bones / %2zu meshes / clip %3d (%zu frames)\n",ch.name.c_str(),ch.file.c_str(),ch.bones.size(),ch.meshes.size(),ch.idle,assets.clips[size_t(ch.idle)].frames.size());
        }
        std::printf("Loaded %zu characters, %zu clips, %zu animation frames, %zu mesh triangles\n",assets.characters.size(),assets.clips.size(),frames,triangles);
        if(!dump.empty()) {
            FILE* f=std::fopen(dump.c_str(),"w");if(!f)throw std::runtime_error("Cannot create frame dump");
            for(auto& c:assets.clips)for(size_t i=0;i<c.frames.size();++i) {
                auto& frame=c.frames[i];std::fprintf(f,"%d %zu",c.id,i);
                for(float v:{frame.translation.x,frame.translation.y,frame.translation.z})std::fprintf(f," %08x",uint32_t(int32_t(std::lround(v*65536))));
                for(auto a:frame.angles)std::fprintf(f," %08x",(uint32_t(a.x)/2)|((uint32_t(a.y)/2)<<11)|((uint32_t(a.z)/4)<<22));
                std::fputc('\n',f);
            }
            if(std::fclose(f))throw std::runtime_error("Frame dump write failed");
        }
        if(selftest) {
            size_t animated=0;
            for(auto& ch:assets.characters) {
                auto& clip=assets.clips[size_t(ch.idle)];auto a=mk4::pose(ch,clip,0),b=mk4::pose(ch,clip,double(clip.frames.size())*.35);
                float change=0;
                for(size_t i=0;i<a.size();++i)for(size_t j=0;j<9;++j)change+=std::abs(a[i].rotation.m[j]-b[i].rotation.m[j]);
                if(change>.00001f)++animated;
            }
            if(animated!=assets.characters.size())throw std::runtime_error("Some characters have no animated pose change");
            std::printf("PASS: all %zu character rigs animate; all clips and textures decoded with bounds checks\n",animated);
            return 0;
        }
        constexpr int width=1440,height=1000;Canvas cv(width,height);
        SdlApp app("Mortal Kombat 4 - Character Gallery",width,height,headless);
        bool running=true,paused=false,wire=false,skeleton=false,drag=false;int selected=-1,rendered=0;
        float speed=1;double seconds=start_time;uint64_t last=SDL_GetPerformanceCounter();
        if(headless&&!limit)limit=1;
        while(running) {
            SDL_Event ev;
            while(SDL_PollEvent(&ev)) {
                if(ev.type==SDL_QUIT)running=false;
                if(ev.type==SDL_KEYDOWN) {
                    switch(ev.key.keysym.sym) {
                        case SDLK_ESCAPE:running=false;break;
                        case SDLK_SPACE:paused=!paused;break;
                        case SDLK_r:yaw=0;break;
                        case SDLK_w:wire=!wire;break;
                        case SDLK_b:skeleton=!skeleton;break;
                        case SDLK_MINUS:speed=std::max(.1f,speed-.1f);break;
                        case SDLK_EQUALS:case SDLK_PLUS:speed=std::min(3.f,speed+.1f);break;
                        case SDLK_LEFT:yaw-=pi/12;break;
                        case SDLK_RIGHT:yaw+=pi/12;break;
                        default:break;
                    }
                }
                if(ev.type==SDL_MOUSEBUTTONDOWN&&ev.button.button==SDL_BUTTON_LEFT) {
                    drag=true;float x,y;app.logical(ev.button.x,ev.button.y,x,y);
                    auto rs=cards(width,height,assets.characters.size());selected=-1;
                    for(size_t i=0;i<rs.size();++i)if(x>=rs[i].x&&x<rs[i].x+rs[i].w&&y>=rs[i].y&&y<rs[i].y+rs[i].h)selected=int(i);
                }
                if(ev.type==SDL_MOUSEBUTTONUP)drag=false;
                if(ev.type==SDL_MOUSEMOTION&&drag)yaw+=float(ev.motion.xrel)*.01f;
            }
            uint64_t now=SDL_GetPerformanceCounter();double dt=double(now-last)/double(SDL_GetPerformanceFrequency());last=now;
            if(!paused&&rendered>0)seconds+=headless?1./60.:std::min(.1,dt);
            draw(cv,assets,boxes,seconds,speed,yaw,paused,wire,skeleton,selected);
            app.present(cv);
            ++rendered;if(limit&&rendered>=limit)running=false;
        }
        if(!shot.empty()){save_bmp(cv,shot);std::printf("Saved %s\n",shot.c_str());}
        return 0;
    }catch(const std::exception& e) {
        std::fprintf(stderr,"MK4 viewer: %s\n",e.what());
        return 1;
    }
}

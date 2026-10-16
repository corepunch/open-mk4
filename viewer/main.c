#include "mk4_assets.h"
#include "renderer.h"
#include "sdl_app.h"
#include "math_util.h"
#include "cli.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const uint32_t gold=0xffd2b273,ink=0xffe9e7e0,muted=0xff8a939f;
static const float pi=3.14159265358979323846f;
static void cards(int w,int h,size_t count,Rect out[MK4_ROSTER_COUNT]) {
    int cols=6,rows=(int)((count+(size_t)cols-1)/(size_t)cols),margin=26,gap=12,top=112,bottom=52;
    int cw=(w-2*margin-gap*(cols-1))/cols,ch=(h-top-bottom-gap*(rows-1))/rows;
    for(size_t i=0;i<count;++i)out[i]=(Rect){margin+((int)i%cols)*(cw+gap),top+((int)i/cols)*(ch+gap),cw,ch};
}
static void draw(Canvas *cv,const Assets *assets,const Bounds boxes[MK4_ROSTER_COUNT],double seconds,float speed,float yaw,bool paused,bool wire,bool skeleton,int selected) {
    canvas_clear(cv);canvas_rect(cv,(Rect){26,29,4,44},gold);
    canvas_text(cv,44,28,"MORTAL KOMBAT 4",3,ink);canvas_text(cv,45,62,"CHARACTER GALLERY / ORIGINAL PC ASSETS",1,muted);
    canvas_text(cv,cv->w-250,34,paused?"PLAYBACK PAUSED":"IDLE ANIMATIONS",1,gold);
    char metrics[80];snprintf(metrics,sizeof metrics,"18 CHARACTERS / %.2fX",(double)speed);canvas_text(cv,cv->w-250,56,metrics,1,muted);
    canvas_rect(cv,(Rect){26,88,cv->w-52,1},0xff343b45);Rect rs[MK4_ROSTER_COUNT];cards(cv->w,cv->h,assets->characters.count,rs);
    for(size_t i=0;i<assets->characters.count;++i) {
        Rect r=rs[i];const Character *ch=&assets->characters.data[i];const Clip *clip=&assets->clips.data[ch->idle];
        canvas_rect(cv,r,0xff1c222b);canvas_outline(cv,r,(int)i==selected?gold:0xff303944);
        char number[8];snprintf(number,sizeof number,"%02zu",i+1);canvas_text(cv,r.x+12,r.y+11,number,1,muted);
        canvas_rect(cv,(Rect){r.x+12,r.y+r.h-43,r.w-24,1},0xff343b45);
        draw_character(cv,ch,clip,seconds*30.*(double)speed,r,boxes[i],yaw,wire,skeleton);
        canvas_text(cv,r.x+12,r.y+r.h-32,ch->name,1,ink);
        char info[64];snprintf(info,sizeof info,"IDLE %03d / %zu FRAMES",ch->idle,clip->frames.count);canvas_text(cv,r.x+12,r.y+r.h-16,info,1,muted);
    }
    canvas_text(cv,26,cv->h-27,"SPACE PAUSE   DRAG ROTATE   R FRONT   W WIREFRAME   B BONES   -/+ SPEED   ESC QUIT",1,muted);
}
static void usage(void) {
    puts("MK4 SDL2 character gallery\n"
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
int main(int argc,char **argv) {
    const char *archive="MK4/FILESYS.DAT",*exe="MK4/MK-4.EXE",*shot=NULL,*dump=NULL;
    bool headless=false,selftest=false;int limit=0,override_clip=-1;double start_time=0;float yaw=0;
    for(int i=1;i<argc;++i) {
        const char *arg=argv[i];
        if(!strcmp(arg,"--archive"))archive=cli_value(argc,argv,&i);else if(!strcmp(arg,"--exe"))exe=cli_value(argc,argv,&i);
        else if(!strcmp(arg,"--headless"))headless=true;else if(!strcmp(arg,"--self-test"))selftest=true;
        else if(!strcmp(arg,"--screenshot"))shot=cli_value(argc,argv,&i);else if(!strcmp(arg,"--dump-frames"))dump=cli_value(argc,argv,&i);
        else if(!strcmp(arg,"--time"))start_time=cli_double(cli_value(argc,argv,&i));else if(!strcmp(arg,"--frames"))limit=cli_int(cli_value(argc,argv,&i));
        else if(!strcmp(arg,"--clip"))override_clip=cli_int(cli_value(argc,argv,&i));else if(!strcmp(arg,"--yaw"))yaw=(float)cli_double(cli_value(argc,argv,&i))*pi/180;
        else if(!strcmp(arg,"--help")||!strcmp(arg,"-h")){usage();return 0;}else fatal("Unknown option %s",arg);
    }
    require(limit>=0 && isfinite(start_time) && start_time>=0 && isfinite(yaw) && override_clip>=-1 && override_clip<MK4_CLIP_COUNT,"Invalid command-line value");
    Assets assets={0};assets_load(&assets,archive,exe);size_t triangles=0,frames=0;Bounds boxes[MK4_ROSTER_COUNT];
    for(size_t i=0;i<assets.clips.count;++i)frames+=assets.clips.data[i].frames.count;
    for(size_t i=0;i<assets.characters.count;++i) {
        Character *ch=&assets.characters.data[i];int required=0;for(size_t j=0;j<ch->bones.count;++j)required=maxi(required,ch->bones.data[j].channel+1);
        if(override_clip>=0 && assets.clips.data[override_clip].frames.data[0].angles.count==(size_t)required)ch->idle=override_clip;
        for(size_t j=0;j<ch->meshes.count;++j)triangles+=ch->meshes.data[j].triangles.count;
        boxes[i]=character_bounds(ch,&assets.clips.data[ch->idle]);Bounds b=boxes[i];
        if(!isfinite(b.lo.x)||b.hi.y-b.lo.y<.1f||b.hi.y-b.lo.y>10)fatal("Invalid posed bounds for %s",ch->name);
        printf("%-12s %-14s %2zu bones / %2zu meshes / clip %3d (%zu frames)\n",ch->name,ch->file,ch->bones.count,ch->meshes.count,ch->idle,assets.clips.data[ch->idle].frames.count);
    }
    printf("Loaded %zu characters, %zu clips, %zu animation frames, %zu mesh triangles\n",assets.characters.count,assets.clips.count,frames,triangles);
    if(dump) {
        FILE *f=fopen(dump,"w");require(f!=NULL,"Cannot create frame dump");
        for(size_t j=0;j<assets.clips.count;++j) {
            const Clip *c=&assets.clips.data[j];for(size_t i=0;i<c->frames.count;++i) {
                const Frame *frame=&c->frames.data[i];fprintf(f,"%d %zu",c->id,i);
                const float values[]={frame->translation.x,frame->translation.y,frame->translation.z};
                for(size_t k=0;k<3;++k)fprintf(f," %08x",(uint32_t)(int32_t)lroundf(values[k]*65536));
                for(size_t k=0;k<frame->angles.count;++k) {Vec3 a=frame->angles.data[k];fprintf(f," %08x",((uint32_t)a.x/2)|(((uint32_t)a.y/2)<<11)|(((uint32_t)a.z/4)<<22));}
                fputc('\n',f);
            }
        }
        bool failed=ferror(f)!=0;int closed=fclose(f);require(!failed && !closed,"Frame dump write failed");
    }
    if(selftest) {
        size_t animated=0;for(size_t j=0;j<assets.characters.count;++j) {
            const Character *ch=&assets.characters.data[j];const Clip *clip=&assets.clips.data[ch->idle];Pose a[MK4_MAX_BONES],b[MK4_MAX_BONES];
            pose(ch,clip,0,a);pose(ch,clip,(double)clip->frames.count*.35,b);float change=0;
            for(size_t i=0;i<ch->bones.count;++i)for(size_t k=0;k<9;++k)change+=fabsf(a[i].rotation.m[k]-b[i].rotation.m[k]);
            if(change>.00001f)++animated;
        }
        require(animated==assets.characters.count,"Some characters have no animated pose change");
        printf("PASS: all %zu character rigs animate; all clips and textures decoded with bounds checks\n",animated);assets_free(&assets);return 0;
    }
    const int width=1440,height=1000;Canvas cv;canvas_init(&cv,width,height);SdlApp app;sdl_app_init(&app,"Mortal Kombat 4 - Character Gallery",width,height,headless);
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
                        case SDLK_MINUS:speed=fmaxf(.1f,speed-.1f);break;
                        case SDLK_EQUALS:case SDLK_PLUS:speed=fmin(3.f,speed+.1f);break;
                        case SDLK_LEFT:yaw-=pi/12;break;
                        case SDLK_RIGHT:yaw+=pi/12;break;
                        default:break;
                    }
                }
                if(ev.type==SDL_MOUSEBUTTONDOWN&&ev.button.button==SDL_BUTTON_LEFT) {
                    drag=true;float x,y;sdl_app_logical(&app,ev.button.x,ev.button.y,&x,&y);
                    Rect rs[MK4_ROSTER_COUNT];cards(width,height,assets.characters.count,rs);selected=-1;
                    for(size_t i=0;i<assets.characters.count;++i)if(x>=rs[i].x&&x<rs[i].x+rs[i].w&&y>=rs[i].y&&y<rs[i].y+rs[i].h)selected=(int)(i);
                }
                if(ev.type==SDL_MOUSEBUTTONUP)drag=false;
                if(ev.type==SDL_MOUSEMOTION&&drag)yaw+=(float)(ev.motion.xrel)*.01f;
            }
            uint64_t now=SDL_GetPerformanceCounter();double dt=(double)(now-last)/(double)(SDL_GetPerformanceFrequency());last=now;
            if(!paused&&rendered>0)seconds+=headless?1./60.:fmin(.1,dt);
            draw(&cv,&assets,boxes,seconds,speed,yaw,paused,wire,skeleton,selected);
            sdl_app_present(&app,&cv);
            ++rendered;if(limit&&rendered>=limit)running=false;
        }
    if(shot){save_bmp(&cv,shot);printf("Saved %s\n",shot);}
    sdl_app_free(&app);canvas_free(&cv);assets_free(&assets);return 0;
}

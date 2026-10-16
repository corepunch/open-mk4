#include "battle.h"
#include "draw.h"
#include "mk4_package.h"
#include "sdl_app.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
using namespace mk4;
using namespace mk4::game;
void usage(){std::puts(
    "MK4 native battle game\n"
    "  --archive PATH     default MK4/FILESYS.DAT\n"
    "  --exe PATH         default MK4/MK-4.EXE\n"
    "  --p1 N --p2 N      roster slots 0..17 (default Scorpion / Sub Zero)\n"
    "  --arena N          arena 0..10 (default 8, The Prison)\n"
    "  --two-player       local controls for player two\n"
    "  --demo             CPU controls both fighters\n"
    "  --headless         render without a window\n"
    "  --frames N         stop after N frames (headless advances exactly 1/60s)\n"
    "  --screenshot PATH  save final frame as BMP\n"
    "  --list             show original roster and arenas\n"
    "  --self-test        validate battle resources and animation compatibility\n"
    "P1: A/D W/S, J punch, K kick, L block, U uppercut, O throw, H run.\n"
    "P2: arrows, keypad 1/2/3/4/5, right Shift run.\n"
    "F1/F2 change fighters, F3 CPU/local, F4 arena, R reset, Enter rematch, Space pause.\n");}
int number(const std::string& s){size_t used=0;int n=std::stoi(s,&used);if(used!=s.size())throw std::runtime_error("Invalid number: "+s);return n;}
void validate(const Assets& assets){
    require(assets.arenas.size()==11,"Incomplete arena list");
    for(const auto& ar:assets.arenas){require(!ar.nodes.empty()&&ar.geometry.textures.size()==3,"Incomplete arena");
        size_t triangles=0;for(const auto& n:ar.nodes)if(!(n.flags&4))triangles+=ar.geometry.meshes[size_t(n.mesh)].triangles.size();
        require(triangles>0,"Empty arena");std::printf("%-20s %-15s %zu nodes / %zu triangles / %zu pages\n",ar.name.c_str(),ar.file.c_str(),ar.nodes.size(),triangles,ar.geometry.textures.size());}
    require(assets.hud.font.sprites.size()==143,"Invalid HUD sprite directory");
    for(size_t i=0;i<assets.characters.size();++i){require(assets.hud.names[i]>=0,"Missing fighter name");
        const auto& ch=assets.characters[i];int channels=0;for(auto& b:ch.bones)channels=std::max(channels,b.channel+1);
        for(auto a:{ACTION_IDLE,ACTION_WALK,ACTION_CROUCH,ACTION_JUMP,ACTION_PUNCH,ACTION_KICK,ACTION_SWEEP,ACTION_UPPERCUT,ACTION_THROW,ACTION_BLOCK,ACTION_HIT,ACTION_KNOCKDOWN,ACTION_VICTORY}){
            const auto& clip=assets.clips[size_t(animation(ch,a))];require(int(clip.frames.front().angles.size())==channels,"Incompatible combat animation");
            for(double f:{0.,double(clip.frames.size())*.5})for(auto& p:pose(ch,clip,f))require(std::isfinite(p.position.x+p.position.y+p.position.z),"Invalid combat pose");}}
    std::puts("PASS: 11 textured arena hierarchies; original HUD/name sprites; all 18 combat rigs compatible");
}
Input held_input(const Uint8* keys,int p) {
    Input in{};
    if(!p){in.left=keys[SDL_SCANCODE_A];in.right=keys[SDL_SCANCODE_D];in.jump=keys[SDL_SCANCODE_W];in.crouch=keys[SDL_SCANCODE_S];in.block=keys[SDL_SCANCODE_L];in.run=keys[SDL_SCANCODE_H];}
    else{in.left=keys[SDL_SCANCODE_LEFT];in.right=keys[SDL_SCANCODE_RIGHT];in.jump=keys[SDL_SCANCODE_UP];in.crouch=keys[SDL_SCANCODE_DOWN];in.block=keys[SDL_SCANCODE_KP_3];in.run=keys[SDL_SCANCODE_RSHIFT];}
    return in;
}
}
int main(int argc,char** argv){
    try{
        std::string archive="MK4/FILESYS.DAT",exe="MK4/MK-4.EXE",shot;bool headless=false,demo=false,cpu=true,selftest=false,list=false;
        int p1=0,p2=4,arena=8,limit=0;
        for(int i=1;i<argc;++i){std::string arg=argv[i];auto value=[&](){if(i+1>=argc)throw std::runtime_error("Missing value for "+arg);return std::string(argv[++i]);};
            if(arg=="--archive")archive=value();else if(arg=="--exe")exe=value();else if(arg=="--screenshot")shot=value();
            else if(arg=="--p1")p1=number(value());else if(arg=="--p2")p2=number(value());else if(arg=="--arena")arena=number(value());
            else if(arg=="--frames")limit=number(value());else if(arg=="--headless")headless=true;else if(arg=="--demo")demo=true;
            else if(arg=="--two-player")cpu=false;else if(arg=="--self-test")selftest=true;else if(arg=="--list")list=true;
            else if(arg=="--help"||arg=="-h"){usage();return 0;}else throw std::runtime_error("Unknown option "+arg);}
        require(p1>=0&&p1<18&&p2>=0&&p2<18&&arena>=0&&arena<11&&limit>=0,"Invalid roster, arena, or frame limit");
        Assets assets;assets.load(archive,exe);assets.load_battle(archive,exe);
        if(list){for(size_t i=0;i<assets.characters.size();++i)std::printf("Fighter %2zu: %s\n",i,assets.characters[i].name.c_str());
            for(size_t i=0;i<assets.arenas.size();++i)std::printf("Arena %2zu: %s\n",i,assets.arenas[i].name.c_str());return 0;}
        if(selftest){validate(assets);return 0;}
        SdlApp app("Mortal Kombat 4 - Battle",1280,800,headless);Canvas cv(1280,800);Battle battle; battle_init(&battle,p1,p2);
        bool running=true,paused=false;int rendered=0;double accumulator=0;uint64_t last=SDL_GetPerformanceCounter();
        std::array<Input,2> pending{};if(headless&&!limit)limit=1;
        while(running){SDL_Event ev;
            while(SDL_PollEvent(&ev)){
                if(ev.type==SDL_QUIT)running=false;
                if(ev.type==SDL_WINDOWEVENT&&ev.window.event==SDL_WINDOWEVENT_FOCUS_LOST){paused=true;pending={};}
                if(ev.type!=SDL_KEYDOWN||ev.key.repeat)continue;
                auto k=ev.key.keysym.sym;
                if(k==SDLK_ESCAPE)running=false;else if(k==SDLK_SPACE){paused=!paused;pending={};}
                else if(k==SDLK_r){battle_reset_match(&battle);pending={};}
                else if(k==SDLK_RETURN&&battle.phase==PHASE_MATCH_OVER){battle_reset_match(&battle);pending={};}
                else if(k==SDLK_F1||k==SDLK_F2){size_t player=k==SDLK_F1?0:1;auto& ch=battle.fighters[player].character;ch=(ch+1)%18;battle_reset_match(&battle);pending={};}
                else if(k==SDLK_F3){cpu=!cpu;demo=false;pending={};}
                else if(k==SDLK_F4){arena=(arena+1)%int(assets.arenas.size());battle_reset_match(&battle);pending={};}
                else if(!paused){if(k==SDLK_j)pending[0].punch=true;if(k==SDLK_k)pending[0].kick=true;
                    if(k==SDLK_u)pending[0].uppercut=true;if(k==SDLK_o)pending[0].grab=true;
                    if(k==SDLK_KP_1)pending[1].punch=true;if(k==SDLK_KP_2)pending[1].kick=true;
                    if(k==SDLK_KP_4)pending[1].uppercut=true;if(k==SDLK_KP_5)pending[1].grab=true;}}
            uint64_t now=SDL_GetPerformanceCounter();double dt=headless?1./60.:std::min(.1,double(now-last)/double(SDL_GetPerformanceFrequency()));last=now;
            if(!paused)accumulator+=dt;else accumulator=0;
            const auto* keys=SDL_GetKeyboardState(nullptr);
            while(accumulator>=1./60.){
                std::array<Input,2> in;
                for(int i=0;i<2;++i){in[size_t(i)]=held_input(keys,i);auto& src=pending[size_t(i)];auto& dst=in[size_t(i)];
                    dst.punch=src.punch;dst.kick=src.kick;dst.uppercut=src.uppercut;dst.grab=src.grab;}
                if(demo)in[0]=battle_cpu(&battle,0);if(cpu||demo)in[1]=battle_cpu(&battle,1);
                battle_step(&battle,in.data());pending={};accumulator-=1./60.;
                if(demo&&battle.phase==PHASE_MATCH_OVER&&battle.phase_ticks>240)battle_reset_match(&battle);}
            draw_battle(cv,assets,battle,arena,cpu,paused);app.present(cv);
            ++rendered;if(limit&&rendered>=limit)running=false;if(!headless)SDL_Delay(1);
        }
        if(!shot.empty())save_bmp(cv,shot);
        std::printf("Rendered %d frames; simulated %llu ticks; health %d / %d; round %d\n",rendered,static_cast<unsigned long long>(battle.ticks),battle.fighters[0].health,battle.fighters[1].health,battle.round);
        return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"MK4 game: %s\n",e.what());return 1;}
}

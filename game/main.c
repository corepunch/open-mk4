#include "draw.h"
#include "sdl_app.h"
#include "math_util.h"
#include "cli.h"
#include <stdio.h>
#include <string.h>
static void usage(void){puts(
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
static void validate(const Assets *assets) {
    require(assets->arenas.count==11,"Incomplete arena list");
    for(size_t i=0;i<assets->arenas.count;++i) {
        const Arena *ar=&assets->arenas.data[i];require(ar->nodes.count && ar->geometry.textures.count==3,"Incomplete arena");
        size_t triangles=0;for(size_t j=0;j<ar->nodes.count;++j)if(!(ar->nodes.data[j].flags&4))triangles+=ar->geometry.meshes.data[ar->nodes.data[j].mesh].triangles.count;
        require(triangles>0,"Empty arena");printf("%-20s %-15s %zu nodes / %zu triangles / %zu pages\n",ar->name,ar->file,ar->nodes.count,triangles,ar->geometry.textures.count);
    }
    require(assets->hud.font.sprites.count==143,"Invalid HUD sprite directory");
    for(size_t i=0;i<assets->characters.count;++i) {
        require(assets->hud.names[i]>=0,"Missing fighter name");const Character *ch=&assets->characters.data[i];int channels=0;
        for(size_t j=0;j<ch->bones.count;++j)channels=maxi(channels,ch->bones.data[j].channel+1);
        for(Action a=ACTION_IDLE;a<=ACTION_VICTORY;++a) {
            const Clip *clip=&assets->clips.data[animation(ch,a)];require((int)clip->frames.data[0].angles.count==channels,"Incompatible combat animation");
            for(int k=0;k<2;++k) {
                Pose nodes[MK4_MAX_BONES];pose(ch,clip,k?(double)clip->frames.count*.5:0.,nodes);
                for(size_t n=0;n<ch->bones.count;++n)require(isfinite(nodes[n].position.x+nodes[n].position.y+nodes[n].position.z),"Invalid combat pose");
            }
        }
    }
    puts("PASS: 11 textured arena hierarchies; original HUD/name sprites; all 18 combat rigs compatible");
}
static Input held_input(const Uint8* keys,int p) {
    Input in={0};
    if(!p){in.left=keys[SDL_SCANCODE_A];in.right=keys[SDL_SCANCODE_D];in.jump=keys[SDL_SCANCODE_W];in.crouch=keys[SDL_SCANCODE_S];in.block=keys[SDL_SCANCODE_L];in.run=keys[SDL_SCANCODE_H];}
    else{in.left=keys[SDL_SCANCODE_LEFT];in.right=keys[SDL_SCANCODE_RIGHT];in.jump=keys[SDL_SCANCODE_UP];in.crouch=keys[SDL_SCANCODE_DOWN];in.block=keys[SDL_SCANCODE_KP_3];in.run=keys[SDL_SCANCODE_RSHIFT];}
    return in;
}
int main(int argc,char** argv){
        const char *archive="MK4/FILESYS.DAT",*exe="MK4/MK-4.EXE",*shot=NULL;bool headless=false,demo=false,cpu=true,selftest=false,list=false;
        int p1=0,p2=4,arena=8,limit=0;
        for(int i=1;i<argc;++i){const char *arg=argv[i];
            if(!strcmp(arg,"--archive"))archive=cli_value(argc,argv,&i);else if(!strcmp(arg,"--exe"))exe=cli_value(argc,argv,&i);else if(!strcmp(arg,"--screenshot"))shot=cli_value(argc,argv,&i);
            else if(!strcmp(arg,"--p1"))p1=cli_int(cli_value(argc,argv,&i));else if(!strcmp(arg,"--p2"))p2=cli_int(cli_value(argc,argv,&i));else if(!strcmp(arg,"--arena"))arena=cli_int(cli_value(argc,argv,&i));
            else if(!strcmp(arg,"--frames"))limit=cli_int(cli_value(argc,argv,&i));else if(!strcmp(arg,"--headless"))headless=true;else if(!strcmp(arg,"--demo"))demo=true;
            else if(!strcmp(arg,"--two-player"))cpu=false;else if(!strcmp(arg,"--self-test"))selftest=true;else if(!strcmp(arg,"--list"))list=true;
            else if(!strcmp(arg,"--help")||!strcmp(arg,"-h")){usage();return 0;}else fatal("Unknown option %s",arg);}
        require(p1>=0&&p1<18&&p2>=0&&p2<18&&arena>=0&&arena<11&&limit>=0,"Invalid roster, arena, or frame limit");
        Assets assets={0};assets_load(&assets,archive,exe);assets_load_battle(&assets,archive,exe);
        if(list){for(size_t i=0;i<assets.characters.count;++i)printf("Fighter %2zu: %s\n",i,assets.characters.data[i].name);
            for(size_t i=0;i<assets.arenas.count;++i)printf("Arena %2zu: %s\n",i,assets.arenas.data[i].name);assets_free(&assets);return 0;}
        if(selftest){validate(&assets);assets_free(&assets);return 0;}
        SdlApp app;sdl_app_init(&app,"Mortal Kombat 4 - Battle",1280,800,headless);Canvas cv;canvas_init(&cv,1280,800);Battle battle; battle_init(&battle,p1,p2);
        bool running=true,paused=false;int rendered=0;double accumulator=0;uint64_t last=SDL_GetPerformanceCounter();
        Input pending[2]={0};if(headless&&!limit)limit=1;
        while(running){SDL_Event ev;
            while(SDL_PollEvent(&ev)){
                if(ev.type==SDL_QUIT)running=false;
                if(ev.type==SDL_WINDOWEVENT&&ev.window.event==SDL_WINDOWEVENT_FOCUS_LOST){paused=true;memset(pending,0,sizeof pending);}
                if(ev.type!=SDL_KEYDOWN||ev.key.repeat)continue;
                SDL_Keycode k=ev.key.keysym.sym;
                if(k==SDLK_ESCAPE)running=false;else if(k==SDLK_SPACE){paused=!paused;memset(pending,0,sizeof pending);}
                else if(k==SDLK_r){battle_reset_match(&battle);memset(pending,0,sizeof pending);}
                else if(k==SDLK_RETURN&&battle.phase==PHASE_MATCH_OVER){battle_reset_match(&battle);memset(pending,0,sizeof pending);}
                else if(k==SDLK_F1||k==SDLK_F2){size_t player=k==SDLK_F1?0:1;int *ch=&battle.fighters[player].character;*ch=(*ch+1)%18;battle_reset_match(&battle);memset(pending,0,sizeof pending);}
                else if(k==SDLK_F3){cpu=!cpu;demo=false;memset(pending,0,sizeof pending);}
                else if(k==SDLK_F4){arena=(arena+1)%(int)(assets.arenas.count);battle_reset_match(&battle);memset(pending,0,sizeof pending);}
                else if(!paused){if(k==SDLK_j)pending[0].punch=true;if(k==SDLK_k)pending[0].kick=true;
                    if(k==SDLK_u)pending[0].uppercut=true;if(k==SDLK_o)pending[0].grab=true;
                    if(k==SDLK_KP_1)pending[1].punch=true;if(k==SDLK_KP_2)pending[1].kick=true;
                    if(k==SDLK_KP_4)pending[1].uppercut=true;if(k==SDLK_KP_5)pending[1].grab=true;}}
            uint64_t now=SDL_GetPerformanceCounter();double dt=headless?1./60.:fmin(.1,(double)(now-last)/(double)(SDL_GetPerformanceFrequency()));last=now;
            if(!paused)accumulator+=dt;else accumulator=0;
            const Uint8 *keys=SDL_GetKeyboardState(NULL);
            while(accumulator>=1./60.){
                Input in[2]={0};
                for(int i=0;i<2;++i){in[(size_t)(i)]=held_input(keys,i);Input *src=&pending[i],*dst=&in[i];
                    dst->punch=src->punch;dst->kick=src->kick;dst->uppercut=src->uppercut;dst->grab=src->grab;}
                if(demo)in[0]=battle_cpu(&battle,0);if(cpu||demo)in[1]=battle_cpu(&battle,1);
                battle_step(&battle,in);memset(pending,0,sizeof pending);accumulator-=1./60.;
                if(demo&&battle.phase==PHASE_MATCH_OVER&&battle.phase_ticks>240)battle_reset_match(&battle);}
            draw_battle(&cv,&assets,&battle,arena,cpu,paused);sdl_app_present(&app,&cv);
            ++rendered;if(limit&&rendered>=limit)running=false;if(!headless)SDL_Delay(1);
        }
        if(shot!=NULL)save_bmp(&cv,shot);
        printf("Rendered %d frames; simulated %llu ticks; health %d / %d; round %d\n",rendered,(unsigned long long)battle.ticks,battle.fighters[0].health,battle.fighters[1].health,battle.round);
        sdl_app_free(&app);canvas_free(&cv);assets_free(&assets);return 0;
}

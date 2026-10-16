#include "battle.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <stdexcept>

namespace {
void check(bool yes,const char* message){if(!yes)throw std::runtime_error(message);}
Battle fight(float distance=1.f){Battle b; battle_init(&b,0,4);b.phase=PHASE_FIGHT;b.fighters[0].x=-distance*.5f;b.fighters[1].x=distance*.5f;return b;}
void advance(Battle& b,int n,std::array<Input,2> in={}){for(int i=0;i<n;++i)battle_step(&b,in.data());}
void attack(Battle& b,Action a,bool guard=false,bool crouch=false){
    std::array<Input,2> in{};in[1].block=guard;in[1].crouch=crouch;
    in[0].punch=a==ACTION_PUNCH;in[0].kick=a==ACTION_KICK||a==ACTION_SWEEP;
    in[0].uppercut=a==ACTION_UPPERCUT;in[0].grab=a==ACTION_THROW;in[0].crouch=a==ACTION_SWEEP;
    battle_step(&b,in.data());in[0]={};advance(b,19,in);
}
}
int main(){try{
    {Battle b; battle_init(&b,0,4);advance(b,89);check(b.phase==PHASE_READY,"Countdown ends too soon");advance(b,1);check(b.phase==PHASE_FIGHT,"Countdown never starts fight");}
    {auto b=fight();attack(b,ACTION_PUNCH);check(b.fighters[1].health==93,"Punch must connect once");advance(b,30);check(b.fighters[1].health==93,"Repeated hit during recovery");}
    {auto b=fight(3);attack(b,ACTION_KICK);check(b.fighters[1].health==100,"Out-of-range attack connects");}
    {auto b=fight();attack(b,ACTION_PUNCH,true);check(b.fighters[1].health==100&&b.effects[0].blocked,"Blocking fails");}
    {auto b=fight();attack(b,ACTION_SWEEP,true);check(b.fighters[1].health==90,"Standing guard should lose to sweep");}
    {auto b=fight();attack(b,ACTION_SWEEP,true,true);check(b.fighters[1].health==100,"Crouch guard fails against sweep");}
    {auto b=fight(.8f);attack(b,ACTION_THROW,true);check(b.fighters[1].health==86,"Throw should beat guard");}
    {auto b=fight();b.fighters[1].height=1.5f;attack(b,ACTION_PUNCH);check(b.fighters[1].health==100,"Ground punch hits above its vertical reach");}
    {auto b=fight();attack(b,ACTION_UPPERCUT);check(b.fighters[1].health==82&&b.fighters[1].action==ACTION_KNOCKDOWN&&b.fighters[1].height>0,"Uppercut does not launch");advance(b,100);check(b.fighters[1].height==0&&b.fighters[1].action==ACTION_IDLE,"Knockdown never recovers");}
    {auto b=fight();std::array<Input,2> in{};in[0].punch=in[1].punch=true;battle_step(&b,in.data());advance(b,6);check(b.fighters[0].health==93&&b.fighters[1].health==93,"Simultaneous attacks must trade");}
    {auto b=fight();b.fighters[1].health=7;attack(b,ACTION_PUNCH);check(b.phase==PHASE_ROUND_OVER&&b.winner==0&&b.fighters[0].wins==1,"KO/round award failed");advance(b,180);check(b.phase==PHASE_READY&&b.round==2&&b.fighters[1].health==100&&b.fighters[0].wins==1,"Next round reset failed");b.phase=PHASE_FIGHT;b.fighters[0].x=-.5f;b.fighters[1].x=.5f;b.fighters[1].health=7;attack(b,ACTION_PUNCH);check(b.phase==PHASE_MATCH_OVER&&b.fighters[0].wins==2,"Best of three match failed");advance(b,300);check(b.phase==PHASE_MATCH_OVER,"Match result should wait for rematch");battle_reset_match(&b);check(b.round==1&&b.fighters[0].wins==0,"Rematch failed");}
    {auto b=fight();b.time_ticks=1;b.fighters[0].health=75;b.fighters[1].health=50;battle_step(&b,std::array<Input,2>{}.data());check(b.winner==0&&b.phase==PHASE_ROUND_OVER,"Timeout winner incorrect");}
    {auto b=fight();b.time_ticks=1;battle_step(&b,std::array<Input,2>{}.data());check(b.winner==-1&&b.fighters[0].wins==0&&b.fighters[1].wins==0,"Draw should not award wins");}
    {auto b=fight();std::array<Input,2> in{};in[0].jump=true;battle_step(&b,in.data());advance(b,90);check(b.fighters[0].height==0&&b.fighters[0].velocity==0,"Jump never lands");}
    {auto b=fight();std::array<Input,2> in{};in[0].right=in[1].left=true;advance(b,600,in);check(std::abs(b.fighters[0].x-b.fighters[1].x)>=.699f,"Bodies overlap");in[0].right=false;in[0].left=true;advance(b,600,in);for(auto f:b.fighters)check(std::abs(f.x)<=b.wall,"Arena wall violated");}
    {auto b=fight();int changes=0,old=200;for(int i=0;i<18000&&b.phase!=PHASE_MATCH_OVER;++i){battle_step(&b,std::array<Input,2>{battle_cpu(&b,0),battle_cpu(&b,1)}.data());int sum=b.fighters[0].health+b.fighters[1].health;if(sum<old)++changes;old=sum;}check(changes>4&&b.phase==PHASE_MATCH_OVER,"CPU match does not progress to completion");}
    std::puts("PASS: countdown; range; single hits; block; sweep; throw; air reach; launch; trades; KO; rounds; timeout; draw; rematch; jump; walls; CPU match");
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}

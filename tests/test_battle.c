#include "battle.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
static void check(bool yes,const char *message) {if(!yes){fprintf(stderr,"FAIL: %s\n",message);exit(1);}}
static Battle fight(float distance) {Battle b;battle_init(&b,0,4);b.phase=PHASE_FIGHT;b.fighters[0].x=-distance*.5f;b.fighters[1].x=distance*.5f;return b;}
static void advance_ticks(Battle *b,int n,const Input *in) {const Input zero[2]={0};for(int i=0;i<n;++i)battle_step(b,in?in:zero);}
static void do_attack(Battle *b,Action a,bool guard,bool crouch) {
    Input in[2]={0};in[1].block=guard;in[1].crouch=crouch;
    in[0].punch=a==ACTION_PUNCH;in[0].kick=a==ACTION_KICK||a==ACTION_SWEEP;
    in[0].uppercut=a==ACTION_UPPERCUT;in[0].grab=a==ACTION_THROW;in[0].crouch=a==ACTION_SWEEP;
    battle_step(b,in);in[0]=(Input){0};advance_ticks(b,19,in);
}
int main(void){
    {Battle b; battle_init(&b,0,4);advance_ticks(&b,89,NULL);check(b.phase==PHASE_READY,"Countdown ends too soon");advance_ticks(&b,1,NULL);check(b.phase==PHASE_FIGHT,"Countdown never starts fight");}
    {Battle b=fight(1.f);do_attack(&b,ACTION_PUNCH,false,false);check(b.fighters[1].health==93,"Punch must connect once");advance_ticks(&b,30,NULL);check(b.fighters[1].health==93,"Repeated hit during recovery");}
    {Battle b=fight(3);do_attack(&b,ACTION_KICK,false,false);check(b.fighters[1].health==100,"Out-of-range attack connects");}
    {Battle b=fight(1.f);do_attack(&b,ACTION_PUNCH,true,false);check(b.fighters[1].health==100&&b.effects[0].blocked,"Blocking fails");}
    {Battle b=fight(1.f);do_attack(&b,ACTION_SWEEP,true,false);check(b.fighters[1].health==90,"Standing guard should lose to sweep");}
    {Battle b=fight(1.f);do_attack(&b,ACTION_SWEEP,true,true);check(b.fighters[1].health==100,"Crouch guard fails against sweep");}
    {Battle b=fight(.8f);do_attack(&b,ACTION_THROW,true,false);check(b.fighters[1].health==86,"Throw should beat guard");}
    {Battle b=fight(1.f);b.fighters[1].height=1.5f;do_attack(&b,ACTION_PUNCH,false,false);check(b.fighters[1].health==100,"Ground punch hits above its vertical reach");}
    {Battle b=fight(1.f);do_attack(&b,ACTION_UPPERCUT,false,false);check(b.fighters[1].health==82&&b.fighters[1].action==ACTION_KNOCKDOWN&&b.fighters[1].height>0,"Uppercut does not launch");advance_ticks(&b,100,NULL);check(b.fighters[1].height==0&&b.fighters[1].action==ACTION_IDLE,"Knockdown never recovers");}
    {Battle b=fight(1.f);Input in[2]={0};in[0].punch=in[1].punch=true;battle_step(&b,in);advance_ticks(&b,6,NULL);check(b.fighters[0].health==93&&b.fighters[1].health==93,"Simultaneous attacks must trade");}
    {Battle b=fight(1.f);b.fighters[1].health=7;do_attack(&b,ACTION_PUNCH,false,false);check(b.phase==PHASE_ROUND_OVER&&b.winner==0&&b.fighters[0].wins==1,"KO/round award failed");advance_ticks(&b,180,NULL);check(b.phase==PHASE_READY&&b.round==2&&b.fighters[1].health==100&&b.fighters[0].wins==1,"Next round reset failed");b.phase=PHASE_FIGHT;b.fighters[0].x=-.5f;b.fighters[1].x=.5f;b.fighters[1].health=7;do_attack(&b,ACTION_PUNCH,false,false);check(b.phase==PHASE_MATCH_OVER&&b.fighters[0].wins==2,"Best of three match failed");advance_ticks(&b,300,NULL);check(b.phase==PHASE_MATCH_OVER,"Match result should wait for rematch");battle_reset_match(&b);check(b.round==1&&b.fighters[0].wins==0,"Rematch failed");}
    {Battle b=fight(1.f);b.time_ticks=1;b.fighters[0].health=75;b.fighters[1].health=50;battle_step(&b,(Input[2]){0});check(b.winner==0&&b.phase==PHASE_ROUND_OVER,"Timeout winner incorrect");}
    {Battle b=fight(1.f);b.time_ticks=1;battle_step(&b,(Input[2]){0});check(b.winner==-1&&b.fighters[0].wins==0&&b.fighters[1].wins==0,"Draw should not award wins");}
    {Battle b=fight(1.f);Input in[2]={0};in[0].jump=true;battle_step(&b,in);advance_ticks(&b,90,NULL);check(b.fighters[0].height==0&&b.fighters[0].velocity==0,"Jump never lands");}
    {Battle b=fight(1.f);Input in[2]={0};in[0].right=in[1].left=true;advance_ticks(&b,600,in);check(fabsf(b.fighters[0].x-b.fighters[1].x)>=.699f,"Bodies overlap");in[0].right=false;in[0].left=true;advance_ticks(&b,600,in);for(int i=0;i<2;++i)check(fabsf(b.fighters[i].x)<=b.wall,"Arena wall violated");}
    {Battle b=fight(1.f);int changes=0,old=200;for(int i=0;i<18000&&b.phase!=PHASE_MATCH_OVER;++i){battle_step(&b,(Input[2]){battle_cpu(&b,0),battle_cpu(&b,1)});int sum=b.fighters[0].health+b.fighters[1].health;if(sum<old)++changes;old=sum;}check(changes>4&&b.phase==PHASE_MATCH_OVER,"CPU match does not progress to completion");}
    puts("PASS: countdown; range; single hits; block; sweep; throw; air reach; launch; trades; KO; rounds; timeout; draw; rematch; jump; walls; CPU match");
return 0;
}

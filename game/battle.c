#include "battle.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

static float clamp(float x, float lo, float hi) { return fminf(hi, fmaxf(lo, x)); }
static void finish_round(Battle *battle);
static void set_action(Fighter *f,Action a){if(f->action!=a){f->action=a;f->ticks=0;f->connected=false;}}
static bool locked(const Fighter *f) { return battle_move(f->action)||f->action==ACTION_HIT||f->action==ACTION_KNOCKDOWN; }
const Move* battle_move(Action a) {
    static const Move punch={5,4,12,7,19,1.25f,.13f,false,false,false};
    static const Move kick={10,5,18,12,25,1.85f,.32f,false,false,false};
    static const Move sweep={11,5,23,10,45,1.65f,.25f,true,true,false};
    static const Move uppercut={9,5,30,18,60,1.35f,.5f,false,true,false};
    static const Move grab={6,2,25,14,42,.95f,.45f,false,true,true};
    switch(a){case ACTION_PUNCH:return &punch;case ACTION_KICK:return &kick;case ACTION_SWEEP:return &sweep;
        case ACTION_UPPERCUT:return &uppercut;case ACTION_THROW:return &grab;default:return NULL;}
}
void battle_init(Battle *battle,int p1,int p2){memset(battle,0,sizeof *battle);battle->wall=5.f;battle->fighters[0].character=p1;battle->fighters[1].character=p2;battle_reset_match(battle);}
void battle_reset_match(Battle *battle){for(int i=0;i<2;++i)battle->fighters[i].wins=0;battle->round=0;battle->ticks=0;battle_next_round(battle);}
void battle_next_round(Battle *battle){
    ++battle->round;
    for(size_t i=0;i<2;++i){int ch=battle->fighters[i].character,wins=battle->fighters[i].wins;battle->fighters[i]=(Fighter){0};
        Fighter *f=&battle->fighters[i];f->character=ch;f->health=100;f->wins=wins;f->x=i?2.f:-2.f;f->facing=i?-1:1;}
    memset(battle->effects,0,sizeof battle->effects);battle->phase=PHASE_READY;battle->phase_ticks=0;battle->time_ticks=99*60;battle->winner=-1;
}
static void finish_round(Battle *battle){
    battle->winner=battle->fighters[0].health==battle->fighters[1].health?-1:(battle->fighters[0].health>battle->fighters[1].health?0:1);
    if(battle->winner>=0){++battle->fighters[(size_t)(battle->winner)].wins;set_action(&battle->fighters[battle->winner],ACTION_VICTORY);}
    for(int i=0;i<2;++i)if(battle->fighters[i].health<=0)set_action(&battle->fighters[i],ACTION_KNOCKDOWN);
    battle->phase=(battle->winner>=0&&battle->fighters[(size_t)(battle->winner)].wins>=2)?PHASE_MATCH_OVER:PHASE_ROUND_OVER;battle->phase_ticks=0;
}
void battle_step(Battle *battle,const Input input[2]) {
    ++battle->ticks;++battle->phase_ticks;for(int i=0;i<2;++i)if(battle->effects[i].ticks>0)--battle->effects[i].ticks;
    if(battle->phase==PHASE_READY){if(battle->phase_ticks>=90){battle->phase=PHASE_FIGHT;battle->phase_ticks=0;}return;}
    if(battle->phase!=PHASE_FIGHT){for(int i=0;i<2;++i)++battle->fighters[i].ticks;if(battle->phase==PHASE_ROUND_OVER&&battle->phase_ticks>=180)battle_next_round(battle);return;}
    --battle->time_ticks;
    for(size_t i=0;i<2;++i) {
        Fighter *f=&battle->fighters[i];const Input *in=&input[i];++f->ticks;
        if(!locked(f)&&f->height==0)f->facing=f->x<=battle->fighters[1-i].x?1:-1;
        const Move *m=battle_move(f->action);
        if(m) {
            if(f->ticks>=m->startup+m->active+m->recovery)set_action(f,f->height>0?ACTION_JUMP:ACTION_IDLE);
        } else if((f->action==ACTION_HIT&&f->ticks>=24)||(f->action==ACTION_KNOCKDOWN&&f->ticks>=54&&f->height==0))set_action(f,ACTION_IDLE);
        if(f->height>0 || f->velocity>0) {
            f->height+=f->velocity*MK4_TICK_SECONDS;f->velocity-=15.f*MK4_TICK_SECONDS;
            if(f->height<=0){f->height=0;f->velocity=0;if(f->action==ACTION_JUMP)set_action(f,ACTION_IDLE);}
        }
        if(locked(f))continue;
        f->crouching=in->crouch&&f->height==0;
        if(in->block){set_action(f,ACTION_BLOCK);continue;}
        if(in->grab&&f->height==0&&!f->crouching)set_action(f,ACTION_THROW);
        else if(in->uppercut&&f->height==0)set_action(f,ACTION_UPPERCUT);
        else if(in->punch)set_action(f,f->crouching?ACTION_UPPERCUT:ACTION_PUNCH);
        else if(in->kick)set_action(f,f->crouching?ACTION_SWEEP:ACTION_KICK);
        else {
            if(in->jump&&f->height==0){f->velocity=6.8f;f->height=.01f;set_action(f,ACTION_JUMP);}
            int dir=(int)(in->right)-(int)(in->left);
            if(!f->crouching){f->x+=(float)(dir)*(in->run?4.5f:2.5f)*MK4_TICK_SECONDS;}
            if(f->height>0)set_action(f,ACTION_JUMP);
            else set_action(f,f->crouching?ACTION_CROUCH:dir?ACTION_WALK:ACTION_IDLE);
        }
    }
    // Body collision uses symmetric correction, including at the arena walls.
    for(int i=0;i<2;++i)battle->fighters[i].x=clamp(battle->fighters[i].x,-battle->wall,battle->wall);
    Fighter *a=&battle->fighters[0], *b=&battle->fighters[1];
    if(fabsf(a->height-b->height)<1.25f && fabsf(a->x-b->x)<.7f) {
        float sign=a->x<=b->x?1.f:-1.f,mid=(a->x+b->x)*.5f;
        mid=clamp(mid,-battle->wall+.35f,battle->wall-.35f);a->x=mid-sign*.35f;b->x=mid+sign*.35f;
    }
    // Resolve from one snapshot so attacks that become active together can trade.
    Fighter snapshot[2];memcpy(snapshot,battle->fighters,sizeof snapshot);
    for(size_t i=0;i<2;++i) {
        const Fighter *attacker=&snapshot[i], *defender=&snapshot[1-i];const Move* m=battle_move(attacker->action);
        if(!m||attacker->connected||attacker->ticks<m->startup||attacker->ticks>=m->startup+m->active)continue;
        float distance=(defender->x-attacker->x)*(float)(attacker->facing);
        if(distance<0||distance>m->reach||fabsf(attacker->height-defender->height)>.9f)continue;
        if(m->low&&defender->height>.2f)continue;
        if(m->unblockable&&(defender->height>0||defender->action==ACTION_KNOCKDOWN||defender->action==ACTION_HIT))continue;
        if(defender->action==ACTION_KNOCKDOWN)continue;
        battle->fighters[i].connected=true;
        bool blocked=defender->action==ACTION_BLOCK&&!m->unblockable&&(!m->low||defender->crouching);
        Fighter *target=&battle->fighters[1-i];int damage=blocked?0:m->damage;
        target->health=fmaxf(0,target->health-damage);
        target->x=clamp(target->x+(float)(attacker->facing)*m->push*(blocked?.4f:1.f),-battle->wall,battle->wall);
        if(!blocked) {
            set_action(target,m->launch?ACTION_KNOCKDOWN:ACTION_HIT);
            // Store hit duration as a negative offset into the fixed recovery window.
            target->ticks=(m->launch?54:24)-m->stun;
            if(m->launch){target->velocity=4.7f;target->height=fmaxf(.01f,target->height);}
        }
        battle->effects[i]=(HitEffect){(attacker->x+defender->x)*.5f,defender->height+(m->low?.25f:1.2f),10,blocked};
    }
    if(a->health<=0||b->health<=0||battle->time_ticks<=0)finish_round(battle);
}
Input battle_cpu(const Battle *battle,int player) {
    Input in={0};const Fighter *f=&battle->fighters[player], *other=&battle->fighters[1-player];
    if(battle->phase!=PHASE_FIGHT)return in;
    float distance=fabsf(f->x-other->x);int cycle=(int)((battle->ticks+(uint64_t)(player)*17)%120);
    if(distance>1.05f){in.left=f->x>other->x;in.right=f->x<other->x;in.run=distance>3;}
    if(distance<2 && battle_move(other->action)&&cycle%50<15){in.block=true;in.crouch=other->action==ACTION_SWEEP;}
    if(distance<1.8f){in.punch=cycle==7;in.kick=cycle==45;in.uppercut=cycle==86;in.grab=distance<1&&cycle==108;}
    in.jump=distance>2&&cycle==115;return in;
}
const char* action_name(Action a){
    switch(a){case ACTION_IDLE:return "READY";case ACTION_WALK:return "MOVE";case ACTION_CROUCH:return "CROUCH";
        case ACTION_JUMP:return "JUMP";case ACTION_PUNCH:return "PUNCH";case ACTION_KICK:return "KICK";
        case ACTION_SWEEP:return "SWEEP";case ACTION_UPPERCUT:return "UPPERCUT";case ACTION_THROW:return "THROW";
        case ACTION_BLOCK:return "BLOCK";case ACTION_HIT:return "HIT";case ACTION_KNOCKDOWN:return "KNOCKDOWN";case ACTION_VICTORY:return "WINNER";}
    return "";
}

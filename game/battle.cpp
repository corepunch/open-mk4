#include "battle.h"
#include <algorithm>
#include <cmath>

namespace mk4::game {
namespace {
void set_action(Fighter& f,Action a){if(f.action!=a){f.action=a;f.ticks=0;f.connected=false;}}
bool locked(const Fighter& f) { return move(f.action)||f.action==Action::Hit||f.action==Action::Knockdown; }
}
const Move* move(Action a) {
    static const Move punch{5,4,12,7,19,1.25f,.13f};
    static const Move kick{10,5,18,12,25,1.85f,.32f};
    static const Move sweep{11,5,23,10,45,1.65f,.25f,true,true};
    static const Move uppercut{9,5,30,18,60,1.35f,.5f,false,true};
    static const Move grab{6,2,25,14,42,.95f,.45f,false,true,true};
    switch(a){case Action::Punch:return &punch;case Action::Kick:return &kick;case Action::Sweep:return &sweep;
        case Action::Uppercut:return &uppercut;case Action::Throw:return &grab;default:return nullptr;}
}
Battle::Battle(int p1,int p2){fighters[0].character=p1;fighters[1].character=p2;reset_match();}
void Battle::reset_match(){for(auto& f:fighters)f.wins=0;round=0;ticks=0;next_round();}
void Battle::next_round(){
    ++round;
    for(size_t i=0;i<2;++i){int ch=fighters[i].character,wins=fighters[i].wins;fighters[i]=Fighter{};
        auto& f=fighters[i];f.character=ch;f.wins=wins;f.x=i?2.f:-2.f;f.facing=i?-1:1;}
    effects={};phase=Phase::Ready;phase_ticks=0;time_ticks=99*60;winner=-1;
}
void Battle::finish_round(){
    winner=fighters[0].health==fighters[1].health?-1:(fighters[0].health>fighters[1].health?0:1);
    if(winner>=0){++fighters[size_t(winner)].wins;set_action(fighters[size_t(winner)],Action::Victory);}
    for(auto& f:fighters)if(f.health<=0)set_action(f,Action::Knockdown);
    phase=(winner>=0&&fighters[size_t(winner)].wins>=2)?Phase::MatchOver:Phase::RoundOver;phase_ticks=0;
}
void Battle::step(const std::array<Input,2>& input) {
    ++ticks;++phase_ticks;for(auto& e:effects)if(e.ticks>0)--e.ticks;
    if(phase==Phase::Ready){if(phase_ticks>=90){phase=Phase::Fight;phase_ticks=0;}return;}
    if(phase!=Phase::Fight){for(auto& f:fighters)++f.ticks;if(phase==Phase::RoundOver&&phase_ticks>=180)next_round();return;}
    --time_ticks;
    for(size_t i=0;i<2;++i) {
        auto& f=fighters[i];const auto& in=input[i];++f.ticks;
        if(!locked(f)&&f.height==0)f.facing=f.x<=fighters[1-i].x?1:-1;
        if(const auto* m=move(f.action)) {
            if(f.ticks>=m->startup+m->active+m->recovery)set_action(f,f.height>0?Action::Jump:Action::Idle);
        } else if((f.action==Action::Hit&&f.ticks>=24)||(f.action==Action::Knockdown&&f.ticks>=54&&f.height==0))set_action(f,Action::Idle);
        if(f.height>0 || f.velocity>0) {
            f.height+=f.velocity*tick_seconds;f.velocity-=15.f*tick_seconds;
            if(f.height<=0){f.height=0;f.velocity=0;if(f.action==Action::Jump)set_action(f,Action::Idle);}
        }
        if(locked(f))continue;
        f.crouching=in.crouch&&f.height==0;
        if(in.block){set_action(f,Action::Block);continue;}
        if(in.grab&&f.height==0&&!f.crouching)set_action(f,Action::Throw);
        else if(in.uppercut&&f.height==0)set_action(f,Action::Uppercut);
        else if(in.punch)set_action(f,f.crouching?Action::Uppercut:Action::Punch);
        else if(in.kick)set_action(f,f.crouching?Action::Sweep:Action::Kick);
        else {
            if(in.jump&&f.height==0){f.velocity=6.8f;f.height=.01f;set_action(f,Action::Jump);}
            int dir=int(in.right)-int(in.left);
            if(!f.crouching){f.x+=float(dir)*(in.run?4.5f:2.5f)*tick_seconds;}
            if(f.height>0)set_action(f,Action::Jump);
            else set_action(f,f.crouching?Action::Crouch:dir?Action::Walk:Action::Idle);
        }
    }
    // Body collision uses symmetric correction, including at the arena walls.
    for(auto& f:fighters)f.x=std::clamp(f.x,-wall,wall);
    auto& a=fighters[0];auto& b=fighters[1];
    if(std::abs(a.height-b.height)<1.25f && std::abs(a.x-b.x)<.7f) {
        float sign=a.x<=b.x?1.f:-1.f,mid=(a.x+b.x)*.5f;
        mid=std::clamp(mid,-wall+.35f,wall-.35f);a.x=mid-sign*.35f;b.x=mid+sign*.35f;
    }
    // Resolve from one snapshot so attacks that become active together can trade.
    auto snapshot=fighters;
    for(size_t i=0;i<2;++i) {
        const auto& attacker=snapshot[i];const auto& defender=snapshot[1-i];const Move* m=move(attacker.action);
        if(!m||attacker.connected||attacker.ticks<m->startup||attacker.ticks>=m->startup+m->active)continue;
        float distance=(defender.x-attacker.x)*float(attacker.facing);
        if(distance<0||distance>m->reach||std::abs(attacker.height-defender.height)>.9f)continue;
        if(m->low&&defender.height>.2f)continue;
        if(m->unblockable&&(defender.height>0||defender.action==Action::Knockdown||defender.action==Action::Hit))continue;
        if(defender.action==Action::Knockdown)continue;
        fighters[i].connected=true;
        bool blocked=defender.action==Action::Block&&!m->unblockable&&(!m->low||defender.crouching);
        auto& target=fighters[1-i];int damage=blocked?0:m->damage;
        target.health=std::max(0,target.health-damage);
        target.x=std::clamp(target.x+float(attacker.facing)*m->push*(blocked?.4f:1.f),-wall,wall);
        if(!blocked) {
            set_action(target,m->launch?Action::Knockdown:Action::Hit);
            // Store hit duration as a negative offset into the fixed recovery window.
            target.ticks=(m->launch?54:24)-m->stun;
            if(m->launch){target.velocity=4.7f;target.height=std::max(.01f,target.height);}
        }
        effects[i]={(attacker.x+defender.x)*.5f,defender.height+(m->low?.25f:1.2f),10,blocked};
    }
    if(a.health<=0||b.health<=0||time_ticks<=0)finish_round();
}
Input Battle::cpu(int player) const {
    Input in;const auto& f=fighters[size_t(player)];const auto& other=fighters[size_t(1-player)];
    if(phase!=Phase::Fight)return in;
    float distance=std::abs(f.x-other.x);int cycle=int((ticks+uint64_t(player)*17)%120);
    if(distance>1.05f){in.left=f.x>other.x;in.right=f.x<other.x;in.run=distance>3;}
    if(distance<2 && move(other.action)&&cycle%50<15){in.block=true;in.crouch=other.action==Action::Sweep;}
    if(distance<1.8f){in.punch=cycle==7;in.kick=cycle==45;in.uppercut=cycle==86;in.grab=distance<1&&cycle==108;}
    in.jump=distance>2&&cycle==115;return in;
}
const char* action_name(Action a){
    switch(a){case Action::Idle:return "READY";case Action::Walk:return "MOVE";case Action::Crouch:return "CROUCH";
        case Action::Jump:return "JUMP";case Action::Punch:return "PUNCH";case Action::Kick:return "KICK";
        case Action::Sweep:return "SWEEP";case Action::Uppercut:return "UPPERCUT";case Action::Throw:return "THROW";
        case Action::Block:return "BLOCK";case Action::Hit:return "HIT";case Action::Knockdown:return "KNOCKDOWN";case Action::Victory:return "WINNER";}
    return "";
}
}

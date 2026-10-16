#pragma once
#include <array>
#include <cstdint>

namespace mk4::game {
constexpr float tick_seconds=1.f/60.f;
enum class Action { Idle, Walk, Crouch, Jump, Punch, Kick, Sweep, Uppercut, Throw, Block, Hit, Knockdown, Victory };
enum class Phase { Ready, Fight, RoundOver, MatchOver };
struct Input {
    bool left=false,right=false,jump=false,crouch=false,block=false,run=false;
    bool punch=false,kick=false,uppercut=false,grab=false;
};
struct Move { int startup,active,recovery,damage,stun;float reach,push;bool low=false,launch=false,unblockable=false; };
const Move* move(Action);
struct Fighter {
    int character=0,health=100,wins=0,facing=1,ticks=0;
    Action action=Action::Idle;
    float x=0,height=0,velocity=0;
    bool connected=false,crouching=false;
};
struct HitEffect { float x=0,height=0;int ticks=0;bool blocked=false; };
struct Battle {
    std::array<Fighter,2> fighters;
    std::array<HitEffect,2> effects;
    Phase phase=Phase::Ready;
    int phase_ticks=0,time_ticks=99*60,round=1,winner=-1;
    uint64_t ticks=0;
    float wall=5.f;
    Battle(int p1=0,int p2=4);
    void reset_match();
    void next_round();
    void step(const std::array<Input,2>&);
    Input cpu(int player) const;
private:
    void finish_round();
};
const char* action_name(Action);
}

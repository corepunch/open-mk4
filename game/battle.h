#ifndef MK4_GAME_BATTLE_H
#define MK4_GAME_BATTLE_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MK4_TICK_SECONDS (1.f / 60.f)
typedef enum Action { ACTION_IDLE, ACTION_WALK, ACTION_CROUCH, ACTION_JUMP, ACTION_PUNCH, ACTION_KICK, ACTION_SWEEP, ACTION_UPPERCUT, ACTION_THROW, ACTION_BLOCK, ACTION_HIT, ACTION_KNOCKDOWN, ACTION_VICTORY } Action;
typedef enum Phase { PHASE_READY, PHASE_FIGHT, PHASE_ROUND_OVER, PHASE_MATCH_OVER } Phase;
typedef struct Input {
    bool left, right, jump, crouch, block, run;
    bool punch, kick, uppercut, grab;
} Input;
typedef struct Move {
    int startup, active, recovery, damage, stun;
    float reach, push;
    bool low, launch, unblockable;
} Move;
typedef struct Fighter {
    int character, health, wins, facing, ticks;
    Action action;
    float x, height, velocity;
    bool connected, crouching;
} Fighter;
typedef struct HitEffect { float x, height; int ticks; bool blocked; } HitEffect;
typedef struct Battle {
    Fighter fighters[2];
    HitEffect effects[2];
    Phase phase;
    int phase_ticks, time_ticks, round, winner;
    uint64_t ticks;
    float wall;
} Battle;

const Move *battle_move(Action action);
void battle_init(Battle *battle, int p1, int p2);
void battle_reset_match(Battle *battle);
void battle_next_round(Battle *battle);
void battle_step(Battle *battle, const Input input[2]);
Input battle_cpu(const Battle *battle, int player);
const char *action_name(Action action);

#ifdef __cplusplus
}
#endif
#endif

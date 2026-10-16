#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"

/* Approximate stance tables for fighters whose retail moves are not yet
 * ported. They follow the retail buttons from the shared tables in
 * docs/scorpion/input-and-commands.md section 6: back+LK sweeps, crouching
 * HP uppercuts and a close LP throws. Other selections use the generic move
 * for the button. A character with recovered moves owns its tables. */
#define ENTER_STATE(name, state)                                                                                                                               \
  static void enter_##name(Battle *battle, Fighter *fighter) {                                                                                                 \
    fighter_enter_state(battle, fighter, state);                                                                                                               \
  }
ENTER_STATE(punch, S_PUNCH)
ENTER_STATE(kick, S_KICK)
ENTER_STATE(sweep, S_SWEEP)
ENTER_STATE(uppercut, S_UPPERCUT)
ENTER_STATE(throw, S_THROW)

/* 0xb333 Q16: retail close-LP throw distance. */
#define MK4_THROW_RANGE 0.7f

static void press_punch(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_punch);
}

static void press_low_punch(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(battle, fighter) <= MK4_THROW_RANGE ? enter_throw : enter_punch);
}

static void press_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_kick);
}

static void press_low_kick(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? enter_sweep : enter_kick);
}

static void press_uppercut(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_sweep(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_sweep);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = press_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_uppercut, [BUTTON_HIGH_KICK] = press_sweep, [BUTTON_LOW_KICK] = press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = press_punch, [BUTTON_LOW_PUNCH] = press_punch, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_kick}};
// clang-format on

const Controls mk4_controls = {NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping}};

/* 460570/460710, 491780: forward walking .06, backward .7 scale.
 * 460980/4609f0: .13 running, 2% energy per tick, 40-tick recovery delay.
 * 422800 restores 5% after that delay. Straight jump: 46f7e0. */
const Motion mk4_motion = {0xf5c, 0x2147, 0x30a3, 0x28f};

#define WALK(c) {.clip = c, .last = MOVE_END}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef walk_gaits[][2] = {
  {WALK(269), WALK(274)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(266), WALK(271)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(268), WALK(273)},
  {WALK(265), WALK(270)},
  {WALK(267), WALK(272)},
  {WALK(265), WALK(270)},
  {WALK(267), WALK(272)},
  {WALK(590), WALK(589)},
};
// clang-format on
static const MoveDef run_open = WALK(317);
static const MoveDef run_cycle = WALK(307);
#undef WALK

void mk4_walk(Battle *battle, Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  bool forward = direction * fighter->facing > 0;
  bool run = forward && (fighter->held & BUTTON_BIT(BUTTON_RUN)) && fighter->run_energy > 0 && fighter->roster != 15;
  int character = roster_def(fighter->roster)->asset;
  character = character == 16 ? 2 : character == 17 ? 7 : character;
  if (run) {
    bool continuing = fighter->state == S_WALK && (fighter->current_move == &run_open || fighter->current_move == &run_cycle);
    const MoveDef *move = continuing ? &run_cycle : &run_open;
    if (fighter->current_move != move) {
      fighter_start_move(battle, fighter, S_WALK, move);
    }
    fighter->sample_override = continuing ? 11 + fighter->ticks % 13 : 0;
    fighter->run_energy = maxi(0, fighter->run_energy - 0x51e);
    fighter->run_delay = 40;
    fighter_set_speed(battle, fighter, mk4_motion.run);
    fighter->body.walking = true;
    return;
  }
  if (fighter->armed) {
    weapon_stance(battle, fighter, S_WALK);
  } else {
    const MoveDef *move = &walk_gaits[character][forward ? 0 : 1];
    if (fighter->current_move != move || fighter->state != S_WALK) {
      fighter_start_move(battle, fighter, S_WALK, move);
    }
  }
  int count = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count : 30;
  fighter->sample_override = fighter->ticks % count;
  q16 speed = forward ? mk4_motion.walk : -q16_mul(mk4_motion.walk, 0xb333);
  fighter_set_speed(battle, fighter, speed);
  fighter->body.walking = true;
}

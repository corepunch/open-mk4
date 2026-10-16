#include "game.h"
#include "mk4_characters.h"

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

const Motion mk4_motion = {2.5f, 4.5f, 6.8f, 15.f};

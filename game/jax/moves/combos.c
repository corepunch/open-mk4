#include "../character.h"

static void press_combo_hp(Fighter *fighter);
static void combo_tail(Fighter *fighter);
static const ButtonTable jax_close_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_combo_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable jax_combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable jax_close_hk_followup = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [4] = {{.clip = 262, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  [5] = {{.clip = 261, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  [6] = {{.clip = 314, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
};
// clang-format on
static const NativeGroundProfile profile = {
    attacks, {&jax_close_hp_followup, &jax_combo_hp_followup, &jax_close_hk_followup}, fighter_ready, 5, 2, 27, 0, 0, 0, NULL};

void jax_close_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[4].move);
  fighter->script_phase = 4;
  fighter_start_fiber(fighter, jax_entry_retail_script_004e61e0, NULL);
}

void jax_combo_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[5].move);
  fighter->script_phase = 5;
  fighter_start_fiber(fighter, jax_entry_retail_script_004e69b0, NULL);
}

void jax_close_hk(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &attacks[6].move);
  fighter->script_phase = 6;
  fighter_start_fiber(fighter, jax_entry_retail_script_004e6190, NULL);
}

static void combo_tail(Fighter *fighter) {
  native_ground_tail(fighter, &profile);
}

void jax_script_close_hp_tail(Fighter *fighter) {
  combo_tail(fighter);
}

void jax_script_combo_hp_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  combo_tail(fighter);
}

void jax_script_close_hk_tail(Fighter *fighter) {
  combo_tail(fighter);
}

static void press_combo_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_combo_hp);
}

void jax_press_air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, jax_close_hp);
  } else {
    jax_press_hp(fighter);
  }
}

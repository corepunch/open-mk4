#include "../character.h"

static void press_combo_hp(Fighter *fighter);
static void combo_tail(Fighter *fighter);
static const ButtonTable reptile_close_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_combo_hp,
    [BUTTON_LOW_PUNCH] = reptile_press_low_punch,
    [BUTTON_HIGH_KICK] = reptile_press_kick,
    [BUTTON_LOW_KICK] = reptile_press_low_kick}};
static const ButtonTable reptile_combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = reptile_press_punch,
    [BUTTON_LOW_PUNCH] = reptile_press_low_punch,
    [BUTTON_HIGH_KICK] = reptile_press_kick,
    [BUTTON_LOW_KICK] = reptile_press_low_kick}};
static const ButtonTable reptile_close_hk_followup = {{[BUTTON_HIGH_PUNCH] = reptile_press_punch,
    [BUTTON_LOW_PUNCH] = reptile_press_low_punch,
    [BUTTON_HIGH_KICK] = reptile_press_kick,
    [BUTTON_LOW_KICK] = reptile_press_low_kick}};

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [4] = {{.clip = 262, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  [5] = {{.clip = 261, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  [6] = {{.clip = 314, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
};
// clang-format on
static const NativeGroundProfile profile = {
    attacks, {&reptile_close_hp_followup, &reptile_combo_hp_followup, &reptile_close_hk_followup}, fighter_ready, 5, 2, 27, 0, 0, 0, NULL};

void reptile_close_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[4].move);
  fighter->script_phase = 4;
  fighter_start_fiber(fighter, reptile_entry_retail_script_004e61e0, NULL);
}

void reptile_combo_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[5].move);
  fighter->script_phase = 5;
  fighter_start_fiber(fighter, reptile_entry_retail_script_004e69b0, NULL);
}

void reptile_close_hk(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &attacks[6].move);
  fighter->script_phase = 6;
  fighter_start_fiber(fighter, reptile_entry_retail_script_004e6190, NULL);
}

static void combo_tail(Fighter *fighter) {
  native_ground_tail(fighter, &profile);
}

void reptile_script_close_hp_tail(Fighter *fighter) {
  combo_tail(fighter);
}

void reptile_script_combo_hp_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  combo_tail(fighter);
}

void reptile_script_close_hk_tail(Fighter *fighter) {
  combo_tail(fighter);
}

static void press_combo_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_combo_hp);
}

void reptile_press_air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, reptile_close_hp);
  } else {
    reptile_press_punch(fighter);
  }
}

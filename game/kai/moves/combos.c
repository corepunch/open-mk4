#include "../character.h"

static void press_combo_hp(Fighter *fighter);
static void combo_tail(Fighter *fighter);
static const ButtonTable kai_close_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_combo_hp,
    [BUTTON_LOW_PUNCH] = kai_press_low_punch,
    [BUTTON_HIGH_KICK] = kai_press_high_kick,
    [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const ButtonTable kai_combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = kai_press_high_punch,
    [BUTTON_LOW_PUNCH] = kai_press_low_punch,
    [BUTTON_HIGH_KICK] = kai_press_high_kick,
    [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const ButtonTable kai_close_hk_followup = {{[BUTTON_HIGH_PUNCH] = kai_press_high_punch,
    [BUTTON_LOW_PUNCH] = kai_press_low_punch,
    [BUTTON_HIGH_KICK] = kai_press_high_kick,
    [BUTTON_LOW_KICK] = kai_press_low_kick}};

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [4] = {{.clip = 262, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  [5] = {{.clip = 261, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  [6] = {{.clip = 314, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
};
// clang-format on
static const NativeGroundProfile profile = {
    attacks, {&kai_close_hp_followup, &kai_combo_hp_followup, &kai_close_hk_followup}, fighter_ready, 5, 2, 27, 0, 0, 0, NULL};

void kai_close_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[4].move);
  fighter->script_phase = 4;
  kai_entry_retail_script_004e61e0(fighter);
}

void kai_combo_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &attacks[5].move);
  fighter->script_phase = 5;
  kai_entry_retail_script_004e69b0(fighter);
}

void kai_close_hk(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &attacks[6].move);
  fighter->script_phase = 6;
  kai_entry_retail_script_004e6190(fighter);
}

static void combo_tail(Fighter *fighter) {
  native_ground_tail(fighter, &profile);
}

void kai_script_close_hp_tail(Fighter *fighter) {
  combo_tail(fighter);
}

void kai_script_combo_hp_tail(Fighter *fighter) {
  native_hold(fighter, 1, combo_tail);
}

void kai_script_close_hk_tail(Fighter *fighter) {
  combo_tail(fighter);
}

static void press_combo_hp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_combo_hp);
}

void kai_press_air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, kai_close_hp);
  } else {
    kai_press_high_punch(fighter);
  }
}

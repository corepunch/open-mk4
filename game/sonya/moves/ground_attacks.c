#include "../character.h"

static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static void sonya_crouch_hk(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 308, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 53, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
};
// clang-format on
static const NativeGroundProfile ground_profile = {ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, fighter_ready, 2, 27};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = sonya_press_combo_hp,
    [BUTTON_LOW_PUNCH] = sonya_press_lp,
    [BUTTON_HIGH_KICK] = sonya_press_kick,
    [BUTTON_LOW_KICK] = sonya_press_lk}};
static const ButtonTable combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = sonya_press_jab, [BUTTON_LOW_PUNCH] = sonya_press_lp, [BUTTON_HIGH_KICK] = sonya_press_kick, [BUTTON_LOW_KICK] = sonya_press_lk}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = sonya_press_knee_hp,
    [BUTTON_LOW_PUNCH] = sonya_press_lp,
    [BUTTON_HIGH_KICK] = sonya_press_combo_hk,
    [BUTTON_LOW_KICK] = sonya_press_lk}};

void sonya_ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

void sonya_ground_native_next(Fighter *fighter) {
  native_ground_next(fighter, &ground_profile);
}

void sonya_ground_attack(Fighter *fighter, int index) {
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {sonya_entry_retail_script_004e5d90, sonya_ground_native_tail, sonya_entry_retail_script_004e6158,
      sonya_entry_retail_script_004e6020, sonya_entry_retail_script_004e61e0, sonya_entry_retail_script_004e69b0, sonya_entry_retail_script_004e6190};
  scripts[index](fighter);
}

void sonya_crouch_lp(Fighter *fighter) {
  sonya_ground_attack(fighter, G_CROUCH_LP);
}

static void sonya_crouch_hk(Fighter *fighter) {
  sonya_ground_attack(fighter, G_CROUCH_HK);
}

void sonya_press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : sonya_crouch_lp);
}

void sonya_press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, sonya_crouch_hk);
}

void sonya_press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, retail_crouch_low_kick);
}

void sonya_script_knee_tail(Fighter *fighter) {
  sonya_ground_native_tail(fighter);
}

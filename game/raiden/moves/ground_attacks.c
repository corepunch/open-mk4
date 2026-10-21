#include "../character.h"

static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  [4] = {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  [5] = {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  [6] = {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
};
// clang-format on
static const NativeGroundProfile ground_profile = {ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, fighter_ready, 2, 27};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = raiden_press_combo_hp,
    [BUTTON_LOW_PUNCH] = raiden_press_lp,
    [BUTTON_HIGH_KICK] = raiden_press_kick,
    [BUTTON_LOW_KICK] = raiden_press_lk}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = raiden_press_jab,
    [BUTTON_LOW_PUNCH] = raiden_press_lp,
    [BUTTON_HIGH_KICK] = raiden_press_kick,
    [BUTTON_LOW_KICK] = raiden_press_lk}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = raiden_press_knee_hp,
    [BUTTON_LOW_PUNCH] = raiden_press_lp,
    [BUTTON_HIGH_KICK] = raiden_press_combo_hk,
    [BUTTON_LOW_KICK] = raiden_press_lk}};

void raiden_ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

void raiden_ground_native_next(Fighter *fighter) {
  native_ground_next(fighter, &ground_profile);
}

void raiden_ground_attack(Fighter *fighter, int index) {
  require(index >= G_COMBO_HP && index <= G_CLOSE_HK, "Ground profile only owns remaining close-native attacks");
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : 0;
  static const FighterFn scripts[] = {

      [G_COMBO_HP] = raiden_entry_retail_script_004e69b0,
      [G_CLOSE_HK] = raiden_entry_retail_script_004e6190,
  };
  scripts[index](fighter);
}

void raiden_script_knee_tail(Fighter *fighter) {
  raiden_ground_native_tail(fighter);
}

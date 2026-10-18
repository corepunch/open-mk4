#include "../character.h"

static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static void liu_kang_crouch_lk(Fighter *fighter);
static void liu_kang_crouch_hk(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {.move = {.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}}, /* generated 4e6d08 */
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {.move = {.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}}, /* generated 4e60b8 */
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 64, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14}, 14, 39, 67},
};
// clang-format on
static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, liu_kang_ready, 4, 2, 27, 19, 20, 44, NULL};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = liu_kang_press_combo_hp,
    [BUTTON_LOW_PUNCH] = liu_kang_press_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_kick,
    [BUTTON_LOW_KICK] = liu_kang_press_lk}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = liu_kang_press_jab,
    [BUTTON_LOW_PUNCH] = liu_kang_press_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_kick,
    [BUTTON_LOW_KICK] = liu_kang_press_lk}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = liu_kang_press_knee_hp,
    [BUTTON_LOW_PUNCH] = liu_kang_press_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_combo_hk,
    [BUTTON_LOW_KICK] = liu_kang_press_lk}};

void liu_kang_ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

void liu_kang_ground_attack(Fighter *fighter, int index) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_COMBO_HP || index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {liu_kang_entry_retail_script_004e6d08, liu_kang_ground_native_tail, liu_kang_entry_retail_script_004e6158,
      liu_kang_entry_retail_script_004e60b8, liu_kang_entry_retail_script_004e61e0, liu_kang_entry_retail_script_004e69b0,
      liu_kang_entry_retail_script_004e6190, liu_kang_entry_retail_script_004e6e90};
  FighterFn script = scripts[index];
  fighter_start_fiber(fighter, script, NULL);
}

void liu_kang_crouch_lp(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_CROUCH_LP);
}

static void liu_kang_crouch_lk(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_CROUCH_LK);
}

static void liu_kang_crouch_hk(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_CROUCH_HK);
}

void liu_kang_press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : liu_kang_crouch_lp);
}

void liu_kang_press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, liu_kang_crouch_hk);
}

void liu_kang_press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, liu_kang_crouch_lk);
}

void liu_kang_liu_script_crouch_lk_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  liu_kang_ground_native_tail(fighter);
}

void liu_kang_liu_script_knee_tail(Fighter *fighter) {
  liu_kang_ground_native_tail(fighter);
}

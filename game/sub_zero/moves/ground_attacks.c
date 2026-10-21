#include "../character.h"

void sub_zero_ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &sub_zero_ground_profile);
}

void sub_zero_ground_attack(Fighter *fighter, int index) {
  require(index >= G_COMBO_HP && index <= G_CLOSE_HK, "Ground profile only owns remaining close-native attacks");
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP ? S_PUNCH : S_KICK, &sub_zero_ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : 0;
  static const FighterFn scripts[] = {

      [G_COMBO_HP] = sub_zero_entry_retail_script_004e69c8,
      [G_CLOSE_HK] = sub_zero_entry_retail_script_004e6190,
  };
  scripts[index](fighter);
}

void sub_zero_script_knee_tail(Fighter *fighter) {
  sub_zero_ground_native_tail(fighter);
}

#include "../character.h"

static void sub_zero_crouch_lk(Fighter *fighter);
static void sub_zero_crouch_hk(Fighter *fighter);

void sub_zero_ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &sub_zero_ground_profile);
}

void sub_zero_ground_attack(Fighter *fighter, int index) {
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &sub_zero_ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {sub_zero_entry_retail_script_004e6da0, sub_zero_ground_native_tail, sub_zero_entry_retail_script_004e6158,
      sub_zero_entry_retail_script_004e5fd8, sub_zero_entry_retail_script_004e61e0, sub_zero_entry_retail_script_004e69c8,
      sub_zero_entry_retail_script_004e6190};
  scripts[index](fighter);
}

void sub_zero_crouch_lp(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_CROUCH_LP);
}

static void sub_zero_crouch_lk(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_CROUCH_LK);
}

static void sub_zero_crouch_hk(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_CROUCH_HK);
}

void sub_zero_press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : sub_zero_crouch_lp);
}

void sub_zero_press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, sub_zero_crouch_hk);
}

void sub_zero_press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, sub_zero_crouch_lk);
}

void sub_zero_script_crouch_lk_tail(Fighter *fighter) {
  native_ground_next(fighter, &sub_zero_ground_profile);
}

void sub_zero_script_knee_tail(Fighter *fighter) {
  sub_zero_ground_native_tail(fighter);
}

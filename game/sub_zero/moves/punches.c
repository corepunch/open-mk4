#include "../character.h"

static void sub_zero_combo_hp(Fighter *fighter);

void sub_zero_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &sub_zero_s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  sub_zero_entry_retail_script_004e62f0(fighter);
}

void sub_zero_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    sub_zero_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  retail_high_punch(fighter);
}

static void sub_zero_combo_hp(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_COMBO_HP);
}

void sub_zero_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sub_zero_hp);
}

void sub_zero_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    sub_zero_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, sub_zero_combo_hp);
  }
}

void sub_zero_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, sub_zero_enter_uppercut);
  } else {
    sub_zero_press_jab(fighter);
  }
}

void sub_zero_script_close_hp_tail(Fighter *fighter) {
  sub_zero_ground_native_tail(fighter);
}

void sub_zero_script_combo_hp_tail(Fighter *fighter) {
  native_ground_next(fighter, &sub_zero_ground_profile);
}

/* Shared CPU selections use these native entries directly, as 45f5f0 does.
 * Human command patterns and held-button predicates do not gate drone actions. */
void sub_zero_cpu_close_hp(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_CLOSE_HP);
}

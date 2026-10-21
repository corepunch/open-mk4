#include "../character.h"

static void raiden_combo_hp(Fighter *fighter);

void raiden_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &raiden_s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  raiden_entry_retail_script_004e62f0(fighter);
}

void raiden_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    raiden_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  retail_high_punch(fighter);
}

static void raiden_combo_hp(Fighter *fighter) {
  raiden_ground_attack(fighter, G_COMBO_HP);
}

void raiden_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : raiden_hp);
}

void raiden_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    raiden_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, raiden_combo_hp);
  }
}

void raiden_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, raiden_enter_uppercut);
  } else {
    raiden_press_jab(fighter);
  }
}

void raiden_script_close_hp_tail(Fighter *fighter) {
  raiden_ground_native_tail(fighter);
}

void raiden_script_combo_hp_tail(Fighter *fighter) {
  raiden_ground_native_next(fighter);
}

void raiden_cpu_close_hp(Fighter *fighter) {
  raiden_ground_attack(fighter, G_CLOSE_HP);
}

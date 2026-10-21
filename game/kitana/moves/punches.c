#include "../character.h"

static void kitana_combo_hp(Fighter *fighter);

void kitana_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &kitana_s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  kitana_entry_retail_script_004e62f0(fighter);
}

void kitana_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    kitana_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  retail_high_punch(fighter);
}

static void kitana_combo_hp(Fighter *fighter) {
  kitana_ground_attack(fighter, G_COMBO_HP);
}

void kitana_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : kitana_hp);
}

void kitana_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    kitana_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, kitana_combo_hp);
  }
}

void kitana_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, kitana_enter_uppercut);
  } else {
    kitana_press_jab(fighter);
  }
}

void kitana_script_close_hp_tail(Fighter *fighter) {
  kitana_ground_native_tail(fighter);
}

void kitana_script_combo_hp_tail(Fighter *fighter) {
  kitana_ground_native_next(fighter);
}

void kitana_cpu_close_hp(Fighter *fighter) {
  kitana_ground_attack(fighter, G_CLOSE_HP);
}

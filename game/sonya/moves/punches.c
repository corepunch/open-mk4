#include "../character.h"

static void sonya_combo_hp(Fighter *fighter);

void sonya_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &sonya_s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  sonya_entry_retail_script_004e62f0(fighter);
}

void sonya_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    sonya_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &sonya_s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, sonya_hp_script, NULL);
}

static void sonya_combo_hp(Fighter *fighter) {
  sonya_ground_attack(fighter, G_COMBO_HP);
}

void sonya_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sonya_hp);
}

void sonya_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    sonya_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, sonya_combo_hp);
  }
}

void sonya_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, sonya_enter_uppercut);
  } else {
    sonya_press_jab(fighter);
  }
}

void sonya_script_close_hp_tail(Fighter *fighter) {
  sonya_ground_native_tail(fighter);
}

void sonya_script_combo_hp_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  sonya_ground_native_tail(fighter);
}

void sonya_cpu_close_hp(Fighter *fighter) {
  sonya_ground_attack(fighter, G_CLOSE_HP);
}

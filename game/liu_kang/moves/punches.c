#include "../character.h"

static void liu_kang_combo_hp(Fighter *fighter);

void liu_kang_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    liu_kang_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  retail_high_punch(fighter);
}

static void liu_kang_combo_hp(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_COMBO_HP);
}

void liu_kang_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : liu_kang_hp);
}

void liu_kang_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    liu_kang_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, liu_kang_combo_hp);
  }
}

void liu_kang_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, liu_kang_enter_uppercut);
  } else {
    liu_kang_press_jab(fighter);
  }
}

void liu_kang_liu_script_close_hp_tail(Fighter *fighter) {
  liu_kang_ground_native_tail(fighter);
}

void liu_kang_liu_script_combo_hp_tail(Fighter *fighter) {
  liu_kang_ground_native_tail(fighter);
}

/* 438e50/60/70/80 select 4df8e8/928/968/9a8[3]:
 * bicycle, low fireball, flying kick, low fireball. */
void liu_kang_cpu_close_hp(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_CLOSE_HP);
}

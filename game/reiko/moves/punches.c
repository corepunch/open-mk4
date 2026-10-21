#include "../character.h"

static void reiko_high_punch(Fighter *fighter);

static void reiko_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    reiko_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void reiko_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : reiko_high_punch);
}

void reiko_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? retail_lp_throw : retail_jab);
}

void reiko_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, retail_uppercut);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

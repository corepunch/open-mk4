#include "../character.h"

static void noob_saibot_high_punch(Fighter *fighter);

static void noob_saibot_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void noob_saibot_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : noob_saibot_high_punch);
}

void noob_saibot_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? retail_lp_throw : retail_jab);
}

void noob_saibot_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, retail_uppercut);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

#include "../character.h"

void kai_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    kai_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void kai_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : kai_high_punch);
}

void kai_press_low_punch(Fighter *fighter) {
  if (fighter->armed) {
    fighter_interrupt(fighter, fighter->weapon->lp);
    return;
  }
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, false)) {
    fighter_interrupt(fighter, kai_lp_throw);
    return;
  }
  fighter_interrupt(fighter, retail_jab);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

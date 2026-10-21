#include "../character.h"

static void fujin_entry_high_punch(Fighter *fighter);
static void fujin_enter_punch(Fighter *fighter);

static void fujin_entry_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

static void fujin_enter_punch(Fighter *fighter) {
  fujin_entry_high_punch(fighter);
}

void fujin_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : fujin_enter_punch);
}

void fujin_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? fujin_enter_throw : retail_jab);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

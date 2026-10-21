#include "../character.h"

static void reptile_entry_high_punch(Fighter *fighter);
static void reptile_entry_low_punch(Fighter *fighter);

static void reptile_entry_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    reptile_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

static void reptile_entry_low_punch(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.7f) {
    retail_lp_throw(fighter);
  } else {
    retail_jab(fighter);
  }
}

void reptile_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : reptile_entry_high_punch);
}

void reptile_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : reptile_entry_low_punch);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

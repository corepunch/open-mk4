#include "../character.h"

static void jax_high_punch(Fighter *fighter);

static void jax_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void jax_press_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_high_punch);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

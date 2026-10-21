#include "../character.h"

static void jax_high_punch(Fighter *fighter);

void jax_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &jax_s_jab);
  jax_entry_retail_script_004e62f0(fighter);
}

static void jax_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    jax_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void jax_press_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_high_punch);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

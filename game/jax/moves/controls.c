#include "../character.h"

void jax_press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? fighter->weapon->lp : (fighter_distance(fighter) <= 0.7f ? retail_lp_throw : retail_jab);
  fighter_interrupt(fighter, entry);
}

void jax_press_hk(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, retail_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, retail_close_high_kick);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void jax_press_lk(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, retail_lk_throw);
    return;
  }

  FighterFn entry = fighter_holds_back(fighter) ? retail_sweep : retail_low_kick;
  fighter_interrupt(fighter, entry);
}

#include "../character.h"

void kai_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, kai_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, kai_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void kai_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, kai_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_sweep : retail_low_kick);
}

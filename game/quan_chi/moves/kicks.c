#include "../character.h"

void quan_chi_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, quan_chi_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, quan_chi_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void quan_chi_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, quan_chi_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? quan_chi_sweep : retail_low_kick);
}

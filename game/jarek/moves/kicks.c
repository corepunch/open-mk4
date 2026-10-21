#include "../character.h"

void jarek_script_low_kick_avoid(Fighter *fighter) {
  fighter->avoids_high = true;
}

void jarek_script_low_kick_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}

void jarek_press_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, jarek_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, jarek_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void jarek_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, jarek_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_sweep : retail_low_kick);
}

#include "../character.h"

/* 46a760 runs 470c60 with block 4e5d30 (pose words 17, 17, 16): three
 * B[1] queries latch the first contact and skip crouching (2001) victims.
 * The shared runner keeps the selected pose, as for Fujin's 4e5d00, and
 * 4e5c70 inherits it. */

void shinnok_press_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, retail_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, shinnok_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void shinnok_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, retail_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_sweep : retail_low_kick);
}

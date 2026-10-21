#include "../character.h"

/* 46a760 -> 470c60 with block 4e5d30: B[1] is queried while poses 17, 17
 * and 16 are submitted. A contact latches and the victim's crouch action
 * 2001 skips the query. 46a770 then stores action 1006 (470e80) and enters
 * recovery 4e5c70 from pose 16. */

void noob_saibot_press_high_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, noob_saibot_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, noob_saibot_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void noob_saibot_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, noob_saibot_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? noob_saibot_sweep : retail_low_kick);
}

void noob_saibot_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_sweep);
}

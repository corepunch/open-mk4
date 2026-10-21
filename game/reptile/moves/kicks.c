#include "../character.h"

/* 4e5ca0 selects 4e6c90: clip 66 opens at 3 and advances twice per visit
 * to 17, then 46a760 runs 470c60 with block 4e5d30. */

/* 4e5d30 selector 1: B[1], 13%, reactions 16/13/6. */

/* 4e5d30 lists poses 17, 17 and 16 with one B[1] query per visit; the
 * terminator calls 46a770 on the following visit. */

void reptile_press_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, reptile_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, reptile_close_hk);
    return;
  }

  fighter_interrupt(fighter, retail_high_kick);
}

void reptile_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, reptile_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reptile_sweep : retail_low_kick);
}

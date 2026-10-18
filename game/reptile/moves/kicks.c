#include "../character.h"

static void reptile_high_kick(Fighter *fighter);
static void reptile_low_kick(Fighter *fighter);

static void reptile_high_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &reptile_s_kick, reptile_entry_retail_script_004e5ca0);
}

static void reptile_low_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &reptile_s_kick, reptile_entry_retail_script_004e5d90);
}

void reptile_press_kick(Fighter *fighter) {
  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, reptile_close_hk);
    return;
  }

  fighter_interrupt(fighter, reptile_high_kick);
}

void reptile_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, reptile_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reptile_sweep : reptile_low_kick);
}

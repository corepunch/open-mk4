#include "../character.h"

static void noob_saibot_high_kick(Fighter *fighter);
static void noob_saibot_low_kick(Fighter *fighter);

void noob_saibot_script_kick_finish(Fighter *fighter) {
  noob_saibot_entry_retail_script_004e5c70(fighter);
}

static void noob_saibot_high_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e5ca0);
}

static void noob_saibot_low_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e5d90);
}

void noob_saibot_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, noob_saibot_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, noob_saibot_close_hk);
    return;
  }

  fighter_interrupt(fighter, noob_saibot_high_kick);
}

void noob_saibot_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, noob_saibot_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? noob_saibot_sweep : noob_saibot_low_kick);
}

void noob_saibot_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_sweep);
}

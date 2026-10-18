#include "../character.h"

static void shinnok_low_kick(Fighter *fighter);
static const MoveDef s_low_kick = {.clip = 69, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};

void shinnok_high_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &shinnok_s_high_kick, shinnok_entry_retail_script_004e5ca0);
}

static void shinnok_low_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_low_kick, shinnok_entry_retail_script_004e5d90);
}

void shinnok_script_kick_finish(Fighter *fighter) {
  shinnok_entry_retail_script_004e5c70(fighter);
}

void shinnok_press_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, shinnok_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, shinnok_close_hk);
    return;
  }

  fighter_interrupt(fighter, shinnok_high_kick);
}

void shinnok_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, shinnok_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? shinnok_sweep : shinnok_low_kick);
}

#include "../character.h"

static const MoveDef s_short_kick = {.clip = 65, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

void jarek_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_short_kick);
  jarek_entry_retail_script_004e5ca0(fighter);
}

void jarek_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &jarek_s_low_kick);
  fighter->action = ACTION_RETAIL_0102;
  jarek_entry_retail_script_004e5d90(fighter);
}

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

  fighter_interrupt(fighter, jarek_high_kick);
}

void jarek_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, jarek_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? jarek_sweep : jarek_low_kick);
}

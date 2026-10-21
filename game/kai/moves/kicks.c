#include "../character.h"

static const MoveDef s_narrow_kick = {.clip = 71, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

static void kai_low_kick(Fighter *fighter);

void kai_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, kai_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, kai_close_hk);
    return;
  }

  fighter_interrupt(fighter, kai_high_kick);
}

void kai_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, kai_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kai_sweep : kai_low_kick);
}

void kai_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_narrow_kick);
  kai_entry_retail_script_004e5ca0(fighter);
}

static void kai_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &kai_s_low_kick);
  fighter->action = ACTION_RETAIL_0102;
  kai_entry_retail_script_004e5d90(fighter);
}

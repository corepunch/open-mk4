#include "../character.h"

static const MoveDef s_narrow_kick = {.clip = 71, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

static void quan_chi_low_kick(Fighter *fighter);

void quan_chi_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_narrow_kick);
  quan_chi_entry_retail_script_004e5ca0(fighter);
}

static void quan_chi_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &quan_chi_s_high_kick);
  fighter->action = ACTION_RETAIL_0102;
  quan_chi_entry_retail_script_004e5d90(fighter);
}

void quan_chi_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, quan_chi_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, quan_chi_close_hk);
    return;
  }

  fighter_interrupt(fighter, quan_chi_high_kick);
}

void quan_chi_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, quan_chi_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? quan_chi_sweep : quan_chi_low_kick);
}

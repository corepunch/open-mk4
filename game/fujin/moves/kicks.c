#include "../character.h"

static void fujin_script_kick_finish(Fighter *fighter);
static void fujin_low_kick(Fighter *fighter);
static void fujin_high_kick(Fighter *fighter);
static void fujin_entry_low_kick(Fighter *fighter);
static void fujin_enter_kick(Fighter *fighter);
static const MoveDef s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD | SF_LOW};

static void fujin_script_kick_finish(Fighter *fighter) {
  fujin_entry_retail_script_004e5c70(fighter);
}

static void fujin_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_low_kick);
  fighter_start_fiber(fighter, fujin_entry_retail_script_004e5d90, NULL);
}

static void fujin_high_kick(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &fujin_s_kick, fujin_entry_retail_script_004e5ca0);
}

static void fujin_entry_low_kick(Fighter *fighter) {
  fujin_low_kick(fighter);
}

static void fujin_enter_kick(Fighter *fighter) {
  fujin_high_kick(fighter);
}

void fujin_press_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fujin_press_roundhouse(fighter);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, fujin_close_hk);
    return;
  }

  fighter_interrupt(fighter, fujin_enter_kick);
}

void fujin_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, fujin_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? fujin_enter_sweep : fujin_low_kick);
}

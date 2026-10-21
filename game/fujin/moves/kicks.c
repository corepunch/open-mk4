#include "../character.h"

static void fujin_entry_low_kick(Fighter *fighter);
static void fujin_enter_kick(Fighter *fighter);

static void fujin_entry_low_kick(Fighter *fighter) {
  retail_low_kick(fighter);
}

static void fujin_enter_kick(Fighter *fighter) {
  retail_high_kick(fighter);
}

void fujin_press_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
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

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? fujin_enter_sweep : fujin_entry_low_kick);
}

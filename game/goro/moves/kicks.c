#include "../character.h"

void goro_event_counter_kick(Fighter *fighter);
static void goro_high_kick(Fighter *fighter);
static void goro_low_kick(Fighter *fighter);

void goro_event_counter_kick(Fighter *fighter) {
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfb70, 598, 31, 0);
}

void goro_script_kick_finish(Fighter *fighter) {
  goro_entry_retail_script_004e5c70(fighter);
}

static void goro_high_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e5ca0);
}

static void goro_low_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e5d90);
}

void goro_enter_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_high_kick);
}

void goro_enter_low_kick(Fighter *fighter) {
  /*460fc0/48ebd0 selects4e6ef8[6]=46ef90 while holding back. */
  if (fighter_holds_back(fighter)) {
    goro_enter_sweep(fighter);
    return;
  }
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, goro_lk_throw);
    return;
  }
  fighter_interrupt(fighter, goro_low_kick);
}

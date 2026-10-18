#include "../character.h"

static const MoveDef s_narrow_kick = {.clip = 71, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

static void cage_shadow_kick(Fighter *fighter);
void cage_cpu_kick(Fighter *fighter);
static void cage_high_kick(Fighter *fighter);
static void cage_low_kick(Fighter *fighter);

void cage_script_cpu_kick_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 0;
}

void cage_script_kick_finish(Fighter *fighter) {
  native_narrow_kick_contact(fighter, cage_entry_retail_script_004e6998);
}

static void cage_shadow_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &cage_s_kick);
  fighter->sample_override = 27;
  fighter_start_fiber(fighter, cage_entry_retail_script_004e5ba8, NULL);
}

void cage_cpu_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_cpu_kick_script, cage_entry_retail_script_004e80a8);
}

static void cage_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_narrow_kick);
  cage_entry_retail_script_004e5ca0(fighter);
}

static void cage_low_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_kick, cage_entry_retail_script_004e5d90);
}

void cage_press_high_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, cage_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, cage_close_hk);
    return;
  }

  fighter_interrupt(fighter, cage_high_kick);
}

void cage_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, cage_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? cage_sweep : cage_low_kick);
}

void cage_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, cage_sweep);
}

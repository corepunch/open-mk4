#include "../character.h"

static void jarek_kick_frame(Fighter *fighter);
static void jarek_script_low_kick_finish(Fighter *fighter);
static const AttackRecord jarek_high_kick_record = {89718, -55705, 86441, 70123, 16, 13, 6, 0, 8519};

void jarek_high_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &jarek_s_high_kick, jarek_entry_retail_script_004e5ca0);
}

void jarek_low_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &jarek_s_low_kick, jarek_entry_retail_script_004e5d90);
}

static void jarek_kick_frame(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &jarek_high_kick_record);
  }
}

void jarek_script_kick_finish(Fighter *fighter) {
  native_calls(fighter, 2, 1, jarek_kick_frame);
  jarek_entry_retail_script_004e5c70(fighter);
}

void jarek_script_low_kick_avoid(Fighter *fighter) {
  fighter->avoids_high = true;
}

void jarek_script_low_kick_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}

static void jarek_script_low_kick_finish(Fighter *fighter) {
  fighter->avoids_high = false;
  fighter_fiber_finish(jarek_ready);
}

void jarek_press_kick(Fighter *fighter) {
  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, jarek_close_hk);
    return;
  }

  fighter_interrupt(fighter, jarek_high_kick);
}

void jarek_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? jarek_sweep : jarek_low_kick);
}

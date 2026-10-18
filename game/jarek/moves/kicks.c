#include "../character.h"

static const MoveDef s_short_kick = {.clip = 65, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

static void jarek_kick_frame(Fighter *fighter);
static void jarek_script_low_kick_finish(Fighter *fighter);
static const AttackRecord jarek_high_kick_record = {0x1451e, -0xc7ae, 0x10000, 0xc7ae, 16, 13, 6, 0, 0x2147};

void jarek_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_short_kick);
  jarek_entry_retail_script_004e5ca0(fighter);
}

void jarek_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &jarek_s_low_kick);
  fighter->action = ACTION_RETAIL_0102;
  jarek_entry_retail_script_004e5d90(fighter);
}

static void jarek_kick_frame(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &jarek_high_kick_record);
  }
}

void jarek_script_kick_finish(Fighter *fighter) {
  native_kick_contact_phase(fighter, jarek_kick_frame, jarek_entry_retail_script_004e5d20);
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

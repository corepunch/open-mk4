#include "../character.h"

static void raiden_uppercut_contact(Fighter *fighter);
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d};

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void raiden_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || raiden_victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

/* Phase branches are ordinary C; blocking playback owns only samples/events. */

void raiden_uppercut_script(Fighter *fighter) {
  native_uppercut_tail(fighter, raiden_uppercut_contact);
}

void raiden_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &raiden_s_uppercut);
  raiden_entry_retail_script_004e62a0(fighter);
}

void raiden_enter_uppercut(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

void raiden_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, raiden_enter_uppercut);
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void liu_kang_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

/* Phase branches are ordinary C; blocking playback owns only samples/events. */

static void uppercut_script(Fighter *fighter) {
  native_uppercut_tail(fighter, liu_kang_uppercut_contact, liu_kang_ready);
}

static void liu_kang_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  fighter_start_fiber(fighter, retail_script_004e62a0, NULL);
}

static void enter_uppercut(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

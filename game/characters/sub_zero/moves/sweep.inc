/* 0x46f080: no gate; queries each visit until a contact. */
static void sub_zero_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

static void sub_zero_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  retail_script_004e6798(fighter);
}

static void enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void sweep_recover(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_recovery);
}

static void script_sweep_tail(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_contact);
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void sonya_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

static void sonya_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e6798, NULL);
}

static void enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, sonya_sweep_contact);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(sonya_ready);
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void liu_kang_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

static void liu_kang_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e67d8, NULL);
}

static void enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void liu_script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, liu_kang_sweep_contact);
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(liu_kang_ready);
}

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void liu_kang_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

static void liu_kang_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter_start_fiber(fighter, retail_script_004e6c90, NULL);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : liu_kang_hk);
}

static void liu_script_kick_tail(Fighter *fighter) {
  native_calls(fighter, 3, 0, liu_kang_kick_contact);
  retail_script_004e5c70(fighter);
}

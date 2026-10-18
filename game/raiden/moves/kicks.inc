/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void raiden_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

static void raiden_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter_start_fiber(fighter, retail_script_004e5ca0, NULL);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? raiden_roundhouse : raiden_hk);
}

static void script_kick_tail(Fighter *fighter) {
  native_calls(fighter, 3, 1, raiden_kick_contact);
  retail_script_004e6998(fighter);
}

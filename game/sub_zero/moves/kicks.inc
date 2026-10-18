/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void sub_zero_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

static void sub_zero_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  retail_script_004e5c80(fighter);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sub_zero_roundhouse : sub_zero_hk);
}

static void kick_contact(Fighter *fighter) {
  fighter->sample_override = fighter_sample(fighter) + 1;
  sub_zero_kick_contact(fighter);
}

static void script_kick_tail(Fighter *fighter) {
  hold_phase(fighter, &kick_contact_phase);
}

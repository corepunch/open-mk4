static void tanya_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void tanya_high_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void tanya_low_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void tanya_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_high_kick);
}

static void tanya_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? tanya_sweep : tanya_low_kick);
}

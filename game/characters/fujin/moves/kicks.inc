static void fujin_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void fujin_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_low_kick);
  fighter_start_fiber(fighter, retail_script_004e5d90, NULL);
}

static void fujin_high_kick(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void fujin_entry_low_kick(Fighter *fighter) {
  fujin_low_kick(fighter);
}

static void fujin_enter_kick(Fighter *fighter) {
  fujin_high_kick(fighter);
}

static void fujin_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_kick);
}

static void fujin_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? fujin_enter_sweep : fujin_low_kick);
}

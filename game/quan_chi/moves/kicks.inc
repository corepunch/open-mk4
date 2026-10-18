static void quan_chi_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void quan_chi_high_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void quan_chi_low_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e5d90);
}

static void press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_high_kick);
}

static void press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? quan_chi_sweep : quan_chi_low_kick);
}

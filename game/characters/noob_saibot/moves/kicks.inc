static void noob_saibot_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void noob_saibot_high_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void noob_saibot_low_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void noob_saibot_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_high_kick);
}

static void noob_saibot_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? noob_saibot_sweep : noob_saibot_low_kick);
}

static void noob_saibot_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_sweep);
}

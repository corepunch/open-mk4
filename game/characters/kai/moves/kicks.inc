static void kai_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_high_kick);
}

static void kai_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47c520);
}

static void kai_high_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void kai_low_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void kai_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

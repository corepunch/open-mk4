static void kai_jab(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void kai_high_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_air_attack, retail_script_004e8210);
}

static void kai_low_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void kai_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47bd10);
}

static void kai_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_low_punch);
}

static void tanya_uppercut(Fighter *fighter) {
  tanya_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void tanya_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_uppercut);
}

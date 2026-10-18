static void kai_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, kai_uppercut);
}

static void kai_uppercut(Fighter *fighter) {
  kai_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

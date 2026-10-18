static void reptile_uppercut(Fighter *fighter) {
  reptile_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void reptile_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_uppercut);
}

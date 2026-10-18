static void goro_uppercut(Fighter *fighter) {
  goro_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void goro_enter_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, goro_uppercut);
}

static void quan_chi_uppercut(Fighter *fighter) {
  quan_chi_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_uppercut);
}

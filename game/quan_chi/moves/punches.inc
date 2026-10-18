static void quan_chi_jab(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void quan_chi_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_jab);
}

static void press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? quan_chi_lp_throw : quan_chi_jab);
}

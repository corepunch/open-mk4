static void jarek_jab(Fighter *fighter) {
  jarek_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void jarek_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
}

static void jarek_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_high_punch);
}

static void jarek_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? jarek_lp_throw : jarek_jab);
}

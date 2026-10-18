static void goro_jab(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void goro_high_punch(Fighter *fighter) {
  goro_jab(fighter);
}

static void goro_enter_jab(Fighter *fighter) {
  fighter_interrupt(fighter, goro_jab);
}

static void goro_enter_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? goro_lp_throw : goro_jab);
}

static void reiko_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void reiko_jab(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void reiko_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_jab);
}

static void reiko_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? reiko_lp_throw : reiko_jab);
}

static void reiko_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_uppercut);
}

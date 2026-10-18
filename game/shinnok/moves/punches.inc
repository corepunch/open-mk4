static void shinnok_jab(Fighter *fighter) {
  shinnok_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void shinnok_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
}

static void shinnok_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_jab);
}

static void shinnok_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? shinnok_lp_throw : shinnok_jab);
}

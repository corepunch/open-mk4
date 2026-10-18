static void fujin_entry_high_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

static void fujin_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_retail_jab);
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void fujin_enter_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

static void fujin_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_punch);
}

static void fujin_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? fujin_enter_throw : fujin_enter_punch);
}

static void reptile_jab(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void reptile_entry_high_punch(Fighter *fighter) {
  reptile_jab(fighter);
}

static void reptile_entry_low_punch(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.7f) {
    reptile_lp_throw(fighter);
  } else {
    reptile_jab(fighter);
  }
}

static void reptile_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_jab);
}

static void reptile_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_entry_low_punch);
}

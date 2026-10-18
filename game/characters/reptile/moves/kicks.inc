static void reptile_high_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void reptile_low_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void reptile_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_high_kick);
}

static void reptile_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reptile_sweep : reptile_low_kick);
}

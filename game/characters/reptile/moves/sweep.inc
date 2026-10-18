static void reptile_sweep(Fighter *fighter) {
  reptile_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void reptile_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_sweep);
}

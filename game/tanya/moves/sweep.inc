static void tanya_sweep(Fighter *fighter) {
  tanya_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void tanya_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_sweep);
}

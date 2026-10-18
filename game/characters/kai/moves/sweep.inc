static void kai_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_478df0);
}

static void kai_sweep(Fighter *fighter) {
  kai_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

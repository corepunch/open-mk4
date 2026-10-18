static void fujin_sweep(Fighter *fighter) {
  fujin_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void fujin_entry_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

static void fujin_enter_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

static void fujin_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_sweep);
}

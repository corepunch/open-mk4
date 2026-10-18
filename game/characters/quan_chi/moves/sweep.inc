static void quan_chi_script_sweep_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_sweep(Fighter *fighter) {
  quan_chi_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_sweep);
}

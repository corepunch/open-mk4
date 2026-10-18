static void jax_sweep(Fighter *fighter) {
  jax_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void jax_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jax_sweep);
}

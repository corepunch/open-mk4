static void jax_uppercut(Fighter *fighter) {
  jax_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void jax_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jax_uppercut);
}

static void jax_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void jax_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    jax_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void jax_command_spike(Fighter *fighter) {
  jax_spike_fatality(fighter);
}

static void kai_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    kai_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void kai_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

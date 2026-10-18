static void cage_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void cage_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

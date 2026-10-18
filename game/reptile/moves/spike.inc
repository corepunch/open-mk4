static void reptile_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void reptile_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void reptile_entry_spike(Fighter *fighter) {
  reptile_spike_fatality(fighter);
}

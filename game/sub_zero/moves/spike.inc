static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_close_fatality(fighter);
}

static void script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void spike_start(Fighter *fighter) {
  native_hold(fighter, maxi(1, 30 - g_battle.phase_ticks), retail_script_004e6950);
}

static void sub_zero_spike_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SPIKE);
}

static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_close_fatality(fighter);
}

static void liu_spike_finish(Fighter *fighter) {
  fatal_pose(fighter, 119, 0, true);
  finish_scene(fighter);
}

static void liu_script_spike_tail(Fighter *fighter) {
  mk4_native_spike_tail(fighter, liu_spike_finish);
}

static void liu_kang_spike_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SPIKE);
}

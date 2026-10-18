static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void fan_start(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void kitana_fan_fatality(Fighter *fighter) {
  arena_fatality(fighter, FINISH_FAN);
}

static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

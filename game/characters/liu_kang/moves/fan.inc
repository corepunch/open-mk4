static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

static void liu_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* 43d530 returns to the bounded native scene coordinator. */
}

static void liu_script_fan_victim_end(Fighter *victim) {
  victim->appearance = APPEARANCE_HIDDEN;
  fighter_fiber_finish(NULL);
}

static void liu_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void liu_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void liu_kang_fan_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_FAN);
}

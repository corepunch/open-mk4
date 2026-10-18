static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  retail_script_004e0148(fighter);
}

static void script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* Repetition is expanded from the generated phase on entry. */
}

static void script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  native_repeat(fighter, &retail_script_004e0138_phase, 31, fan_release);
  native_play(fighter, false);
}

static void fan_start(Fighter *fighter) {
  retail_script_004e0188(opponent(fighter));
  retail_script_004e0118(fighter);
}

static void sub_zero_fan_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_FAN);
}

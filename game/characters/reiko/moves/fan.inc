static void reiko_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void reiko_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void reiko_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void reiko_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void reiko_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

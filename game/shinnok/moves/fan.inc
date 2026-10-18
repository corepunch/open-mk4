static void shinnok_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void shinnok_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void shinnok_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void shinnok_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    shinnok_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, shinnok_fan_start, NULL);
}

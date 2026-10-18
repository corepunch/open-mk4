static void jarek_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void jarek_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void jarek_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void jarek_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    jarek_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, jarek_fan_start, NULL);
}

static bool jarek_fan_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = JAREK_FAN_SCENE;
    battle_event_program(event, 1);
    return true;
  }
  if (event->phase == JAREK_FAN_SCENE) {
    fighter_start_fiber(fighter, retail_script_004e0138, NULL);
    event->phase = JAREK_FAN_FINISH;
    battle_event_program(event, 78);
    return true;
  }
  fighter_start_fiber(fighter, retail_script_004e0148, NULL);
  fighter_start_fiber(victim, retail_script_004e01a0, NULL);
  return false;
}

static void jarek_script_fan_scene(Fighter *fighter) {
  (void)battle_event_start(fighter, jarek_fan_event, false, true);
}

static void jarek_script_fan_victim_scene(Fighter *fighter) {
  Fighter *owner = (Fighter *)fighter_opponent(fighter);
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  mk4_script_scene_pose(victim, 416, 108, 69);
  fighter_start_fiber(owner, retail_script_004e01b0, NULL);
}

static bool jarek_fan_finish_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  (void)event;
  (void)fighter;
  (void)victim;
  battle_end_finisher();
  return false;
}

static bool jarek_fan_finish_pose(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  (void)event;
  (void)fighter;
  mk4_script_scene_pose(victim, 416, 108, 69);
  return g_battle.phase == PHASE_FATALITY;
}

static void jarek_script_fan_finish(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jarek_fan_finish_event, false, true);
  battle_event_program(event, 103);
  battle_event_frame(event, 0, 0, jarek_fan_finish_pose);
}

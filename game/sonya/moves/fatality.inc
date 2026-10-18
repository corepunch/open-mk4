static void arena_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_scene_pose(fighter, id == FINISH_FAN ? 415 : 293, id == FINISH_FAN ? 130 : 20, 0);
  mk4_script_scene_pose(opponent(fighter), id == FINISH_FAN ? 416 : 13, id == FINISH_FAN ? 108 : 56, 0);
  fighter_start_fiber(fighter, id == FINISH_FAN ? fan_start : spike_start, NULL);
}

/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
static void scene_end_watch(Fighter *fighter) {
  int clip = fighter_move(fighter)->clip;
  if (g_battle.phase != PHASE_FATALITY) {
    fighter->body.physics = NULL;
    return;
  }
  if (fighter->sample_override == fighter_move(fighter)->last && (clip == 573 || clip == 388 || clip == 114)) {
    fighter->body.physics = NULL;
    battle_end_finisher();
  }
}

static void script_victory(Fighter *fighter) {
  retail_script_004e6a40(fighter);
}

static void enter_victory(Fighter *fighter) {
  fighter_start_move(fighter, S_VICTORY, &s_victory);
  fighter_start_fiber(fighter, script_victory, NULL);
}

static bool can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

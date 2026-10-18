static void script_first_release(Fighter *fighter) {
  battle_shatter(opponent(fighter));
  fighter->body.physics = scene_end_watch;
  retail_script_004e01d0(fighter);
}

static void first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_KITANA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 129, 0);
  mk4_script_scene_pose(opponent(fighter), 389, 1, 0);
  fighter_start_fiber(fighter, retail_script_004e01c0, NULL);
}

static bool can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

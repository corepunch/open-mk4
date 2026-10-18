static void script_second_blend(Fighter *fighter) {
  mk4_script_blend_to(fighter, 123, 101, 100, 0xccc, 0x1999);
  retail_script_004e56d8(fighter);
}

static void script_second_particles(Fighter *fighter) {
  battle_particles(fighter, 10, 8, true);
}

static void script_second_effect(Fighter *fighter) {
  battle_particles(opponent(fighter), 18, 12, true);
  retail_script_004e56f0(fighter);
}

static void script_second_release(Fighter *fighter) {
  battle_particles(opponent(fighter), 2, 24, true);
  retail_script_004e5740(fighter);
}

static void script_second_end(Fighter *fighter) {
  fighter->body.physics = scene_end_watch;
  retail_script_004e5760(fighter);
}

static void script_second_victim_end(Fighter *fighter) {
  battle_particles(fighter, 2, 24, true);
  retail_script_004e5788(fighter);
}

static void second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 123, 101, 0);
  mk4_script_scene_pose(opponent(fighter), 13, 56, 0);
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14000);
  }
  fighter_start_fiber(fighter, retail_script_004e56c8, NULL);
}

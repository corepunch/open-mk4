#include "../character.h"

void raiden_script_second_blend(Fighter *fighter) {
  mk4_mmove_blend_to(fighter, 123, 101, 100, 0xccc, 0x1999, raiden_entry_retail_script_004e56d8);
}

void raiden_script_second_particles(Fighter *fighter) {
  battle_particles(fighter, 10, 8, true);
}

void raiden_script_second_effect(Fighter *fighter) {
  battle_particles(raiden_opponent(fighter), 18, 12, true);
  raiden_entry_retail_script_004e56f0(fighter);
}

void raiden_script_second_release(Fighter *fighter) {
  battle_particles(raiden_opponent(fighter), 2, 24, true);
  raiden_entry_retail_script_004e5740(fighter);
}

void raiden_script_second_end(Fighter *fighter) {
  fighter->body.physics = raiden_scene_end_watch;
  raiden_entry_retail_script_004e5760(fighter);
}

void raiden_script_second_victim_end(Fighter *fighter) {
  battle_particles(fighter, 2, 24, true);
  raiden_entry_retail_script_004e5788(fighter);
}

void raiden_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 123, 101, 0);
  mk4_script_scene_pose(raiden_opponent(fighter), 13, 56, 0);
  Fighter *victim = raiden_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14000);
  }
  raiden_entry_retail_script_004e56c8(fighter);
}

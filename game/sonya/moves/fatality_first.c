#include "../character.h"

void sonya_script_first_release(Fighter *fighter) {
  battle_shatter(sonya_opponent(fighter));
  fighter->body.physics = sonya_scene_end_watch;
  sonya_entry_retail_script_004e01d0(fighter);
}

void sonya_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 129, 0);
  mk4_script_scene_pose(sonya_opponent(fighter), 389, 1, 0);
  fighter_start_fiber(fighter, sonya_entry_retail_script_004e01c0, NULL);
}

bool sonya_can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

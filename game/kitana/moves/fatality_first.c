#include "../character.h"

void kitana_script_first_release(Fighter *fighter) {
  battle_shatter(kitana_opponent(fighter));
  fighter->body.physics = kitana_scene_end_watch;
  kitana_entry_retail_script_004e01d0(fighter);
}

void kitana_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_KITANA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 129, 0);
  mk4_script_scene_pose(kitana_opponent(fighter), 389, 1, 0);
  kitana_entry_retail_script_004e01c0(fighter);
}

bool kitana_can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

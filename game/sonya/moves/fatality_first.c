#include "retail_vm.h"
#include "../character.h"

void sonya_script_first_release(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  fighter->body.physics = sonya_scene_end_watch;
  retail_vm_enter_owned(fighter, 0x4e01d0, fighter_ready, 2);
}

void sonya_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 389, 0);
  retail_vm_enter_owned(fighter, 0x4e01c0, fighter_ready, 2);
}

bool sonya_can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

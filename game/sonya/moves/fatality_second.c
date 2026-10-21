#include "retail_vm.h"
#include "../character.h"

/* 465ed0 holds 15 visits before returning to the two-cycle coordinator. */
void sonya_script_second_pause(Fighter *fighter) {
  native_hold(fighter, 15, sonya_script_second_cycle_end);
}

/* 466000 returns to the native coordinator: script_phase counts the cycles
 * that remain, and the result pose follows the second. */
void sonya_script_second_cycle_end(Fighter *fighter) {
  if (--fighter->script_phase > 0) {
    retail_vm_enter_owned(fighter, 0x4e5960, fighter_ready, 2);
    return;
  }
  fighter->body.physics = sonya_scene_end_watch;
  retail_vm_enter_owned(fighter, 0x4e59a8, fighter_ready, 2);
}

void sonya_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    retail_vm_enter_owned(fighter, 0x4e5980, fighter_ready, 2);
  } else {
    retail_vm_enter_owned(fighter, 0x4e5990, fighter_ready, 2);
  }
}

void sonya_script_second_repeat(Fighter *fighter) {
  fighter->script_phase = 2;
  retail_vm_enter_owned(fighter, 0x4e5960, fighter_ready, 2);
}

void sonya_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  retail_vm_enter_owned(fighter, 0x4e5940, fighter_ready, 2);
}

void sonya_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  retail_vm_enter_owned(victim, 0x4e5a20, fighter_ready, 2);
}

void sonya_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 558, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 13, 0);
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  retail_vm_enter_owned(fighter, 0x4e5930, fighter_ready, 2);
}

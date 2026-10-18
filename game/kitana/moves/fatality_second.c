#include "../character.h"

void kitana_script_second_pause(Fighter *fighter) {
  (void)fighter;
  native_delay(fighter, 15);
}

void kitana_script_second_cycle_end(Fighter *fighter) {
  (void)fighter; /* 466000 returns to the native two-cycle coordinator. */
}

void kitana_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    kitana_entry_retail_script_004e5980(fighter);
  } else {
    kitana_entry_retail_script_004e5990(fighter);
  }
}

void kitana_script_second_repeat(Fighter *fighter) {
  for (int remaining = 2; remaining > 0; --remaining) {
    fighter->script_phase = remaining;
    kitana_entry_retail_script_004e5960(fighter);
  }
  fighter->body.physics = kitana_scene_end_watch;
  kitana_entry_retail_script_004e59a8(fighter);
}

void kitana_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  kitana_entry_retail_script_004e5940(fighter);
}

void kitana_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  kitana_entry_retail_script_004e5a20(victim);
}

void kitana_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_KITANA_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 558, 16, 0);
  mk4_script_scene_pose(kitana_opponent(fighter), 13, 56, 0);
  Fighter *victim = kitana_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  fighter_start_fiber(fighter, kitana_entry_retail_script_004e5930, NULL);
}

bool kitana_can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

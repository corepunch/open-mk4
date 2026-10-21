#include "../character.h"

/* 465ed0 holds 15 visits before returning to the two-cycle coordinator. */
void sonya_script_second_pause(Fighter *fighter) {
  native_hold(fighter, 15, sonya_script_second_cycle_end);
}

/* 466000 returns to the native coordinator: script_phase counts the cycles
 * that remain, and the result pose follows the second. */
void sonya_script_second_cycle_end(Fighter *fighter) {
  if (--fighter->script_phase > 0) {
    sonya_entry_retail_script_004e5960(fighter);
    return;
  }
  fighter->body.physics = sonya_scene_end_watch;
  sonya_entry_retail_script_004e59a8(fighter);
}

void sonya_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    sonya_entry_retail_script_004e5980(fighter);
  } else {
    sonya_entry_retail_script_004e5990(fighter);
  }
}

void sonya_script_second_repeat(Fighter *fighter) {
  fighter->script_phase = 2;
  sonya_entry_retail_script_004e5960(fighter);
}

void sonya_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  sonya_entry_retail_script_004e5940(fighter);
}

void sonya_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  sonya_entry_retail_script_004e5a20(victim);
}

void sonya_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 558, 16, 0);
  mk4_script_scene_pose(sonya_opponent(fighter), 13, 56, 0);
  Fighter *victim = sonya_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  sonya_entry_retail_script_004e5930(fighter);
}

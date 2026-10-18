#include "../character.h"

static void jax_fan_start(Fighter *fighter);
static void jax_fan_fatality(Fighter *fighter);
static void jax_command_fan(Fighter *fighter);

static void jax_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, jax_entry_retail_script_004e0188, NULL);
  jax_entry_retail_script_004e0118(fighter);
}

void jax_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, jax_retail_script_004e0138_repeat_phase(), jax_entry_retail_script_004e0148, jax_entry_retail_script_004e01a0);
}

static void jax_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, jax_fan_start, NULL);
}

static void jax_command_fan(Fighter *fighter) {
  jax_fan_fatality(fighter);
}

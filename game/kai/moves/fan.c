#include "../character.h"

static void kai_fan_start(Fighter *fighter);
void kai_fan_fatality(Fighter *fighter);

static void kai_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, kai_entry_retail_script_004e0188, NULL);
  kai_entry_retail_script_004e0118(fighter);
}

void kai_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    kai_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, kai_fan_start, NULL);
}

void kai_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, kai_retail_script_004e0138_repeat_phase(), kai_entry_retail_script_004e0148, kai_entry_retail_script_004e01a0);
}

void kai_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void kai_script_fan_victory(Fighter *fighter) {
  kai_entry_retail_script_004e0158(fighter);
}

void kai_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

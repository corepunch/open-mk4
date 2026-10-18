#include "../character.h"

void goro_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void goro_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void goro_script_fan_victory(Fighter *fighter) {
  goro_entry_retail_script_004e0158(fighter);
}

void goro_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, goro_retail_script_004e0138_repeat_phase(), goro_entry_retail_script_004e0148, goro_entry_retail_script_004e01a0);
}

void goro_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, goro_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, goro_entry_retail_script_004e0118, NULL);
}

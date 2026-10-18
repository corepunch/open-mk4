#include "../character.h"

void sonya_script_fan_victory(Fighter *fighter) {
  sonya_entry_retail_script_004e0158(fighter);
}

void sonya_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, sonya_retail_script_004e0138_repeat_phase(), sonya_entry_retail_script_004e0148, sonya_entry_retail_script_004e01a0);
}

void sonya_fan_start(Fighter *fighter) {
  fighter_start_fiber(sonya_opponent(fighter), sonya_entry_retail_script_004e0188, NULL);
  sonya_entry_retail_script_004e0118(fighter);
}

void sonya_fan_fatality(Fighter *fighter) {
  sonya_arena_fatality(fighter, FINISH_FAN);
}

bool sonya_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && sonya_can_close_fatality(fighter);
}

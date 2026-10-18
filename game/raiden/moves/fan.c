#include "../character.h"

void raiden_script_fan_victory(Fighter *fighter) {
  raiden_entry_retail_script_004e0158(fighter);
}

void raiden_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, raiden_retail_script_004e0138_repeat_phase(), raiden_entry_retail_script_004e0148, raiden_entry_retail_script_004e01a0);
}

void raiden_fan_start(Fighter *fighter) {
  fighter_start_fiber(raiden_opponent(fighter), raiden_entry_retail_script_004e0188, NULL);
  raiden_entry_retail_script_004e0118(fighter);
}

void raiden_fan_fatality(Fighter *fighter) {
  raiden_arena_fatality(fighter, FINISH_FAN);
}

bool raiden_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && raiden_can_close_fatality(fighter);
}

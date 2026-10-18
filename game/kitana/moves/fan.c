#include "../character.h"

void kitana_script_fan_victory(Fighter *fighter) {
  kitana_entry_retail_script_004e0158(fighter);
}

void kitana_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, kitana_retail_script_004e0138_repeat_phase(), kitana_entry_retail_script_004e0148, kitana_entry_retail_script_004e01a0);
}

void kitana_fan_start(Fighter *fighter) {
  fighter_start_fiber(kitana_opponent(fighter), kitana_entry_retail_script_004e0188, NULL);
  kitana_entry_retail_script_004e0118(fighter);
}

void kitana_fan_fatality(Fighter *fighter) {
  kitana_arena_fatality(fighter, FINISH_FAN);
}

bool kitana_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && kitana_can_close_fatality(fighter);
}

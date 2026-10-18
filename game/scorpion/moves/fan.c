#include "../character.h"

bool scorpion_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && scorpion_can_beast_fatality(fighter);
}

void scorpion_fan_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_FAN);
}

void scorpion_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void scorpion_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void scorpion_script_fan_victory(Fighter *fighter) {
  scorpion_entry_retail_script_004e0158(fighter);
}

void scorpion_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, scorpion_retail_script_004e0138_repeat_phase(), scorpion_entry_retail_script_004e0148, scorpion_entry_retail_script_004e01a0);
}

void scorpion_fan_start(Fighter *fighter) {
  fighter_start_fiber(scorpion_opponent(fighter), scorpion_entry_retail_script_004e0188, NULL);
  scorpion_entry_retail_script_004e0118(fighter);
}

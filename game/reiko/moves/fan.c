#include "../character.h"

void reiko_fan_fatality(Fighter *fighter);

void reiko_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void reiko_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void reiko_script_fan_victory(Fighter *fighter) {
  reiko_entry_retail_script_004e0158(fighter);
}

void reiko_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, reiko_retail_script_004e0138_repeat_phase(), reiko_entry_retail_script_004e0148, reiko_entry_retail_script_004e01a0);
}

void reiko_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, reiko_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, reiko_entry_retail_script_004e0118, NULL);
}

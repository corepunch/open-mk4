#include "../character.h"

void tanya_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void tanya_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void tanya_script_fan_victory(Fighter *fighter) {
  tanya_entry_retail_script_004e0158(fighter);
}

void tanya_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, tanya_retail_script_004e0138_repeat_phase(), tanya_entry_retail_script_004e0148, tanya_entry_retail_script_004e01a0);
}

void tanya_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, tanya_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, tanya_entry_retail_script_004e0118, NULL);
}

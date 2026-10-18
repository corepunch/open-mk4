#include "../character.h"

void quan_chi_fan_fatality(Fighter *fighter);

void quan_chi_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void quan_chi_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void quan_chi_script_fan_victory(Fighter *fighter) {
  quan_chi_entry_retail_script_004e0158(fighter);
}

void quan_chi_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, quan_chi_retail_script_004e0138_repeat_phase(), quan_chi_entry_retail_script_004e0148, quan_chi_entry_retail_script_004e01a0);
}

void quan_chi_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, quan_chi_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, quan_chi_entry_retail_script_004e0118, NULL);
}

#include "../character.h"

void quan_chi_script_fan_cycle(Fighter *fighter);
void quan_chi_script_fan_victim_end(Fighter *fighter);
void quan_chi_script_fan_victory(Fighter *fighter);
void quan_chi_fan_fatality(Fighter *fighter);

void quan_chi_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void quan_chi_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void quan_chi_script_fan_victory(Fighter *fighter) {
  quan_chi_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  quan_chi_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  quan_chi_entry_retail_script_004e0148(fighter);
}

void quan_chi_script_fan_tail(Fighter *fighter) {
  quan_chi_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, quan_chi_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void quan_chi_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  quan_chi_entry_retail_script_004e0188(victim);
  quan_chi_entry_retail_script_004e0118(fighter);
}

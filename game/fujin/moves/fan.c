#include "../character.h"

void fujin_script_fan_cycle(Fighter *fighter);
void fujin_script_fan_victim_end(Fighter *fighter);
void fujin_script_fan_victory(Fighter *fighter);
static void fujin_fan_fatality(Fighter *fighter);
void fujin_entry_fan(Fighter *fighter);

void fujin_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void fujin_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void fujin_script_fan_victory(Fighter *fighter) {
  fujin_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  fujin_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  fujin_entry_retail_script_004e0148(fighter);
}

void fujin_script_fan_tail(Fighter *fighter) {
  fujin_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, fujin_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

static void fujin_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fujin_entry_retail_script_004e0188(victim);
  fujin_entry_retail_script_004e0118(fighter);
}

void fujin_entry_fan(Fighter *fighter) {
  fujin_fan_fatality(fighter);
}

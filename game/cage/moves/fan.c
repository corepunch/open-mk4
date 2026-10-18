#include "../character.h"

void cage_script_fan_cycle(Fighter *fighter);
void cage_script_fan_victim_end(Fighter *fighter);
void cage_script_fan_victory(Fighter *fighter);
void cage_fan_fatality(Fighter *fighter);

void cage_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void cage_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void cage_script_fan_victory(Fighter *fighter) {
  cage_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  cage_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  cage_entry_retail_script_004e0148(fighter);
}

void cage_script_fan_tail(Fighter *fighter) {
  cage_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, cage_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void cage_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  cage_entry_retail_script_004e0188(victim);
  cage_entry_retail_script_004e0118(fighter);
}

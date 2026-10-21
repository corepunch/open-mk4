#include "../character.h"

static void fan_release(Fighter *fighter);

void kitana_script_fan_victory(Fighter *fighter) {
  kitana_entry_retail_script_004e0158(fighter);
}

void kitana_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = kitana_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  kitana_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  kitana_entry_retail_script_004e0148(fighter);
}

void kitana_script_fan_tail(Fighter *fighter) {
  kitana_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, kitana_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void kitana_fan_start(Fighter *fighter) {
  kitana_entry_retail_script_004e0188(kitana_opponent(fighter));
  kitana_entry_retail_script_004e0118(fighter);
}

void kitana_fan_fatality(Fighter *fighter) {
  kitana_arena_fatality(fighter, FINISH_FAN);
}

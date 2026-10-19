#include "../character.h"

static void fan_release(Fighter *fighter);

void raiden_script_fan_victory(Fighter *fighter) {
  raiden_entry_retail_script_004e0158(fighter);
}

void raiden_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = raiden_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  raiden_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  raiden_entry_retail_script_004e0148(fighter);
}

void raiden_script_fan_tail(Fighter *fighter) {
  raiden_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, raiden_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void raiden_fan_start(Fighter *fighter) {
  raiden_entry_retail_script_004e0188(raiden_opponent(fighter));
  raiden_entry_retail_script_004e0118(fighter);
}

void raiden_fan_fatality(Fighter *fighter) {
  raiden_arena_fatality(fighter, FINISH_FAN);
}

bool raiden_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && raiden_can_close_fatality(fighter);
}

#include "../character.h"

static void fan_release(Fighter *fighter);

bool liu_kang_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && liu_kang_can_close_fatality(fighter);
}

void liu_kang_liu_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* 43d530 returns to the bounded native scene coordinator. */
}

void liu_kang_liu_script_fan_victim_end(Fighter *victim) {
  victim->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(victim, NULL);
}

void liu_kang_liu_script_fan_victory(Fighter *fighter) {
  liu_kang_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = liu_kang_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  liu_kang_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  liu_kang_entry_retail_script_004e0148(fighter);
}

void liu_kang_liu_script_fan_tail(Fighter *fighter) {
  liu_kang_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, liu_kang_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void liu_kang_fan_fatality(Fighter *fighter) {
  liu_kang_start_fatality(fighter, FINISH_FAN);
}

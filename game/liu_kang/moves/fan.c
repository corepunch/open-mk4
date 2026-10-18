#include "../character.h"

bool liu_kang_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && liu_kang_can_close_fatality(fighter);
}

void liu_kang_liu_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* 43d530 returns to the bounded native scene coordinator. */
}

void liu_kang_liu_script_fan_victim_end(Fighter *victim) {
  victim->appearance = APPEARANCE_HIDDEN;
  fighter_fiber_finish(NULL);
}

void liu_kang_liu_script_fan_victory(Fighter *fighter) {
  liu_kang_entry_retail_script_004e0158(fighter);
}

void liu_kang_liu_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, liu_kang_retail_script_004e0138_repeat_phase(), liu_kang_entry_retail_script_004e0148, liu_kang_entry_retail_script_004e01a0);
}

void liu_kang_fan_fatality(Fighter *fighter) {
  liu_kang_start_fatality(fighter, FINISH_FAN);
}

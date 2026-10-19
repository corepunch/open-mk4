#include "../character.h"

void goro_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

/* 43db50 hides the fan victim and retires its scene stream. */
void goro_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void goro_script_fan_victory(Fighter *fighter) {
  goro_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  goro_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  goro_entry_retail_script_004e0148(fighter);
}

/* 43d460 repeats the owner's 4e0138 phase 31 times before the release. */
void goro_script_fan_tail(Fighter *fighter) {
  goro_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, goro_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

/* 467e00 is roster-generic; Goro reaches it from the CPU stage-8 finish
 * (434520 -> 434940 -> action 10041), not from a command record. */
void goro_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  goro_entry_retail_script_004e0188(victim);
  goro_entry_retail_script_004e0118(fighter);
}

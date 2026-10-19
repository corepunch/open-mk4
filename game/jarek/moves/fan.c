#include "../character.h"

void jarek_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void jarek_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void jarek_script_fan_victory(Fighter *fighter) {
  jarek_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  jarek_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  /* 43d559 rebinds owner clip 415 at pose 69 before the 4e0148 release. */
  mk4_mmove_bind(fighter, 415, 130, 69);
  jarek_entry_retail_script_004e0148(fighter);
}

/* 43d460 -> 43d4b0 repeats until counter 534694 (31, from 43d398) ends.
 * Each pass 474e70 tests the peer's roster: female slots 2 and 9 (Kitana
 * uses 2) enter 4e0138 on clip 415; every other victim makes 43d4cf bind
 * clip 414 at pose 0 and enter 4e0128. Both tails reach 43d540. */
void jarek_script_fan_tail(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  const mmove_t *cycle;
  if (roster == 2 || roster == 9) {
    jarek_entry_retail_script_004e0138(fighter);
    cycle = jarek_retail_script_004e0138_repeat_phase();
  } else {
    mk4_mmove_bind(fighter, 414, 9, 0);
    jarek_entry_retail_script_004e0128(fighter);
    cycle = jarek_retail_script_004e0128_repeat_phase();
  }
  native_repeat(fighter, cycle, 31, fan_release);
  native_play(fighter, false);
}

void jarek_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  jarek_entry_retail_script_004e0188(victim);
  jarek_entry_retail_script_004e0118(fighter);
}

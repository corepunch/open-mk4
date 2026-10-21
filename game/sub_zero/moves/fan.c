#include "../character.h"

static void fan_release(Fighter *fighter);

static void fan_release(Fighter *fighter) {
  Fighter *victim = sub_zero_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  sub_zero_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  sub_zero_entry_retail_script_004e0148(fighter);
}

void sub_zero_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* Repetition is expanded from the generated phase on entry. */
}

void sub_zero_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void sub_zero_script_fan_victory(Fighter *fighter) {
  sub_zero_entry_retail_script_004e0158(fighter);
}

void sub_zero_script_fan_tail(Fighter *fighter) {
  native_repeat(fighter, sub_zero_retail_script_004e0138_phase(), 31, fan_release);
  native_play(fighter, false);
}

void sub_zero_fan_start(Fighter *fighter) {
  sub_zero_entry_retail_script_004e0188(sub_zero_opponent(fighter));
  sub_zero_entry_retail_script_004e0118(fighter);
}

void sub_zero_fan_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_FAN);
}

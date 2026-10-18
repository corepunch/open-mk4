#include "../character.h"

void jax_script_fan_cycle(Fighter *fighter);
void jax_script_fan_victim_end(Fighter *fighter);
void jax_script_fan_victory(Fighter *fighter);
void jax_fan_fatality(Fighter *fighter);

void jax_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void jax_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void jax_script_fan_victory(Fighter *fighter) {
  jax_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  jax_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  jax_entry_retail_script_004e0148(fighter);
}

void jax_script_fan_tail(Fighter *fighter) {
  jax_entry_retail_script_004e0138(fighter);
  native_repeat(fighter, jax_retail_script_004e0138_repeat_phase(), 31, fan_release);
  native_play(fighter, false);
}

void jax_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  jax_entry_retail_script_004e0188(victim);
  jax_entry_retail_script_004e0118(fighter);
}

void jax_command_fan(Fighter *fighter) {
  jax_fan_fatality(fighter);
}

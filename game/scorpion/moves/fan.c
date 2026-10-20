#include "../character.h"

static void fan_release(Fighter *fighter);

bool scorpion_can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && scorpion_can_beast_fatality(fighter);
}

void scorpion_fan_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_FAN);
}

void scorpion_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
  if (--fighter->native_variant > 0) {
    scorpion_entry_retail_script_004e0138(fighter);
  } else {
    fan_release(fighter);
  }
}

void scorpion_script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

void scorpion_script_fan_victory(Fighter *fighter) {
  scorpion_entry_retail_script_004e0158(fighter);
}

static void fan_release(Fighter *fighter) {
  Fighter *victim = scorpion_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  scorpion_entry_retail_script_004e01a0(victim);
  mk4_native_fan_rise(fighter);
  scorpion_entry_retail_script_004e0148(fighter);
}

void scorpion_script_fan_tail(Fighter *fighter) {
  fighter->native_variant = 31;
  scorpion_entry_retail_script_004e0138(fighter);
}

void scorpion_fan_start(Fighter *fighter) {
  scorpion_entry_retail_script_004e0188(scorpion_opponent(fighter));
  scorpion_entry_retail_script_004e0118(fighter);
}

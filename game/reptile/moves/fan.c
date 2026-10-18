#include "../character.h"

static void reptile_fan_fatality(Fighter *fighter);
void reptile_entry_fan(Fighter *fighter);

void reptile_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void reptile_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void reptile_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, reptile_retail_script_004e0138_repeat_phase(), reptile_entry_retail_script_004e0148, reptile_entry_retail_script_004e01a0);
}

static void reptile_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, reptile_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, reptile_entry_retail_script_004e0118, NULL);
}

void reptile_entry_fan(Fighter *fighter) {
  reptile_fan_fatality(fighter);
}

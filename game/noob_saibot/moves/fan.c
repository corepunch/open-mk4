#include "../character.h"

void noob_saibot_fan_fatality(Fighter *fighter);

void noob_saibot_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

void noob_saibot_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

void noob_saibot_script_fan_victory(Fighter *fighter) {
  noob_saibot_entry_retail_script_004e0158(fighter);
}

void noob_saibot_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(
      fighter, noob_saibot_retail_script_004e0138_repeat_phase(), noob_saibot_entry_retail_script_004e0148, noob_saibot_entry_retail_script_004e01a0);
}

void noob_saibot_fan_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    return;
  }

  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, noob_saibot_entry_retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, noob_saibot_entry_retail_script_004e0118, NULL);
}

#include "../character.h"

/* 465ed0 holds 15 visits before returning to the two-cycle coordinator. */
void kitana_script_second_pause(Fighter *fighter) {
  native_hold(fighter, 15, kitana_script_second_cycle_end);
}

/* 466000 returns to the native coordinator: script_phase counts the cycles
 * that remain, and the result pose follows the second. */
void kitana_script_second_cycle_end(Fighter *fighter) {
  if (--fighter->script_phase > 0) {
    kitana_entry_retail_script_004e5960(fighter);
    return;
  }
  fighter->body.physics = kitana_scene_end_watch;
  kitana_entry_retail_script_004e59a8(fighter);
}

void kitana_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    kitana_entry_retail_script_004e5980(fighter);
  } else {
    kitana_entry_retail_script_004e5990(fighter);
  }
}

void kitana_script_second_repeat(Fighter *fighter) {
  fighter->script_phase = 2;
  kitana_entry_retail_script_004e5960(fighter);
}

void kitana_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  kitana_entry_retail_script_004e5940(fighter);
}

void kitana_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  kitana_entry_retail_script_004e5a20(victim);
}

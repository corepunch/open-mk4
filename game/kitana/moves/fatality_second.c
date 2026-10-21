#include "retail_vm.h"
#include "../character.h"

/* 465ed0 holds 15 visits before returning to the two-cycle coordinator. */
void kitana_script_second_pause(Fighter *fighter) {
  native_hold(fighter, 15, kitana_script_second_cycle_end);
}

/* 466000 returns to the native coordinator: script_phase counts the cycles
 * that remain, and the result pose follows the second. */
void kitana_script_second_cycle_end(Fighter *fighter) {
  if (--fighter->script_phase > 0) {
    retail_vm_enter_owned(fighter, 0x4e5960, fighter_ready, 16);
    return;
  }
  fighter->body.physics = kitana_scene_end_watch;
  retail_vm_enter_owned(fighter, 0x4e59a8, fighter_ready, 16);
}

void kitana_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    retail_vm_enter_owned(fighter, 0x4e5980, fighter_ready, 16);
  } else {
    retail_vm_enter_owned(fighter, 0x4e5990, fighter_ready, 16);
  }
}

void kitana_script_second_repeat(Fighter *fighter) {
  fighter->script_phase = 2;
  retail_vm_enter_owned(fighter, 0x4e5960, fighter_ready, 16);
}

void kitana_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  retail_vm_enter_owned(fighter, 0x4e5940, fighter_ready, 16);
}

void kitana_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  retail_vm_enter_owned(victim, 0x4e5a20, fighter_ready, 16);
}

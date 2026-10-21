#include "retail_vm.h"
#include "../character.h"

void retail_weapon_native_496720(Fighter *fighter);
static const MoveDef s_weapon_back_hp = {.clip = 228, .last = 39, .flags = RETAIL, .action = 0x112};
static const AttackRecord weapon_back_hp_record = {98959, -28835, 67502, 36700, 55, 55, 119, 0, 13107};
static const AttackRecord weapon_back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};

void sonya_script_weapon_hp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_hp_record, 7);
}

void sonya_script_weapon_lp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_lp_record, 2);
}

void retail_weapon_native_496720(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_weapon_back_hp);
  mk4_script_schedule_motion(fighter, 0x1999, 3, 12);
  fighter->mmove_cleanup = sonya_special_cleanup;
  retail_vm_enter_owned(fighter, 0x4ec7a0, fighter_ready, 2);
}

void sonya_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_roll);
}

#include "retail_vm.h"
#include "../character.h"

static const AttackRecord weapon_back_hp_record = {98959, -28835, 67502, 36700, 55, 55, 119, 0, 13107};
static const AttackRecord weapon_back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};

void kitana_script_weapon_hp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_hp_record, 7);
}

void kitana_script_weapon_lp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_lp_record, 2);
}

void kitana_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_roll);
}

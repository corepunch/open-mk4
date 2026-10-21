#include "retail_vm.h"
#include "../character.h"

static const MoveDef upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x113};
static const MoveDef victim = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
static const AttackRecord upper_record = {98959, -45809, 67502, 28835, 146, 10, 119, 0, 11796};
static const AttackRecord back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};

static void weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

void retail_weapon_native_495860(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &upper);
  fighter->mmove_cleanup = weapon_cleanup;
  native_schedule_speed(fighter, 0x1999, 7, 13);
  retail_vm_enter_owned(fighter, 0x4ec4f0, fighter_ready, 14);
}

void cage_weapon_upper_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 226 && fighter_sample(fighter) == 29, "Cage upper weapon contact needs its selected VM startup");
  weapon_script_contact_tail(fighter, &upper_record, 2);
}

void cage_weapon_back_lp_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 227 && fighter_sample(fighter) == 29, "Cage back LP contact needs its selected VM startup");
  weapon_script_contact_tail(fighter, &back_lp_record, 2);
}

void cage_weapon_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}

void cage_weapon_reaction(Fighter *fighter) {
  require(fighter->reaction == 63 || fighter->reaction == 139 || fighter->reaction == 149, "Invalid Cage weapon victim reaction");
  if (fighter->reaction == 149) {
    fighter->damage_scale = 0x8000;
  }
  battle_wall_recoil(fighter);
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &victim);
  fighter->flags |= FF_REACTING;
  if (fighter->reaction == 149) {
    retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 14);
  } else {
    retail_vm_enter_owned(fighter, 0x4e8d38, mk4_mmove_thrown_finish, 14);
  }
}

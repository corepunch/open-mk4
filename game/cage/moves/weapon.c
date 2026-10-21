#include "native_services.h"

static const MoveDef upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x113};
static const MoveDef victim = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};

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

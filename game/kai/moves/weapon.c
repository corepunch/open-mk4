#include "retail_vm.h"
#include "../character.h"

void retail_weapon_native_495a90(Fighter *fighter);
static void weapon_cleanup(Fighter *fighter);
static const MoveDef back_move = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x10e};
static const MoveDef victim_move = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
static const AttackRecord back_record = {98959, -45809, 67502, 28835, 146, 10, 119, 0, 11796};

static void weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

void retail_weapon_native_495a90(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &back_move);
  fighter->mmove_cleanup = weapon_cleanup;
  /* 495a90 /4ec598 installs delayed travel before the selected VM stream. */
  native_schedule_speed(fighter, 0x1999, 7, 13);
  retail_vm_enter_owned(fighter, 0x4ec5a8, mk4_mmove_thrown_finish, 11);
}

void kai_weapon_back_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 226 && fighter_sample(fighter) == 29, "Kai back weapon contact needs its selected VM startup");
  weapon_script_contact_tail(fighter, &back_record, 2);
}

/* 4ecb40[type63-60] selects4959f0 ->4eca08/4eca18. These use
 * the HP strike data directly, bypassing ordinary air/back input selection. */

void kai_weapon_victim_getup(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}

void kai_weapon_reaction(Fighter *fighter) {
  int reaction = fighter->reaction;
  bool airborne = fighter_airborne(fighter);
  require(reaction == 142 || reaction == 145 || reaction == 146 || reaction == 44, "Invalid Kai weapon reaction");
  mk4_script_disarm_self(fighter);
  if (reaction != 146) {
    mk4_script_indexed_sound(fighter, 6);
    battle_wall_recoil(fighter);
  }
  fighter_start_move(fighter, S_KNOCKDOWN, &victim_move);
  fighter->flags |= FF_REACTING;
  if (reaction == 142) {
    /* 48fed0 swaps to the attacking peer, then48fee0 positions its
     * opponent (this victim) 1.8 units away and half a unit above the peer. */
    const Fighter *peer = fighter_opponent(fighter);
    fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + q16_mul(fighter_axis(peer, AXIS_X), 0x1cccc);
    fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + q16_mul(fighter_axis(peer, AXIS_Z), 0x1cccc);
    fighter->body.pos[AXIS_Y] = peer->body.pos[AXIS_Y] - 0x8000;
    retail_vm_enter_owned(fighter, 0x4e8b78, mk4_mmove_thrown_finish, 11);
  } else if (reaction == 145 || reaction == 44) {
    if (airborne) {
      fighter->body.gravity = 0x28f;
      fighter->body.vel[AXIS_Y] = 0x1999;
    }
    retail_vm_enter_owned(fighter, 0x4e8f50, mk4_mmove_thrown_finish, 11);
  } else {
    retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 11);
  }
}

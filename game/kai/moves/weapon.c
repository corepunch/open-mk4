#include "retail_vm.h"
#include "../character.h"

static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back(Fighter *fighter);
static void weapon_cleanup(Fighter *fighter);
static const MoveDef back_move = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x10e};
static const MoveDef victim_move = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 230, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {124518, -34078, 83230, 51118, 142, 142, 119, 0, 11796}, 0, 8, 11, 3, 1, 1, 10, 0x2147, 34},
  {{.clip = 229, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {124518, -34078, 83230, 51118, 145, 145, 119, 0, 10485}, 0, 4, 7, 3, 1, 1, 8, 0x1999, 32},
};
// clang-format on
static const AttackRecord back_record = {98959, -45809, 67502, 28835, 146, 10, 119, 0, 11796};
static const WeaponDef native_weapon = {0x63, weapon_hp, weapon_lp, weapon_back, weapon_back, NULL, weapon_throw, equip_weapon, SCENE_MODEL_KAI_WEAPON};

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[0], true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    fighter_enter_state(fighter, S_THROW);
  } else if (fighter_holds_back(fighter)) {
    weapon_back(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[1], true);
  }
}

static void weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

static void weapon_back(Fighter *fighter) {
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

void kai_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 11);
}

/* 4ecb40[type63-60] selects4959f0 ->4eca08/4eca18. These use
 * the HP strike data directly, bypassing ordinary air/back input selection. */
static void weapon_followup(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[0], true);
}

void kai_draw_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_followup(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 11);
  }
}

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

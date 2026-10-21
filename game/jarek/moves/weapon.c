#include "retail_vm.h"
#include "../character.h"

static void equip(Fighter *fighter);
static void hp(Fighter *fighter);
static void lp(Fighter *fighter);
static void back_hp(Fighter *fighter);
static void back_lp(Fighter *fighter);
static const MoveDef upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x113};
static const MoveDef victim = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x4001};
static const AttackRecord upper_record = {98959, -45809, 67502, 28835, 146, 10, 119, 0, 11796};
static const WeaponDef knife = {0x6a, hp, lp, back_hp, back_lp, NULL, weapon_throw, equip, SCENE_MODEL_JAREK_WEAPON};

static void equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &knife);
}

static void cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

static void back_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &upper);
  fighter->mmove_cleanup = cleanup;
  native_schedule_speed(fighter, 0x1999, 7, 13);
  retail_vm_enter_owned(fighter, 0x4ec4f0, fighter_ready, 12);
}

static void back_lp(Fighter *fighter) {
  weapon_start_retail_strike(fighter, 0x4ec538, 0x4ec528, 0x10e, true);
}

static void hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    back_hp(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec4c8, 0x0, 0x111, false);
  }
}

/* 495980 has the throw/back gates but no air selector. The ordinary LP
 * callback handles air first; follow-up table4ecb40 enters this ground body. */
static void ground_lp(Fighter *fighter) {
  if (fighter_distance(fighter) <= .7f && !fighter_airborne(fighter_opponent(fighter)) && !(fighter_opponent(fighter)->flags & FF_REACTING)) {
    retail_lp_throw(fighter);
  } else if (fighter_holds_back(fighter)) {
    back_lp(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec510, 0x0, 0x111, false);
  }
}

static void lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else {
    ground_lp(fighter);
  }
}

void jarek_weapon_upper_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 226 && fighter_sample(fighter) == 29, "Knife upper contact needs the selected VM startup");
  weapon_script_contact_tail(fighter, &upper_record, 2);
}

void jarek_script_weapon_draw_finish(Fighter *fighter) {
  equip(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 12);
}

void jarek_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip(fighter);
    weapon_followup_victim(fighter);
    ground_lp(fighter);
  } else {
    weapon_draw(fighter, &knife);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 12);
  }
}

void jarek_weapon_reaction(Fighter *fighter) {
  require(fighter->reaction == 148 || fighter->reaction == 62, "Invalid knife victim entry");
  mk4_script_disarm_self(fighter);
  const Fighter *peer = fighter_opponent(fighter);
  fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + q16_mul(fighter_axis(peer, AXIS_X), 0x1b333);
  fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + q16_mul(fighter_axis(peer, AXIS_Z), 0x1b333);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, S_HIT, &victim);
  fighter->flags |= FF_REACTING;
  retail_vm_enter_owned(fighter, 0x4e8c08, mk4_mmove_thrown_finish, 12);
}

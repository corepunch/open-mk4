#include "../character.h"

static void equip(Fighter *fighter);
static void hp(Fighter *fighter);
static void lp(Fighter *fighter);
static void back_hp(Fighter *fighter);
static void back_lp(Fighter *fighter);
static const MoveDef upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x113};
static const MoveDef victim = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x4001};
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 239, .last = 40, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {85196, -54394, 68812, 57016, 149, 149, 119, 0, 10485}, 0, 3, 9, 2, 5, 0, 0, 0, 34},
  {{.clip = 229, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {126484, -32768, 64880, 57016, 44, 44, 119, 0, 9830}, 0, 4, 7, 3, 1, 0, 0, 0, 32},
  {{.clip = 228, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {111411, -35389, 85852, 27459, 148, 148, 118, 0, 9830}, 0, 10, 16, 5, 5, 1, 14, 0x2666, 34},
};
// clang-format on
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
  jarek_entry_retail_script_004ec4f0(fighter);
}

static void back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[2], true);
}

static void hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, true, jarek_entry_retail_script_004e66d0);
  } else if (fighter_holds_back(fighter)) {
    back_hp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[0], false);
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
    weapon_start_strike(fighter, &strikes[1], false);
  }
}

static void lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, false, jarek_entry_retail_script_004e66f0);
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
  jarek_entry_retail_script_004e62d8(fighter);
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
    jarek_entry_retail_script_004e62c0(fighter);
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
  jarek_entry_retail_script_004e8c08(fighter);
}

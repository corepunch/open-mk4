#include "../character.h"

static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
static const MoveDef upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x113};
static const MoveDef back_lp = {.clip = 227, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x112};
static const MoveDef victim = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 239, .last = 40, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {85196, -54394, 68812, 57016, 149, 149, 119, 0, 10485}, 0, 3, 9, 2, 5, 0, 0, 0, 34},
  {{.clip = 230, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {152043, -62259, 99614, 85131, 63, 63, 119, 0, 9830}, 0, 8, 10, 3, 5, 0, 0, 0, 34},
};
// clang-format on
static const AttackRecord upper_record = {98959, -45809, 67502, 28835, 146, 10, 119, 0, 11796};
static const AttackRecord back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};
static const WeaponDef mace = {0x6e, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_CAGE_WEAPON};

static void equip_weapon(Fighter *fighter) {
  require(cage_weapon_descriptor[0] == mace.type, "Cage weapon identity must match the reviewed retail descriptor");
  fighter_equip_weapon(fighter, &mace);
}

static void weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

static void weapon_upper(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &upper);
  fighter->mmove_cleanup = weapon_cleanup;
  native_schedule_speed(fighter, 0x1999, 7, 13);
  cage_entry_retail_script_004ec4f0(fighter);
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[0], false);
}

static void weapon_back_lp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &back_lp);
  fighter->mmove_cleanup = weapon_cleanup;
  native_schedule_speed(fighter, 0x1999, 7, 13);
  cage_entry_retail_script_004ec7f0(fighter);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_hp(fighter);
  } else {
    weapon_upper(fighter);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_lp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[1], false);
  }
}

void cage_weapon_upper_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 226 && fighter_sample(fighter) == 29, "Cage upper weapon contact needs its selected VM startup");
  weapon_script_contact_tail(fighter, &upper_record, 2);
}

void cage_weapon_back_lp_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 227 && fighter_sample(fighter) == 29, "Cage back LP contact needs its selected VM startup");
  weapon_script_contact_tail(fighter, &back_lp_record, 2);
}

void cage_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  cage_entry_retail_script_004e62d8(fighter);
}

void cage_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_lp(fighter);
  } else {
    weapon_draw(fighter, &mace);
    cage_entry_retail_script_004e62c0(fighter);
  }
}

void cage_weapon_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}

void cage_weapon_getup_roll(Fighter *fighter) {
  mk4_mmove_getup(fighter, 0x4ea220);
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
    cage_entry_retail_script_004e8cc0(fighter);
  } else {
    cage_entry_retail_script_004e8d38(fighter);
  }
}

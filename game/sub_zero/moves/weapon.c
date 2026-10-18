#include "../character.h"

static const WeaponDef scepter;
static void equip_scepter(Fighter *fighter);
static void scepter_attack(Fighter *fighter, int index);
static void scepter_hp(Fighter *fighter);
static void scepter_lp(Fighter *fighter);
static void scepter_back_hp(Fighter *fighter);
static void scepter_back_lp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike scepter_attacks[] = {
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {128385, -35323, 92274, 22872, 51, 51, 119, 0, 11796}, 6, 10, 16, 4, 1, 0, 0, 0, 0x20},
  {{.clip = 250, .last = 31, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {115277, 20316, 73400, 51052, 150, 150, 118, ATTACK_LOW, 9830}, 0, 4, 12, 2, 9, 0, 0, 0, 0x22},
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 8, 0x147a, 0x22},
  {{.clip = 228, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0113}, {123076, -26804, 91684, 14352, 151, 151, 118, 0, 0}, 0, 10, 16, 5, 5, 1, 14, 0x2666, 0x22},
};
// clang-format on
static const WeaponDef scepter = {0x6b, scepter_hp, scepter_lp, scepter_back_hp, scepter_back_lp, NULL, weapon_throw, equip_scepter, SCENE_MODEL_SCEPTER};

void sub_zero_armed_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void sub_zero_armed_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void sub_zero_armed_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

void sub_zero_armed_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(
        fighter, index == 0 || index == 2, (index == 0 || index == 2) ? sub_zero_entry_retail_script_004e66d0 : sub_zero_entry_retail_script_004e66f0);
  } else {
    weapon_start_strike(fighter, &scepter_attacks[index], true);
  }
}

static void scepter_hp(Fighter *fighter) {
  scepter_attack(fighter, 0);
}

static void scepter_lp(Fighter *fighter) {
  scepter_attack(fighter, 1);
}

static void scepter_back_hp(Fighter *fighter) {
  scepter_attack(fighter, 2);
}

static void scepter_back_lp(Fighter *fighter) {
  scepter_attack(fighter, 3);
}

bool sub_zero_can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

void sub_zero_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_scepter(fighter);
    weapon_followup_victim(fighter);
    weapon_start_strike(fighter, &scepter_attacks[2], false);
    fighter->action = ACTION_RETAIL_010E;
  } else {
    weapon_draw(fighter, &scepter);
    sub_zero_entry_retail_script_004e62c0(fighter);
  }
}

bool sub_zero_can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

void sub_zero_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

void sub_zero_script_draw_equip(Fighter *fighter) {
  equip_scepter(fighter);
  sub_zero_entry_retail_script_004e62d8(fighter);
}

#include "retail_vm.h"
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

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, index == 0 || index == 2 ? 0 : 1);
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
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 4);
  }
}

void sub_zero_script_draw_equip(Fighter *fighter) {
  equip_scepter(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 4);
}

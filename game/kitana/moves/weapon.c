#include "retail_vm.h"
#include "../character.h"

static const WeaponDef native_weapon;
static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 230, .last = 44, .flags = RETAIL, .action = 0x111}, {124518, -34078, 83230, 51118, 54, 54, 119, 0, 11796}, 0, 8, 11, 3, 1, 0, 0, 0, 34}, /* 4ec760 */
  {{.clip = 229, .last = 44, .flags = RETAIL, .action = 0x111}, {126484, -32768, 64880, 57016, 56, 56, 119, 0, 9830}, 0, 4, 7, 3, 1, 0, 0, 0, 32}, /* 4ec778 */
};
// clang-format on
static const MoveDef s_weapon_back_hp = {.clip = 228, .last = 39, .flags = RETAIL, .action = 0x112};
static const MoveDef s_weapon_back_lp = {.clip = 227, .last = 59, .flags = RETAIL, .action = 0x112};
static const AttackRecord weapon_back_hp_record = {98959, -28835, 67502, 36700, 55, 55, 119, 0, 13107};
static const AttackRecord weapon_back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};
static const WeaponDef native_weapon = {
    /* Kitana's roster slot selects the same descriptor 4ece38 and rig as
     * Sonya, so both use the shared retail weapon model. */
    0x62, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_SONYA_WEAPON};

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[0], true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[1], true);
  }
}

void kitana_script_weapon_hp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_hp_record, 7);
}

void kitana_script_weapon_lp_tail(Fighter *fighter) {
  mk4_mmove_weapon_tail(fighter, &weapon_back_lp_record, 2);
}

static void weapon_back_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_weapon_back_hp);
  mk4_script_schedule_motion(fighter, 0x1999, 3, 12);
  fighter->mmove_cleanup = kitana_special_cleanup;
  retail_vm_enter_owned(fighter, 0x4ec7a0, fighter_ready, 16);
}

static void weapon_back_lp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_weapon_back_lp);
  mk4_script_schedule_motion(fighter, 0x1999, 8, 13);
  fighter->mmove_cleanup = kitana_special_cleanup;
  retail_vm_enter_owned(fighter, 0x4ec7f0, fighter_ready, 16);
}

void kitana_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 16);
}

void kitana_weapon(Fighter *fighter) {
  if (fighter->armed) {
    weapon_throw(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_hp(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 16);
  }
}

void kitana_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, kitana_script_throw_getup);
}

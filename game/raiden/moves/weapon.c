#include "../character.h"

static const WeaponDef native_weapon;
static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 233, .last = 44, .flags = RETAIL, .action = 0x112}, {104857, -27525, 70778, 20971, 51, 51, 119, 0, 9830}, 0, 8, 16, 5, 0, 0, 0, 0, 34}, /* 4ec810 */
  {{.clip = 234, .last = 44, .flags = RETAIL, .action = 0x112}, {94371, -24903, 74711, 22282, 47, 47, 119, 0, 9830}, 0, 8, 16, 4, 0, 0, 0, 0, 34}, /* 4ec848 */
  {{.clip = 238, .last = 36, .flags = RETAIL, .action = 0x112}, {114688, -24903, 71434, 35389, 53, 10, 119, 0, 9830}, 0, 8, 25, 3, 0, 8, 17, 4587, 34}, /* 4ec838 */
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = 0x112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 5242, 34}, /* 4ec870 */
};
// clang-format on
static const WeaponDef native_weapon = {0x61, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_RAIDEN_STAFF};

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

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[2], true);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[3], true);
}

void raiden_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  raiden_entry_retail_script_004e62d8(fighter);
}

void raiden_weapon(Fighter *fighter) {
  if (fighter->armed) {
    weapon_throw(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_hp(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    raiden_entry_retail_script_004e62c0(fighter);
  }
}

void raiden_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, raiden_script_throw_getup);
}

void raiden_script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_hold(victim, 20, raiden_script_weapon_victim_getup);
}

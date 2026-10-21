#include "retail_vm.h"
#include "../character.h"

static const WeaponDef native_weapon;
static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
static const WeaponDef native_weapon = {0x61, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_RAIDEN_STAFF};

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec810, 0x0, 0x112, true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec848, 0x0, 0x112, true);
  }
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_retail_strike(fighter, 0x4ec838, 0x4ec828, 0x112, true);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_retail_strike(fighter, 0x4ec870, 0x4ec860, 0x112, true);
}

void raiden_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 1);
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
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 1);
  }
}

void raiden_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_flip);
}

void raiden_script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_hold(victim, 20, raiden_script_weapon_victim_getup);
}

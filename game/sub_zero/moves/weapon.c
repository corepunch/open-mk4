#include "retail_vm.h"
#include "../character.h"

static const WeaponDef scepter;
static void equip_scepter(Fighter *fighter);
static void scepter_attack(Fighter *fighter, int index);
static void scepter_hp(Fighter *fighter);
static void scepter_lp(Fighter *fighter);
static void scepter_back_hp(Fighter *fighter);
static void scepter_back_lp(Fighter *fighter);
static const WeaponDef scepter = {0x6b, scepter_hp, scepter_lp, scepter_back_hp, scepter_back_lp, NULL, weapon_throw, equip_scepter, SCENE_MODEL_SCEPTER};

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, index == 0 || index == 2 ? 0 : 1);
  } else {
    switch (index) {
      case 0:
        weapon_start_retail_strike(fighter, 0x4ec458, 0x0, ACTION_RETAIL_0111, true);
        break;
      case 1:
        weapon_start_retail_strike(fighter, 0x4ec490, 0x0, ACTION_RETAIL_0111, true);
        break;
      case 2:
        weapon_start_retail_strike(fighter, 0x4ec480, 0x4ec470, ACTION_RETAIL_0112, true);
        break;
      case 3:
        weapon_start_retail_strike(fighter, 0x4ec4b8, 0x4ec4a8, ACTION_RETAIL_0113, true);
        break;
      default:
        require(false, "Invalid weapon native selection");
        break;
    }
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
    weapon_start_retail_strike(fighter, 0x4ec480, 0x4ec470, ACTION_RETAIL_0112, false);
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

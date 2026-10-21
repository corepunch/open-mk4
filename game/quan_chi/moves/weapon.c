#include "retail_vm.h"
#include "../character.h"

static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);

/* Button table 4ecc48: HP 495780, LP 496ba0. 495780 tests Back (48ebd0) and
 * strikes 4ec9d8, otherwise 496a70 travels (4ec828) into 4ec838; 496ba0
 * gives 46dbb0's close LP throw priority, then 494990 strikes 4ec848 or
 * dispatches its Back variant 496be0 (travel 4ec860, strike 4ec870).
 * 495780 leaves the action unchanged; the portable back-HP strike uses the
 * same 112 action written by the weapon's other strikes. */
/* Descriptor 4ecf38: type 6f, rig 50c310 (share2_g.geo mesh 1). */
static const WeaponDef native_weapon = {
    0x6f, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_QUAN_CHI_WEAPON};

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_retail_strike(fighter, 0x4ec9d8, 0x0, 0x112, false);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_retail_strike(fighter, 0x4ec870, 0x4ec860, 0x112, true);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_hp(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec838, 0x4ec828, 0x112, true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    retail_lp_throw(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_lp(fighter);
  } else {
    weapon_start_retail_strike(fighter, 0x4ec848, 0x0, 0x112, false);
  }
}

/* 46d260: equip after the draw clip reaches its grip sample, then 4e62d8. */
void quan_chi_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 8);
}

/* 46d1d0: an armed fighter throws (45f5f0); a follow-up equips through
 * 494610 and table 4ecb40[6f - 60] = 496be0; otherwise draw 4e62c0. */
void quan_chi_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_lp(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 8);
  }
}

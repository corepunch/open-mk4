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
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x112}, {128385, -35323, 92274, 22872, 51, 51, 119, 0, 11796}, 6, 10, 16, 4, 1, 0, 0, 0, 32}, /* 4ec9d8 */
  {{.clip = 238, .last = 36, .flags = SF_LOCK | SF_RECORD, .action = 0x112}, {114688, -24903, 71434, 35389, 53, 10, 119, 0, 9830}, 0, 8, 25, 3, 0, 8, 17, 0x11eb, 34}, /* 4ec838 */
  {{.clip = 234, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x112}, {94371, -24903, 74711, 22282, 47, 47, 119, 0, 9830}, 0, 8, 16, 4, 0, 0, 0, 0, 34}, /* 4ec848 */
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34}, /* 4ec870 */
};
// clang-format on
/* Descriptor 4ecf38: type 6f, rig 50c310 (share2_g.geo mesh 1). */
static const WeaponDef native_weapon = {
    0x6f, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_QUAN_CHI_WEAPON};

static void equip_weapon(Fighter *fighter) {
  require(quan_chi_weapon_descriptor[0] == (uint32_t)native_weapon.type, "Quan Chi weapon identity must match the reviewed retail descriptor");
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[0], false);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[3], true);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, true, quan_chi_entry_retail_script_004e66d0);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_hp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[1], true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, false, quan_chi_entry_retail_script_004e66f0);
  } else if (mk4_can_ground_throw(fighter, false)) {
    quan_chi_lp_throw(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_lp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[2], false);
  }
}

/* 46d260: equip after the draw clip reaches its grip sample, then 4e62d8. */
void quan_chi_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  quan_chi_entry_retail_script_004e62d8(fighter);
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
    quan_chi_entry_retail_script_004e62c0(fighter);
  }
}

void quan_chi_armed_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void quan_chi_armed_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void quan_chi_armed_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

void quan_chi_armed_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

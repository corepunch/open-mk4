#include "retail_vm.h"
#include "../character.h"

static const WeaponDef native_weapon;
static void weapon_equip(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
static void weapon_first_victim(Fighter *fighter);
static void weapon_second_victim(Fighter *fighter);
static const NativeAxeProfile axe_profile = {weapon_first_victim, weapon_second_victim};
static const WeaponDef native_weapon = {0x69, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, weapon_equip, SCENE_MODEL_AXE};

static void weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else {
    weapon_axe_strike(fighter, fighter_holds_back(fighter) ? 2 : 0, &axe_profile);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_lp(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    fighter_enter_state(fighter, S_THROW);
  } else {
    weapon_axe_strike(fighter, 1, &axe_profile);
  }
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_axe_strike(fighter, 2, &axe_profile);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_axe_strike(fighter, 3, &axe_profile);
}

static void weapon_first_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, false, (RetailScriptContinuation){0x4ea040, mk4_script_thrown_ready, 7});
}

static void weapon_second_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, true, (RetailScriptContinuation){0x4ea0b8, mk4_script_thrown_ready, 7});
}

void reiko_script_weapon_draw_finish(Fighter *fighter) {
  weapon_equip(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 7);
}

void reiko_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    weapon_equip(fighter);
    weapon_followup_victim(fighter);
    weapon_back_hp(fighter);
    fighter->action = ACTION_RETAIL_010E;
  } else {
    weapon_draw(fighter, &native_weapon);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 7);
  }
}

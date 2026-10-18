#include "../character.h"

static const WeaponDef s_jax_weapon;
static void jax_script_weapon_draw_finish(Fighter *fighter);
static void jax_weapon_hp(Fighter *fighter);
static void jax_weapon_lp(Fighter *fighter);
static void jax_weapon_back_hp(Fighter *fighter);
static void jax_weapon_back_lp(Fighter *fighter);
static void jax_weapon_equip(Fighter *fighter);
static void jax_command_weapon(Fighter *fighter);
static const MoveDef s_weapon_draw = {.clip = 244, .last = MOVE_END, .flags = SF_LOCK};
static const WeaponDef s_jax_weapon = {
    0x69, jax_weapon_hp, jax_weapon_lp, jax_weapon_back_hp, jax_weapon_back_lp, NULL, weapon_throw, jax_weapon_equip, SCENE_MODEL_SWORD};

static void jax_script_weapon_draw_finish(Fighter *fighter) {
  jax_entry_retail_script_004e62d8(fighter);
}

static void jax_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jax_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jax_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jax_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jax_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &s_jax_weapon);
}

void jax_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
    return;
  }
  if (fighter->flags & FF_FOLLOW_UP) {
    jax_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    jax_weapon_back_hp(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_weapon_draw);
  weapon_draw(fighter, &s_jax_weapon);
  fighter_start_fiber(fighter, jax_entry_retail_script_004e62c0, NULL);
}

static void jax_command_weapon(Fighter *fighter) {
  jax_weapon_action(fighter);
}

#include "../character.h"

static const WeaponDef jarek_weapon;
static void jarek_weapon_hp(Fighter *fighter);
static void jarek_weapon_lp(Fighter *fighter);
static void jarek_weapon_back_hp(Fighter *fighter);
static void jarek_weapon_back_lp(Fighter *fighter);
static void jarek_weapon_equip(Fighter *fighter);
static const MoveDef s_weapon_draw = {.clip = 244, .last = MOVE_END, .flags = SF_LOCK};
static const WeaponDef jarek_weapon = {
    0x6a, jarek_weapon_hp, jarek_weapon_lp, jarek_weapon_back_hp, jarek_weapon_back_lp, NULL, weapon_throw, jarek_weapon_equip, SCENE_MODEL_SWORD};

void jarek_script_weapon_draw_finish(Fighter *fighter) {
  jarek_entry_retail_script_004e62d8(fighter);
}

static void jarek_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &jarek_weapon);
}

void jarek_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
    return;
  }
  if (fighter->flags & FF_FOLLOW_UP) {
    jarek_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    jarek_weapon_back_hp(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_weapon_draw);
  weapon_draw(fighter, &jarek_weapon);
  fighter_start_fiber(fighter, jarek_entry_retail_script_004e62c0, NULL);
}

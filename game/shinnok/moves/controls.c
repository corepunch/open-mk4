#include "../character.h"

static void shinnok_command_469a00(Fighter *fighter);
static void shinnok_command_469a10(Fighter *fighter);
static void shinnok_command_469a20(Fighter *fighter);
static void shinnok_command_469a30(Fighter *fighter);
static void shinnok_command_469a40(Fighter *fighter);
static void shinnok_command_469a50(Fighter *fighter);
static void shinnok_command_469a60(Fighter *fighter);
static void shinnok_command_469a70(Fighter *fighter);
static void shinnok_command_469a80(Fighter *fighter);
static void shinnok_command_469a90(Fighter *fighter);
static void shinnok_command_469aa0(Fighter *fighter);
static void shinnok_command_469ab0(Fighter *fighter);
static void shinnok_command_469ac0(Fighter *fighter);
static void shinnok_command_469ad0(Fighter *fighter);
static void shinnok_command_467cd0(Fighter *fighter);
static void shinnok_command_467ce0(Fighter *fighter);
static void shinnok_command_46d1d0(Fighter *fighter);

static void shinnok_command_469a00(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_command_469a10(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469a20(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469a30(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469a40(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_command_469a50(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469a60(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469a70(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_469a80(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_469a90(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469aa0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469ab0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469ac0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469ad0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_467cd0(Fighter *fighter) {
  shinnok_fan_fatality(fighter);
}

static void shinnok_command_467ce0(Fighter *fighter) {
  shinnok_spike_fatality(fighter);
}

static void shinnok_command_46d1d0(Fighter *fighter) {
  weapon_draw(fighter, &shinnok_weapon);
}

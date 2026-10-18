#include "../character.h"

static void jarek_command_469a00(Fighter *fighter);
static void jarek_command_469a10(Fighter *fighter);
static void jarek_command_469a20(Fighter *fighter);
static void jarek_command_469a30(Fighter *fighter);
static void jarek_command_469a40(Fighter *fighter);
static void jarek_command_469a50(Fighter *fighter);
static void jarek_command_469a60(Fighter *fighter);
static void jarek_command_469a70(Fighter *fighter);
static void jarek_command_469a80(Fighter *fighter);
static void jarek_command_469a90(Fighter *fighter);
static void jarek_command_469aa0(Fighter *fighter);
static void jarek_command_469ab0(Fighter *fighter);
static void jarek_command_469ac0(Fighter *fighter);
static void jarek_command_469ad0(Fighter *fighter);
static void jarek_command_467cd0(Fighter *fighter);
static void jarek_command_467ce0(Fighter *fighter);
static void jarek_command_46d1d0(Fighter *fighter);
static void jarek_command_4797f0(Fighter *fighter);
static void jarek_command_47b310(Fighter *fighter);
static void jarek_command_47b5d0(Fighter *fighter);
static void jarek_command_497540(Fighter *fighter);

static void jarek_command_469a00(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_command_469a10(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469a20(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469a30(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469a40(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_command_469a50(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469a60(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469a70(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_469a80(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_469a90(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469aa0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469ab0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469ac0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469ad0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_467cd0(Fighter *fighter) {
  jarek_fan_fatality(fighter);
}

static void jarek_command_467ce0(Fighter *fighter) {
  jarek_spike_fatality(fighter);
}

static void jarek_command_46d1d0(Fighter *fighter) {
  jarek_weapon_action(fighter);
}

static void jarek_command_4797f0(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &jarek_s_low_kick, jarek_entry_retail_script_004e80a0);
}

static void jarek_command_47b310(Fighter *fighter) {
  jarek_low_kick(fighter);
}

static void jarek_command_47b5d0(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &jarek_s_high_kick, jarek_entry_retail_script_004e81a8);
}

static void jarek_command_497540(Fighter *fighter) {
  fighter_start_fiber(fighter, jarek_entry_retail_script_004ed118, NULL);
}

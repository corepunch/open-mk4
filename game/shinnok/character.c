#include "character.h"

const MoveDef shinnok_s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
const MoveDef shinnok_s_walk = {.clip = 267, .last = MOVE_END};
const MoveDef shinnok_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
const MoveDef shinnok_s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};

const MoveDef shinnok_s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

const MoveDef shinnok_s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef shinnok_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};

const MoveDef shinnok_s_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};

const MoveDef shinnok_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};

#include "generated/scripts.inc"
#include "generated/frames.inc"

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = shinnok_press_punch,
    [BUTTON_LOW_PUNCH] = shinnok_press_low_punch,
    [BUTTON_HIGH_KICK] = shinnok_press_kick,
    [BUTTON_LOW_KICK] = shinnok_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = shinnok_press_uppercut,
    [BUTTON_LOW_PUNCH] = shinnok_press_uppercut,
    [BUTTON_HIGH_KICK] = shinnok_press_sweep,
    [BUTTON_LOW_KICK] = shinnok_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = shinnok_press_air_hp,
    [BUTTON_LOW_PUNCH] = shinnok_press_air_lp,
    [BUTTON_HIGH_KICK] = shinnok_press_air_hk,
    [BUTTON_LOW_KICK] = shinnok_press_air_lk}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = shinnok_idle,
  [S_WALK] = shinnok_walk,
  [S_CROUCH] = shinnok_crouch,
  [S_JUMP] = NULL,
  [S_PUNCH] = shinnok_jab,
  [S_KICK] = shinnok_high_kick,
  [S_SWEEP] = shinnok_sweep,
  [S_UPPERCUT] = shinnok_uppercut,
  [S_THROW] = shinnok_lp_throw,
  [S_BLOCK] = shinnok_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = NULL,
};
// clang-format on

#include "generated/controls.inc"

static const Controls controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]),
    {&standing, &crouching, &jumping, [STANCE_DIAGONAL_AIR] = &shinnok_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &shinnok_s_idle,
  [S_WALK] = &shinnok_s_walk,
  [S_CROUCH] = &shinnok_s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &shinnok_s_jab,
  [S_KICK] = &shinnok_s_high_kick,
  [S_SWEEP] = &shinnok_s_sweep,
  [S_UPPERCUT] = &shinnok_s_uppercut,
  [S_THROW] = &shinnok_s_throw,
  [S_BLOCK] = &shinnok_s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const WeaponDef shinnok_weapon = {
    0x64, shinnok_weapon_hp, shinnok_weapon_lp, shinnok_weapon_back_hp, shinnok_weapon_back_lp, NULL, weapon_throw, shinnok_weapon_equip, SCENE_MODEL_SWORD};

const CharacterDef mk4_shinnok = {GAME_MK4, 6, "SHINNOK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, NULL, NULL, 0};

void shinnok_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void shinnok_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void shinnok_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void shinnok_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void shinnok_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void shinnok_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void shinnok_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void shinnok_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

void shinnok_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void shinnok_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void shinnok_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void shinnok_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void shinnok_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void shinnok_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void shinnok_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void shinnok_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void shinnok_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void shinnok_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void shinnok_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void shinnok_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void shinnok_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void shinnok_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void shinnok_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void shinnok_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void shinnok_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

const mmove_t *shinnok_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_repeat_phase;
}

void shinnok_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void shinnok_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void shinnok_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

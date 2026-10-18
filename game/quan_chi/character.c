#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 267, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
const MoveDef quan_chi_s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
const MoveDef quan_chi_s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
const MoveDef quan_chi_s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW};
const MoveDef quan_chi_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH};
const MoveDef quan_chi_s_throw = {.clip = 78, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE | SF_RECORD};

static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 120, .last = MOVE_END, .flags = SF_HOLD};

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_high_punch,
    [BUTTON_LOW_PUNCH] = quan_chi_press_low_punch,
    [BUTTON_HIGH_KICK] = quan_chi_press_high_kick,
    [BUTTON_LOW_KICK] = quan_chi_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_uppercut,
    [BUTTON_LOW_PUNCH] = quan_chi_press_uppercut,
    [BUTTON_HIGH_KICK] = quan_chi_press_sweep,
    [BUTTON_LOW_KICK] = quan_chi_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_air_hp,
    [BUTTON_LOW_PUNCH] = quan_chi_press_air_lp,
    [BUTTON_HIGH_KICK] = quan_chi_press_air_hk,
    [BUTTON_LOW_KICK] = quan_chi_press_air_lk}};
static const ButtonTable straight_air = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_air_hp,
    [BUTTON_LOW_PUNCH] = quan_chi_press_air_lp,
    [BUTTON_HIGH_KICK] = quan_chi_press_air_hk,
    [BUTTON_LOW_KICK] = quan_chi_press_air_lk}};

#include "generated/controls.inc"

static const Controls quan_chi_controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]),
    {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping, [STANCE_DIAGONAL_AIR] = &quan_chi_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_PUNCH] = quan_chi_jab,
  [S_KICK] = quan_chi_high_kick,
  [S_SWEEP] = quan_chi_sweep,
  [S_UPPERCUT] = quan_chi_uppercut,
  [S_THROW] = quan_chi_lp_throw,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const uint32_t quan_chi_weapon_descriptor[] = {
  0x6f,
  0x1430c4,
  0x144356,
  0x13b312,
  0x13eeb9,
  0x13ee9e,
  0x13eea1,
  0xffffb334,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &quan_chi_s_jab,
  [S_KICK] = &quan_chi_s_high_kick,
  [S_SWEEP] = &quan_chi_s_sweep,
  [S_UPPERCUT] = &quan_chi_s_uppercut,
  [S_THROW] = &quan_chi_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_quan_chi = {GAME_MK4, 8, "QUAN CHI", &mk4_motion, moves, S_COUNT, &quan_chi_controls, enter, -78643, mk4_walk, mk4_jump, NULL, NULL, 0};

void quan_chi_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void quan_chi_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void quan_chi_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void quan_chi_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void quan_chi_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void quan_chi_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void quan_chi_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void quan_chi_entry_retail_script_004e6998(Fighter *fighter) {
  retail_script_004e6998(fighter);
}

void quan_chi_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void quan_chi_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void quan_chi_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void quan_chi_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void quan_chi_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void quan_chi_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void quan_chi_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void quan_chi_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void quan_chi_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void quan_chi_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void quan_chi_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void quan_chi_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void quan_chi_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void quan_chi_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void quan_chi_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void quan_chi_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void quan_chi_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

const mmove_t *quan_chi_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_repeat_phase;
}

void quan_chi_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void quan_chi_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void quan_chi_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void quan_chi_entry_retail_script_004e6270(Fighter *fighter) {
  retail_script_004e6270(fighter);
}

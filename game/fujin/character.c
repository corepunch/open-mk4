#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef fujin_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};

const MoveDef fujin_s_kick = {65, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};

const MoveDef fujin_s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef fujin_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef fujin_s_throw = {81, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef fujin_s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {120, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Retail inventory descriptor 0x4ece98, preserved as encoded EXE words. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t fujin_weapon_descriptor[] = {
  0x68,
  0x141b6b,
  0x142dc4,
  0x13b360,
  0x13edd5,
  0x13eea4,
  0x13eea7,
  0xffffcccd,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
const MoveDef *const fujin_moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &fujin_s_jab,
  [S_KICK] = &fujin_s_kick,
  [S_SWEEP] = &fujin_s_sweep,
  [S_UPPERCUT] = &fujin_s_uppercut,
  [S_THROW] = &fujin_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &fujin_s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static const ButtonTable fujin_standing = {{[BUTTON_HIGH_PUNCH] = fujin_press_punch,
    [BUTTON_LOW_PUNCH] = fujin_press_low_punch,
    [BUTTON_HIGH_KICK] = fujin_press_kick,
    [BUTTON_LOW_KICK] = fujin_press_low_kick}};
static const ButtonTable fujin_crouching = {{[BUTTON_HIGH_PUNCH] = fujin_press_uppercut,
    [BUTTON_LOW_PUNCH] = fujin_press_uppercut,
    [BUTTON_HIGH_KICK] = fujin_press_sweep,
    [BUTTON_LOW_KICK] = fujin_press_sweep}};
static const ButtonTable fujin_jumping = {{[BUTTON_HIGH_PUNCH] = fujin_press_air_punch,
    [BUTTON_LOW_PUNCH] = fujin_press_air_lp,
    [BUTTON_HIGH_KICK] = fujin_press_air_hk,
    [BUTTON_LOW_KICK] = fujin_press_air_lk}};

static const Controls fujin_controls = {NULL, 0,
    {[STANCE_STANDING] = &fujin_standing, [STANCE_CROUCHING] = &fujin_crouching, [STANCE_JUMP] = &fujin_jumping, [STANCE_DIAGONAL_AIR] = &fujin_diagonal_air}};

const CharacterDef mk4_fujin = {GAME_MK4, 5, "FUJIN", &mk4_motion, fujin_moves, S_COUNT, &fujin_controls, NULL, -73531, mk4_walk, mk4_jump, NULL, NULL, 0};

void fujin_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void fujin_entry_retail_script_004de610(Fighter *fighter) {
  retail_script_004de610(fighter);
}

void fujin_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_script_004de6b8(fighter);
}

void fujin_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void fujin_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void fujin_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void fujin_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void fujin_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void fujin_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void fujin_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

void fujin_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void fujin_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void fujin_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void fujin_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void fujin_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void fujin_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void fujin_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void fujin_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void fujin_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void fujin_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void fujin_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void fujin_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void fujin_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void fujin_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void fujin_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void fujin_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void fujin_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

const mmove_t *fujin_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_repeat_phase;
}

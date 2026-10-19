#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef tanya_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef tanya_s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef tanya_s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef tanya_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef tanya_s_throw = {79, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {124, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

#include "generated/scripts.inc"
#include "generated/frames.inc"

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = tanya_press_high_punch,
    [BUTTON_LOW_PUNCH] = tanya_press_low_punch,
    [BUTTON_HIGH_KICK] = tanya_press_high_kick,
    [BUTTON_LOW_KICK] = tanya_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = tanya_press_uppercut,
    [BUTTON_LOW_PUNCH] = tanya_press_uppercut,
    [BUTTON_HIGH_KICK] = tanya_press_sweep,
    [BUTTON_LOW_KICK] = tanya_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = tanya_press_air_hp,
    [BUTTON_LOW_PUNCH] = tanya_press_air_lp,
    [BUTTON_HIGH_KICK] = tanya_press_air_hk,
    [BUTTON_LOW_KICK] = tanya_press_air_lk}};

#include "generated/controls.inc"

static const Controls tanya_controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]),
    {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping, [STANCE_DIAGONAL_AIR] = &tanya_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = tanya_armed_idle,
  [S_WALK] = tanya_armed_walk,
  [S_CROUCH] = tanya_armed_crouch,
  [S_BLOCK] = tanya_armed_block,
  [S_PUNCH] = tanya_jab,
  [S_KICK] = tanya_high_kick,
  [S_SWEEP] = tanya_sweep,
  [S_UPPERCUT] = tanya_uppercut,
  [S_THROW] = tanya_lp_throw,
};
// clang-format on

/* Retail Tanya uses the shared standing/crouching button tables. Air attacks
 * and LK throw remain explicit entries for command and CPU callers. */

/* Retail descriptor 4ecd98: preserve the encoded eight-word data block. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t tanya_weapon_descriptor[] = {
  0x6d,
  0x141bfe,
  0x142dd9,
  0x13b31e,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffcccd,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &tanya_s_jab,
  [S_KICK] = &tanya_s_kick,
  [S_SWEEP] = &tanya_s_sweep,
  [S_UPPERCUT] = &tanya_s_uppercut,
  [S_THROW] = &tanya_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_tanya = {
    GAME_MK4, 9, "TANYA", &mk4_motion, moves, S_COUNT, &tanya_controls, enter, -71100, mk4_walk, mk4_jump, &retail_cpu_moves, NULL, 0};

void tanya_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void tanya_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void tanya_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void tanya_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void tanya_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void tanya_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void tanya_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void tanya_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

void tanya_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void tanya_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void tanya_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void tanya_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void tanya_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void tanya_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void tanya_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void tanya_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void tanya_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void tanya_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void tanya_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void tanya_entry_retail_script_004e6890(Fighter *fighter) {
  retail_script_004e6890(fighter);
}

void tanya_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_script_004e68a8(fighter);
}

void tanya_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

const mmove_t *tanya_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_phase;
}

void tanya_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void tanya_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void tanya_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void tanya_entry_retail_script_004e0138(Fighter *fighter) {
  retail_script_004e0138(fighter);
}

void tanya_entry_retail_script_004ed268(Fighter *fighter) {
  retail_script_004ed268(fighter);
}

void tanya_entry_retail_script_004ed2c0(Fighter *fighter) {
  retail_script_004ed2c0(fighter);
}

void tanya_entry_retail_script_004e81f0(Fighter *fighter) {
  retail_script_004e81f0(fighter);
}

void tanya_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_script_004e84b8(fighter);
}

void tanya_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void tanya_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void tanya_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void tanya_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void tanya_entry_retail_script_004ed190(Fighter *fighter) {
  retail_script_004ed190(fighter);
}

void tanya_entry_retail_script_004e9db8(Fighter *fighter) {
  retail_script_004e9db8(fighter);
}

void tanya_entry_retail_script_004e8bb0(Fighter *fighter) {
  retail_script_004e8bb0(fighter);
}

void tanya_entry_retail_script_004e5798(Fighter *fighter) {
  retail_script_004e5798(fighter);
}

void tanya_entry_retail_script_004e57c0(Fighter *fighter) {
  retail_script_004e57c0(fighter);
}

void tanya_entry_retail_script_004e5820(Fighter *fighter) {
  retail_script_004e5820(fighter);
}

void tanya_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_script_004e5b38(fighter);
}

void tanya_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_script_004e6a40(fighter);
}

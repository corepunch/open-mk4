#include "character.h"

const MoveDef jax_s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
const MoveDef jax_s_walk = {.clip = 265, .last = MOVE_END};
const MoveDef jax_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
const MoveDef jax_s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};
const MoveDef jax_s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

const MoveDef jax_s_sweep = {.clip = 62, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef jax_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};

const MoveDef jax_s_throw = {.clip = 83, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
const MoveDef jax_s_thrown = {.clip = 145, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

const MoveDef jax_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 412, .last = MOVE_END, .flags = SF_HOLD};

#include "generated/scripts.inc"
#include "generated/frames.inc"

const AttackRecord jax_s_jax_jab_record = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 10, 10, 40, 0, 0x170a};

static const ButtonTable s_standing = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable s_crouching = {{[BUTTON_HIGH_PUNCH] = jax_press_uppercut,
    [BUTTON_LOW_PUNCH] = jax_press_uppercut,
    [BUTTON_HIGH_KICK] = jax_press_sweep,
    [BUTTON_LOW_KICK] = jax_press_sweep}};
static const ButtonTable s_jumping = {{[BUTTON_HIGH_PUNCH] = jax_press_air_hp,
    [BUTTON_LOW_PUNCH] = jax_press_air_lp,
    [BUTTON_HIGH_KICK] = jax_press_air_kick,
    [BUTTON_LOW_KICK] = jax_press_air_kick}};
static const Controls s_controls = {NULL, 0, {&s_standing, &s_crouching, &s_jumping}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = jax_idle,
  [S_WALK] = jax_walk,
  [S_CROUCH] = jax_crouch,
  [S_JUMP] = mk4_jump,
  [S_PUNCH] = jax_jab,
  [S_KICK] = jax_high_kick,
  [S_SWEEP] = jax_sweep,
  [S_UPPERCUT] = jax_uppercut,
  [S_THROW] = jax_lp_throw,
  [S_BLOCK] = jax_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = NULL,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &jax_s_idle,
  [S_WALK] = &jax_s_walk,
  [S_CROUCH] = &jax_s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &jax_s_jab,
  [S_KICK] = &jax_s_high_kick,
  [S_SWEEP] = &jax_s_sweep,
  [S_UPPERCUT] = &jax_s_uppercut,
  [S_THROW] = &jax_s_throw,
  [S_BLOCK] = &jax_s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_jax = {GAME_MK4, 13, "JAX", &mk4_motion, moves, S_COUNT, &s_controls, enter, -75169, mk4_walk, mk4_jump, NULL, NULL, 0};

void jax_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void jax_entry_retail_script_004de358(Fighter *fighter) {
  retail_script_004de358(fighter);
}

void jax_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void jax_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void jax_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void jax_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void jax_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void jax_entry_retail_script_004e1390(Fighter *fighter) {
  retail_script_004e1390(fighter);
}

void jax_entry_retail_script_004e13a0(Fighter *fighter) {
  retail_script_004e13a0(fighter);
}

void jax_entry_retail_script_004e13b0(Fighter *fighter) {
  retail_script_004e13b0(fighter);
}

void jax_entry_retail_script_004e13c0(Fighter *fighter) {
  retail_script_004e13c0(fighter);
}

void jax_entry_retail_script_004e13d0(Fighter *fighter) {
  retail_script_004e13d0(fighter);
}

void jax_entry_retail_script_004e13e0(Fighter *fighter) {
  retail_script_004e13e0(fighter);
}

void jax_entry_retail_script_004e18f0(Fighter *fighter) {
  retail_script_004e18f0(fighter);
}

void jax_entry_retail_script_004e1958(Fighter *fighter) {
  retail_script_004e1958(fighter);
}

void jax_entry_retail_script_004e5a80(Fighter *fighter) {
  retail_script_004e5a80(fighter);
}

void jax_entry_retail_script_004e5a98(Fighter *fighter) {
  retail_script_004e5a98(fighter);
}

void jax_entry_retail_script_004e5b10(Fighter *fighter) {
  retail_script_004e5b10(fighter);
}

void jax_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_script_004e5b38(fighter);
}

void jax_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void jax_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void jax_entry_retail_script_004e5dd8(Fighter *fighter) {
  retail_script_004e5dd8(fighter);
}

void jax_entry_retail_script_004e5df8(Fighter *fighter) {
  retail_script_004e5df8(fighter);
}

void jax_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void jax_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void jax_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void jax_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void jax_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void jax_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void jax_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void jax_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void jax_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void jax_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void jax_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void jax_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void jax_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void jax_entry_retail_script_004e6890(Fighter *fighter) {
  retail_script_004e6890(fighter);
}

void jax_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_script_004e68a8(fighter);
}

void jax_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void jax_entry_retail_script_004e8150(Fighter *fighter) {
  retail_script_004e8150(fighter);
}

void jax_entry_retail_script_004e8174(Fighter *fighter) {
  retail_script_004e8174(fighter);
}

void jax_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

void jax_entry_retail_script_004ed090(Fighter *fighter) {
  retail_script_004ed090(fighter);
}

const mmove_t *jax_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_repeat_phase;
}

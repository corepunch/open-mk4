#include "retail_vm.h"
#include "character.h"

const MoveDef jax_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};

const MoveDef jax_s_thrown = {.clip = 145, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

const MoveDef jax_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 412, .last = MOVE_END, .flags = SF_HOLD};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_JUMP] = mk4_jump,
  [S_THROW] = retail_lp_throw,
  [S_BLOCK] = weapon_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = NULL,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &jax_s_crouch,
  [S_JUMP] = &s_jump,

  [S_BLOCK] = &jax_s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_jax = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &retail_stances,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void jax_entry_retail_script_004de518(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de518, fighter_ready, 13);
}

void jax_entry_retail_script_004de538(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de538, fighter_ready, 13);
}

void jax_entry_retail_script_004de558(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de558, fighter_ready, 13);
}

void jax_entry_retail_script_004de568(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de568, fighter_ready, 13);
}

void jax_entry_retail_script_004e1390(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1390, fighter_ready, 13);
}

void jax_entry_retail_script_004e13a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13a0, fighter_ready, 13);
}

void jax_entry_retail_script_004e13b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13b0, fighter_ready, 13);
}

void jax_entry_retail_script_004e13c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13c0, fighter_ready, 13);
}

void jax_entry_retail_script_004e13d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13d0, fighter_ready, 13);
}

void jax_entry_retail_script_004e13e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13e0, fighter_ready, 13);
}

void jax_entry_retail_script_004e13f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e13f0, fighter_ready, 13);
}

void jax_entry_retail_script_004e5a80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5a80, fighter_ready, 13);
}

void jax_entry_retail_script_004e5a98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5a98, fighter_ready, 13);
}

void jax_entry_retail_script_004e5b10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b10, fighter_ready, 13);
}

void jax_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b38, fighter_ready, 13);
}

void jax_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 13);
}

void jax_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 13);
}

void jax_entry_retail_script_004e8150(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8150, fighter_ready, 13);
}

void jax_entry_retail_script_004ea028(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea028, fighter_ready, 13);
}

void jax_entry_retail_script_004e9e38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9e38, fighter_ready, 13);
}

void jax_entry_retail_script_004e89d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e89d8, fighter_ready, 13);
}

void jax_entry_retail_script_004ed090(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed090, fighter_ready, 13);
}

void jax_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_script_thrown_ready, 13);
}

void jax_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 13);
}

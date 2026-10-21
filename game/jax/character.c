#include "retail_vm.h"
#include "character.h"

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

static const ButtonTable s_standing = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable s_crouching = {{[BUTTON_HIGH_PUNCH] = jax_press_uppercut,
    [BUTTON_LOW_PUNCH] = jax_press_uppercut,
    [BUTTON_HIGH_KICK] = retail_press_sweep,
    [BUTTON_LOW_KICK] = retail_press_sweep}};
static const ButtonTable s_jumping = {{[BUTTON_HIGH_PUNCH] = jax_press_air_hp,
    [BUTTON_LOW_PUNCH] = jax_press_air_lp,
    [BUTTON_HIGH_KICK] = jax_press_air_hk,
    [BUTTON_LOW_KICK] = jax_press_air_lk}};
#include "generated/controls.inc"

static const Controls s_controls = {NULL, 0, {&s_standing, &s_crouching, &s_jumping, [STANCE_DIAGONAL_AIR] = &jax_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_JUMP] = mk4_jump,
  [S_PUNCH] = retail_jab,
  [S_KICK] = retail_high_kick,
  [S_SWEEP] = retail_sweep,
  [S_UPPERCUT] = jax_uppercut,
  [S_THROW] = jax_lp_throw,
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

const CharacterDef mk4_jax = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &s_controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = NULL,
    .move_list_count = 0,
};

void jax_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 13);
}

void jax_entry_retail_script_004de358(Fighter *fighter) {
  /* 4de358 is speed data consumed by490ab0, not a VM program. */
  mk4_mmove_lk_owner_return(fighter, jax_script_lk_finish);
}

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

void jax_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 13);
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

void jax_entry_retail_script_004e5dd8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5dd8, fighter_ready, 13);
}

void jax_entry_retail_script_004e5df8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5df8, fighter_ready, 13);
}

void jax_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 13);
}

void jax_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, fighter_ready, 13);
}

void jax_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 13);
}

void jax_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 13);
}

void jax_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 13);
}

void jax_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 13);
}

void jax_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 13);
}

void jax_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 13);
}

void jax_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, fighter_ready, 13);
}

void jax_entry_retail_script_004e6890(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6890, fighter_ready, 13);
}

void jax_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e68a8, fighter_ready, 13);
}

void jax_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 13);
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

void jax_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 13);
}

void jax_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 13);
}

void jax_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 13);
}

void jax_entry_retail_script_004e5e18(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e18, fighter_ready, 13);
}

void jax_entry_retail_script_004e98e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e98e8, fighter_ready, 13);
}

void jax_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 13);
}

void jax_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 13);
}

void jax_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_script_thrown_ready, 13);
}

void jax_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 13);
}

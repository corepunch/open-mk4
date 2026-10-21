#include "retail_vm.h"
#include "character.h"

const MoveDef shinnok_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
const MoveDef shinnok_s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};

const MoveDef shinnok_s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

const MoveDef shinnok_s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef shinnok_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};

const MoveDef shinnok_s_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE | SF_RECORD, .action = ACTION_RETAIL_0500};

const MoveDef shinnok_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};

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
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_JUMP] = NULL,
  [S_PUNCH] = retail_jab,
  [S_KICK] = retail_high_kick,
  [S_SWEEP] = shinnok_sweep,
  [S_UPPERCUT] = shinnok_uppercut,
  [S_THROW] = shinnok_lp_throw,
  [S_BLOCK] = weapon_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = NULL,
};
// clang-format on

#include "generated/controls.inc"

static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping, [STANCE_DIAGONAL_AIR] = &shinnok_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
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

const CharacterDef mk4_shinnok = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = NULL,
    .move_list_count = 0,
};

void shinnok_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6798, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6860(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6860, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6878(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6878, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e8d08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8d08, mk4_mmove_thrown_finish, 6);
}

void shinnok_entry_retail_script_004e9460(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9460, mk4_mmove_thrown_finish, 6);
}

void shinnok_entry_retail_script_004e9498(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9498, mk4_mmove_thrown_finish, 6);
}

void shinnok_entry_retail_script_004ec8b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec8b0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2ef0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ef0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f00(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f00, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f10, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f20, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f30(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f30, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f40, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f78, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2f88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2f88, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2fb8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2fb8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e0088(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0088, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2fc8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2fc8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e2fd8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2fd8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e00b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e00b8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e00e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e00e0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e00f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e00f8, fighter_ready, 6);
}

void shinnok_entry_retail_script_004ea270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea270, fighter_ready, 6);
}

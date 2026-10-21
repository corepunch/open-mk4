#include "retail_vm.h"
#include "character.h"

const MoveDef shinnok_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};

const MoveDef shinnok_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};

static const ButtonTable jumping = {.press = {[BUTTON_HIGH_PUNCH] = shinnok_press_air_hp,
                                        [BUTTON_LOW_PUNCH] = shinnok_press_air_lp,
                                        [BUTTON_HIGH_KICK] = shinnok_press_air_hk,
                                        [BUTTON_LOW_KICK] = shinnok_press_air_lk}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_JUMP] = NULL,
  [S_THROW] = retail_lp_throw,
  [S_BLOCK] = weapon_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = NULL,
};
// clang-format on

static const Controls controls = {NULL, 0, {&retail_ground_buttons[0], &retail_ground_buttons[1], &jumping, [STANCE_DIAGONAL_AIR] = &shinnok_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &shinnok_s_crouch,
  [S_JUMP] = &s_jump,

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
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void shinnok_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 6);
}

void shinnok_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 6);
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

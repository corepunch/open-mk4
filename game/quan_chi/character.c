#include "retail_vm.h"
#include "character.h"

static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};

static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 120, .last = MOVE_END, .flags = SF_HOLD};

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_high_punch,
    [BUTTON_LOW_PUNCH] = quan_chi_press_low_punch,
    [BUTTON_HIGH_KICK] = quan_chi_press_high_kick,
    [BUTTON_LOW_KICK] = quan_chi_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = retail_press_uppercut,
    [BUTTON_LOW_PUNCH] = retail_press_uppercut,
    [BUTTON_HIGH_KICK] = retail_press_sweep,
    [BUTTON_LOW_KICK] = retail_press_sweep}};
static const ButtonTable straight_air = {{[BUTTON_HIGH_PUNCH] = quan_chi_press_air_hp,
    [BUTTON_LOW_PUNCH] = quan_chi_press_air_lp,
    [BUTTON_HIGH_KICK] = quan_chi_press_air_hk,
    [BUTTON_LOW_KICK] = quan_chi_press_air_lk}};

static const Controls quan_chi_controls = {
    NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &straight_air, [STANCE_DIAGONAL_AIR] = &quan_chi_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
  [S_THROW] = retail_lp_throw,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t quan_chi_weapon_descriptor[] = {
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
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,

  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_quan_chi = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &quan_chi_controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void quan_chi_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e28, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e50, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e9910(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9910, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004ed3a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed3a0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004ed3c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed3c0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e9cd8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9cd8, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e8100(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8100, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e8a48(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8a48, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e9868(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9868, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e8818(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8818, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e9818(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9818, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e9838(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9838, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e14f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14f8, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1508(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1508, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1518(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1518, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1528(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1528, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1538(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1538, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1558(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1558, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1568(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1568, fighter_ready, 8);
}

void quan_chi_entry_retail_script_004e1580(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1580, fighter_ready, 8);
}

#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef tanya_s_throw = {79, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {124, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = tanya_press_high_punch,
    [BUTTON_LOW_PUNCH] = tanya_press_low_punch,
    [BUTTON_HIGH_KICK] = tanya_press_high_kick,
    [BUTTON_LOW_KICK] = tanya_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = retail_press_uppercut,
    [BUTTON_LOW_PUNCH] = retail_press_uppercut,
    [BUTTON_HIGH_KICK] = retail_press_sweep,
    [BUTTON_LOW_KICK] = retail_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = tanya_press_air_hp,
    [BUTTON_LOW_PUNCH] = tanya_press_air_lp,
    [BUTTON_HIGH_KICK] = tanya_press_air_hk,
    [BUTTON_LOW_KICK] = tanya_press_air_lk}};

#include "generated/controls.inc"

static const Controls tanya_controls = {
    NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping, [STANCE_DIAGONAL_AIR] = &tanya_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
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
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_THROW] = &tanya_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_tanya = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &tanya_controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = NULL,
    .move_list_count = 0,
};

void tanya_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 9);
}

void tanya_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 9);
}

void tanya_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 9);
}

void tanya_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 9);
}

void tanya_entry_retail_script_004ed268(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed268, fighter_ready, 9);
}

void tanya_entry_retail_script_004ed2c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e81f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81f0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 9);
}

void tanya_entry_retail_script_004ed190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed190, fighter_ready, 9);
}

void tanya_entry_retail_script_004e9db8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9db8, mk4_mmove_thrown_finish, 9);
}

void tanya_entry_retail_script_004e8bb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bb0, mk4_mmove_thrown_finish, 9);
}

void tanya_entry_retail_script_004e5798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5798, fighter_ready, 9);
}

void tanya_entry_retail_script_004e57c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e57c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e5820(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5820, fighter_ready, 9);
}

void tanya_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b38, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 9);
}

void tanya_entry_retail_script_004e1598(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1598, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15a8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15b8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15d0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15e0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e1600(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1600, fighter_ready, 9);
}

void tanya_entry_retail_script_004e16d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e16d0, fighter_ready, 9);
}

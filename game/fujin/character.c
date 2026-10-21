#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef fujin_s_throw = {81, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

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
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
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
static const ButtonTable fujin_crouching = {{[BUTTON_HIGH_PUNCH] = retail_press_uppercut,
    [BUTTON_LOW_PUNCH] = retail_press_uppercut,
    [BUTTON_HIGH_KICK] = retail_press_sweep,
    [BUTTON_LOW_KICK] = retail_press_sweep}};
static const ButtonTable fujin_jumping = {{[BUTTON_HIGH_PUNCH] = fujin_press_air_punch,
    [BUTTON_LOW_PUNCH] = fujin_press_air_lp,
    [BUTTON_HIGH_KICK] = fujin_press_air_hk,
    [BUTTON_LOW_KICK] = fujin_press_air_lk}};

static const Controls fujin_controls = {NULL, 0,
    {[STANCE_STANDING] = &fujin_standing, [STANCE_CROUCHING] = &fujin_crouching, [STANCE_JUMP] = &fujin_jumping, [STANCE_DIAGONAL_AIR] = &fujin_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
  [S_THROW] = fujin_enter_throw,
};
// clang-format on

const CharacterDef mk4_fujin = {
    .motion = &mk4_motion,
    .moves = fujin_moves,
    .move_count = S_COUNT,
    .controls = &fujin_controls,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void fujin_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 5);
}

void fujin_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 5);
}

void fujin_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 5);
}

void fujin_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 5);
}

void fujin_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 5);
}

void fujin_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8258(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8258, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8298(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8298, fighter_ready, 5);
}

void fujin_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 5);
}

void fujin_entry_retail_script_004ed408(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed408, fighter_ready, 5);
}

void fujin_entry_retail_script_004ed788(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed788, fujin_bow_followup_next, 5);
}

void fujin_entry_retail_script_004e9e10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9e10, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c00(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c00, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c20, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c50, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9fe0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9fe0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9ff0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ff0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8740(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8740, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8770(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8770, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_mmove_thrown_finish, 5);
}

void fujin_entry_retail_script_004ea050(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea050, mk4_mmove_thrown_finish, 5);
}

void fujin_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8830(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8830, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9fb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9fb0, fighter_ready, 5);
}

void fujin_entry_retail_script_004dfed0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfed0, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea2d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2d8, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea2a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2a8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f58, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f80, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f98, fighter_ready, 5);
}

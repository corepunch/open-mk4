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

const MoveDef fujin_s_sweep = {.clip = 62, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW | SF_RECORD, .action = ACTION_RETAIL_010C};
const MoveDef fujin_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
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

#include "generated/controls.inc"

static const Controls fujin_controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]),
    {[STANCE_STANDING] = &fujin_standing, [STANCE_CROUCHING] = &fujin_crouching, [STANCE_JUMP] = &fujin_jumping, [STANCE_DIAGONAL_AIR] = &fujin_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = fujin_armed_idle,
  [S_WALK] = fujin_armed_walk,
  [S_CROUCH] = fujin_armed_crouch,
  [S_BLOCK] = fujin_armed_block,
  [S_THROW] = fujin_enter_throw,
};
// clang-format on

const CharacterDef mk4_fujin = {
    GAME_MK4, 5, "FUJIN", &mk4_motion, fujin_moves, S_COUNT, &fujin_controls, state_entries, -73531, mk4_walk, mk4_jump, &retail_cpu_moves, NULL, 0};

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

void fujin_entry_retail_script_004e5d20(Fighter *fighter) {
  retail_script_004e5d20(fighter);
}

void fujin_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void fujin_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
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

void fujin_entry_retail_script_004e6890(Fighter *fighter) {
  retail_script_004e6890(fighter);
}

void fujin_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_script_004e68a8(fighter);
}

void fujin_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

const mmove_t *fujin_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_phase;
}

void fujin_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void fujin_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void fujin_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void fujin_entry_retail_script_004e8258(Fighter *fighter) {
  retail_script_004e8258(fighter);
}

void fujin_entry_retail_script_004e8298(Fighter *fighter) {
  retail_script_004e8298(fighter);
}

void fujin_entry_retail_script_004e0138(Fighter *fighter) {
  retail_script_004e0138(fighter);
}

void fujin_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void fujin_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void fujin_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void fujin_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void fujin_entry_retail_script_004ed408(Fighter *fighter) {
  retail_script_004ed408(fighter);
}

void fujin_entry_retail_script_004ed788(Fighter *fighter) {
  retail_script_004ed788(fighter);
}

void fujin_entry_retail_script_004e9e10(Fighter *fighter) {
  retail_script_004e9e10(fighter);
}

void fujin_entry_retail_script_004e5c00(Fighter *fighter) {
  retail_script_004e5c00(fighter);
}

void fujin_entry_retail_script_004e5c20(Fighter *fighter) {
  retail_script_004e5c20(fighter);
}

void fujin_entry_retail_script_004e5c50(Fighter *fighter) {
  retail_script_004e5c50(fighter);
}

void fujin_entry_retail_script_004e9fe0(Fighter *fighter) {
  retail_script_004e9fe0(fighter);
}

void fujin_entry_retail_script_004e9ff0(Fighter *fighter) {
  retail_script_004e9ff0(fighter);
}

void fujin_entry_retail_script_004e8740(Fighter *fighter) {
  retail_script_004e8740(fighter);
}

void fujin_entry_retail_script_004e8770(Fighter *fighter) {
  retail_script_004e8770(fighter);
}

void fujin_entry_retail_script_004ea040(Fighter *fighter) {
  retail_script_004ea040(fighter);
}

void fujin_entry_retail_script_004ea050(Fighter *fighter) {
  retail_script_004ea050(fighter);
}

void fujin_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_script_004ea0b8(fighter);
}

void fujin_entry_retail_script_004e8830(Fighter *fighter) {
  retail_script_004e8830(fighter);
}

void fujin_entry_retail_script_004e9fb0(Fighter *fighter) {
  retail_script_004e9fb0(fighter);
}

void fujin_entry_retail_script_004dfed0(Fighter *fighter) {
  retail_script_004dfed0(fighter);
}

void fujin_entry_retail_script_004ea2d8(Fighter *fighter) {
  retail_script_004ea2d8(fighter);
}

void fujin_entry_retail_script_004ea2a8(Fighter *fighter) {
  retail_script_004ea2a8(fighter);
}

#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Encoded Goro roster descriptor retained from the EXE inventory at 0x4ecf18. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t goro_retail_weapon_descriptor[] = {
  0x6c,
  0x141bf0,
  0x142dd3,
  0x13b330,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0x0,
};
// clang-format on

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {590, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {603, 18, 0, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {602, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef goro_s_jab = {597, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef goro_s_kick = {598, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef goro_s_sweep = {601, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef goro_s_uppercut = {600, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef goro_s_throw = {604, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

static const MoveDef s_block = {595, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {593, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef goro_s_knockdown = {581, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {594, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &goro_s_jab,
  [S_KICK] = &goro_s_kick,
  [S_SWEEP] = &goro_s_sweep,
  [S_UPPERCUT] = &goro_s_uppercut,
  [S_THROW] = &goro_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &goro_s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

/* Portable task83 fallback uses a known Goro box while its retail payload remains unavailable. */

static const ButtonTable goro_standing = {{[BUTTON_HIGH_PUNCH] = goro_enter_jab,
    [BUTTON_LOW_PUNCH] = goro_enter_low_punch,
    [BUTTON_HIGH_KICK] = goro_enter_high_kick,
    [BUTTON_LOW_KICK] = goro_enter_low_kick}};
static const ButtonTable goro_crouching = {{[BUTTON_HIGH_PUNCH] = goro_enter_uppercut,
    [BUTTON_LOW_PUNCH] = goro_enter_uppercut,
    [BUTTON_HIGH_KICK] = goro_enter_sweep,
    [BUTTON_LOW_KICK] = goro_enter_sweep}};
static const ButtonTable goro_jumping = {{[BUTTON_HIGH_PUNCH] = goro_enter_air_hp,
    [BUTTON_LOW_PUNCH] = goro_enter_air_lp,
    [BUTTON_HIGH_KICK] = goro_enter_air_hk,
    [BUTTON_LOW_KICK] = goro_enter_air_lk}};

static const CommandDef goro_commands[] = {
    {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, goro_air_throw},
};

static const Controls goro_controls = {
    goro_commands, 1, {[STANCE_STANDING] = &goro_standing, [STANCE_CROUCHING] = &goro_crouching, [STANCE_JUMP] = &goro_jumping}};

static const CpuMoves goro_cpu_moves = {.enter = {[CPU_PROJECTILE] = goro_cpu_projectile}};

const CharacterDef mk4_goro = {GAME_MK4, 15, "GORO", &mk4_motion, moves, S_COUNT, &goro_controls, NULL, -85458, mk4_walk, NULL, &goro_cpu_moves, NULL, 0};

void goro_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void goro_entry_retail_script_004de358(Fighter *fighter) {
  retail_script_004de358(fighter);
}

void goro_entry_retail_script_004de6e4(Fighter *fighter) {
  retail_script_004de6e4(fighter);
}

void goro_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void goro_entry_retail_script_004de7e8(Fighter *fighter) {
  retail_script_004de7e8(fighter);
}

void goro_entry_retail_script_004dfb60(Fighter *fighter) {
  retail_script_004dfb60(fighter);
}

void goro_entry_retail_script_004dfb70(Fighter *fighter) {
  retail_script_004dfb70(fighter);
}

void goro_entry_retail_script_004dfbb8(Fighter *fighter) {
  retail_script_004dfbb8(fighter);
}

void goro_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void goro_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void goro_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void goro_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void goro_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void goro_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

void goro_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void goro_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void goro_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void goro_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void goro_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void goro_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void goro_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void goro_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void goro_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void goro_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void goro_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void goro_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void goro_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void goro_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void goro_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void goro_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void goro_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

void goro_entry_retail_script_004ed790(Fighter *fighter) {
  retail_script_004ed790(fighter);
}

const mmove_t *goro_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_repeat_phase;
}

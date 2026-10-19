#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef reiko_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef reiko_s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef reiko_s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef reiko_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef reiko_s_throw = {78, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {113, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reiko_weapon_descriptor[] = {
  0x69,
  0x141b95,
  0x142dca,
  0x13b33c,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffb334,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &reiko_s_jab,
  [S_KICK] = &reiko_s_kick,
  [S_SWEEP] = &reiko_s_sweep,
  [S_UPPERCUT] = &reiko_s_uppercut,
  [S_THROW] = &reiko_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static const ButtonTable reiko_standing = {{[BUTTON_HIGH_PUNCH] = reiko_press_high_punch,
    [BUTTON_LOW_PUNCH] = reiko_press_low_punch,
    [BUTTON_HIGH_KICK] = reiko_press_high_kick,
    [BUTTON_LOW_KICK] = reiko_press_low_kick}};
static const ButtonTable reiko_crouching = {{[BUTTON_HIGH_PUNCH] = reiko_press_crouching_hp,
    [BUTTON_LOW_PUNCH] = reiko_press_crouching_hp,
    [BUTTON_HIGH_KICK] = reiko_press_crouching_kick,
    [BUTTON_LOW_KICK] = reiko_press_crouching_kick}};
static const ButtonTable reiko_air = {{[BUTTON_HIGH_PUNCH] = reiko_press_air_hp,
    [BUTTON_LOW_PUNCH] = reiko_press_air_lp,
    [BUTTON_HIGH_KICK] = reiko_press_air_hk,
    [BUTTON_LOW_KICK] = reiko_press_air_lk}};

#include "generated/controls.inc"

static const Controls reiko_controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]),
    {[STANCE_STANDING] = &reiko_standing, [STANCE_CROUCHING] = &reiko_crouching, [STANCE_JUMP] = &reiko_air, [STANCE_DIAGONAL_AIR] = &reiko_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = reiko_armed_idle,
  [S_WALK] = reiko_armed_walk,
  [S_CROUCH] = reiko_armed_crouch,
  [S_BLOCK] = reiko_armed_block,

  [S_THROW] = reiko_lp_throw,
};
// clang-format on

const CharacterDef mk4_reiko = {
    GAME_MK4, 7, "REIKO", &mk4_motion, moves, S_COUNT, &reiko_controls, state_entries, -77660, mk4_walk, mk4_jump, &retail_cpu_moves, NULL, 0};

void reiko_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void reiko_entry_retail_script_004de640(Fighter *fighter) {
  retail_script_004de640(fighter);
}

void reiko_entry_retail_script_004de664(Fighter *fighter) {
  /* 4de664 is a490350 callback table, not a VM body. */
  fighter->facing = -fighter->facing;
}

void reiko_entry_retail_script_004de668(Fighter *fighter) {
  retail_script_004de668(fighter);
}

void reiko_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_script_004de6b8(fighter);
}

void reiko_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void reiko_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void reiko_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void reiko_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void reiko_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void reiko_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void reiko_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

void reiko_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void reiko_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void reiko_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void reiko_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void reiko_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void reiko_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void reiko_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void reiko_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void reiko_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void reiko_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void reiko_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void reiko_entry_retail_script_004e6890(Fighter *fighter) {
  retail_script_004e6890(fighter);
}

void reiko_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_script_004e68a8(fighter);
}

void reiko_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void reiko_entry_retail_script_004e9b40(Fighter *fighter) {
  retail_script_004e9b40(fighter);
}

void reiko_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_script_004e9bd0(fighter);
}

const mmove_t *reiko_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_phase;
}

void reiko_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void reiko_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void reiko_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void reiko_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void reiko_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void reiko_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void reiko_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void reiko_entry_retail_script_004ea040(Fighter *fighter) {
  retail_script_004ea040(fighter);
}

void reiko_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_script_004ea0b8(fighter);
}

void reiko_entry_retail_script_004e0138(Fighter *fighter) {
  retail_script_004e0138(fighter);
}

void reiko_entry_retail_script_004e82b8(Fighter *fighter) {
  retail_script_004e82b8(fighter);
}

void reiko_entry_retail_script_004e82e0(Fighter *fighter) {
  retail_script_004e82e0(fighter);
}

void reiko_entry_retail_script_004e8310(Fighter *fighter) {
  retail_script_004e8310(fighter);
}

void reiko_entry_retail_script_004e8818(Fighter *fighter) {
  retail_script_004e8818(fighter);
}

void reiko_entry_retail_script_004e9880(Fighter *fighter) {
  retail_script_004e9880(fighter);
}

void reiko_entry_retail_script_004ed038(Fighter *fighter) {
  retail_script_004ed038(fighter);
}

void reiko_entry_retail_script_004e97f8(Fighter *fighter) {
  retail_script_004e97f8(fighter);
}

void reiko_entry_retail_script_004e2cf0(Fighter *fighter) {
  retail_script_004e2cf0(fighter);
}

void reiko_entry_retail_script_004e2d18(Fighter *fighter) {
  retail_script_004e2d18(fighter);
}

void reiko_entry_retail_script_004e2d28(Fighter *fighter) {
  retail_script_004e2d28(fighter);
}

void reiko_entry_retail_script_004e2d38(Fighter *fighter) {
  retail_script_004e2d38(fighter);
}

void reiko_entry_retail_script_004e2d48(Fighter *fighter) {
  retail_script_004e2d48(fighter);
}

void reiko_entry_retail_script_004e2d58(Fighter *fighter) {
  retail_script_004e2d58(fighter);
}

void reiko_entry_retail_script_004e2d68(Fighter *fighter) {
  retail_script_004e2d68(fighter);
}

void reiko_entry_retail_script_004e2d78(Fighter *fighter) {
  retail_script_004e2d78(fighter);
}

void reiko_entry_retail_script_004e2d88(Fighter *fighter) {
  retail_script_004e2d88(fighter);
}

void reiko_entry_retail_script_004e2dc0(Fighter *fighter) {
  retail_script_004e2dc0(fighter);
}

void reiko_entry_retail_script_004e2dd0(Fighter *fighter) {
  retail_script_004e2dd0(fighter);
}

void reiko_entry_retail_script_004e2de0(Fighter *fighter) {
  retail_script_004e2de0(fighter);
}

void reiko_entry_retail_script_004e2df8(Fighter *fighter) {
  retail_script_004e2df8(fighter);
}

void reiko_entry_retail_script_004e2e08(Fighter *fighter) {
  retail_script_004e2e08(fighter);
}

void reiko_entry_retail_script_004e2e18(Fighter *fighter) {
  retail_script_004e2e18(fighter);
}

void reiko_entry_retail_script_004e2e40(Fighter *fighter) {
  retail_script_004e2e40(fighter);
}

void reiko_entry_retail_script_004e2e50(Fighter *fighter) {
  retail_script_004e2e50(fighter);
}

void reiko_entry_retail_script_004d2d10(Fighter *fighter) {
  retail_script_004d2d10(fighter);
}

void reiko_entry_retail_script_004d2d60(Fighter *fighter) {
  retail_script_004d2d60(fighter);
}

void reiko_entry_retail_script_004d2d90(Fighter *fighter) {
  retail_script_004d2d90(fighter);
}

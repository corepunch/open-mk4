#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 267, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH};
static const MoveDef s_throw = {.clip = 78, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 120, .last = MOVE_END, .flags = SF_HOLD};

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_uppercut, [BUTTON_HIGH_KICK] = press_sweep, [BUTTON_LOW_KICK] = press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable straight_air = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable air_punch_followup = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp}};
static const ButtonTable diagonal_air = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable air_followup = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
// clang-format on

static const Controls quan_chi_controls = {NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping}};

static const FighterFn enter[S_COUNT] = {
    [S_PUNCH] = quan_chi_jab,
    [S_KICK] = quan_chi_high_kick,
    [S_SWEEP] = quan_chi_sweep,
    [S_UPPERCUT] = quan_chi_uppercut,
    [S_THROW] = quan_chi_lp_throw,
};

static const uint32_t quan_chi_weapon_descriptor[] = {0x6f, 0x1430c4, 0x144356, 0x13b312, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffffb334};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_high_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_quan_chi = {GAME_MK4, 8, "QUAN CHI", &mk4_motion, moves, S_COUNT, &quan_chi_controls, enter, -78643, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/fan.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"

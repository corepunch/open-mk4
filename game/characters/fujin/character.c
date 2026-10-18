#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_retail_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {65, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD | SF_LOW};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {81, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {120, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Retail inventory descriptor 0x4ece98, preserved as encoded EXE words. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t fujin_weapon_descriptor[] = {0x68, 0x141b6b, 0x142dc4, 0x13b360, 0x13edd5, 0x13eea4, 0x13eea7, 0xffffcccd};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static void fujin_jab(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable fujin_standing = {{[BUTTON_HIGH_PUNCH] = fujin_press_punch, [BUTTON_LOW_PUNCH] = fujin_press_low_punch, [BUTTON_HIGH_KICK] = fujin_press_kick, [BUTTON_LOW_KICK] = fujin_press_low_kick}};
static const ButtonTable fujin_crouching = {{[BUTTON_HIGH_PUNCH] = fujin_press_uppercut, [BUTTON_LOW_PUNCH] = fujin_press_uppercut, [BUTTON_HIGH_KICK] = fujin_press_sweep, [BUTTON_LOW_KICK] = fujin_press_sweep}};
static const ButtonTable fujin_jumping = {{[BUTTON_HIGH_PUNCH] = fujin_press_air_punch, [BUTTON_LOW_PUNCH] = fujin_press_air_lp, [BUTTON_HIGH_KICK] = fujin_press_air_hk, [BUTTON_LOW_KICK] = fujin_press_air_lk}};
// clang-format on

static const Controls fujin_controls = {NULL, 0, {[STANCE_STANDING] = &fujin_standing, [STANCE_CROUCHING] = &fujin_crouching, [STANCE_JUMP] = &fujin_jumping}};

const CharacterDef mk4_fujin = {GAME_MK4, 5, "FUJIN", &mk4_motion, moves, S_COUNT, &fujin_controls, NULL, -73531, mk4_walk, mk4_jump, NULL};

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

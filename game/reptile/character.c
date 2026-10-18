#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {268, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {76, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_reaction_40 = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {121, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Reptile's retail inventory descriptor at 0x4ece78, preserved as encoded words. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reptile_weapon_descriptor[] = {0x67, 0x141b64, 0x142dc1, 0x13b35a, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffff999a};
// clang-format on

/* Retail D[23] at 0x4ec038; the 0x47a6d0 callback queries it during its loop. */
static const AttackRecord reptile_script_contact = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 158, 158, 165, 0, 0x1eb8};
/* Retail D[21] at 0x4ec028; callback 0x47b260 probes it during recovery. */
static const AttackRecord reptile_loop_contact = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 40, 10, 9, 0, 0x170a};

enum {
  REPTILE_EVENT_VISIBILITY = 1,
  REPTILE_EVENT_TASK_76
};

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable reptile_standing = {{[BUTTON_HIGH_PUNCH] = reptile_press_punch, [BUTTON_LOW_PUNCH] = reptile_press_low_punch, [BUTTON_HIGH_KICK] = reptile_press_kick, [BUTTON_LOW_KICK] = reptile_press_low_kick}};
static const ButtonTable reptile_crouching = {{[BUTTON_HIGH_PUNCH] = reptile_press_uppercut, [BUTTON_LOW_PUNCH] = reptile_press_uppercut, [BUTTON_HIGH_KICK] = reptile_press_sweep, [BUTTON_LOW_KICK] = reptile_press_sweep}};
static const ButtonTable reptile_jumping = {{[BUTTON_HIGH_PUNCH] = reptile_press_air_hp, [BUTTON_LOW_PUNCH] = reptile_press_air_lp, [BUTTON_HIGH_KICK] = reptile_press_air_hk, [BUTTON_LOW_KICK] = reptile_press_air_lk}};
// clang-format on

static const Controls reptile_controls = {
    NULL, 0, {[STANCE_STANDING] = &reptile_standing, [STANCE_CROUCHING] = &reptile_crouching, [STANCE_JUMP] = &reptile_jumping}};

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

const CharacterDef mk4_reptile = {GAME_MK4, 10, "REPTILE", &mk4_motion, moves, S_COUNT, &reptile_controls, NULL, -73531, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/clone.inc"
#include "moves/common.inc"
#include "moves/fan.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"

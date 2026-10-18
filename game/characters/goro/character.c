#include "character.h"

static void goro_script_lk_return(Fighter *fighter);
static void goro_script_lk_finish(Fighter *fighter);
static void goro_script_lp_pair_start(Fighter *fighter);
static void goro_script_lp_phase_repeat(Fighter *fighter);
static void goro_script_event_finish(Fighter *fighter);
static void goro_event_progress(Fighter *fighter);
static void goro_event_counter_kick(Fighter *fighter);
static void goro_event_counter_sweep(Fighter *fighter);
static void goro_cpu_projectile_spawn(Fighter *fighter, int task_type);
static const MoveDef s_knockdown;

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Encoded Goro roster descriptor retained from the EXE inventory at 0x4ecf18. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t goro_retail_weapon_descriptor[] = {0x6c, 0x141bf0, 0x142dd3, 0x13b330, 0x13eeb6, 0x13ee98, 0x13ee9b, 0x0};
// clang-format on

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {590, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {603, 18, 0, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {602, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {597, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {598, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {601, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {600, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {604, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_cpu_projectile = {.clip = 592, .last = 24, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_block = {595, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {593, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {581, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {594, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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

/* Portable task83 fallback uses a known Goro box while its retail payload remains unavailable. */
static const AttackRecord goro_cpu_projectile_record = {0x12147, 0x51e, 0x9999, 0x4ccc, 23, 23, 4, ATTACK_MARK, 0x1999};

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable goro_standing = {{[BUTTON_HIGH_PUNCH] = goro_enter_jab, [BUTTON_LOW_PUNCH] = goro_enter_low_punch, [BUTTON_HIGH_KICK] = goro_enter_high_kick, [BUTTON_LOW_KICK] = goro_enter_low_kick}};
static const ButtonTable goro_crouching = {{[BUTTON_HIGH_PUNCH] = goro_enter_uppercut, [BUTTON_LOW_PUNCH] = goro_enter_uppercut, [BUTTON_HIGH_KICK] = goro_enter_sweep, [BUTTON_LOW_KICK] = goro_enter_sweep}};
static const ButtonTable goro_jumping = {{[BUTTON_HIGH_PUNCH] = goro_enter_air_hp, [BUTTON_LOW_PUNCH] = goro_enter_air_lp, [BUTTON_HIGH_KICK] = goro_enter_air_hk, [BUTTON_LOW_KICK] = goro_enter_air_lk}};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef goro_commands[] = {
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, goro_air_throw},
};
// clang-format on

static const Controls goro_controls = {
    goro_commands, 1, {[STANCE_STANDING] = &goro_standing, [STANCE_CROUCHING] = &goro_crouching, [STANCE_JUMP] = &goro_jumping}};

static const CpuMoves goro_cpu_moves = {.enter = {[CPU_PROJECTILE] = goro_cpu_projectile}};

const CharacterDef mk4_goro = {GAME_MK4, 15, "GORO", &mk4_motion, moves, S_COUNT, &goro_controls, NULL, -85458, mk4_walk, NULL, &goro_cpu_moves};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/fan.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"

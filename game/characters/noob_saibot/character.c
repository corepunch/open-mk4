#include "character.h"

static void noob_saibot_script_air_throw_continue(Fighter *fighter);
static void noob_saibot_script_air_throw_end(Fighter *fighter);

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {78, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_noob_air_throw = {.clip = 74, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_noob_air_thrown = {.clip = 150, .last = 27, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_noob_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_noob_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {113, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
const uint32_t noob_saibot_weapon_descriptor[] = {0x69, 0x141b95, 0x142dca, 0x13b33c, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffb334};

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

void noob_saibot_command_498b70(Fighter *fighter);
void noob_saibot_command_498e70(Fighter *fighter);

static const CommandDef noob_saibot_commands[] = {
    {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0, NULL, noob_saibot_command_498b70},
    {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, CMD_AIR, noob_saibot_shadow_up_enabled, noob_saibot_command_498e70},
};

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable noob_saibot_standing = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_high_punch, [BUTTON_LOW_PUNCH] = noob_saibot_press_low_punch, [BUTTON_HIGH_KICK] = noob_saibot_press_high_kick, [BUTTON_LOW_KICK] = noob_saibot_press_low_kick}};
static const ButtonTable noob_saibot_crouching = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_crouching_hp, [BUTTON_LOW_PUNCH] = noob_saibot_press_crouching_hp, [BUTTON_HIGH_KICK] = noob_saibot_press_crouching_kick, [BUTTON_LOW_KICK] = noob_saibot_press_crouching_kick}};
static const ButtonTable noob_saibot_air = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_air_hp, [BUTTON_LOW_PUNCH] = noob_saibot_press_air_lp, [BUTTON_HIGH_KICK] = noob_saibot_press_air_hk, [BUTTON_LOW_KICK] = noob_saibot_press_air_lk}};
// clang-format on

static const Controls noob_saibot_controls = {noob_saibot_commands, (int)(sizeof noob_saibot_commands / sizeof noob_saibot_commands[0]),
    {[STANCE_STANDING] = &noob_saibot_standing, [STANCE_CROUCHING] = &noob_saibot_crouching, [STANCE_JUMP] = &noob_saibot_air}};

const CharacterDef mk4_noob_saibot = {GAME_MK4, 17, "NOOB SAIBOT", &mk4_motion, moves, S_COUNT, &noob_saibot_controls, NULL, -77660, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"

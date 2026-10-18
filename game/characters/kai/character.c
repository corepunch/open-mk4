#include "character.h"

/* Kai's move clips and contact timing come from the slot-11 retail tables.
 * Script bodies are emitted from game/characters/kai/port.json. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {265, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_high_kick = {71, 0, MOVE_END, 10, 5, 18, 12, 28, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_low_kick = {70, 0, MOVE_END, 9, 5, 18, 12, 28, 1.55f, 0.27f, SF_ATTACK | SF_LOCK | SF_LOW, NULL, 0};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 44, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 20, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {37, 0, MOVE_END, 6, 2, 25, 14, 66, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_thrown = {157, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_attack = {61, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {122, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_stage_event_clip = {.clip = 0, .last = 0, .flags = SF_LOCK};
static const AttackRecord s_kai_attack_b9 = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};
static const AttackRecord s_kai_attack_b10 = {0x1851e, -0xbd2f, 0x10000, 0x9958, 127, 127, 5, 0, 0x1c28};

#include "generated/scripts.inc"
#include "generated/frames.inc"

static void kai_high_kick(Fighter *fighter);
static void kai_uppercut(Fighter *fighter);
static void kai_air_hp(Fighter *fighter);
static void kai_air_lp(Fighter *fighter);
static void kai_air_kick(Fighter *fighter);
static void kai_air_diagonal_lk(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable kai_standing = {{[BUTTON_HIGH_PUNCH] = kai_press_high_punch, [BUTTON_LOW_PUNCH] = kai_press_low_punch, [BUTTON_HIGH_KICK] = kai_press_high_kick, [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const ButtonTable kai_crouching = {{[BUTTON_HIGH_PUNCH] = kai_press_uppercut, [BUTTON_LOW_PUNCH] = kai_press_uppercut, [BUTTON_HIGH_KICK] = kai_press_sweep, [BUTTON_LOW_KICK] = kai_press_sweep}};
static const ButtonTable kai_jumping = {{[BUTTON_HIGH_PUNCH] = kai_press_air_hp, [BUTTON_LOW_PUNCH] = kai_press_air_lp, [BUTTON_HIGH_KICK] = kai_press_air_hk, [BUTTON_LOW_KICK] = kai_press_air_lk}};
// clang-format on

static const Controls kai_controls = {NULL, 0, {[STANCE_STANDING] = &kai_standing, [STANCE_CROUCHING] = &kai_crouching, [STANCE_JUMP] = &kai_jumping}};

enum {
  KAI_STAGE_EVENT_PLAY_CLIP = 1,
  KAI_STAGE_EVENT_SETTLE
};

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = kai_idle,
    [S_WALK] = kai_walk,
    [S_CROUCH] = kai_crouch,
    [S_JUMP] = NULL,
    [S_PUNCH] = kai_jab,
    [S_KICK] = kai_high_kick,
    [S_SWEEP] = kai_sweep,
    [S_UPPERCUT] = kai_uppercut,
    [S_THROW] = kai_lp_throw,
    [S_BLOCK] = kai_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = kai_ready,
};

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

/* Encoded Kai weapon descriptor 0x4ecef8; attack moves remain routed through
 * the selected weapon's native inventory entry until their strike records
 * are ported. */
const uint32_t kai_weapon_descriptor[] = {0x63, 0x141b33, 0x142daf, 0x13b330, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffc000};

const CharacterDef mk4_kai = {GAME_MK4, 11, "KAI", &mk4_motion, moves, S_COUNT, &kai_controls, enter, -74829, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/fan.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"

#include "game.h"
#include "mk4_characters.h"

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void scorpion_ready(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, fighter->height > 0.f ? S_JUMP : S_IDLE);
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveFrame kick_frames[] = {
  {0, NULL},
  {1, NULL},
  {2, NULL},
  {4, NULL},
  {6, NULL},
  {8, NULL},
  {10, NULL},
  {12, NULL},
  {14, NULL},
  {16, NULL},
  {17, NULL},
  {17, NULL},
  {17, NULL},
  {16, NULL},
  {16, NULL},
  {15, NULL},
  {13, NULL},
  {12, NULL},
  {11, NULL},
  {10, NULL},
  {9, NULL},
  {8, NULL},
  {7, NULL},
  {6, NULL},
  {5, NULL},
  {4, NULL},
  {3, NULL},
  {2, NULL},
  {1, NULL},
  {0, NULL},
};
// clang-format on

static const MoveSequence kick_sequence = {
    kick_frames, (int)(sizeof kick_frames / sizeof kick_frames[0]), scorpion_ready};

/* Damage, stun, reach, push and other moves remain the native approximation.
 * Kick contact queries are steps 11..13 in the original no-contact trace. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_jab = {
    294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL};
static const MoveDef s_kick = {
    66, 0, MOVE_END, 11, 3, 16, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, &kick_sequence};
static const MoveDef s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f,
    SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL};
static const MoveDef s_uppercut = {
    293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL};
static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f,
    SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL};
static const MoveDef s_hit = {
    247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL};
static const MoveDef s_knockdown = {
    186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL};

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

static void scorpion_jab(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_jab);
}

static void scorpion_kick(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KICK, &s_kick);
}

static const FighterFn enter[S_COUNT] = {
    [S_PUNCH] = scorpion_jab,
    [S_KICK] = scorpion_kick,
};

const CharacterDef mk4_scorpion = {
    GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT, enter};

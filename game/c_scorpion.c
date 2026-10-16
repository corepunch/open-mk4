#include "game.h"
#include "mk4_characters.h"

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void scorpion_ready(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, fighter->height > 0.f ? S_JUMP : S_IDLE);
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment kick_segments[] = {
  {0, 0, 1, 1},
  {3, 4, 2, 1},
  {10, 17, 0, 1},
  {13, 16, -1, 2},
  {16, 13, -1, 1},
};
// clang-format on

static const MoveSequence kick_sequence = {kick_segments, (int)(sizeof kick_segments / sizeof kick_segments[0]), NULL, 0, 30, scorpion_ready};

static void scorpion_jab_next(Battle *battle, Fighter *fighter);
static void scorpion_jab_second_end(Battle *battle, Fighter *fighter);
static void scorpion_jab_finish_end(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_loop_end(Battle *battle, Fighter *fighter);

static void scorpion_contact(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter->contact_ready = true;
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment jab_segments[] = {
  {0, 0, 1, 1},
  {8, 7, 0, 1},
};
static const MoveSegment jab_recover_segments[] = {
  {0, 9, 2, 1},
  {3, 17, 0, 1},
  {5, 19, 2, 1},
};
static const MoveSegment jab_second_segments[] = {{0, 0, 1, 1}};
static const MoveSegment jab_stop_segments[] = {{0, 10, 1, 1}};
static const MoveSegment jab_finish_segments[] = {
  {0, 11, 1, 1},
  {8, 18, 0, 1},
};
static const MoveSegment sweep_segments[] = {
  {0, 0, 2, 1},
  {4, 10, 0, 1},
  {5, 10, 1, 1},
};
static const MoveSegment uppercut_segments[] = {
  {0, 0, 1, 1},
  {6, 5, 0, 1},
  {7, 6, 1, 1},
};
static const MoveSegment uppercut_loop_segments[] = {
  {0, 8, -1, 1},
  {3, 7, 1, 1},
};
static const MoveSegment uppercut_late_segments[] = {
  {0, 9, 0, 1},
  {2, 8, -1, 1},
  {5, 7, 1, 1},
};
static const MoveSegment uppercut_miss_end_segments[] = {{0, 8, 1, 1}};
static const MoveSegment uppercut_hit_end_segments[] = {{0, 9, 1, 1}};
static const MoveEvent contact_event[] = {{0, scorpion_contact}};
// clang-format on

#define SEQUENCE(segments, count, end) {segments, (int)(sizeof segments / sizeof segments[0]), NULL, 0, count, end}
#define CONTACT_SEQUENCE(segments, count, end) {segments, (int)(sizeof segments / sizeof segments[0]), contact_event, 1, count, end}

static const MoveSequence jab_sequence = SEQUENCE(jab_segments, 9, scorpion_jab_next);
static const MoveSequence jab_recover_sequence = SEQUENCE(jab_recover_segments, 7, scorpion_ready);
static const MoveSequence jab_second_sequence = SEQUENCE(jab_second_segments, 10, scorpion_jab_second_end);
static const MoveSequence jab_stop_sequence = CONTACT_SEQUENCE(jab_stop_segments, 3, scorpion_ready);
static const MoveSequence jab_finish_sequence = CONTACT_SEQUENCE(jab_finish_segments, 9, scorpion_jab_finish_end);
static const MoveSequence sweep_sequence = SEQUENCE(sweep_segments, 41, scorpion_ready);
static const MoveSequence uppercut_sequence = SEQUENCE(uppercut_segments, 10, scorpion_uppercut_recover);
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, scorpion_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, scorpion_ready);

/* Damage, stun, reach, push and other moves remain the native approximation.
 * Kick contact queries are steps 11..13 in the original no-contact trace. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL};
static const MoveDef s_jab = {294, 0, MOVE_END, 8, 1, 7, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, &jab_sequence};
static const MoveDef s_kick = {66, 0, MOVE_END, 11, 3, 16, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, &kick_sequence};
static const MoveDef s_sweep = {63, 0, MOVE_END, 18, 2, 21, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, &sweep_sequence};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 7, 3, 27, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, &uppercut_sequence};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */
static const MoveDef s_jab_recover = {.clip = 294, .flags = SF_LOCK, .sequence = &jab_recover_sequence};
static const MoveDef s_jab_second = {.clip = 295, .flags = SF_LOCK, .sequence = &jab_second_sequence};
static const MoveDef s_jab_stop = {295, 0, MOVE_END, 0, 0, 0, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK | SF_SCRIPT_CONTACT, &jab_stop_sequence};
static const MoveDef s_jab_finish = {295, 0, MOVE_END, 0, 0, 0, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK | SF_SCRIPT_CONTACT, &jab_finish_sequence};
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = SF_LOCK, .sequence = &uppercut_loop_sequence};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = SF_LOCK, .sequence = &uppercut_late_sequence};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = SF_LOCK, .sequence = &uppercut_miss_end_sequence};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = SF_LOCK, .sequence = &uppercut_hit_end_sequence};

static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL};
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

static bool scorpion_repeat_jab(const Battle *battle, const Fighter *fighter) {
  const Fighter *other = fighter == &battle->fighters[0] ? &battle->fighters[1] : &battle->fighters[0];
  return (fighter->buffered & BUTTON_BIT(BUTTON_PUNCH)) != 0 && (other->x - fighter->x) * (float)fighter->facing >= 0.f;
}

static void scorpion_jab_next(Battle *battle, Fighter *fighter) {
  const MoveDef *next = scorpion_repeat_jab(battle, fighter) ? &s_jab_second : &s_jab_recover;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void scorpion_jab_second_end(Battle *battle, Fighter *fighter) {
  const MoveDef *next = scorpion_repeat_jab(battle, fighter) ? &s_jab_finish : &s_jab_stop;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void scorpion_jab_finish_end(Battle *battle, Fighter *fighter) {
  (void)battle;
  /* Native 0x46ddd0 queries contact before 0x46de60 consumes the next press.
   * Preserve this strike's latch until resolution, then select the next phase. */
  fighter->contact_ready = true;
  fighter->after_contact = scorpion_jab_next;
}

static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter) {
  bool late = fighter->contact_tick == 9;
  ContactResult result = fighter->contact;
  fighter->move_loops = result == CONTACT_MISS ? 3 : result == CONTACT_BLOCK ? 2 : 8;
  fighter_start_move(battle, fighter, S_UPPERCUT, late ? &s_uppercut_late : &s_uppercut_loop);
  /* Only the no-contact path holds sample 8 before final forward recovery. */
  fighter->contact = result;
}

static void scorpion_uppercut_loop_end(Battle *battle, Fighter *fighter) {
  ContactResult result = fighter->contact;
  const MoveDef *next;
  fighter->move_loops--;
  if (fighter->move_loops > 0) {
    next = &s_uppercut_loop;
  } else {
    next = result == CONTACT_MISS ? &s_uppercut_miss_end : &s_uppercut_hit_end;
  }
  fighter_start_move(battle, fighter, S_UPPERCUT, next);
  fighter->contact = result;
}

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

const CharacterDef mk4_scorpion = {GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT, enter};

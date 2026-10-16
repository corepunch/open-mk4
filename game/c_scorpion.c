#include "game.h"
#include "mk4_characters.h"

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void scorpion_ready(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, fighter_airborne(fighter) ? S_JUMP : S_IDLE);
}

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord jab_record = {0xf333, -0x47ae, 0xcccc, 0x7333, 37, 11, 1, 0, 0xa3d};            /* 0x4ebd38, query A selector 1 */
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Battle *battle, const Fighter *fighter) {
  return fighter_opponent(battle, fighter)->action;
}

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate reads victim +0x5c, which is not modeled. */
static void scorpion_jab_contact(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int action = victim->action;
  if (fighter->connected || action == 0x2001 || action == 0x106 || action == 0x107) {
    return;
  }
  if (action == 0x10c && fighter_sample(victim) < 5) {
    return;
  }
  battle_contact(battle, fighter, &jab_record);
}

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void scorpion_kick_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected && victim_action(battle, fighter) != 0x2001) {
    battle_contact(battle, fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void scorpion_sweep_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(battle, fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void scorpion_uppercut_contact(Battle *battle, Fighter *fighter) {
  if (fighter->connected || victim_action(battle, fighter) == 0x2001) {
    return;
  }
  if (battle_contact(battle, fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveEvent kick_events[] = {{11, scorpion_kick_contact}, {12, scorpion_kick_contact}, {13, scorpion_kick_contact}};
static const MoveSegment kick_segments[] = {
  {0, 0, 1, 1},
  {3, 4, 2, 1},
  {10, 17, 0, 1},
  {13, 16, -1, 2},
  {16, 13, -1, 1},
};
// clang-format on

static const MoveSequence kick_sequence = {kick_segments, (int)(sizeof kick_segments / sizeof kick_segments[0]), kick_events, 3, 30, scorpion_ready, NULL};

static void scorpion_jab_next(Battle *battle, Fighter *fighter);
static void scorpion_jab_second_end(Battle *battle, Fighter *fighter);
static void scorpion_jab_finish_end(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_loop_end(Battle *battle, Fighter *fighter);

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
static const MoveEvent jab_events[] = {{8, scorpion_jab_contact}};
static const MoveEvent jab_strike_events[] = {{0, scorpion_jab_contact}};
static const MoveEvent sweep_events[] = {{18, scorpion_sweep_contact}, {19, scorpion_sweep_contact}};
static const MoveEvent uppercut_events[] = {{7, scorpion_uppercut_contact}, {8, scorpion_uppercut_contact}, {9, scorpion_uppercut_contact}};
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
#define EVENT_SEQUENCE(segments, events, count, end) {segments, COUNT(segments), events, COUNT(events), count, end, NULL}

static const MoveSequence jab_sequence = EVENT_SEQUENCE(jab_segments, jab_events, 9, scorpion_jab_next);
static const MoveSequence jab_recover_sequence = SEQUENCE(jab_recover_segments, 7, scorpion_ready);
static const MoveSequence jab_second_sequence = SEQUENCE(jab_second_segments, 10, scorpion_jab_second_end);
static const MoveSequence jab_stop_sequence = EVENT_SEQUENCE(jab_stop_segments, jab_strike_events, 3, scorpion_ready);
static const MoveSequence jab_finish_sequence = EVENT_SEQUENCE(jab_finish_segments, jab_strike_events, 9, scorpion_jab_finish_end);
static const MoveSequence sweep_sequence = EVENT_SEQUENCE(sweep_segments, sweep_events, 41, scorpion_ready);
static const MoveSequence uppercut_sequence = EVENT_SEQUENCE(uppercut_segments, uppercut_events, 10, scorpion_uppercut_recover);
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, scorpion_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, scorpion_ready);

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .sequence = &jab_sequence, .action = 0x10b};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .sequence = &kick_sequence, .action = 0x100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .sequence = &sweep_sequence, .action = 0x10c};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .sequence = &uppercut_sequence, .action = 0x109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */
static const MoveDef s_jab_recover = {.clip = 294, .flags = RETAIL, .sequence = &jab_recover_sequence, .action = 0x10b};
static const MoveDef s_jab_second = {.clip = 295, .flags = RETAIL, .sequence = &jab_second_sequence, .action = 0x10b};
static const MoveDef s_jab_stop = {.clip = 295, .flags = RETAIL, .sequence = &jab_stop_sequence, .action = 0x10b};
static const MoveDef s_jab_finish = {.clip = 295, .flags = RETAIL, .sequence = &jab_finish_sequence, .action = 0x10b};
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_loop_sequence, .action = 0x109};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_late_sequence, .action = 0x109};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_miss_end_sequence, .action = 0x109};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_hit_end_sequence, .action = 0x109};

static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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
  const Fighter *other = fighter_opponent(battle, fighter);
  q16 dx = other->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  return (fighter->buffered & BUTTON_BIT(BUTTON_LOW_PUNCH)) != 0 && (fighter->facing > 0 ? dx >= 0 : dx <= 0);
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
  /* Native 0x46ddd0 queries contact before 0x46de60 consumes the next press. */
  scorpion_jab_contact(battle, fighter);
  scorpion_jab_next(battle, fighter);
}

static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter) {
  bool late = fighter->contact_tick == 9;
  ContactResult result = fighter->contact;
  fighter->move_loops = result == CONTACT_MISS ? 3 : result == CONTACT_BLOCK ? 2 : 8;
  fighter_start_move(battle, fighter, S_UPPERCUT, late ? &s_uppercut_late : &s_uppercut_loop);
  /* Only the no-contact path holds sample 8 before final forward recovery. */
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
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
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

static void scorpion_jab(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
}

static void scorpion_kick(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KICK, &s_kick);
}

static const FighterFn enter[S_COUNT] = {
    [S_PUNCH] = scorpion_jab,
    [S_KICK] = scorpion_kick,
};

const CharacterDef mk4_scorpion = {GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, &mk4_controls, enter, -76873};

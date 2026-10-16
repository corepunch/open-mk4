#include "game.h"
#include "math_util.h"

/* Shared reaction phases used by script attack records. Source addresses
 * are in docs/scorpion/combat-core.md. These are our C continuations, not
 * an emulation of the retail task scheduler. Goro retains his own clips. */

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define PHASE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
#define PHASE_UNTIL(segments, count, end, until) {segments, COUNT(segments), NULL, 0, count, end, until}

enum {
  FRICTION = 0xe666,    /* 0x4913a0: velocity *= 0.9 per world pump */
  FRICTION_STOP = 0x41, /* stop once vx*vx + vz*vz <= this */
  ACTION_GETUP = 0x200e,
  ACTION_BLOCK = 0x2005
};

static void neutral(Battle *battle, Fighter *fighter) {
  if (fighter->health > 0) {
    fighter_enter_state(battle, fighter, S_IDLE);
  }
}

static bool landed(const Battle *battle, const Fighter *fighter) {
  (void)battle;
  return !fighter_airborne(fighter) && fighter->body.gravity == 0;
}

static bool never(const Battle *battle, const Fighter *fighter) {
  (void)battle;
  (void)fighter;
  return false;
}

/* 0x4913a0 group callback. */
static void friction(Battle *battle, Fighter *fighter) {
  (void)battle;
  q16 *v = fighter->body.vel;
  v[AXIS_X] = q16_mul(v[AXIS_X], FRICTION);
  v[AXIS_Z] = q16_mul(v[AXIS_Z], FRICTION);
  if (q16_mul(v[AXIS_X], v[AXIS_X]) + q16_mul(v[AXIS_Z], v[AXIS_Z]) <= FRICTION_STOP) {
    v[AXIS_X] = 0;
    v[AXIS_Z] = 0;
    fighter->body.physics = NULL;
  }
}

static void stop_sliding(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter->body.vel[AXIS_X] = 0;
  fighter->body.vel[AXIS_Z] = 0;
}

static void slide(Battle *battle, Fighter *fighter, q16 speed) {
  fighter_set_speed(battle, fighter, -speed);
  fighter->body.physics = friction;
}

static void launch(Battle *battle, Fighter *fighter, q16 speed, q16 vy, q16 gravity) {
  fighter_set_speed(battle, fighter, -speed);
  fighter->body.vel[AXIS_Y] = vy;
  fighter->body.gravity = gravity;
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment jab_hit_segments[] = {{0, 4, 1, 1}, {9, 14, 2, 1}};
static const MoveSegment stagger_segments[] = {{0, 0, 2, 1}, {18, 38, 0, 1}, {20, 40, 2, 1}, {24, 50, 0, 1}};
static const MoveSegment kick_hit_segments[] = {{0, 4, 0, 1}, {13, 6, 1, 1}};
static const MoveSegment block_short_segments[] = {{0, 3, 2, 1}, {4, 7, -2, 1}};
static const MoveSegment block_long_segments[] = {{0, 3, 2, 1}, {4, 7, -2, 1}, {7, 3, 2, 1}, {11, 7, -2, 1}};
static const MoveSegment forward_segments[] = {{0, 0, 1, 1}};
static const MoveSegment jab_air_land_segments[] = {{0, 31, 1, 1}};
static const MoveSegment uppercut_land_segments[] = {{0, 53, 0, 1}, {1, 55, 2, 1}};
static const MoveSegment uppercut_ko_segments[] = {{0, 53, 0, 1}, {1, 55, 2, 1}, {28, 109, 0, 1}};
static const MoveEvent stagger_events[] = {{19, stop_sliding}};
// clang-format on

static void sweep_rise(Battle *battle, Fighter *fighter);
static void uppercut_land(Battle *battle, Fighter *fighter);
static void uppercut_rise(Battle *battle, Fighter *fighter);
static void kick_air_rise(Battle *battle, Fighter *fighter);
static void jab_air_land(Battle *battle, Fighter *fighter);
static void jab_air_rise(Battle *battle, Fighter *fighter);

static const MoveSequence jab_hit_sequence = PHASE(jab_hit_segments, 12, neutral);
static const MoveSequence stagger_sequence = {stagger_segments, COUNT(stagger_segments), stagger_events, 1, 25, neutral, NULL};
static const MoveSequence kick_hit_sequence = PHASE(kick_hit_segments, 26, neutral);
static const MoveSequence block_short_sequence = PHASE(block_short_segments, 7, neutral);
static const MoveSequence block_long_sequence = PHASE(block_long_segments, 14, neutral);
static const MoveSequence sweep_fall_sequence = PHASE(forward_segments, 29, sweep_rise);
static const MoveSequence sweep_rise_sequence = PHASE(forward_segments, 29, neutral);
static const MoveSequence uppercut_fall_sequence = PHASE_UNTIL(forward_segments, 53, uppercut_land, landed);
static const MoveSequence uppercut_land_sequence = PHASE(uppercut_land_segments, 28, uppercut_rise);
static const MoveSequence uppercut_ko_sequence = PHASE_UNTIL(uppercut_ko_segments, 29, neutral, never);
static const MoveSequence getup_sequence = PHASE(forward_segments, 15, neutral);
static const MoveSequence kick_air_sequence = PHASE_UNTIL(forward_segments, 30, kick_air_rise, landed);
static const MoveSequence air_getup_sequence = PHASE(forward_segments, 16, neutral);
static const MoveSequence jab_air_sequence = PHASE_UNTIL(forward_segments, 30, jab_air_land, landed);
static const MoveSequence jab_air_land_sequence = PHASE(jab_air_land_segments, 3, jab_air_rise);

#define REACTION(name, clip, flags, sequence, action) static const MoveDef name = {clip, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, flags, &sequence, action}
REACTION(jab_hit, 311, SF_LOCK, jab_hit_sequence, 0);
REACTION(stagger, 182, SF_LOCK, stagger_sequence, 0);
REACTION(kick_hit, 311, SF_LOCK, kick_hit_sequence, 0);
REACTION(block_short, 321, SF_LOCK, block_short_sequence, ACTION_BLOCK);
REACTION(block_long, 321, SF_LOCK, block_long_sequence, ACTION_BLOCK);
REACTION(sweep_fall, 195, SF_LOCK, sweep_fall_sequence, 0);
REACTION(sweep_rise_move, 197, SF_LOCK, sweep_rise_sequence, 0);
REACTION(uppercut_fall, 128, SF_LOCK, uppercut_fall_sequence, 0);
REACTION(uppercut_land_move, 128, SF_LOCK, uppercut_land_sequence, 0);
REACTION(uppercut_ko, 128, SF_LOCK, uppercut_ko_sequence, 0);
REACTION(uppercut_rise_move, 328, SF_LOCK, getup_sequence, ACTION_GETUP);
REACTION(kick_air, 189, SF_LOCK, kick_air_sequence, 0);
REACTION(kick_air_rise_move, 191, SF_LOCK, air_getup_sequence, ACTION_GETUP);
REACTION(jab_air, 183, SF_LOCK, jab_air_sequence, 0);
REACTION(jab_air_land_move, 183, SF_LOCK, jab_air_land_sequence, ACTION_GETUP);
REACTION(jab_air_rise_move, 185, SF_LOCK, air_getup_sequence, ACTION_GETUP);

static void sweep_rise(Battle *battle, Fighter *fighter) {
  stop_sliding(battle, fighter);
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &sweep_rise_move);
}

static void uppercut_land(Battle *battle, Fighter *fighter) {
  /* A KO victim stays down; 0x48fc20 ends the round instead of a getup. */
  fighter_start_move(battle, fighter, S_KNOCKDOWN, fighter->health == 0 ? &uppercut_ko : &uppercut_land_move);
}

static void uppercut_rise(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &uppercut_rise_move);
}

static void kick_air_rise(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &kick_air_rise_move);
}

static void jab_air_land(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &jab_air_land_move);
}

static void jab_air_rise(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &jab_air_rise_move);
}

/* 1: jab blocked. */
static void react_jab_block(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_BLOCK, &block_short);
  slide(battle, fighter, 0x3d70); /* 0.24 */
}

/* 4: sweep blocked. */
static void react_low_block(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_BLOCK, &block_long);
  slide(battle, fighter, 0x2666); /* 0.15 */
}

/* 6: kick or uppercut blocked. */
static void react_block(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_BLOCK, &block_long);
  slide(battle, fighter, 0x3333); /* 0.20 */
}

/* 11 (0x482070): jab air hit; air reactions scale later damage by 0.6. */
static void react_jab_air(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &jab_air);
  fighter->damage_scale = 0x9999;
  launch(battle, fighter, 0xccc, -0x23d7, 0x28f); /* 0.05, -0.14, 0.01 */
}

/* 13 (0x481d30): kick air hit. */
static void react_kick_air(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &kick_air);
  fighter->damage_scale = 0x9999;
  launch(battle, fighter, 0x11eb, -0x23d7, 0x28f); /* 0.07, -0.14, 0.01 */
}

/* 16 (0x482a10): kick hit. The stagger check 0x488ea0 is not ported. */
static void react_kick_hit(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_HIT, &kick_hit);
  slide(battle, fighter, 0x2666); /* 0.15 */
}

/* 18 (0x483050): sweep trip, also when airborne; Y is forced to the floor. */
static void react_sweep(Battle *battle, Fighter *fighter) {
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->body.vel[AXIS_Y] = 0;
  fighter->body.gravity = 0;
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &sweep_fall);
  fighter_set_speed(battle, fighter, -0xf5c); /* 0.06 */
}

/* 34 (0x4838e0): uppercut launch from the current height. */
static void react_uppercut(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &uppercut_fall);
  launch(battle, fighter, 0x7ae, -0x3333, 0x20c); /* 0.03, -0.20, 0.008 */
}

/* 37 (0x483b40): jab hit, or a stagger once 0.2 damage (0.15 for a CPU)
 * has accumulated, or always against Goro. */
static void react_jab_hit(Battle *battle, Fighter *fighter) {
  bool staggers = fighter->damage_total >= 0x3333 || (!fighter->human && fighter->damage_total >= 0x2666) || fighter->roster == 15;
  if (staggers) {
    fighter_start_move(battle, fighter, S_HIT, &stagger);
    fighter_set_speed(battle, fighter, -0x1999); /* 0.1 */
  } else {
    fighter_start_move(battle, fighter, S_HIT, &jab_hit);
  }
}

static void react_approximate(Battle *battle, Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
    fighter_enter_state(battle, fighter, S_KNOCKDOWN);
  } else {
    fighter_enter_state(battle, fighter, S_HIT);
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn reactions[256] = {
  [0] = react_block,
  [112] = react_block,
  [1] = react_jab_block,
  [4] = react_low_block,
  [6] = react_block,
  [11] = react_jab_air,
  [13] = react_kick_air,
  [16] = react_kick_hit,
  [18] = react_sweep,
  [34] = react_uppercut,
  [37] = react_jab_hit,
};
// clang-format on

/* 0x48f880: the victim's move is replaced at once; velocities, gravity and
 * its physics callback are cleared and it is marked as reacting. The
 * reaction starts on the victim's next update. */
void battle_react(Battle *battle, Fighter *victim, int reaction) {
  require(reaction >= 0 && reaction < COUNT(reactions), "Invalid reaction id");
  FighterFn entry = reactions[reaction] != NULL && victim->roster != 15 ? reactions[reaction] : react_approximate;
  (void)battle;
  for (int i = 0; i < 3; ++i) {
    victim->body.vel[i] = 0;
  }
  victim->body.gravity = 0;
  victim->body.physics = NULL;
  victim->body.on_land = NULL;
  fighter_interrupt(victim, entry);
  victim->flags |= FF_REACTING;
}

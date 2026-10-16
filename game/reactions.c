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
static const MoveSegment slide_crouch_block_segments[] = {{0, 0, 1, 1}, {5, 4, -1, 1}};
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
static const MoveSequence slide_crouch_block_sequence = PHASE(slide_crouch_block_segments, 10, neutral);
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
REACTION(slide_block, 321, SF_LOCK, block_long_sequence, ACTION_BLOCK);
REACTION(slide_crouch_block, 255, SF_LOCK, slide_crouch_block_sequence, ACTION_BLOCK);
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

/* Additional Scorpion record reactions. Each row names the decoded clip,
 * pose hold, landing boundary, motion and getup; no runtime script decoding.
 * Camera-relative tumble motion uses our pair axis and no camera roll. */
typedef struct ReactionSpec {
  MoveDef move;
  int hold_at, hold_ticks, land_at, getup;
  q16 speed, vy, gravity, scale;
  bool friction, no_guard;
} ReactionSpec;

#define R(c, n, a) {.clip = c, .last = n, .flags = SF_LOCK, .action = a}
// clang-format off: Keep table entries on one row, including long rows.
static const ReactionSpec extra_reactions[256] = {
  [10] = {R(183, 33, 0x4003), 0, 0, 30, 185, 0x11eb, -0x23d7, 0x28f, 0, false, false},
  [12] = {R(131, 79, 0x4003), 0, 0, 40, 328, 0xccc, -0x23d7, 0x20c, 0, false, false},
  [17] = {R(134, 17, 0x4001), 6, 8, -1, 0, 0x2666, 0, 0, 0, true, false},
  [21] = {R(311, 18, 0x4003), 4, 13, -1, 0, 0, 0, 0, 0x8000, false, true},
  [22] = {R(186, 19, 0x4003), 0, 0, 14, 188, 0x1999, 0, 0, 0, false, false},
  [23] = {R(131, 44, 0x4003), 0, 0, 41, 185, 0xf5c, -0x2b85, 0x28f, 0x9999, false, false},
  [25] = {R(311, 18, 0x4001), 4, 13, -1, 0, 0x2666, 0, 0, 0, true, false},
  [26] = {R(128, 109, 0x4003), 0, 0, 53, 0, 0x7ae, -0x3333, 0x20c, 0x6666, false, false},
  [27] = {R(134, 17, 0x4001), 6, 8, -1, 0, 0x51e, 0, 0, 0x9999, false, true},
  [28] = {R(134, 17, 0x4001), 0, 0, -1, 0, 0x1eb8, 0, 0, 0, true, false},
  [29] = {R(134, 17, 0x4001), 0, 0, -1, 0, 0x1eb8, 0, 0, 0, true, false},
  [32] = {R(324, 13, 0x4001), 0, 0, -1, 0, 0, 0, 0, 0, false, false},
  [38] = {R(311, 18, 0x4003), 4, 13, -1, 0, 0, 0, 0, 0x8000, false, true},
  [39] = {R(186, 19, 0x4003), 0, 0, 14, 188, 0x1999, 0, 0, 0, false, false},
  [43] = {R(311, 18, 0x4002), 4, 13, -1, 0, 0, 0, 0, 0x9999, false, true},
  [48] = {R(180, 19, 0x4001), 0, 6, -1, 0, 0x3333, 0, 0, 0, true, false},
  [49] = {R(131, 79, 0x4003), 0, 0, 40, 328, 0xa3d, -0x23d7, 0x24d, 0, false, false},
  [50] = {R(132, 59, 0x4003), 0, 0, 14, 328, 0x11eb, 0, 0, 0, false, false},
  [51] = {R(319, 82, 0x4003), 0, 0, 34, 328, -0x1999, -0x2147, 0x20c, 0, false, false},
  [52] = {R(181, 82, 0x4003), 0, 0, 51, 328, 0x7ae, -0x3851, 0x20c, 0x6666, false, false},
  [90] = {R(311, 18, 0x4001), 4, 13, -1, 0, 0x2666, 0, 0, 0, true, false},
  [99] = {R(326, 19, 0x4001), 0, 0, -1, 0, 0xccc, 0, 0, 0, false, false},
  [122] = {R(179, 53, 0x4003), 0, 0, 39, 328, -0xa3d, -0x30a3, 0x28f, 0x9999, false, false},
  [154] = {R(131, 79, 0x4003), 0, 0, 40, 0, 0x1999, -0x23d7, 0x24d, 0, false, false},
  [155] = {R(132, 59, 0x4003), 0, 0, 13, 328, 0x1999, 0x666, 0x147, 0, false, false},
  [173] = {R(182, 64, 0x4001), 0, 0, -1, 0, 0x3333, 0, 0, 0, true, false},
};
static const MoveDef crouch_reaction = R(254, 4, 0x4001);
static const MoveDef tumble_start = R(142, 16, 0x4003);
static const MoveDef tumble_loop = R(141, 16, 0x4003);
static const MoveDef tumble_land_move = R(143, 28, 0x4003);
static const MoveDef getup_back = R(328, 14, ACTION_GETUP);
static const MoveDef getup_front = R(188, 15, ACTION_GETUP);
static const MoveDef getup_side = R(185, 15, ACTION_GETUP);
// clang-format on
#undef R

static void getup_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(fighter_move(fighter)->last, fighter->ticks);
  if (fighter->ticks > fighter_move(fighter)->last) {
    neutral(battle, fighter);
  }
}

static void begin_getup(Battle *battle, Fighter *fighter, int clip) {
  stop_sliding(battle, fighter);
  if (fighter->health <= 0) {
    fighter->script = NULL;
    return;
  }
  if (!clip) {
    neutral(battle, fighter);
    return;
  }
  fighter_start_move(battle, fighter, S_KNOCKDOWN, clip == 188 ? &getup_front : clip == 185 ? &getup_side : &getup_back);
  fighter->script = getup_tick;
  fighter->sample_override = 0;
}

static void extra_reaction_tick(Battle *battle, Fighter *fighter) {
  const ReactionSpec *spec = &extra_reactions[fighter->reaction];
  int elapsed = fighter->script_timer++;
  int sample = elapsed;
  /* 0x4e8ab8 advances twice through sample 13, then once to landing. */
  if (fighter->reaction == 23) {
    sample = elapsed < 7 ? elapsed * 2 : elapsed + 7;
  }
  if (spec->hold_ticks) {
    sample = elapsed < spec->hold_at ? elapsed : elapsed < spec->hold_at + spec->hold_ticks ? spec->hold_at : elapsed - spec->hold_ticks;
  }
  /* Low-kick/knee scripts submit samples 6..9 with two waits each. */
  if ((fighter->reaction == 17 || fighter->reaction == 27) && elapsed >= 6) {
    sample = elapsed < 18 ? 6 + (elapsed - 6) / 3 : elapsed - 8;
  }
  if (fighter->current_move == &crouch_reaction) {
    fighter->sample_override = mini(4, elapsed);
    if (elapsed >= 9) {
      neutral(battle, fighter);
    }
    return;
  }
  if (spec->land_at >= 0 && sample >= spec->land_at && fighter_airborne(fighter)) {
    sample = spec->land_at;
    fighter->script_timer--;
  }
  fighter->sample_override = mini(spec->move.last, sample);
  if (sample > spec->move.last) {
    begin_getup(battle, fighter, spec->getup);
  }
}

static void tumble_tick(Battle *battle, Fighter *fighter) {
  if (fighter->current_move == &tumble_land_move) {
    fighter->sample_override = mini(28, fighter->ticks);
    if (fighter->ticks >= (fighter->reaction == 100 ? 49 : 37)) {
      begin_getup(battle, fighter, fighter->reaction == 100 ? 0 : 328);
    }
    return;
  }
  fighter->sample_override = mini(16, fighter->ticks);
  if (!fighter_airborne(fighter) && fighter->ticks >= 17) {
    fighter_start_move(battle, fighter, S_KNOCKDOWN, &tumble_land_move);
    fighter->script = tumble_tick;
    fighter->sample_override = 0;
  } else if (fighter->ticks >= 17) {
    fighter_start_move(battle, fighter, S_KNOCKDOWN, &tumble_loop);
    fighter->script = tumble_tick;
    fighter->sample_override = 0;
  }
}

static void react_extra(Battle *battle, Fighter *fighter) {
  int id = fighter->reaction;
  bool teleport_air = id == 90 && fighter_airborne(fighter);
  if (id == 20 || id == 100 || id == 156) {
    fighter_start_move(battle, fighter, S_KNOCKDOWN, &tumble_start);
    fighter->script = tumble_tick;
    fighter->sample_override = 0;
    launch(battle, fighter, id == 20 ? 0xccc : 0xa3d, -0x147a, id == 156 ? 0x1ca : 0xde);
    return;
  }
  if ((id == 90 || id == 173) && fighter_airborne(fighter)) {
    fighter->reaction = id = 10;
  }
  if (id == 32 && (fighter->damage_total >= 0x3333 || (!fighter->human && fighter->damage_total >= 0x2666))) {
    react_jab_hit(battle, fighter);
    return;
  }
  const ReactionSpec *spec = &extra_reactions[id];
  bool crouch = fighter->crouching && (id == 17 || id == 28 || id == 29);
  fighter_start_move(battle, fighter, spec->land_at < 0 ? S_HIT : S_KNOCKDOWN, crouch ? &crouch_reaction : &spec->move);
  fighter->script = extra_reaction_tick;
  fighter->sample_override = spec->hold_at == 4 ? 4 : 0;
  fighter->script_timer = spec->hold_at == 4 ? 4 : 0;
  if (spec->scale) {
    fighter->damage_scale = spec->scale;
  }
  if (spec->no_guard) {
    fighter->flags |= FF_NO_GUARD;
  }
  if (spec->friction) {
    slide(battle, fighter, crouch && id == 17 ? 0x2b85 : spec->speed);
  } else {
    /* 0x4e9980 shares clip 183 with jab air, but retreats at 0.03. */
    launch(battle, fighter, teleport_air ? 0x7ae : id == 52 && fighter->hits > 1 ? 0xf5c : spec->speed, spec->vy, spec->gravity);
  }
  if (fighter_airborne(fighter) && fighter->body.gravity == 0) {
    fighter->body.gravity = 0x28f;
  }
}

static void react_extra_block(Battle *battle, Fighter *fighter) {
  int id = fighter->reaction;
  fighter_start_move(battle, fighter, S_BLOCK, id == 8 ? &block_short : &block_long);
  if (id == 3 || id == 143) {
    fighter->action = 0x4004;
  }
  slide(battle, fighter, id == 8 ? 0 : id == 7 ? 0x4000 : id == 5 ? 0x2e14 : 0xf5c);
}

/* 486d00 -> 4e9e68: two repetitions of seven standing poses, no push.
 * 486f90 instead binds crouching clip 255 and plays it out and back. */
static void react_slide_block(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_BLOCK, fighter->crouching ? &slide_crouch_block : &slide_block);
}

static void react_approximate(Battle *battle, Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
    fighter_enter_state(battle, fighter, S_KNOCKDOWN);
  } else {
    fighter_enter_state(battle, fighter, S_HIT);
  }
}

/* 486230/486310/486340/4863b0: six 6-tick shakes, 88-tick hold,
 * three 6-tick shakes. Pose and body stay fixed; cosmetic shake is omitted.
 * Air thaw: 4e9d38 sets gravity 0x147 and waits for landing, then 486530. */
static const MoveDef frozen = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK | SF_HOLD, .action = 0x1003};
static const MoveDef thaw_landing = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};
static const MoveDef thaw_hop = {.clip = 310, .first = 7, .last = 18, .flags = SF_LOCK, .action = 0x1005};

static void thaw_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    neutral(battle, fighter);
  }
}

static void frozen_tick(Battle *battle, Fighter *fighter) {
  if (fighter->script_phase == 0 && fighter->ticks < 142) {
    return;
  }
  if (fighter_airborne(fighter)) {
    fighter->appearance = APPEARANCE_NORMAL;
    fighter->body.gravity = 0x147;
    fighter->script_phase = 1;
    return;
  }
  fighter->appearance = APPEARANCE_NORMAL;
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->script_phase == 1) {
    fighter_start_move(battle, fighter, S_JUMP, &thaw_landing);
    fighter->script = thaw_tick;
    fighter->sample_override = 18;
  } else {
    neutral(battle, fighter);
  }
}

static void thaw_hop_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(18, 7 + fighter->ticks);
  if (!fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, &thaw_landing);
    fighter->script = thaw_tick;
    fighter->sample_override = 18;
  }
}

static void react_freeze(Battle *battle, Fighter *fighter) {
  if (fighter->action == 0x1003) {
    /* 486430 calls 480f80, which switches to the opponent: double freeze
     * backfires. It does not recursively refresh this victim's timer. */
    Fighter *attacker = &battle->fighters[fighter == &battle->fighters[0] ? 1 : 0];
    battle_react(battle, attacker, 67);
    if (fighter_airborne(fighter)) {
      fighter_start_move(battle, fighter, S_JUMP, &thaw_hop);
      fighter->script = thaw_hop_tick;
      fighter->sample_override = 7;
      fighter->body.vel[AXIS_Y] = -0x51e;
      fighter->body.gravity = 0x20c;
    } else {
      neutral(battle, fighter);
    }
    return;
  }
  int clip = fighter->appearance == APPEARANCE_FROZEN ? fighter->frozen_clip : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter_start_move(battle, fighter, S_HIT, &frozen);
  fighter->script = frozen_tick;
  fighter->sample_override = sample;
  fighter->frozen_clip = clip;
  fighter->appearance = APPEARANCE_FROZEN;
  fighter->damage_scale = fighter->hits > 0 ? 0x4ccc : 0x8000;
  fighter->flags |= FF_REACTING | FF_NO_GUARD;
}

/* 4831e0 / 4e94b0: low scepter hit, then the human down-held
 * branch 45b620 chooses crouch clip 193 instead of standing rise 194. */
static const MoveDef scepter_low_hit = {.clip = 192, .last = 34, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef scepter_low_crouch = {.clip = 193, .last = 15, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef scepter_low_rise = {.clip = 194, .last = 23, .flags = SF_LOCK, .action = 0x4003};

static void scepter_low_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(fighter_move(fighter)->last, fighter->ticks);
  if (fighter->current_move == &scepter_low_hit && fighter->ticks >= 24) {
    stop_sliding(battle, fighter);
  }
  if (fighter->ticks > fighter_move(fighter)->last) {
    if (fighter->current_move == &scepter_low_hit) {
      bool down = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
      fighter_start_move(battle, fighter, S_KNOCKDOWN, down ? &scepter_low_crouch : &scepter_low_rise);
      fighter->script = scepter_low_tick;
      fighter->sample_override = 0;
    } else {
      neutral(battle, fighter);
    }
  }
}

static void react_scepter_low(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KNOCKDOWN, &scepter_low_hit);
  fighter->script = scepter_low_tick;
  fighter->sample_override = 0;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_set_speed(battle, fighter, -0xf5c);
}

static void react_scepter_freeze(Battle *battle, Fighter *fighter) {
  if (fighter->hits > 3) {
    /* 486210 -> 4835f0 uses the shared weapon-throw tumble. */
    fighter->reaction = 154;
    react_extra(battle, fighter);
  } else {
    react_freeze(battle, fighter);
  }
}

/* 482c20, reaction 120: restart one of the current block poses, otherwise
 * use 180. The old no-guard flag survives a follow-up weapon draw. */
static const MoveDef weapon_followup[] = {
    {.clip = 255, .last = 4, .flags = SF_LOCK},
    {.clip = 222, .last = 4, .flags = SF_LOCK},
    {.clip = 247, .last = 4, .flags = SF_LOCK},
    {.clip = 321, .last = 32, .flags = SF_LOCK},
    {.clip = 180, .last = 19, .flags = SF_LOCK},
};

static void weapon_followup_tick(Battle *battle, Fighter *fighter) {
  int last = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count - 1 : fighter_move(fighter)->last;
  fighter->sample_override = mini(last, fighter->ticks);
  if (fighter->ticks > last) {
    neutral(battle, fighter);
  }
}

static void react_weapon_followup(Battle *battle, Fighter *fighter) {
  int selected = 4, clip = fighter_move(fighter)->clip;
  unsigned no_guard = fighter->flags & FF_NO_GUARD;
  int action = fighter->action;
  for (int i = 0; i < 4; ++i) {
    if (weapon_followup[i].clip == clip) {
      selected = i;
    }
  }
  fighter_start_move(battle, fighter, S_BLOCK, &weapon_followup[selected]);
  fighter->script = weapon_followup_tick;
  fighter->sample_override = 0;
  fighter->flags |= no_guard;
  fighter->action = action;
}

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn reactions[256] = {
  [0] = react_block,
  [3] = react_extra_block,
  [5] = react_extra_block,
  [7] = react_extra_block,
  [8] = react_extra_block,
  [143] = react_extra_block,
  [10] = react_extra,
  [12] = react_extra,
  [17] = react_extra,
  [20] = react_extra,
  [21] = react_extra,
  [22] = react_extra,
  [23] = react_extra,
  [25] = react_extra,
  [26] = react_extra,
  [27] = react_extra,
  [28] = react_extra,
  [29] = react_extra,
  [32] = react_extra,
  [38] = react_extra,
  [39] = react_extra,
  [43] = react_extra,
  [48] = react_extra,
  [49] = react_extra,
  [50] = react_extra,
  [51] = react_extra,
  [52] = react_extra,
  [150] = react_scepter_low,
  [151] = react_scepter_freeze,
  [120] = react_weapon_followup,
  [67] = react_freeze,
  [90] = react_extra,
  [99] = react_extra,
  [100] = react_extra,
  [122] = react_extra,
  [154] = react_extra,
  [155] = react_extra,
  [156] = react_extra,
  [173] = react_extra,

  [112] = react_block,
  [118] = react_block,
  [119] = react_block,
  [1] = react_jab_block,
  [4] = react_low_block,
  [6] = react_block,
  [11] = react_jab_air,
  [13] = react_kick_air,
  [16] = react_kick_hit,
  [18] = react_sweep,
  [34] = react_uppercut,
  [37] = react_jab_hit,
  [165] = react_slide_block,
};
// clang-format on

/* 0x48f880: the victim's move is replaced at once; velocities, gravity and
 * its physics callback are cleared and it is marked as reacting. The
 * reaction starts on the victim's next update. */
void battle_react(Battle *battle, Fighter *victim, int reaction) {
  require(reaction >= 0 && reaction < COUNT(reactions), "Invalid reaction id");
  /* Drop rules for the reaction ids currently used by our script records.
   * Native 0x494520 is called by these reaction paths, not by ordinary block. */
  if (victim->armed && victim->disarm != NULL &&
      ((reaction >= 10 && reaction <= 13) || (reaction >= 15 && reaction <= 23) || reaction == 34 || (reaction >= 48 && reaction <= 52) || reaction == 70 ||
          reaction == 150 || (reaction == 151 && victim->hits > 3) || reaction == 122 || reaction == 26 || reaction == 28 || reaction == 29 || reaction == 38 ||
          reaction == 39 || reaction == 90 || reaction == 154 || reaction == 155 || reaction == 156 || reaction == 173)) {
    victim->disarm(battle, victim);
  }
  victim->reaction = reaction;
  if (reaction != 67 && reaction != 151) {
    for (int i = 0; i < 3; ++i) {
      victim->body.angle[i] = 0;
    }
    victim->body.pos[AXIS_Y] += victim->body.root_y;
    victim->body.root_y = 0;
  }
  FighterFn entry = reactions[reaction] != NULL && (victim->roster != 15 || reaction == 67 || (reaction == 151 && victim->hits <= 3)) ? reactions[reaction]
                                                                                                                                      : react_approximate;
  (void)battle;
  for (int i = 0; i < 3; ++i) {
    victim->body.vel[i] = 0;
    victim->body.spin[i] = 0;
  }
  victim->body.gravity = 0;
  victim->body.physics = NULL;
  victim->body.on_land = NULL;
  fighter_interrupt(victim, entry);
  victim->flags |= FF_REACTING;
}

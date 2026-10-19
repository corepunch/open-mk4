#include "game.h"
#include "native.h"
#include "mk4_characters.h"

void reptile_reaction_40(Fighter *fighter);
void reptile_acid_victim(Fighter *fighter);
void reptile_dash_victim(Fighter *fighter);
void reptile_second_victim(Fighter *fighter);
void reptile_weapon_reaction(Fighter *fighter);
void fujin_bolt_victim(Fighter *fighter);
void fujin_wind_victim(Fighter *fighter);
void fujin_wind_slam_victim(Fighter *fighter);
void fujin_triple_victim(Fighter *fighter);
void fujin_grab_victim(Fighter *fighter);
void fujin_second_victim(Fighter *fighter);
void goro_quake_victim(Fighter *fighter);
void goro_punch_victim(Fighter *fighter);
void goro_kick_victim(Fighter *fighter);
void goro_stomp_victim(Fighter *fighter);
void goro_uppercut_victim(Fighter *fighter);
void kai_second_victim(Fighter *fighter);
#include "script.h"
#include "math_util.h"

/* Shared reaction phases used by script attack records. Source addresses
 * are in docs/scorpion/combat-core.md. These are our C continuations, not
 * an emulation of the retail task scheduler. Goro retains his own clips. */

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))

enum {
  FRICTION = 0xe666,    /* 0x4913a0: velocity *= 0.9 per world pump */
  FRICTION_STOP = 0x41, /* stop once vx*vx + vz*vz <= this */
  ACTION_GETUP = 0x200e,
  ACTION_BLOCK = 0x2005
};

static void neutral(Fighter *fighter) {
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_IDLE);
  } else {
    fighter_stop_mmove(fighter);
  }
}

static bool landed(const Fighter *fighter) {
  return !fighter_airborne(fighter) && fighter->body.gravity == 0;
}

/* 0x4913a0 group callback. */
void fighter_friction(Fighter *fighter) {
  q16 *v = fighter->body.vel;
  v[AXIS_X] = q16_mul(v[AXIS_X], FRICTION);
  v[AXIS_Z] = q16_mul(v[AXIS_Z], FRICTION);
  if (q16_mul(v[AXIS_X], v[AXIS_X]) + q16_mul(v[AXIS_Z], v[AXIS_Z]) <= FRICTION_STOP) {
    v[AXIS_X] = 0;
    v[AXIS_Z] = 0;
    fighter->body.physics = NULL;
  }
}

static void stop_sliding(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = 0;
  fighter->body.vel[AXIS_Z] = 0;
}

static void slide(Fighter *fighter, q16 speed) {
  fighter_set_speed(fighter, -speed);
  fighter->body.physics = fighter_friction;
}

/* 488ea0 -> 480fc0(2) -> 487530/570: a grounded attacker recoils
 * when the victim is in the outer ten square units of the arena. Keep
 * the attacking pose, slide back .3 with friction, wait 8+5 ticks, then
 * wait for an airborne reacting opponent before returning control. */
static void wall_recoil_buttons(Fighter *fighter) {
  if (fighter->script_phase == 0x10a || fighter->script_phase == 0x10b) {
    fighter_enter_stance(fighter, STANCE_STANDING);
  }
}

static void wall_recoil_wait(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->mmove_hold = (victim->flags & FF_REACTING) && fighter_airborne(victim);
  if (!fighter->mmove_hold) {
    fighter_neutral(fighter);
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void wall_recoil(Fighter *fighter) {
  int sample = fighter_pose_sample(fighter);
  int action = fighter->action;
  fighter_upright(fighter);
  fighter->pending = NULL;
  fighter->state = S_HIT;
  fighter->ticks = 0;
  fighter->action = 0;
  fighter->script_phase = action;
  fighter->sample_override = sample;
  fighter->buttons = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
  native_begin(fighter, 1, 13, NULL);
  for (int frame = 1; frame <= 13; ++frame) {
    native_frame(fighter, frame, sample, frame == 8 ? wall_recoil_buttons : frame == 13 ? wall_recoil_wait : NULL);
  }
  native_play(fighter, false);
  slide(fighter, 0x4ccc);
}

void battle_wall_recoil(Fighter *victim) {
  Fighter *attacker = (Fighter *)fighter_opponent(victim);
  int64_t x = victim->body.pos[AXIS_X], z = victim->body.pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (victim->body_contact && x * x + z * z >= radius * radius - 10LL * Q16_ONE * Q16_ONE && !(victim->flags & FF_MARK) && !fighter_airborne(attacker)) {
    fighter_interrupt(attacker, wall_recoil);
  }
}

static void launch(Fighter *fighter, q16 speed, q16 vy, q16 gravity) {
  fighter_set_speed(fighter, -speed);
  fighter->body.vel[AXIS_Y] = vy;
  fighter->body.gravity = gravity;
}

/* These maps include the entry pose. Later poses and actions are installed
 * once in a fixed-frame program; callbacks never read an age clock. */
typedef struct ReactionPhase {
  MoveDef move;
  const int *samples;
  int count;
  FighterFn end;
  FighterTest until;
} ReactionPhase;

static void sweep_rise(Fighter *fighter);
static void uppercut_land(Fighter *fighter);
static void uppercut_rise(Fighter *fighter);
static void kick_air_rise(Fighter *fighter);
static void jab_air_land(Fighter *fighter);
static void jab_air_rise(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const int forward_samples[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52};
static const int jab_hit_samples[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18};
static const int stagger_samples[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 38, 38, 40, 42, 44, 46, 50};
static const int kick_hit_samples[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
static const int block_samples[] = {3, 5, 7, 9, 7, 5, 3, 3, 5, 7, 9, 7, 5, 3};
static const int crouch_block_samples[] = {0, 1, 2, 3, 4, 4, 3, 2, 1, 0};
static const int uppercut_land_samples[] = {53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109};
static const int jab_air_land_samples[] = {31, 32, 33};
// clang-format on

#define REACTION(name, clip_id, move_flags, poses, frames, finish, gate, action_id)                                                                            \
  static const ReactionPhase name = {                                                                                                                          \
      .move = {.clip = clip_id, .last = MOVE_END, .flags = move_flags, .action = action_id}, .samples = poses, .count = frames, .end = finish, .until = gate}

REACTION(jab_hit, 311, SF_LOCK, jab_hit_samples, COUNT(jab_hit_samples), neutral, NULL, 0);
REACTION(stagger, 182, SF_LOCK, stagger_samples, COUNT(stagger_samples), neutral, NULL, 0);
REACTION(kick_hit, 311, SF_LOCK, kick_hit_samples, COUNT(kick_hit_samples), neutral, NULL, 0);
REACTION(block_short, 321, SF_LOCK, block_samples, 7, neutral, NULL, ACTION_BLOCK);
REACTION(block_long, 321, SF_LOCK, block_samples, COUNT(block_samples), neutral, NULL, ACTION_BLOCK);
REACTION(slide_block, 321, SF_LOCK, block_samples, COUNT(block_samples), neutral, NULL, ACTION_BLOCK);
REACTION(slide_crouch_block, 255, SF_LOCK, crouch_block_samples, COUNT(crouch_block_samples), neutral, NULL, ACTION_BLOCK);
REACTION(fireball_armed_block, 247, SF_LOCK, crouch_block_samples, COUNT(crouch_block_samples), neutral, NULL, ACTION_BLOCK);
REACTION(fireball_armed_crouch_block, 222, SF_LOCK, crouch_block_samples, COUNT(crouch_block_samples), neutral, NULL, ACTION_BLOCK);
REACTION(sweep_fall, 195, SF_LOCK, forward_samples, 29, sweep_rise, NULL, 0);
REACTION(sweep_rise_move, 197, SF_LOCK, forward_samples, 29, neutral, NULL, 0);
REACTION(uppercut_fall, 128, SF_LOCK, forward_samples, 53, uppercut_land, landed, 0);
REACTION(uppercut_land_move, 128, SF_LOCK, uppercut_land_samples, 28, uppercut_rise, NULL, 0);
REACTION(uppercut_ko, 128, SF_LOCK, uppercut_land_samples, COUNT(uppercut_land_samples), neutral, NULL, 0);
REACTION(uppercut_rise_move, 328, SF_LOCK, forward_samples, 15, neutral, NULL, ACTION_GETUP);
REACTION(kick_air, 189, SF_LOCK, forward_samples, 30, kick_air_rise, landed, 0);
REACTION(kick_air_rise_move, 191, SF_LOCK, forward_samples, 16, neutral, NULL, ACTION_GETUP);
REACTION(jab_air, 183, SF_LOCK, forward_samples, 30, jab_air_land, landed, 0);
REACTION(jab_air_land_move, 183, SF_LOCK, jab_air_land_samples, COUNT(jab_air_land_samples), jab_air_rise, NULL, ACTION_GETUP);
REACTION(jab_air_rise_move, 185, SF_LOCK, forward_samples, 16, neutral, NULL, ACTION_GETUP);
#undef REACTION

static void shared_reaction_finish(Fighter *fighter) {
  const ReactionPhase *phase = fighter->native_context;
  fighter_finish_mmove(fighter, phase->end);
}

static void shared_reaction_gate(Fighter *fighter) {
  const ReactionPhase *phase = fighter->native_context;
  fighter->mmove_hold = !phase->until(fighter);
}

static void start_reaction(Fighter *fighter, int state, const ReactionPhase *phase) {
  fighter_start_move(fighter, state, &phase->move);
  fighter->native_context = phase;
  fighter->sample_override = phase->samples[0];
  native_begin(fighter, 1, phase->count, shared_reaction_finish);
  for (int frame = 1; frame <= phase->count; ++frame) {
    native_frame(fighter, frame, phase->samples[mini(frame, phase->count - 1)], NULL);
  }
  if (phase == &stagger) {
    fighter->native.frames[18].thinkfunc = stop_sliding;
  }
  if (phase->until != NULL) {
    fighter->native.frames[phase->count - 1].thinkfunc = shared_reaction_gate;
  }
  native_play(fighter, false);
}

static void sweep_rise(Fighter *fighter) {
  stop_sliding(fighter);
  start_reaction(fighter, S_KNOCKDOWN, &sweep_rise_move);
}

static void uppercut_land(Fighter *fighter) {
  /* A KO victim stays down; 0x48fc20 ends the round instead of a getup. */
  start_reaction(fighter, S_KNOCKDOWN, fighter->health == 0 ? &uppercut_ko : &uppercut_land_move);
}

static void uppercut_rise(Fighter *fighter) {
  start_reaction(fighter, S_KNOCKDOWN, &uppercut_rise_move);
}

static void kick_air_rise(Fighter *fighter) {
  start_reaction(fighter, S_KNOCKDOWN, &kick_air_rise_move);
}

static void jab_air_land(Fighter *fighter) {
  start_reaction(fighter, S_KNOCKDOWN, &jab_air_land_move);
}

static void jab_air_rise(Fighter *fighter) {
  start_reaction(fighter, S_KNOCKDOWN, &jab_air_rise_move);
}

/* 1: jab blocked. */
static void react_jab_block(Fighter *fighter) {
  start_reaction(fighter, S_BLOCK, &block_short);
  slide(fighter, 0x3d70); /* 0.24 */
}

/* 4: sweep blocked. */
static void react_low_block(Fighter *fighter) {
  start_reaction(fighter, S_BLOCK, &block_long);
  slide(fighter, 0x2666); /* 0.15 */
}

/* 6: kick or uppercut blocked. */
static void react_block(Fighter *fighter) {
  start_reaction(fighter, S_BLOCK, &block_long);
  slide(fighter, 0x3333); /* 0.20 */
}

/* 11 (0x482070): jab air hit; air reactions scale later damage by 0.6. */
static void react_jab_air(Fighter *fighter) {
  start_reaction(fighter, S_KNOCKDOWN, &jab_air);
  fighter->damage_scale = 0x9999;
  launch(fighter, 0xccc, -0x23d7, 0x28f); /* 0.05, -0.14, 0.01 */
}

/* 13 (0x481d30): kick air hit. */
static void react_kick_air(Fighter *fighter) {
  battle_wall_recoil(fighter);
  start_reaction(fighter, S_KNOCKDOWN, &kick_air);
  fighter->damage_scale = 0x9999;
  launch(fighter, 0x11eb, -0x23d7, 0x28f); /* 0.07, -0.14, 0.01 */
}

/* 123 (0x481ce0 -> 4e89d8): reaction 13's clip-189 launch, landing and
 * 488c00 getup without its 0x9999 damage scale or 488ea0 wall recoil. The
 * native and its script both restart camera shake row 0 {3, 4, .15}; the
 * script adds sound B[1] and context sound 0. Its decorative 40a470
 * selectors 9/4 remain omitted, as for reaction 13's selector 4. */
static void react_kick_air_impact(Fighter *fighter) {
  battle_camera_shake(3, 4, 0x2666);
  battle_sound(SOUND_B, 1);
  mk4_script_context_sound(fighter, 0);
  start_reaction(fighter, S_KNOCKDOWN, &kick_air);
  launch(fighter, 0x11eb, -0x23d7, 0x28f);
}

/* Kai skull reactions. 129 (485f10) disarms, then airborne victims take
 * reaction 22's 483030/4e9430 path; grounded ones run 4e9c80: sound B[1],
 * clip 132 two samples per visit to 14, stop, then 488ba0 getup 328.
 * 130 (485f50) disarms, sets action 100a and runs 4e9ca8: sound B[3],
 * clip 179 launch, landing at 39 and 488b80's getup 328. */
static void react_extra(Fighter *fighter);

static void react_kai_skull(Fighter *fighter) {
  if (fighter->reaction == 129 && fighter_airborne(fighter)) {
    fighter->reaction = 22;
  } else {
    battle_sound(SOUND_B, fighter->reaction == 129 ? 1 : 3);
  }
  react_extra(fighter);
}

/* 16 (0x482a10): kick hit, including the wall recoil check. */
static void react_kick_hit(Fighter *fighter) {
  battle_wall_recoil(fighter);
  start_reaction(fighter, S_HIT, &kick_hit);
  slide(fighter, 0x2666); /* 0.15 */
}

/* 18 (0x483050): sweep trip, also when airborne; Y is forced to the floor. */
static void react_sweep(Fighter *fighter) {
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->body.vel[AXIS_Y] = 0;
  fighter->body.gravity = 0;
  start_reaction(fighter, S_KNOCKDOWN, &sweep_fall);
  fighter_set_speed(fighter, -0xf5c); /* 0.06 */
}

/* 34 (0x4838e0): uppercut launch from the current height. */
static void react_uppercut(Fighter *fighter) {
  battle_wall_recoil(fighter);
  start_reaction(fighter, S_KNOCKDOWN, &uppercut_fall);
  launch(fighter, 0x7ae, -0x3333, 0x20c); /* 0.03, -0.20, 0.008 */
}

/* 37 (0x483b40): jab hit, or a stagger once 0.2 damage (0.15 for a CPU)
 * has accumulated, or always against Goro. */
static void react_jab_hit(Fighter *fighter) {
  bool staggers = fighter->damage_total >= 0x3333 || (!fighter->human && fighter->damage_total >= 0x2666) || fighter->roster == 15;
  if (staggers) {
    battle_wall_recoil(fighter);
    start_reaction(fighter, S_HIT, &stagger);
    fighter_set_speed(fighter, -0x1999); /* 0.1 */
  } else {
    start_reaction(fighter, S_HIT, &jab_hit);
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
  [54] = {R(183, 33, 0x4003), 0, 0, 30, 185, 0x11eb, -0x23d7, 0x28f, 0, false, false},
  [59] = {R(189, 30, 0x4003), 0, 0, 30, 191, 0x3d7, -0x3851, 0x28f, 0x8000, false, false},
  [60] = {R(311, 18, 0x4001), 0, 0, -1, 0, 0xccc, 0, 0, 0, false, false},
  [62] = {R(130, 39, 0x4001), 0, 0, -1, 0, 0xccc, 0, 0, 0, false, false},
  [90] = {.move = R(311, 18, 0x4001), .land_at = -1},
  [99] = {R(326, 19, 0x4001), 0, 0, -1, 0, 0xccc, 0, 0, 0, false, false},
  [122] = {R(179, 53, 0x4003), 0, 0, 39, 328, -0xa3d, -0x30a3, 0x28f, 0x9999, false, false},
  [129] = {R(132, 59, 0x4003), 0, 0, 14, 328, 0xa3d, 0, 0, 0, false, false},
  [130] = {R(179, 53, 0x100a), 0, 0, 39, 328, -0x51e, -0x4ccc, 0x353, 0, false, false},
  [154] = {R(131, 79, 0x4003), 0, 0, 40, 0, 0x1999, -0x23d7, 0x24d, 0, false, false},
  [155] = {R(132, 59, 0x4003), 0, 0, 13, 328, 0x1999, 0x666, 0x147, 0, false, false},
  [173] = {.move = R(182, 64, 0x4001), .land_at = -1},
};
static const MoveDef crouch_reaction = R(254, 4, 0x4001);
static const MoveDef tumble_start = R(142, 16, 0x4003);
static const MoveDef tumble_loop = R(141, 16, 0x4003);
static const MoveDef tumble_land_move = R(143, 28, 0x4003);
static const MoveDef getup_back = R(328, 14, ACTION_GETUP);
static const MoveDef getup_front = R(188, 15, ACTION_GETUP);
static const MoveDef getup_kick = R(191, 15, ACTION_GETUP);
static const MoveDef getup_side = R(185, 15, ACTION_GETUP);
// clang-format on
#undef R

static void reaction_play(Fighter *fighter, int first, int last, int final, FighterFn end) {
  native_begin(fighter, first, final, end);
  for (int frame = first; frame <= final; ++frame) {
    native_frame(fighter, frame, mini(last, frame), NULL);
  }
  native_play(fighter, false);
}

static void begin_getup(Fighter *fighter, int clip) {
  stop_sliding(fighter);
  if (fighter->health <= 0) {
    fighter->script = NULL;
    fighter_stop_mmove(fighter);
    return;
  }
  if (!clip) {
    neutral(fighter);
    return;
  }
  fighter_start_move(fighter, S_KNOCKDOWN, clip == 188 ? &getup_front : clip == 185 ? &getup_side : clip == 191 ? &getup_kick : &getup_back);
  fighter->sample_override = 0;
  int last = fighter_move(fighter)->last;
  reaction_play(fighter, 1, last, last + 1, neutral);
}

static void reaction_land_gate(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

/* 4e9c98 opcode 5b (45b010) stops reaction 129 at its sample-14 boundary. */
static void reaction_stop_gate(Fighter *fighter) {
  mk4_script_stop(fighter);
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void extra_reaction_end(Fighter *fighter) {
  begin_getup(fighter, extra_reactions[fighter->reaction].getup);
}

static int reaction_pose(const Fighter *fighter, const ReactionSpec *spec, int elapsed) {
  if (fighter->current_move == &crouch_reaction) {
    return mini(4, elapsed);
  }
  int sample = elapsed;
  if (fighter->reaction == 23 || fighter->reaction == 129) {
    sample = elapsed < 7 ? elapsed * 2 : elapsed + 7;
  }
  if (spec->hold_ticks) {
    sample = elapsed < spec->hold_at ? elapsed : elapsed < spec->hold_at + spec->hold_ticks ? spec->hold_at : elapsed - spec->hold_ticks;
  }
  if ((fighter->reaction == 17 || fighter->reaction == 27) && elapsed >= 6) {
    sample = elapsed < 18 ? 6 + (elapsed - 6) / 3 : elapsed - 8;
  }
  return sample;
}

static void extra_reaction_play(Fighter *fighter) {
  const ReactionSpec *spec = &extra_reactions[fighter->reaction];
  int first = spec->hold_at == 4 ? 4 : 0;
  bool crouch = fighter->current_move == &crouch_reaction;
  int last = first;
  while (crouch ? last < 9 : reaction_pose(fighter, spec, last) <= spec->move.last) {
    ++last;
  }
  native_begin(fighter, first, last, crouch ? neutral : extra_reaction_end);
  bool landing = false;
  for (int frame = first; frame <= last; ++frame) {
    int pose = reaction_pose(fighter, spec, frame);
    bool gate = !crouch && !landing && spec->land_at >= 0 && pose >= spec->land_at;
    native_frame(fighter, frame, mini(crouch ? 4 : spec->move.last, pose), gate ? (fighter->reaction == 129 ? reaction_stop_gate : reaction_land_gate) : NULL);
    landing |= gate;
  }
  native_play(fighter, false);
}

static void tumble_play(Fighter *fighter);

static void tumble_next(Fighter *fighter) {
  if (fighter->current_move == &tumble_land_move) {
    begin_getup(fighter, (fighter->reaction == 100 || fighter->reaction == 61) ? 0 : 328);
    return;
  }
  fighter_start_move(fighter, S_KNOCKDOWN, fighter_airborne(fighter) ? &tumble_loop : &tumble_land_move);
  fighter->sample_override = 0;
  tumble_play(fighter);
}

static void tumble_play(Fighter *fighter) {
  int final = fighter->current_move == &tumble_land_move ? ((fighter->reaction == 100 || fighter->reaction == 61) ? 49 : 37) : 17;
  reaction_play(fighter, 1, fighter->current_move == &tumble_land_move ? 28 : 16, final, tumble_next);
}

static void react_extra(Fighter *fighter) {
  switch (fighter->reaction) {
    case 10:
    case 17:
    case 25:
    case 26:
    case 28:
    case 29:
    case 51:
    case 52:
    case 99:
      battle_wall_recoil(fighter);
      break;
    default:
      break;
  }
  int id = fighter->reaction;
  bool teleport_air = id == 90 && fighter_airborne(fighter);
  if (id == 15 || id == 20 || id == 57 || id == 61 || id == 100 || id == 156) {
    fighter_start_move(fighter, S_KNOCKDOWN, &tumble_start);
    fighter->sample_override = 0;
    tumble_play(fighter);
    launch(fighter, id == 57 || id == 61 ? 0xa3d : id == 15 || id == 20 ? 0xccc : 0xa3d, -0x147a, id == 156 ? 0x1ca : 0xde);
    return;
  }
  if ((id == 90 || id == 173) && fighter_airborne(fighter)) {
    fighter->reaction = id = 10;
  }
  if (id == 32 && (fighter->damage_total >= 0x3333 || (!fighter->human && fighter->damage_total >= 0x2666))) {
    react_jab_hit(fighter);
    return;
  }
  const ReactionSpec *spec = &extra_reactions[id];
  bool crouch = fighter->crouching && (id == 17 || id == 28 || id == 29);
  fighter_start_move(fighter, spec->land_at < 0 ? S_HIT : S_KNOCKDOWN, crouch ? &crouch_reaction : &spec->move);
  if (id == 90 || id == 173) {
    mk4_script_stop_body(fighter);
    scorpion_generated_reaction(fighter);
    return;
  }
  fighter->sample_override = spec->hold_at == 4 ? 4 : 0;
  extra_reaction_play(fighter);
  if (spec->scale) {
    fighter->damage_scale = spec->scale;
  }
  if (spec->no_guard) {
    fighter->flags |= FF_NO_GUARD;
  }
  if (spec->friction) {
    slide(fighter, crouch && id == 17 ? 0x2b85 : spec->speed);
  } else {
    /* 0x4e9980 shares clip 183 with jab air, but retreats at 0.03. */
    launch(fighter, teleport_air ? 0x7ae : id == 52 && fighter->hits > 1 ? 0xf5c : spec->speed, spec->vy, spec->gravity);
  }
  if (fighter_airborne(fighter) && fighter->body.gravity == 0) {
    fighter->body.gravity = 0x28f;
  }
}

static void react_extra_block(Fighter *fighter) {
  int id = fighter->reaction;
  start_reaction(fighter, S_BLOCK, id == 8 ? &block_short : &block_long);
  if (id == 3 || id == 143) {
    fighter->action = 0x4004;
  }
  slide(fighter, id == 8 ? 0 : id == 7 ? 0x4000 : id == 5 ? 0x2e14 : 0xf5c);
}

/* 486d00 -> 4e9e68: two repetitions of seven standing poses, no push.
 * 486f90 instead binds crouching clip 255 and plays it out and back. */
static void react_slide_block(Fighter *fighter) {
  start_reaction(fighter, S_BLOCK, fighter->crouching ? &slide_crouch_block : &slide_block);
}

/* 2: 486ca0 -> 486d30/486e20. Two standing cycles, or the crouching
 * out-and-back path; armed standing uses 4e9ec8's five-sample clip 247.
 * Unlike the jab block, all paths retreat at .13 with friction. */
static void react_fireball_block(Fighter *fighter) {
  const ReactionPhase *phase = fighter->crouching ? &slide_crouch_block : &block_long;
  if (fighter->armed) {
    phase = fighter->crouching ? &fireball_armed_crouch_block : &fireball_armed_block;
  }
  start_reaction(fighter, S_BLOCK, phase);
  slide(fighter, fighter->reaction == 9 ? 0x3333 : 0x2147);
}

static void react_approximate(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
    fighter_enter_state(fighter, S_KNOCKDOWN);
  } else {
    fighter_enter_state(fighter, S_HIT);
  }
}

/* 486230/486310/486340/4863b0: six 6-tick shakes, 88-tick hold,
 * three 6-tick shakes. Pose and body stay fixed; cosmetic shake is omitted.
 * Air thaw: 4e9d38 sets gravity 0x147 and waits for landing, then 486530. */
static const MoveDef frozen = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK | SF_HOLD, .action = 0x1003};
static const MoveDef thaw_landing = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};
static const MoveDef thaw_hop = {.clip = 310, .first = 7, .last = 18, .flags = SF_LOCK, .action = 0x1005};

static void thaw_landing_start(Fighter *fighter) {
  fighter_start_move(fighter, S_JUMP, &thaw_landing);
  fighter->sample_override = 18;
  native_begin(fighter, 1, 10, neutral);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, mini(27, 18 + frame), NULL);
  }
  native_play(fighter, false);
}

static void frozen_ground(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void frozen_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  thaw_landing_start(fighter);
}

static void frozen_thaw(Fighter *fighter) {
  fighter->appearance = APPEARANCE_NORMAL;
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x147;
    fighter->script_phase = 1;
    native_begin(fighter, 0, 0, frozen_land);
    native_frame(fighter, 0, fighter_sample(fighter), frozen_ground);
    native_play(fighter, false);
  } else {
    fighter->flags &= ~(unsigned)FF_NO_GUARD;
    neutral(fighter);
  }
}

static void frozen_offset(Fighter *fighter, float offset) {
  fighter_visual_offset(fighter, (q16)offset, (q16)offset);
}

static void frozen_play(Fighter *fighter, int sample) {
  native_begin(fighter, 1, 142, frozen_thaw);
  for (int frame = 1; frame <= 142; ++frame) {
    bool shake = (frame < 36 && frame % 6 < 3) || (frame >= 124 && frame < 142 && (frame - 124) % 6 < 3);
    fighter->native.poses[frame - 1] = sample;
    fighter->native.frames[frame - 1] = (mframe_t){frozen_offset, shake ? 0xccc : 0, NULL};
  }
  native_play(fighter, false);
}

static void thaw_hop_land(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    thaw_landing_start(fighter);
  }
}

static void thaw_hop_hold(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 18, thaw_hop_land);
  native_play(fighter, false);
}

static void thaw_hop_play(Fighter *fighter) {
  native_begin(fighter, 1, 11, thaw_hop_hold);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, 7 + frame, thaw_hop_land);
  }
  native_play(fighter, false);
}

static void react_freeze(Fighter *fighter) {
  if (fighter->action == 0x1003) {
    /* 486430 calls 480f80, which switches to the opponent: double freeze
     * backfires. It does not recursively refresh this victim's timer. */
    Fighter *attacker = &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
    battle_react(attacker, 67);
    if (fighter_airborne(fighter)) {
      fighter_start_move(fighter, S_JUMP, &thaw_hop);
      fighter->sample_override = 7;
      thaw_hop_play(fighter);
      fighter->body.vel[AXIS_Y] = -0x51e;
      fighter->body.gravity = 0x20c;
    } else {
      neutral(fighter);
    }
    return;
  }
  int clip = fighter->appearance == APPEARANCE_FROZEN ? fighter->frozen_clip : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter_start_move(fighter, S_HIT, &frozen);
  fighter->sample_override = sample;
  frozen_play(fighter, sample);
  fighter->frozen_clip = clip;
  fighter->appearance = APPEARANCE_FROZEN;
  fighter->damage_scale = fighter->hits > 0 ? 0x4ccc : 0x8000;
  fighter->flags |= FF_REACTING | FF_NO_GUARD;
  fighter_visual_offset(fighter, 0xccc, 0xccc);
}

/* 4831e0 / 4e94b0: low scepter hit, then the human down-held
 * branch 45b620 chooses crouch clip 193 instead of standing rise 194. */
static const MoveDef scepter_low_hit = {.clip = 192, .last = 34, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef scepter_low_crouch = {.clip = 193, .last = 15, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef scepter_low_rise = {.clip = 194, .last = 23, .flags = SF_LOCK, .action = 0x4003};

static void scepter_low_play(Fighter *fighter);

static void scepter_low_next(Fighter *fighter) {
  if (fighter->current_move == &scepter_low_hit) {
    bool down = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
    fighter_start_move(fighter, S_KNOCKDOWN, down ? &scepter_low_crouch : &scepter_low_rise);
    fighter->sample_override = 0;
    scepter_low_play(fighter);
  } else {
    neutral(fighter);
  }
}

static void scepter_low_play(Fighter *fighter) {
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 1, last + 1, scepter_low_next);
  for (int frame = 1; frame <= last + 1; ++frame) {
    native_frame(fighter, frame, mini(last, frame), frame == 24 && fighter->current_move == &scepter_low_hit ? stop_sliding : NULL);
  }
  native_play(fighter, false);
}

static void react_scepter_low(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &scepter_low_hit);
  fighter->sample_override = 0;
  scepter_low_play(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_set_speed(fighter, -0xf5c);
}

static void react_scepter_freeze(Fighter *fighter) {
  if (fighter->hits > 3) {
    /* 486210 -> 4835f0 uses the shared weapon-throw tumble. */
    fighter->reaction = 154;
    react_extra(fighter);
  } else {
    react_freeze(fighter);
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

static void weapon_followup_play(Fighter *fighter) {
  int last = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count - 1 : fighter_move(fighter)->last;
  reaction_play(fighter, 1, last, last + 1, neutral);
}

static void react_weapon_followup(Fighter *fighter) {
  int selected = 4, clip = fighter_move(fighter)->clip;
  unsigned no_guard = fighter->flags & FF_NO_GUARD;
  int action = fighter->action;
  for (int i = 0; i < 4; ++i) {
    if (weapon_followup[i].clip == clip) {
      selected = i;
    }
  }
  fighter_start_move(fighter, S_BLOCK, &weapon_followup[selected]);
  fighter->sample_override = 0;
  weapon_followup_play(fighter);
  fighter->flags |= no_guard;
  fighter->action = action;
}

/* Reached Liu Kang records: 15/19/31/64/65. Scripts 4e9508,
 * 4e94b0, 4e96e0, 4e9de8 and 4e9d88; shared across victim rosters. */
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef liu_reaction_moves[] = {
  {.clip = 130, .last = 39, .flags = SF_LOCK, .action = ACTION_RETAIL_4001},
  {.clip = 127, .last = 39, .flags = SF_LOCK, .action = ACTION_RETAIL_4001},
  {.clip = 131, .last = 79, .flags = SF_LOCK, .action = ACTION_RETAIL_4003},
  {.clip = 192, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003},
  {.clip = 193, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E},
  {.clip = 194, .last = 23, .flags = SF_LOCK, .action = ACTION_RETAIL_200E},
};
// clang-format on

static void liu_record_impact(Fighter *fighter) {
  battle_scene_impact(fighter);
}

static void liu_record_script(Fighter *fighter) {
  int id = fighter->reaction;
  if (id == 65) {
    battle_sound(SOUND_A, 38);
    slide(fighter, 0x5999);
    native_play_to(fighter, 26, 1);
    fighter->body.physics = NULL;
    fighter_set_speed(fighter, 0x147a);
    native_play_to(fighter, 31, 1);
    stop_sliding(fighter);
    native_play_to(fighter, 39, 1);
  } else if (id == 31) {
    launch(fighter, 0x1999, -0x23d7, 0x24d);
    battle_sound(SOUND_B, 3);
    fighter->body.on_land = liu_record_impact;
    native_play_to(fighter, 39, 1);
    native_until(fighter, native_grounded);
    native_play_to(fighter, 79, 1);
  } else {
    fighter_set_speed(fighter, -0xf5c);
    native_play_to(fighter, 24, 1);
    battle_scene_impact(fighter);
    stop_sliding(fighter);
    native_play_to(fighter, 34, 1);
    native_delay(fighter, 1);
    if (fighter->health <= 0) {
      fighter_fiber_finish(NULL);
    }
    bool down = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
    fighter_fiber_move(fighter, S_KNOCKDOWN, &liu_reaction_moves[down ? 4 : 5]);
    fighter->sample_override = 0;
    native_play_to(fighter, down ? 15 : 23, 1);
  }
  native_delay(fighter, 1);
  fighter->body.on_land = NULL;
  stop_sliding(fighter);
  fighter_fiber_finish(fighter->health > 0 ? fighter_ready : NULL);
}

static void react_liu_record(Fighter *fighter) {
  int index = fighter->reaction == 64 ? 0 : fighter->reaction == 65 ? 1 : fighter->reaction == 31 ? 2 : 3;
  fighter_start_move(fighter, index < 2 ? S_HIT : S_KNOCKDOWN, &liu_reaction_moves[index]);
  fighter->sample_override = 0;
  if (fighter->reaction == 64) {
    liu_kang_entry_retail_script_004e9de8(fighter);
  } else {
    fighter_start_fiber(fighter, liu_record_script, NULL);
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn reactions[256] = {
  [47] = raiden_generated_reaction,
  [53] = raiden_generated_reaction,
  [66] = raiden_generated_reaction,
  [82] = raiden_generated_reaction,
  [83] = raiden_generated_reaction,
  [24] = sonya_generated_reaction,
  [55] = sonya_generated_reaction,
  [56] = sonya_generated_reaction,
  [68] = sonya_generated_reaction,
  [86] = sonya_generated_reaction,
  [87] = sonya_generated_reaction,
  [121] = sonya_generated_reaction,
  [54] = react_extra,
  [57] = react_extra,
  [59] = react_extra,
  [60] = react_extra,
  [61] = react_extra,
  [62] = jarek_weapon_reaction,
  [117] = react_block,
  [15] = react_extra,
  [19] = react_liu_record,
  [31] = react_liu_record,
  [64] = react_liu_record,
  [65] = react_liu_record,
  [2] = react_fireball_block,
  [9] = react_fireball_block,
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
  [92] = fujin_wind_victim,
  [93] = fujin_triple_victim,
  [94] = fujin_triple_victim,
  [95] = fujin_triple_victim,
  [97] = fujin_wind_slam_victim,
  [98] = fujin_grab_victim,
  [181] = fujin_second_victim,
  [179] = shinnok_second_victim,
  [99] = react_extra,
  [100] = react_extra,
  [101] = fujin_bolt_victim,
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
  [123] = react_kick_air_impact,
  [129] = react_kai_skull,
  [130] = react_kai_skull,
  [16] = react_kick_hit,
  [18] = react_sweep,
  [34] = react_uppercut,
  [37] = react_jab_hit,
  [165] = react_slide_block,
  [157] = jax_dash_victim_hit,
  [168] = jax_dash_victim_throw,
  [183] = jax_fatality_2_head,
  [125] = jax_slam_victim,
  [162] = cage_shadow_victim,
  [163] = cage_power_victim,
  [126] = kai_handstand_reaction,
  [127] = kai_handstand_reaction,
  [167] = kai_handstand_reaction,
  [142] = kai_weapon_reaction,
  [145] = kai_weapon_reaction,
  [146] = kai_weapon_reaction,
  [63] = cage_weapon_reaction,
  [149] = cage_weapon_reaction,
  [44] = kai_weapon_reaction,
  [148] = jarek_weapon_reaction,
  [42] = jarek_ball_victim,
  [139] = cage_weapon_reaction,
  [140] = shinnok_weapon_reaction,
  [141] = shinnok_weapon_reaction,
  [185] = cage_second_victim_hit,
  [136] = goro_quake_victim,
  [189] = goro_punch_victim,
  [190] = goro_kick_victim,
  [191] = goro_stomp_victim,
  [192] = goro_uppercut_victim,
  [178] = kai_second_victim,
  [41] = reptile_acid_victim,
  [40] = reptile_reaction_40,
  [158] = reptile_dash_victim,
  [180] = reptile_second_victim,
  [96] = reptile_weapon_reaction,
  [137] = reptile_weapon_reaction,
  [144] = reptile_weapon_reaction,
};
// clang-format on

/* 0x48f880: the victim's move is replaced at once; velocities, gravity and
 * its physics callback are cleared and it is marked as reacting. The
 * reaction starts on the victim's next update. */
void battle_react(Fighter *victim, int reaction) {
  require(reaction >= 0 && reaction < COUNT(reactions), "Invalid reaction id");
  q16 retained_velocity[3];
  if (reaction == 97) {
    memcpy(retained_velocity, victim->body.vel, sizeof retained_velocity);
  }
  victim->body_contact = false;
  /* Drop rules for the reaction ids currently used by our script records.
   * Native 0x494520 is called by these reaction paths, not by ordinary block. */
  if (victim->armed && victim->disarm != NULL &&
      ((reaction >= 10 && reaction <= 13) || (reaction >= 15 && reaction <= 23) || reaction == 31 || reaction == 64 || reaction == 65 || reaction == 34 ||
          (reaction >= 48 && reaction <= 62) || reaction == 70 || reaction == 150 || (reaction == 151 && victim->hits > 3) || reaction == 122 ||
          reaction == 26 || reaction == 28 || reaction == 29 || reaction == 38 || reaction == 39 || reaction == 90 || reaction == 154 || reaction == 155 ||
          reaction == 156 || reaction == 162 || reaction == 173 || reaction == 123 || reaction == 129 || reaction == 130 || reaction == 41 || reaction == 40 || reaction == 158)) {
    victim->disarm(victim);
  }
  /* 483d60 hands an airborne acid victim (48ef30) to reaction 10's 481320. */
  if (reaction == 41 && fighter_airborne(victim)) {
    reaction = 10;
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
  if (victim->roster == 16 && (reaction == 24 || reaction == 55 || reaction == 56 || reaction == 68 || reaction == 86 || reaction == 87 || reaction == 121)) {
    entry = kitana_generated_reaction;
  }

  for (int i = 0; i < 3; ++i) {
    victim->body.vel[i] = 0;
    victim->body.spin[i] = 0;
  }
  victim->body.gravity = 0;
  victim->body.physics = NULL;
  victim->body.on_land = NULL;
  fighter_interrupt(victim, entry);
  /* Native488310 changes gravity on the existing vortex motion. Preserve
   * that motion across cancellation of the previous portable frame action. */
  if (reaction == 97) {
    memcpy(victim->body.vel, retained_velocity, sizeof retained_velocity);
  }
  victim->flags |= FF_REACTING;
}

#include "game.h"
#include "native.h"
#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native_services.h"

void reptile_reaction_40(Fighter *fighter);
void reptile_second_victim(Fighter *fighter);
void fujin_bolt_victim(Fighter *fighter);
void fujin_wind_victim(Fighter *fighter);
void fujin_wind_slam_victim(Fighter *fighter);
void fujin_triple_victim(Fighter *fighter);
void fujin_grab_victim(Fighter *fighter);
void fujin_second_victim(Fighter *fighter);
void goro_quake_victim(Fighter *fighter);
void goro_kick_victim(Fighter *fighter);
void goro_stomp_victim(Fighter *fighter);
void kai_second_victim(Fighter *fighter);
#include "script.h"
#include "math_util.h"

/* Shared reaction phases used by script attack records. Source addresses
 * are in docs/scorpion/combat-core.md. These are our C continuations, not
 * an emulation of the retail task scheduler. Native entry selection is loaded. */

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
  mk4_script_disarm_self(fighter);
  start_reaction(fighter, S_KNOCKDOWN, &jab_air);
  fighter->damage_scale = 0x9999;
  launch(fighter, 0xccc, -0x23d7, 0x28f); /* 0.05, -0.14, 0.01 */
}

/* 13 (0x481d30): kick air hit. */
static void react_kick_air(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
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
  mk4_script_disarm_self(fighter);
  battle_camera_shake(3, 4, 0x2666);
  battle_sound(SOUND_B, 1);
  mk4_script_context_sound(fighter, 0);
  start_reaction(fighter, S_KNOCKDOWN, &kick_air);
  launch(fighter, 0x11eb, -0x23d7, 0x28f);
}

/* 16 (0x482a10): kick hit, including the wall recoil check. */
static void react_kick_hit(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  battle_wall_recoil(fighter);
  start_reaction(fighter, S_HIT, &kick_hit);
  slide(fighter, 0x2666); /* 0.15 */
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

static void reaction_play(Fighter *fighter, int first, int last, int final, FighterFn end) {
  native_begin(fighter, first, final, end);
  for (int frame = first; frame <= final; ++frame) {
    native_frame(fighter, frame, mini(last, frame), NULL);
  }
  native_play(fighter, false);
}

static void react_extra_block(Fighter *fighter) {
  uint32_t id = retail_reaction_native(fighter);
  start_reaction(fighter, S_BLOCK, id == 0x486c70 ? &block_short : &block_long);
  if (id == 0x486bf0 || id == 0x486970) {
    fighter->action = 0x4004;
  }
  slide(fighter, id == 0x486c70 ? 0 : id == 0x486ab0 ? 0x4000 : id == 0x486b70 ? 0x2e14 : 0xf5c);
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
  slide(fighter, retail_reaction_native(fighter) == 0x486b30 ? 0x3333 : 0x2147);
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

static void react_scepter_freeze(Fighter *fighter) {
  if (fighter->hits > 3) {
    /* 486210 -> 4835f0 uses the shared weapon-throw tumble. */
    retail_reaction_fall(fighter);
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

/* Native entry registry. Reaction IDs and roster dispatch stay in retail data. */
FighterFn retail_engine_reaction(uint32_t entry) {
  switch (entry) {
    case 0x465a80:
      return cage_second_victim_hit;
    case 0x466b40:
      return jax_fatality_2_head;
    case 0x4814c0:
      return kai_second_victim;
    case 0x481510:
      return shinnok_second_victim;
    case 0x481530:
      return reptile_second_victim;
    case 0x4816b0:
      return fujin_second_victim;
    case 0x481ce0:
      return react_kick_air_impact;
    case 0x481d30:
      return react_kick_air;
    case 0x481d90:
      return quan_chi_flip_reaction;
    case 0x482070:
      return react_jab_air;
    case 0x4821a0:
      return tanya_weapon_reaction;
    case 0x482a10:
      return react_kick_hit;
    case 0x482c20:
      return react_weapon_followup;
    case 0x482de0:
      return kai_handstand_reaction;
    case 0x4834d0:
      return goro_quake_victim;
    case 0x483550:
      return reptile_reaction_40;
    case 0x483570:
      return goro_kick_victim;
    case 0x4835a0:
      return jarek_ball_victim;
    case 0x4836b0:
      return cage_shadow_victim;
    case 0x483b40:
      return react_jab_hit;
    case 0x484060:
      return quan_chi_steal_reaction;
    case 0x4842d0:
      return goro_stomp_victim;
    case 0x4843b0:
      return quan_chi_stomp_reaction;
    case 0x485f90:
      return quan_chi_skull_reaction;
    case 0x486210:
      return react_scepter_freeze;
    case 0x486230:
      return react_freeze;
    case 0x486710:
      return tanya_weapon_reaction;
    case 0x486770:
      return fujin_bolt_victim;
    case 0x4867b0:
      return jax_dash_victim_throw;
    case 0x486970:
      return react_extra_block;
    case 0x4869c0:
      return react_block;
    case 0x4869f0:
      return react_block;
    case 0x486a30:
      return react_block;
    case 0x486a70:
      return react_block;
    case 0x486ab0:
      return react_extra_block;
    case 0x486af0:
      return react_block;
    case 0x486b30:
      return react_fireball_block;
    case 0x486b70:
      return react_extra_block;
    case 0x486bb0:
      return react_low_block;
    case 0x486bf0:
      return react_extra_block;
    case 0x486c40:
      return react_jab_block;
    case 0x486c70:
      return react_extra_block;
    case 0x486ca0:
      return react_fireball_block;
    case 0x486cd0:
      return react_block;
    case 0x486d00:
      return react_slide_block;
    case 0x487a10:
      return fujin_grab_victim;
    case 0x487ed0:
      return fujin_wind_victim;
    case 0x488310:
      return fujin_wind_slam_victim;
    case 0x4883f0:
      return jax_dash_victim_hit;
    case 0x488400:
      return cage_power_victim;
    case 0x488420:
      return fujin_triple_victim;
    case 0x488450:
      return fujin_triple_victim;
    case 0x488a50:
      return kai_handstand_reaction;
    case 0x488a80:
      return kai_handstand_reaction;
    default:
      return NULL;
  }
}

/* 0x48f880: the victim's move is replaced at once; velocities, gravity and
 * its physics callback are cleared and it is marked as reacting. The
 * reaction starts on the victim's next update. */
void battle_react(Fighter *victim, int reaction) {
  require(reaction >= 0, "Invalid reaction id");
  uint32_t target = retail_reaction_target(retail_vm_image(), (unsigned)reaction);
  FighterFn native = retail_reaction_entry(retail_vm_image(), (unsigned)reaction);
  q16 retained_velocity[3];
  if (target == 0x488310 || target == 0x487350) {
    memcpy(retained_velocity, victim->body.vel, sizeof retained_velocity);
  }
  victim->body_contact = false;
  victim->reaction = reaction;
  if (target != 0x486230 && target != 0x486210) {
    for (int i = 0; i < 3; ++i) {
      victim->body.angle[i] = 0;
    }
    victim->body.pos[AXIS_Y] += victim->body.root_y;
    victim->body.root_y = 0;
  }
  require(native != NULL, "Unimplemented loaded reaction native");

  for (int i = 0; i < 3; ++i) {
    victim->body.vel[i] = 0;
    victim->body.spin[i] = 0;
  }
  victim->body.gravity = 0;
  victim->body.physics = NULL;
  victim->body.on_land = NULL;
  fighter_interrupt(victim, native);
  /* Native488310 changes gravity on the existing vortex motion. Preserve
   * that motion across cancellation of the previous portable frame action. */
  if (target == 0x488310 || target == 0x487350) {
    memcpy(victim->body.vel, retained_velocity, sizeof retained_velocity);
  }
  victim->flags |= FF_REACTING;
}

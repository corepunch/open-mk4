#include "data/layout.h"
#include "native/capabilities.h"
#include <math.h>
#include <stdlib.h>

/* Named byte addresses of native attack records, checked against the local EXE.
 * Decode the original data through the VM; do not keep copied balance tables. */
enum {
  TORPEDO_ATTACK_RECORD = MK4_LINK_CONTEXTS_TORPEDO_ATTACK_RECORD
};

/* Native service cluster: 45b1f0, 45b230. */

/* Native service cluster: 45b1f0, 45b230. */

/* Native service cluster: 42c7b0, 45b230. */
/* 4de6e4 passes reaction 100 to current-context dispatcher 45b1f0 while the
 * LP-throw victim is still captured; the owner continues its own clip. */
void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    script_vm_enter(fighter, MK4_SCRIPT_GORO_LP_THROW_REPEATED_DAMAGE, fighter_ready);
    return;
  }
  script_vm_enter(fighter, MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE, fighter_ready);
}

/* Native service cluster: 4824f0, 482540, 482790, 484330. */
static void goro_victims_begin_victim(Fighter *victim);
static void goro_victims_stomp_victim_next(Fighter *victim);
static void goro_victims_stomp_victim_rest(Fighter *victim);
static const MoveDef goro_victims_s_victim = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION, .action = ACTION_CODE_4001};

/* 488ba0 and 482790 select 4ea210: standing 328 or held-DOWN crouching 184. */

enum {
  GORO_VICTIMS_STOMP_VICTIM_CLIP = 178,
  GORO_VICTIMS_STOMP_VICTIM_COUNT = 28,
  GORO_VICTIMS_STOMP_VICTIM_LOOP = 15,
  GORO_VICTIMS_STOMP_VICTIM_CYCLES = 5, /* 484330 plus 484420 states 0..3 */
  GORO_VICTIMS_STOMP_VICTIM_REST = 20   /* 4845f0 parks the task for 0x14 visits */
};

/* Shared reaction table entries own a non-Goro victim; the retail
 * victim streams bind their own clips from a locked reaction move. */
static void goro_victims_begin_victim(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &goro_victims_s_victim);
  victim->flags |= FF_REACTING;
}

/* 190 (483570): disarm 494520, then 4ea3e0 -> shared 4e9670 flight. */
void goro_kick_victim(Fighter *victim) {
  goro_victims_begin_victim(victim);
  mk4_script_disarm_self(victim);
  script_vm_enter(victim, MK4_SCRIPT_GORO_KICK_VICTIM, fighter_ready);
}

/* 4824f0: back speed 0x7ae, or 0xf5c after more than one hit, then 4e8e80. */
void goro_script_uppercut_victim_slide(Fighter *victim) {
  fighter_set_speed(victim, victim->hits > 1 ? -0xf5c : -0x7ae);
  script_vm_enter(victim, MK4_SCRIPT_GORO_UPPERCUT_VICTIM_SLIDE, fighter_ready);
}

/* 136 (4834d0): disarm, 48a320 -> 489e80 picks one of list 4ea8f8's three
 * sampled tokens at random, then 4e95c8. */
void goro_quake_victim(Fighter *victim) {
  goro_victims_begin_victim(victim);
  mk4_script_disarm_self(victim);
  action_native_sound_list(MK4_SOUND_RANDOM_GRUNT_LIST);
  script_vm_enter(victim, MK4_SCRIPT_GORO_QUAKE_VICTIM, fighter_ready);
}

/* 191 (4842d0): disarm; an airborne victim falls with 0x1999 velocity and
 * gravity before 4ea400 plays clip 178. */
void goro_stomp_victim(Fighter *victim) {
  goro_victims_begin_victim(victim);
  mk4_script_disarm_self(victim);
  if (fighter_airborne(victim)) {
    victim->body.vel[AXIS_Y] = 0x1999;
    victim->body.gravity = 0x1999;
  }
  script_vm_enter(victim, MK4_SCRIPT_GORO_STOMP_VICTIM, fighter_ready);
}

/* 484650: each cycle shakes the camera, cues effect 13 and replays clip
 * 178 from pose 15. After a cycle it continues only while the stomper still
 * has action 0409 or 0400, at most five cycles, then parks and gets up. */
static void goro_victims_stomp_victim_next(Fighter *victim) {
  int action = fighter_opponent(victim)->action;
  if (action != 0x409 && action != 0x400) {
    goro_victims_stomp_victim_rest(victim);
    return;
  }
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
}

static void goro_victims_stomp_victim_getup(Fighter *victim) {
  mk4_mmove_getup(victim, MK4_GETUP_DESCRIPTOR_FLIP_GETUP);
}

static void goro_victims_stomp_victim_rest(Fighter *victim) {
  native_hold(victim, GORO_VICTIMS_STOMP_VICTIM_REST, goro_victims_stomp_victim_getup);
}

void goro_script_stomp_victim_hold(Fighter *victim) {
  int span = GORO_VICTIMS_STOMP_VICTIM_COUNT - GORO_VICTIMS_STOMP_VICTIM_LOOP;
  mk4_mmove_bind(victim, GORO_VICTIMS_STOMP_VICTIM_CLIP, GORO_VICTIMS_STOMP_VICTIM_COUNT, GORO_VICTIMS_STOMP_VICTIM_LOOP);
  native_begin(victim, 0, span * GORO_VICTIMS_STOMP_VICTIM_CYCLES - 1, goro_victims_stomp_victim_rest);
  for (int cycle = 0; cycle < GORO_VICTIMS_STOMP_VICTIM_CYCLES; ++cycle) {
    for (int pose = 0; pose < span; ++pose) {
      FighterFn action = pose == 0 && cycle > 0 ? goro_victims_stomp_victim_next : NULL;
      native_frame(victim, cycle * span + pose, GORO_VICTIMS_STOMP_VICTIM_LOOP + pose, action);
    }
  }
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
  native_play(victim, false);
}

/* 482790 finishes the bound clip before 488ba0's getup. */
/* 482540: 433c00 clears the CPU reaction latch, then 488ba0. */
void goro_victim_getup(Fighter *victim) {
  mk4_mmove_getup(victim, MK4_GETUP_DESCRIPTOR_FLIP_GETUP);
}

/* Native service cluster: 45b1f0, 45b230. */

/* Native service cluster: 45b1f0, 45b230. */
/* 45b1f0 installs the reaction on the task peer: 183 from the head clap;
 * 100/125 end the LP slam, releasing the pair first. */

/* Native service cluster: 45b230. */
/* Native service cluster: 42c0b0, 42c0d0, 45b1f0, 49c110, 49c160. */
/* Original script words own VM control flow; the remaining native adapters
 * implement engine boundaries. See docs/liu-kang-scripts.md for evidence. */

void liu_kang_liu_script_lk_continue(Fighter *fighter) {
  script_vm_enter(fighter, MK4_SCRIPT_LIU_KANG_LK_CONTINUE, fighter_ready);
}

void liu_kang_liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  fighter_execution_cancel(victim);
  battle_damage(victim, 0x2e14);
  /* 4de428 returns to this native caller, which then enters 4de468. */
  script_vm_enter_then(fighter, MK4_SCRIPT_LIU_KANG_LK_THROW_DAMAGE_RELEASE, MK4_SCRIPT_LIU_KANG_LK_THROW_RETREAT_ENTRY, fighter_ready);
}

void liu_kang_liu_script_spawn_low(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang low-fireball task type");
  liu_kang_fireball_spawn(fighter, 1);
}

void liu_kang_liu_script_spawn_high(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang high-fireball task type");
  liu_kang_fireball_spawn(fighter, 0);
}

/* Native service cluster: 45b230. */
/* Native service cluster: 45b1f0, 45b230, 47d680, 47d6e0, 47d720, 47d7f0, 4889b0, 48d450. */
static void raiden_common_torpedo_pose(Fighter *fighter, float pose);
static void raiden_common_torpedo_cycle(Fighter *fighter);
static void raiden_common_torpedo_land(Fighter *fighter);
static void raiden_common_torpedo_miss_motion(Fighter *fighter);
static void raiden_common_torpedo_recoil(Fighter *fighter);
static void raiden_common_torpedo_release(Fighter *fighter);
static void raiden_common_torpedo_pair(Fighter *fighter);

static const MoveDef raiden_common_s_torpedo = {.clip = 94, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x202};
static const MoveDef raiden_common_s_torpedo_victim = {.clip = 135, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_CODE_4003};

void raiden_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static void raiden_common_torpedo_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

static void raiden_common_torpedo_cycle(Fighter *fighter) {
  native_motion_begin(fighter, 22, NULL);
  for (int frame = 0; frame < 22; ++frame) {
    native_motion_frame(fighter, frame, raiden_common_torpedo_pose, (float)(20 + frame / 2), NULL);
  }
  fighter->native_motion.loop = true;
  native_motion_play(fighter);
}

void raiden_script_torpedo_motion(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  int delay = fighter->ticks & 1;
  int count = delay + (30 - pose) * 2;
  if (count <= 0) {
    raiden_common_torpedo_cycle(fighter);
  } else {
    native_motion_begin(fighter, count, raiden_common_torpedo_cycle);
    for (int frame = 0; frame < count; ++frame) {
      int sample = pose + (frame >= delay ? 1 + (frame - delay) / 2 : 0);
      native_motion_frame(fighter, frame, raiden_common_torpedo_pose, (float)sample, NULL);
    }
    native_motion_play(fighter);
  }
  native_motion_step(fighter);
}

unsigned raiden_script_torpedo_query(Fighter *fighter) {
  AttackRecord raiden_common_torpedo_record = script_vm_attack_record(battle_data(), TORPEDO_ATTACK_RECORD);

  return battle_contact(fighter, &raiden_common_torpedo_record) == CONTACT_MISS ? 0u : 1u;
}

static void raiden_common_torpedo_land(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

void raiden_script_torpedo_miss(Fighter *fighter) {
  fighter->body.physics = NULL;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.vel[axis] = q16_mul(fighter->body.vel[axis], 0x4000);
  }
  fighter->body.gravity = 0x28f;
  native_await(fighter, native_grounded, raiden_common_torpedo_land);
}

/* The step runs inside a native frame row that installs its successor
 * afterwards. The world pump runs this physics later in the same tick,
 * before gravity and integration, so the miss keeps the 47d720 timing. */
static void raiden_common_torpedo_miss_motion(Fighter *fighter) {
  raiden_script_torpedo_miss(fighter);
}

void raiden_script_torpedo_step(Fighter *fighter) {
  int64_t radius2 = 0, next2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int64_t pos = fighter->body.pos[axis];
    int64_t next = pos + fighter->body.vel[axis];
    radius2 += pos * pos;
    next2 += next * next;
  }
  if (radius2 >= (int64_t)10 * Q16_ONE * 10 * Q16_ONE && next2 >= radius2) {
    fighter->body.physics = raiden_common_torpedo_miss_motion;
  }
}

static void raiden_common_torpedo_recoil(Fighter *fighter) {
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  native_await(fighter, native_grounded, raiden_common_torpedo_land);
}

static void raiden_common_torpedo_release(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->pending == NULL && (fighter->flags & FF_PAIRED) && (victim->flags & FF_PAIRED)) {
    mk4_script_release_pair(fighter);
    battle_react(victim, 83);
  } else {
    fighter->flags &= ~(unsigned)FF_PAIRED;
  }
  raiden_common_torpedo_recoil(fighter);
}

static void raiden_common_torpedo_pair(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->pending != NULL || !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
    raiden_common_torpedo_release(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y];
  if (fighter_distance(fighter) > 10.f) {
    raiden_common_torpedo_release(fighter);
  }
}

void raiden_script_torpedo_contact(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (fighter->contact == CONTACT_BLOCK) {
    raiden_common_torpedo_recoil(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_mmove_carry_victim(fighter, &raiden_common_s_torpedo_victim);
  fighter_execution_cancel(victim);
  script_vm_enter(victim, MK4_SCRIPT_RAIDEN_TORPEDO_CONTACT, fighter_ready);
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 50, raiden_common_torpedo_release);
  for (int frame = 1; frame <= 50; ++frame) {
    native_frame(fighter, frame, pose, raiden_common_torpedo_pair);
  }
  native_play(fighter, false);
}

void raiden_torpedo(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  bool fast = fighter_fast_start(fighter);
  fighter_start_move(fighter, S_KICK, &raiden_common_s_torpedo);
  fighter_set_speed(fighter, 0x2666);
  mk4_script_save_motion(fighter);
  fighter->execution.cleanup = raiden_special_cleanup;
  ScriptContinuation script = fast ? (ScriptContinuation){MK4_SCRIPT_RAIDEN_TORPEDO_FAST_STARTUP, fighter_ready}
                                   : (ScriptContinuation){MK4_SCRIPT_RAIDEN_TORPEDO_ORDINARY_STARTUP, fighter_ready};
  script_vm_enter(fighter, script.entry, script.finish);
}

void raiden_script_torpedo_victim_motion(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (!(victim->flags & FF_PAIRED)) {
    victim->body.physics = NULL;
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
}

/* Native service cluster: 45b230. */
/* Native service cluster: 45b1f0, 45b230. */
/* 45b1f0 installs a reaction on the current task's captured peer: 102
 * releases the short LP throw victim; 4dff08 dispatches 180 to the
 * second-fatality victim. */

/* Native service cluster: 42bdf0, 42be00, 450d40, 45b1f0, 45b230, 4807b0. */

void scorpion_special_ready(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  fighter_ready(fighter);
}

void scorpion_spin_script(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, NULL);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, frame % 12, frame == 12 ? scorpion_sword_spin_query : scorpion_sword_spin_control);
  }
  fighter->execution.native_frames.poses[0] = 0;
  native_play(fighter, false);
  fighter->sample_override = pose;
}

void scorpion_native_blend(Fighter *fighter, const SceneClip *blend, FighterFn end) {
  native_scene_blends(fighter, blend, 1, end);
}

void scorpion_script_transform_effect(Fighter *fighter, int type) {
  require(type == 0, "Unexpected transform effect");
  mk4_script_effect(fighter, 18);
}

/* Native service cluster: 45b1f0, 45b230. */

/* Native service cluster: 45b1f0, 45b230. */

void sonya_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

/* Native service cluster: 42c200, 42c220, 42c300, 470420, 49bcb0. */

static const MoveDef sub_zero_common_s_lk_throw_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_CODE_0501};
// clang-format off: Keep table entries on one row, including long rows.
static const int sub_zero_common_lk_owner_idle_frames_samples[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};
// clang-format on
static const mframe_t sub_zero_common_lk_owner_idle_frames[60] = {{0}};
static const mmove_t sub_zero_common_lk_owner_idle_phase = {.firstframe = 0,
    .lastframe = 59,
    .frame = sub_zero_common_lk_owner_idle_frames,
    .endfunc = fighter_ready,
    .samples = sub_zero_common_lk_owner_idle_frames_samples};
static const MoveDef sub_zero_common_s_ice = {.clip = 16, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = ACTION_CODE_0304};
static const MeshAnimation sub_zero_common_ice_meshes = {.first = 75, .count = 8, .period = 1, .scale = Q16_ONE};

void sub_zero_script_lk_rebase(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  script_vm_enter(fighter, MK4_SCRIPT_SUB_ZERO_LK_REBASE, fighter_ready);
}

void sub_zero_script_lk_damage(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  memcpy(fighter->body.vel, fighter->saved_motion, sizeof fighter->body.vel);
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.vel[axis] = -fighter->saved_motion[axis];
  }
  fighter->body.physics = fighter_friction;
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  sub_zero_lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  script_vm_enter(fighter, MK4_SCRIPT_SUB_ZERO_LK_DAMAGE, fighter_ready);
}

void sub_zero_script_lk_owner_return(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_mmove(fighter, S_THROW, &sub_zero_common_s_lk_throw_idle, &sub_zero_common_lk_owner_idle_phase);
}

/* Owner submits paired samples; the victim only watches capture lifetime. */

bool sub_zero_owned_object(const Fighter *fighter, int kind) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == kind && &g_battle.fighters[p->owner] == fighter) {
      return true;
    }
  }
  return false;
}

void sub_zero_script_spawn_ice(Fighter *fighter, int task_type) {
  require(task_type == 66, "Invalid ice projectile task type");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_ICE, 180, sub_zero_ice_projectile);
  if (p != NULL) {
    p->mesh_animation = &sub_zero_common_ice_meshes;
    p->initial_mesh = 74;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      q16 heading = fighter_axis(fighter, axis);
      p->pos[axis] += q16_mul(heading, 0xccc);
      p->vel[axis] = q16_mul(heading, 0x3333);
    }
    p->pos[AXIS_Y] -= 0x28f;
  }
}

bool sub_zero_can_ice(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->hits <= 1 && !sub_zero_owned_object(fighter, PROJECTILE_CLONE);
}

void sub_zero_ice(Fighter *fighter) {
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &sub_zero_common_s_ice);
  fighter->script_phase = followup;
  fighter->sample_override = 0;
  script_vm_enter(fighter, MK4_SCRIPT_SUB_ZERO_ICE, fighter_ready);
}

void sub_zero_native_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end) {
  fighter->execution.cleanup = sub_zero_scene_blend_cleanup;
  native_scene_blends(fighter, clips, count, end);
}

/* Native service cluster: 45b1f0, 45b230, 470420. */

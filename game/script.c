#include "script.h"
#include "retail_vm.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <string.h>

static void bind_clip(Fighter *fighter, int clip, int count, int first) {
  require(count > 0 && first >= 0 && first < count, "Invalid native clip binding");
  /* Changing animation is not an attack re-entry: retain action, contact,
   * flags, buttons, physics and the running native continuation. The bound move
   * is actor-owned storage. */
  fighter->playback_move = *fighter_move(fighter);
  fighter->playback_move.clip = clip;
  fighter->playback_move.first = 0;
  fighter->playback_move.last = count - 1;
  fighter->playback_move.sequence = NULL;
  fighter->current_move = &fighter->playback_move;
  fighter->sequence = NULL;
  fighter->sample_override = first;
}

void mk4_mmove_bind(Fighter *fighter, int clip, int count, int first) {
  bind_clip(fighter, clip, count, first);
  /* Native frame continuations own contact queries. Do not also resolve the
   * approximate startup/active/damage window from an old move header. */
  fighter->playback_move.flags |= SF_RECORD;
}

static void blend_weight_frame(Fighter *fighter, float weight) {
  fighter->blend_weight = (q16)weight;
}

static void blend_finish_frame(Fighter *fighter) {
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

void mk4_mmove_blend_to(Fighter *fighter, int clip, int count, int target, q16 step, q16 first, FighterFn end) {
  require(step > 0 && first >= 0 && first <= Q16_ONE, "Invalid native pose blend");
  int source = fighter_move(fighter)->clip;
  int sample = fighter_sample(fighter);
  bind_clip(fighter, clip, count, target);
  fighter->blend_clip = source;
  fighter->blend_sample = sample;
  fighter->blend_duration = 1;
  fighter->blend_weight = first;
  int frames = (Q16_ONE - first) / step + 1;
  native_begin(fighter, 1, frames, end);
  for (int frame = 1; frame <= frames; ++frame) {
    q16 weight = first + mini(frame, frames - 1) * step;
    fighter->native.frames[frame - 1] = (mframe_t){blend_weight_frame, (float)weight, frame == frames ? blend_finish_frame : NULL};
  }
  native_play(fighter, false);
}

void mk4_script_stop(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->body.gravity = 0;
}

void mk4_script_avoid_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}

void mk4_script_avoid_set(Fighter *fighter) {
  fighter->avoids_high = true;
}

/* 459800 -> 470ed0/470f00. Native CPU combo dispatch remains owned by
 * cpu.c; the shared follow-up window still releases busy immediately. */
void mk4_script_attack_return(Fighter *fighter) {
  if (fighter->flags & FF_FOLLOW_UP) {
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

void mk4_script_indexed_sound(Fighter *fighter, int selector) {
  (void)fighter;
  battle_sound(SOUND_INDEXED, selector);
}

static void script_guard_block_sound(Fighter *fighter, int selector) {
  if ((fighter == &g_battle.fighters[0] || fighter == &g_battle.fighters[1]) && fighter->state == S_BLOCK) {
    battle_sound(SOUND_INDEXED, selector);
  }
}

void mk4_script_guard_block_sound_26(Fighter *fighter) {
  script_guard_block_sound(fighter, 26);
}

void mk4_script_guard_block_sound_28(Fighter *fighter) {
  script_guard_block_sound(fighter, 28);
}

/* Shared retail native contracts. */

static bool script_repeat_jab(const Fighter *fighter);

void mk4_script_thrown_ready(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  if (fighter->health > 0) {
    fighter_ready(fighter);
  } else {
    fighter_finish_mmove(fighter, NULL);
  }
}

void mk4_script_impact(Fighter *fighter) {
  battle_scene_impact(fighter);
}

void mk4_script_impact_callback(Fighter *fighter) {
  fighter->body.on_land = mk4_script_impact;
}

void mk4_script_disarm(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

void mk4_script_disarm_self(Fighter *fighter) {
  if (fighter->armed && fighter->disarm != NULL) {
    fighter->disarm(fighter);
  }
}

void mk4_script_touchable(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_UNTOUCHABLE;
}

void mk4_script_damage(Fighter *fighter, int damage) {
  battle_damage((Fighter *)fighter_opponent(fighter), damage);
}

void mk4_script_pair_adjust(Fighter *fighter, int amount) {
  /* 42ceb0 adjusts group velocity, not controller action. */
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_set_speed(victim, amount);
}

void mk4_script_context(Fighter *fighter, int selector) {
  /* 431520/431700 create a camera task. Our camera already follows both
   * live actors; paired positions remain the source of its framing. */
  (void)fighter;
  require(selector >= 0 && selector <= 81, "Unexpected paired camera context");
}

void mk4_script_roster_sound(Fighter *fighter, int selector) {
  require(selector == 8 || selector == 1, "Unexpected roster sound list");
  battle_sound(selector == 8 ? SOUND_THROW_VOICE : SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

void mk4_script_draw_voice(Fighter *fighter) {
  battle_sound(SOUND_DRAW_VOICE, roster_def(fighter->roster)->asset);
}

void mk4_script_scene_active(Fighter *fighter) {
  fighter->flags |= FF_UNTOUCHABLE;
}

/* 46f650 first calls4906c0: controller return snaps to the actor floor. */
void mk4_mmove_thrown_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_finish_mmove(fighter, fighter->health > 0 ? fighter_ready : NULL);
}

static void throw_hold_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
  }
  victim->mmove_hold = (victim->flags & FF_PAIRED) != 0;
}

static void throw_hold_cleanup(Fighter *victim) {
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
}

void mk4_script_pair_cleanup(Fighter *owner) {
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  bool captured = (owner->flags & FF_PAIRED) != 0;
  owner->flags &= ~(unsigned)FF_PAIRED;
  if (captured && fighter_airborne(owner) && owner->body.gravity == 0) {
    owner->body.gravity = 0x28f;
  }
  if (victim->flags & FF_PAIRED) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
    if (fighter_airborne(victim)) {
      victim->body.gravity = 0x28f;
    }
  }
}

static Fighter *start_throw_pair(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, owner_move);
  fighter_start_move(victim, S_KNOCKDOWN, victim_move);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  for (int axis = 0; axis < 3; ++axis) {
    fighter->saved_motion[axis] = victim->body.pos[axis] - fighter->body.pos[axis];
    victim->body.pos[axis] = fighter->body.pos[axis];
  }
  victim->body.pos[AXIS_Y] += victim->body.ground - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  return victim;
}

static void captured_throw_done(Fighter *victim) {
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static const mframe_t captured_throw_frames[] = {{.thinkfunc = throw_hold_frame}};
static const mmove_t captured_throw = {.firstframe = 0, .lastframe = 0, .frame = captured_throw_frames, .endfunc = captured_throw_done};

void mk4_mmove_throw_start(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn script) {
  Fighter *victim = start_throw_pair(fighter, owner_move, victim_move);
  fighter_mmove_phase(victim, &captured_throw);
  victim->mmove_cleanup = throw_hold_cleanup;
  fighter->mmove_cleanup = mk4_script_pair_cleanup;
  script(fighter);
}

/* Native42c590's capture and original4de798 selector. No fighter clip seed. */
static void lp_throw_script(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4de798, fighter_ready);
}

void retail_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  static const MoveDef owner = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
  static const MoveDef victim = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter->native_variant = 0;
  fighter->script_phase = 0;
  mk4_mmove_throw_start(fighter, &owner, &victim, lp_throw_script);
}

/* Captures the victim in place, keeping its own facing (e.g. Raiden's torpedo 47d7f0). */
void mk4_mmove_carry_victim(Fighter *fighter, const MoveDef *move) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, move);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  victim->sample_override = 0;
  fighter_mmove_phase(victim, &captured_throw);
  victim->mmove_cleanup = throw_hold_cleanup;
}

/* 48f9f0 also copies the owner's facing bit, so the victim clip plays in the owner's frame. */
void mk4_mmove_pair_victim(Fighter *fighter, const MoveDef *move) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  mk4_mmove_carry_victim(fighter, move);
}

Fighter *mk4_script_release_pair(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  return victim;
}

void mk4_script_save_motion(Fighter *fighter) {
  memcpy(fighter->saved_motion, fighter->body.vel, sizeof fighter->saved_motion);
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
}

void mk4_script_restore_motion(Fighter *fighter) {
  memcpy(fighter->body.vel, fighter->saved_motion, sizeof fighter->saved_motion);
}

unsigned mk4_script_jab_gate(Fighter *fighter) {
  bool stop = !script_repeat_jab(fighter);
  fighter->buffered &= ~BUTTON_BIT(BUTTON_LOW_PUNCH);
  return stop ? 4u : 0u;
}

void mk4_script_jab_contact(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int action = victim->action;
  if (fighter->connected || action == ACTION_RETAIL_2001 || action == ACTION_RETAIL_0106 || action == ACTION_RETAIL_0107) {
    return;
  }
  if ((action == ACTION_RETAIL_010C && fighter_sample(victim) < 5) || (action == ACTION_RETAIL_0102 && victim->avoids_high)) {
    return;
  }
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : retail_vm_image();
  uint32_t record = mk4_image_u32(image, 0x53d26c);
  AttackRecord attack = retail_vm_attack_record(image, record * 4);
  battle_contact(fighter, &attack);
}

void mk4_script_stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static bool script_repeat_jab(const Fighter *fighter) {
  const Fighter *other = fighter_opponent(fighter);
  q16 dx = other->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  return (fighter->buffered & BUTTON_BIT(BUTTON_LOW_PUNCH)) != 0 && (fighter->facing > 0 ? dx >= 0 : dx <= 0);
}

void mk4_script_context_sound(Fighter *fighter, int selector) {
  (void)fighter;
  require(selector >= 0 && selector < 20, "Invalid context sound selector");
  battle_sound(SOUND_CONTEXT, selector);
}

void mk4_script_reaction_motion(Fighter *fighter, int left, int right, int up, int gravity) {
  /* 490910 combines heading motion with its camera-oriented perpendicular.
   * The second operand is a lateral component, not a second signed speed. */
  q16 x = fighter_axis(fighter, AXIS_X), z = fighter_axis(fighter, AXIS_Z);
  q16 side_x = z, side_z = -x;
  if (q16_float(side_x) * g_battle.view.x + q16_float(side_z) * g_battle.view.z < 0) {
    side_x = -side_x;
    side_z = -side_z;
  }
  fighter->body.vel[AXIS_X] = q16_mul(x, left) + q16_mul(side_x, right);
  fighter->body.vel[AXIS_Z] = q16_mul(z, left) + q16_mul(side_z, right);
  fighter->body.vel[AXIS_Y] = up;
  fighter->body.gravity = gravity;
}

static void landing_whoosh(Fighter *fighter) {
  (void)fighter;
  battle_sound(SOUND_C, 0x58);
}

static const mframe_t landing_sound_frames[12] = {
    [11] = {.thinkfunc = landing_whoosh},
};

void mk4_script_landing_motion(Fighter *fighter) {
  native_motion_bind(fighter, mk4_script_landing_motion, landing_sound_frames, 12);
  native_motion_step(fighter);
}

void mk4_script_scene_pose(Fighter *fighter, int clip, int sample) {
  require(clip >= 0 && clip < MK4_CLIP_COUNT, "Invalid native scene animation");
  retail_vm_bind_animation(fighter, 0x4fad7c + (uint32_t)clip * 12, sample);
  fighter->action = ACTION_RETAIL_0700;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

static void scene_finish_all(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void fan_rise(Fighter *fighter, float rise) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->body.pos[AXIS_Y] = victim->body.ground - (int)rise * 0x3333;
}

void mk4_native_fan_rise(Fighter *fighter) {
  native_motion_begin(fighter, 102, scene_finish_all);
  for (int frame = 0; frame < 102; ++frame) {
    native_motion_frame(fighter, frame, fan_rise, (float)mini(30, frame + 1), NULL);
  }
  native_motion_play(fighter);
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void spike_rise(Fighter *fighter, float rise) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int height = (int)rise;
  mk4_script_scene_pose(victim, 128, mini(31, height * 2));
  victim->body.pos[AXIS_Y] = height == 22 ? -0x59999 : victim->body.ground - height * 0x3333;
}

void mk4_native_spike_tail(Fighter *fighter, FighterFn end) {
  int first = g_battle.phase_ticks + 1;
  int pose = fighter_sample(fighter);
  native_begin(fighter, first, maxi(first, 324), end);
  for (int frame = first; frame <= fighter->native.move.lastframe; ++frame) {
    native_frame(fighter, frame, mini(19, pose + frame - first + 1), NULL);
    if (frame >= 64) {
      /* Pose and victim movement have separate frame tracks so the callback
       * needs only the authored rise, never the scene clock. */
      fighter->native.frames[frame - first].aifunc = spike_rise;
      fighter->native.frames[frame - first].dist = (float)mini(22, frame - 64);
    }
  }
  native_play(fighter, false);
}

static void spike_finish(Fighter *fighter) {
  const MoveDef *victory = roster_def(fighter->roster)->moves[S_VICTORY];
  fighter_start_move(fighter, S_VICTORY, victory);
  fighter->sample_override = victory->last == MOVE_END ? 0 : victory->last;
  finish_scene(fighter);
}

void mk4_script_spike_tail(Fighter *fighter) {
  mk4_native_spike_tail(fighter, spike_finish);
}

void mk4_script_shake(Fighter *fighter, int selector) {
  (void)fighter;
  // clang-format off: Keep table entries on one row, including long rows.
  static const int pulses[][3] = {
    {3, 4, 0x2666},
    {2, 2, 0x1999},
    {2, 2, 0x1999},
    {4, 3, 0x2666},
    {10, 3, 0x2e14},
    {2, 3, 0x2e14},
    {3, 3, 0x3333},
    {2, 3, 0xf5c},
    {6, 3, 0x2e14},
  };
  // clang-format on
  require(selector >= 0 && selector < 9, "Unexpected camera pulse selector");
  battle_camera_shake(pulses[selector][0], pulses[selector][1], pulses[selector][2]);
}

void mk4_script_effect(Fighter *fighter, int selector) {
  require(selector >= 0 && selector <= 21, "Unexpected decorative effect selector");
  battle_particles(fighter, selector, 3, true);
}

void mk4_script_projectile_step(Projectile *object, const AttackRecord *record, bool hits_crouching) {
  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
  const Fighter *owner = &g_battle.fighters[object->owner];
  if ((hits_crouching || fighter_opponent(owner)->action != ACTION_RETAIL_2001) && battle_projectile_contact(object, record) != CONTACT_MISS) {
    object->kind = 0;
  }
}

/* 49c300 + 49be10/49ba50: actor-independent heading object, .2 units/tick. */
Projectile *mk4_script_projectile_spawn(Fighter *fighter, ProjectileFn step, const MeshAnimation *mesh, q16 forward, q16 y, int lifetime) {
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, lifetime, step);
  if (object == NULL) {
    return NULL;
  }
  object->mesh_animation = mesh;
  object->initial_mesh = mesh->first;
  object->pos[AXIS_Y] += y;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    object->pos[axis] += q16_mul(fighter_axis(fighter, axis), forward);
    object->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
  return object;
}

void mk4_script_schedule_motion(Fighter *fighter, int speed, int delay, int duration) {
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

static void native_ready_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void weapon_tail_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  native_finish_clip(fighter, native_ready_done);
}

void mk4_mmove_weapon_tail(Fighter *fighter, const AttackRecord *record, int queries) {
  native_query_rows(fighter, record, queries, 1, weapon_tail_recover);
}

static void scene_task_step(Projectile *object) {
  if (g_battle.phase == PHASE_FATALITY) {
    object->lifetime = 2;
  } else {
    object->kind = 0;
  }
}

void mk4_script_scene_task(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected scene keepalive task");
  /* 464bd0 has no presentation or combat writes: it retains its task slot
   * through one-tick reschedules. The bounded object pool owns that lifetime. */
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, scene_task_step);
}

void mk4_mmove_wait_grounded(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

void mk4_mmove_hold(Fighter *fighter) {
  fighter->mmove_hold = true;
}

/* 484f10 selects descriptor4fc8f4 at stored pose65 and finishes its131-pose
 * clip. It stops motion before and after playback; the owner keeps running. */
void mk4_mmove_lk_release(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected shared LK throw release");
  Fighter *victim = mk4_script_release_pair(fighter);
  static const MoveDef released = {.clip = 586, .last = 130, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &released);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  victim->sample_override = 65;
  native_begin(victim, 65, 130, mk4_mmove_thrown_finish);
  for (int sample = 65; sample <= 130; ++sample) {
    native_frame(victim, sample, sample, NULL);
  }
  native_play(victim, false);
}

/* 42be10 ->490ab0 reads speed -0x3333 from4de358, opens descriptor53c540
 * (clip282), and advances to its end before controller return. */
void mk4_mmove_lk_owner_return(Fighter *fighter, FighterFn end) {
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : retail_vm_image();
  fighter_set_speed(fighter, -(q16)mk4_image_u32(image, 0x4de358));
  retail_vm_bind_animation(fighter, mk4_image_u32(image, 0x53c540) * 4, 0);
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 0, last, end);
  for (int sample = 0; sample <= last; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

void mk4_mmove_lk_owner_ready(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void mk4_mmove_side_getup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return;
  }
  mk4_mmove_bind(fighter, 185, 16, 0);
  native_begin(fighter, 0, 15, mk4_mmove_thrown_finish);
  for (int sample = 0; sample < 16; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

static const MoveDef getup_move = {.clip = 328, .last = 14, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_200E};

static void getup_crouch_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, NULL);
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_CROUCH);
  }
}

/* 4914c0 consumes speed/clip and crouching speed/clip pairs. The caller
 * owns the selected data; the shared phase owns KO and controller return. */
void mk4_mmove_getup(Fighter *fighter, const NativeGetupDef *profile) {
  require(profile != NULL && profile->count > 0 && profile->crouch_count > 0, "Invalid native getup profile");
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return;
  }
  bool crouch = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
  int clip = crouch ? profile->crouch_clip : profile->clip;
  int count = crouch ? profile->crouch_count : profile->count;
  q16 speed = crouch ? profile->crouch_speed : profile->speed;
  fighter_start_move(fighter, S_KNOCKDOWN, &getup_move);
  mk4_mmove_bind(fighter, clip, count, 0);
  fighter_set_speed(fighter, speed);
  native_begin(fighter, 0, count - 1, crouch ? getup_crouch_finish : mk4_mmove_thrown_finish);
  for (int frame = 0; frame < count; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

static void finish_getup_ready(Fighter *fighter) {
  mk4_mmove_getup(fighter, fighter->native_context);
}

void mk4_mmove_finish_getup(Fighter *fighter, const NativeGetupDef *profile) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  require(first >= 0 && last != MOVE_END && first <= last, "Getup continuation needs its bounded source pose");
  fighter->native_context = profile;
  native_begin(fighter, first, last, finish_getup_ready);
  for (int frame = first; frame <= last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/*42bf00 completes the clip, then uses the existing engine stance-recovery
 * adaptation for the original0xccc blend before controller return. */
static void lk_blend_hold(Fighter *fighter) {
  native_hold(fighter, 20, native_ready_done);
}

void mk4_mmove_lk_blend_return(Fighter *fighter) {
  native_finish_clip(fighter, lk_blend_hold);
}

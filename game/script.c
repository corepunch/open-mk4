#include "retail_layout.h"
#include "script.h"
#include "retail_vm.h"
#include "native.h"
#include "native_services.h"
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
  /* 431520/4315b0/431700 select or queue camera tasks. The engine camera
   * follows the live actors. Finisher input/invulnerability ownership belongs
   * to battle_begin_finisher, not to a roster-specific camera adapter. */
  (void)fighter;
  require(selector >= 0 && selector <= 255, "Unexpected paired camera context");
}

static uint16_t sound_half(const Mk4Image *image, uint32_t address) {
  require(address % 2 == 0, "Unaligned retail sound token");
  return (uint16_t)(mk4_image_u32(image, address & ~3u) >> ((address & 2u) * 8));
}

/* Native489ff0 selects a direct token row or a per-roster random-list row.
 * This remap is independent of script and native-action roster aliases. */
void mk4_script_roster_sound(Fighter *fighter, int selector) {
  require(selector >= 0 && selector < 22 && fighter->roster >= 0 && fighter->roster < MK4_ROSTER_COUNT, "Invalid roster sound selector");
  const Mk4Image *image = battle_data();
  unsigned roster = fighter->roster == 16 ? 10u : (unsigned)fighter->roster;
  uint32_t remap_address = MK4_RETAIL_DATA_SOUND_ROSTER_REMAP + roster;
  int remap = (int8_t)(mk4_image_u32(image, remap_address & ~3u) >> ((remap_address & 3u) * 8));
  require(remap >= 0 && remap < MK4_ROSTER_COUNT, "Invalid roster sound remap");
  uint32_t table = mk4_image_u32(image, MK4_RETAIL_DATA_ROSTER_SOUND_SELECTOR_TABLE + (unsigned)selector * 4) * 4;
  if (table == 0) {
    return;
  }
  unsigned token;
  if (mk4_image_u32(image, table) != 0) {
    token = sound_half(image, table + (unsigned)remap * 2);
  } else {
    uint32_t list = mk4_image_u32(image, table + ((unsigned)remap + 1) * 4) * 4;
    if (list == 0) {
      return;
    }
    unsigned count = sound_half(image, list);
    require(count > 0 && count <= 256, "Invalid roster sound token list");
    token = sound_half(image, list + 2 + battle_random(count) * 2);
  }
  if (token != 0) {
    battle_sound(SOUND_TOKEN, (int)token);
  }
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

static Fighter *start_throw_pair(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, bool align) {
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
    if (align) {
      victim->body.pos[axis] = fighter->body.pos[axis];
    }
  }
  if (align) {
    victim->body.pos[AXIS_Y] += victim->body.ground - fighter->body.ground;
    victim->facing = fighter->facing;
    memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  }
  return victim;
}

static void captured_throw_done(Fighter *victim) {
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static const mframe_t captured_throw_frames[] = {{.thinkfunc = throw_hold_frame}};
static const mmove_t captured_throw = {.firstframe = 0, .lastframe = 0, .frame = captured_throw_frames, .endfunc = captured_throw_done};

void mk4_mmove_throw_start(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn script) {
  Fighter *victim = start_throw_pair(fighter, owner_move, victim_move, true);
  fighter_mmove_phase(victim, &captured_throw);
  victim->mmove_cleanup = throw_hold_cleanup;
  fighter->mmove_cleanup = mk4_script_pair_cleanup;
  script(fighter);
}

void retail_throw_capture(Fighter *fighter, int action, bool align) {
  static const MoveDef owner = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE};
  static const MoveDef victim_move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};
  Fighter *victim = start_throw_pair(fighter, &owner, &victim_move, align);
  fighter->action = action;
  fighter_mmove_phase(victim, &captured_throw);
  victim->mmove_cleanup = throw_hold_cleanup;
  fighter->mmove_cleanup = mk4_script_pair_cleanup;
}

/* Native42c590's capture and original4de798 selector. No fighter clip seed. */
static void lp_throw_script(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_THROW_SCRIPT, fighter_ready);
}

void retail_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  fighter->native_variant = 0;
  fighter->script_phase = 0;
  retail_throw_capture(fighter, ACTION_RETAIL_0500, true);
  lp_throw_script(fighter);
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
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : battle_data();
  uint32_t record = mk4_image_u32(image, MK4_RETAIL_DATA_JAB_CONTACT);
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
  retail_vm_bind_animation(fighter, RETAIL_ANIMATION_TABLE + (uint32_t)clip * RETAIL_ANIMATION_DESCRIPTOR_SIZE, sample);
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

void mk4_script_shake(Fighter *fighter, int selector) {
  (void)fighter;

  enum {
    CAMERA_SHAKE_ROW_COUNT = 9,
    CAMERA_SHAKE_ROW_BYTES = 12
  };

  require(selector >= 0 && selector < CAMERA_SHAKE_ROW_COUNT, "Unexpected camera pulse selector");
  const Mk4Image *image = battle_data();
  uint32_t row = MK4_RETAIL_DATA_CAMERA_SHAKE_ROWS + (unsigned)selector * CAMERA_SHAKE_ROW_BYTES;
  battle_camera_shake((int)mk4_image_u32(image, row), (int)mk4_image_u32(image, row + 4), (q16)mk4_image_u32(image, row + 8));
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
/* 42be10 ->490ab0 reads speed -0x3333 from4de358, opens descriptor53c540
 * (clip282), and advances to its end before controller return. */
void mk4_mmove_lk_owner_return(Fighter *fighter, FighterFn end) {
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : battle_data();
  fighter_set_speed(fighter, -(q16)mk4_image_u32(image, MK4_RETAIL_DATA_MMOVE_LK_OWNER_RETURN));
  retail_vm_bind_animation(fighter, mk4_image_u32(image, MK4_RETAIL_ANIMATION_DESCRIPTOR_MMOVE_LK_OWNER_RETURN) * 4, 0);
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

/* Native488b40/488b70 selects the original side-getup record. */
void mk4_mmove_side_getup(Fighter *fighter) {
  if (fighter->health <= 0) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_MMOVE_SIDE_GETUP, mk4_mmove_thrown_finish);
    return;
  }
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_SIDE_GETUP);
}

static const MoveDef getup_move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_200E};

void retail_crouch_return(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, NULL);
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_CROUCH);
  }
}

/* 4914c0 consumes speed/clip and crouching speed/clip pairs. The caller
 * owns the selected data; the shared phase owns KO and controller return. */
void mk4_mmove_getup(Fighter *fighter, uint32_t record) {
  require(record == MK4_RETAIL_GETUP_DESCRIPTOR_SIDE_GETUP || record == MK4_RETAIL_GETUP_DESCRIPTOR_FLIP_GETUP ||
              record == MK4_RETAIL_GETUP_DESCRIPTOR_ROLL_GETUP || record == MK4_RETAIL_GETUP_DESCRIPTOR_FRONT_GETUP,
      "Unreviewed native getup record");
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return;
  }
  bool crouch = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
  const Mk4Image *image = battle_data();
  uint32_t pair = record + (crouch ? 8 : 0);
  q16 speed = (q16)mk4_image_u32(image, pair);
  uint32_t descriptor = mk4_image_u32(image, pair + 4) * 4;
  fighter_start_move(fighter, S_KNOCKDOWN, &getup_move);
  retail_vm_bind_animation(fighter, descriptor, 0);
  fighter_set_speed(fighter, speed);
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 0, last, crouch ? retail_crouch_return : mk4_mmove_thrown_finish);
  for (int frame = 0; frame <= last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

static void finish_getup_ready(Fighter *fighter) {
  mk4_mmove_getup(fighter, fighter->native_getup);
}

void mk4_mmove_finish_getup(Fighter *fighter, uint32_t record) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  require(first >= 0 && last != MOVE_END && first <= last, "Getup continuation needs its bounded source pose");
  fighter->native_getup = record;
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

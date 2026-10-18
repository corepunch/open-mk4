#include "script.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <string.h>
#include <stdio.h>

static void bind_clip(Fighter *fighter, int clip, int count, int first) {
  require(count > 0 && first >= 0 && first < count, "Invalid generated clip binding");
  /* Changing animation is not an attack re-entry: retain action, contact,
   * flags, buttons, physics and the running fiber's clock. Storage belongs
   * to the actor rather than to a fiber stack that interruption can free. */
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
}

void mk4_script_bind(Fighter *fighter, int clip, int count, int first) {
  require(fighter_fiber_running(fighter), "Generated clip needs a running owner");
  bind_clip(fighter, clip, count, first);
}

void mk4_script_bind_victim(Fighter *fighter, int clip, int count, int first) {
  require(fighter_fiber_running(fighter), "Generated pair needs a running owner");
  bind_clip((Fighter *)fighter_opponent(fighter), clip, count, first);
}

void mk4_script_inherit(Fighter *fighter, int clip, int count, int sample) {
  if (fighter_move(fighter)->clip != clip || fighter_sample(fighter) != sample) {
    char message[160];
    snprintf(message, sizeof message, "Generated entry needs clip %d sample %d; native continuation has clip %d sample %d", clip, sample,
        fighter_move(fighter)->clip, fighter_sample(fighter));
    require(false, message);
  }
  bind_clip(fighter, clip, count, sample);
}

void mk4_script_inherit_current(Fighter *fighter, int count, int sample) {
  const MoveDef *current = fighter_move(fighter);
  require(fighter_fiber_running(fighter), "Generated clip needs a running owner");
  require(current->clip >= 0 && current->last == count - 1 && fighter->sample_override == sample, "Generated entry needs the inherited clip length and sample");
  bind_clip(fighter, current->clip, count, sample);
}

/* A paired frame boundary advances the captured victim only. The generated
 * owner row supplies its own pose. Release gives the victim an independent
 * phase whose first duration starts on the following simulation tick. */
bool mk4_mmove_pair_step(Fighter *owner) {
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  if (victim->pending != NULL || !(owner->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
    fighter_finish_mmove(owner, fighter_ready);
    return false;
  }
  if (!(victim->flags & FF_PAIR_RELEASE)) {
    victim->sample_override = mini(fighter_move(victim)->last, fighter_sample(victim) + 1);
  }
  return true;
}

void mk4_script_pair_advance(Fighter *fighter, int count) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  for (int i = 0; i < count; ++i) {
    fighter_wait_ticks(1);
    if (victim->pending != NULL || !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
      fighter_fiber_finish(fighter_ready);
    }
    fighter->sample_override = mini(fighter_move(fighter)->last, fighter->sample_override + 1);
    if (!(victim->flags & FF_PAIR_RELEASE)) {
      victim->sample_override = mini(fighter_move(victim)->last, victim->sample_override + 1);
    }
  }
}

/* 45a780 submits the remaining cursor through ordinary playback. */
void mk4_script_current(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
}

/* 45bc70/45bd20 -> 428b10: blend from the live pose into sample zero,
 * with step 0xa3d and first weight 0x4000, then resume at sample one. */
void mk4_script_blend_bind(Fighter *fighter, int clip, int count) {
  require(count > 1, "Blend continuation needs sample one");
  mk4_script_blend_to(fighter, clip, count, 0, 0xa3d, 0x4000);
  fighter->sample_override = 1;
}

void mk4_script_blend_to(Fighter *fighter, int clip, int count, int target, q16 step, q16 first) {
  require(step > 0 && first >= 0 && first <= Q16_ONE, "Invalid native pose blend");
  int source = fighter_move(fighter)->clip;
  int sample = fighter->sample_override;
  bind_clip(fighter, clip, count, target);
  fighter->blend_clip = source;
  fighter->blend_sample = sample;
  fighter->blend_duration = 1;
  for (q16 weight = first; weight <= Q16_ONE; weight += step) {
    fighter->blend_weight = weight;
    fighter_wait_ticks(1);
  }
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

void mk4_script_advance(Fighter *fighter, int count, int step) {
  require(count >= 0 && step != 0 && fighter->sample_override >= 0, "Invalid generated counted playback");
  for (int i = 0; i < count; ++i) {
    fighter_wait_ticks(1);
    fighter->sample_override = maxi(0, mini(fighter_move(fighter)->last, fighter->sample_override + step));
  }
}

void mk4_script_finish_clip(Fighter *fighter, int step) {
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && last >= 0, "Generated playback needs a decoded clip length");
  fighter_fiber_play_to(fighter, step > 0 ? last : 0, step);
  /* A full-clip request submits its final sample before returning. */
  fighter_wait_ticks(1);
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

void mk4_script_reverse_return(Fighter *fighter) {
  fighter_fiber_play_to(fighter, 0, -1);
  fighter_wait_ticks(1);
  fighter_fiber_finish(fighter_ready);
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

/* Native contracts shared by generated fighter scripts. */
static const AttackRecord jab_record = {0xf333, -0x47ae, 0xcccc, 0x7333, 37, 11, 1, 0, 0xa3d}; /* 4ebd38 */

static bool script_repeat_jab(const Fighter *fighter);

void mk4_script_ready(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(fighter_ready);
}

void mk4_script_land(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(mk4_land);
}

void mk4_script_thrown_ready(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  if (fighter->health > 0) {
    fighter_ready(fighter);
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

void mk4_script_wait_land(Fighter *fighter) {
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
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

void mk4_script_pair_to(Fighter *owner, int sample) {
  mk4_script_pair_advance(owner, sample - owner->sample_override);
}

/* 45bbc0/45beb0: reverse both live cursors, then hand off for one actor tick. */
void mk4_script_pair_back_to(Fighter *owner, int sample) {
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  while (owner->sample_override > sample) {
    fighter_wait_ticks(1);
    --owner->sample_override;
    victim->sample_override = maxi(0, victim->sample_override - 1);
  }
  fighter_wait_ticks(1);
}

void mk4_script_thrown_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(fighter->health > 0 ? fighter_ready : NULL);
}

static void throw_hold(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  while (victim->flags & FF_PAIRED) {
    fighter_wait_ticks(1);
    if (owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
      victim->flags &= ~(unsigned)FF_PAIRED;
      mk4_script_thrown_finish(victim);
    }
  }
}

void mk4_script_pair_cleanup(Fighter *owner) {
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  owner->flags &= ~(unsigned)FF_PAIRED;
  if (victim->flags & FF_PAIRED) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    fighter_cancel_fiber(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

void mk4_script_throw_start(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn script) {
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
  fighter_start_fiber(fighter, script, mk4_script_pair_cleanup);
  if (victim->flags & FF_PAIRED) {
    fighter_start_fiber(victim, throw_hold, NULL);
  }
}

void mk4_script_pair_victim(Fighter *fighter, const MoveDef *move) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, move);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  victim->sample_override = 0;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  fighter_start_fiber(victim, throw_hold, NULL);
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
  battle_contact(fighter, &jab_record);
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
  /* 490910 chooses camera-side X motion. The portable simulation resolves
   * the selected horizontal motion along the pair axis. */
  require(left == -right || right == 0, "Unresearched camera-relative reaction motion");
  fighter_set_speed(fighter, left);
  fighter->body.vel[AXIS_Y] = up;
  fighter->body.gravity = gravity;
}

void mk4_script_landing_motion(Fighter *fighter) {
  /* 4707e0 is a twelve-tick sound clock, separate from world landing. */
  if (++fighter->script_timer >= 12) {
    fighter->script_timer = 0;
    battle_sound(SOUND_C, 0x58);
  }
}

void mk4_script_scene_pose(Fighter *fighter, int clip, int count, int sample) {
  bind_clip(fighter, clip, count, sample);
  fighter->action = ACTION_RETAIL_0700;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

void mk4_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* Native 43d530 returns to its scene coordinator. */
}

void mk4_script_fan_victim_end(Fighter *victim) {
  victim->appearance = APPEARANCE_HIDDEN;
  fighter_fiber_finish(NULL);
}

static void fan_watch(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int rise = mini(30, ++fighter->script_timer);
  victim->body.pos[AXIS_Y] = victim->body.ground - rise * 0x3333;
  if (fighter->script_timer >= 102) {
    fighter_cancel_all_fibers();
    battle_end_finisher();
  }
}

void mk4_script_fan_tail(Fighter *fighter, FighterFn cycle, FighterFn release, FighterFn victim_release) {
  for (int i = 0; i < 31; ++i) {
    cycle(fighter);
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_fiber(victim);
  victim->sample_override = 69;
  fighter_start_fiber(victim, victim_release, NULL);
  fighter->script_timer = 0;
  fighter->body.physics = fan_watch;
  release(fighter);
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

void mk4_script_spike_tail(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  while (g_battle.phase_ticks < 324) {
    fighter_wait_ticks(1);
    fighter->sample_override = mini(19, fighter->sample_override + 1);
    if (g_battle.phase_ticks >= 64) {
      int rise = mini(22, g_battle.phase_ticks - 64);
      mk4_script_scene_pose(victim, 128, 110, mini(31, rise * 2));
      victim->body.pos[AXIS_Y] = rise == 22 ? -0x59999 : victim->body.ground - rise * 0x3333;
    }
  }
  const MoveDef *victory = roster_def(fighter->roster)->moves[S_VICTORY];
  fighter_fiber_move(fighter, S_VICTORY, victory);
  fighter->sample_override = victory->last == MOVE_END ? 0 : victory->last;
  fighter_fiber_finish(finish_scene);
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

static void scheduled_motion(Fighter *fighter) {
  if (fighter->script_timer > 0) {
    --fighter->script_timer;
  } else if (fighter->move_loops > 0) {
    fighter_set_speed(fighter, fighter->saved_motion[0]);
    --fighter->move_loops;
  } else {
    fighter_set_speed(fighter, 0);
    fighter->body.physics = NULL;
  }
}

void mk4_script_schedule_motion(Fighter *fighter, int speed, int delay, int duration) {
  require(delay >= 0 && duration >= 0, "Invalid scheduled script motion");
  fighter_set_speed(fighter, 0);
  fighter->saved_motion[0] = speed;
  fighter->script_timer = delay;
  fighter->move_loops = duration;
  fighter->body.physics = scheduled_motion;
}

void mk4_script_weapon_tail(Fighter *fighter, const AttackRecord *record, int queries) {
  for (int visit = 0; visit < queries; ++visit) {
    fighter_wait_ticks(1);
    fighter->sample_override = mini(fighter_move(fighter)->last, fighter->sample_override + 1);
    if (battle_contact(fighter, record) != CONTACT_MISS) {
      break;
    }
  }
  fighter->action = ACTION_RETAIL_1006;
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(fighter_ready);
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

#include "script.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <string.h>

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

void mk4_script_bind(Fighter *fighter, int clip, int count, int first) {
  require(fighter_fiber_running(fighter), "Generated clip needs a running owner");
  bind_clip(fighter, clip, count, first);
}

void mk4_script_bind_victim(Fighter *fighter, int clip, int count, int first) {
  require(fighter_fiber_running(fighter), "Generated pair needs a running owner");
  bind_clip((Fighter *)fighter_opponent(fighter), clip, count, first);
}

void mk4_script_inherit(Fighter *fighter, int clip, int count, int sample) {
  require(fighter_move(fighter)->clip == clip && fighter->sample_override == sample, "Generated entry context differs from native continuation");
  bind_clip(fighter, clip, count, sample);
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
  require(selector >= 0 && selector < 64, "Unexpected paired camera context");
}

void mk4_script_roster_sound(Fighter *fighter, int selector) {
  require(selector == 8, "Unexpected roster sound list");
  battle_sound(SOUND_THROW_VOICE, roster_def(fighter->roster)->asset);
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

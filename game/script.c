#include "script.h"
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

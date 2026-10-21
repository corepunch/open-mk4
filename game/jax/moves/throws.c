#include "retail_vm.h"
#include "../character.h"

static const MoveDef s_slam_fall = {.clip = 173, .last = 22, .flags = SF_LOCK | SF_REACTION};

enum {
  SLAM_LIMIT = 4,
  SLAM_START = 55
};

/* LP throw 42c560 -> 4de798 -> 42c7f0 -> 4de4f0 (victim 145, Jax 83, pair to
 * 55) is Jax's multi-slam. 42cab0 counts slams in task +68: below four it
 * continues for a CPU-owned player or when the 45f2b0 chord for that count
 * is held (4de588/5a8/5c8/5e8). Each extra slam (4de518 to 82, 4de538 to
 * 116, then 55..116 again) subtracts 5%; 4de568/4de558 release the victim
 * with reaction 100/125 and a last 5%. */

/* 48a220 -> 489e80(4ea828): one of tokens 760, 765, 770, 775, 780. */
void jax_throw_grunt(void) {
  battle_sound(SOUND_TOKEN, 760 + 5 * (int)battle_random(5));
}

/* 45f2b0 rows: switches 4d00a4 (HP 0x10, BLK 0x20, HK 0x40, Up 0x01) and
 * 4d00a8 (LP 1, LK 2, RUN 4) under masks 0x71/7 must equal the chord. */
static bool slam_chord(const Fighter *fighter, int count) {
  static const unsigned chords[SLAM_LIMIT] = {
      BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_KICK),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK),
  };
  unsigned mask = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) |
                  BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return (fighter->held & mask) == chords[count];
}

/* 42cab0. The simulation has no CPU-ownership flag (48f2f0); a CPU player
 * continues only when its input frame supplies the chord. */
static bool slam_continue(Fighter *fighter) {
  if (fighter->native_variant >= SLAM_LIMIT) {
    return false;
  }
  return slam_chord(fighter, fighter->native_variant++);
}

void jax_lp_slam_first(Fighter *fighter) {
  if (!slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de568, fighter_ready, 13);
    return;
  }
  /* 42c83f saves the victim group position in task +5c..+64. */
  memcpy(fighter->saved_motion, fighter_opponent(fighter)->body.pos, sizeof fighter->saved_motion);
  retail_vm_enter_owned(fighter, 0x4de518, fighter_ready, 13);
}

void jax_lp_slam_second(Fighter *fighter) {
  if (slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de538, fighter_ready, 13);
  } else {
    retail_vm_enter_owned(fighter, 0x4de558, fighter_ready, 13);
  }
}

void jax_lp_slam_third(Fighter *fighter) {
  if (!slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de568, fighter_ready, 13);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  memcpy(victim->body.pos, fighter->saved_motion, sizeof victim->body.pos);
  fighter->sample_override = SLAM_START;
  victim->sample_override = SLAM_START;
  retail_vm_enter_owned(fighter, 0x4de518, fighter_ready, 13);
}

/* 42c9f0 blends into the stance (48fde0, 0xccc) and returns via 470420. */
void jax_lp_throw_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

static void slam_victim_getup(Fighter *victim) {
  /* 485c40: stop motion, 490350 flips the facing bit, 488ba0 -> 4914c0(4ea210). */
  mk4_script_stop_body(victim);
  victim->facing = -victim->facing;
  mk4_mmove_getup(victim, 0x4ea210);
}

/* 4ea4e8[125] -> 485ba0: 485c60 (disarm, 48a220 grunt, shake 0, 489ff0
 * list 6, floor snap), 485d00 repositions the body from the pair
 * geometry, then 4287a0(4e9bc0) plays clip 173 before 485c40. */
void jax_slam_victim(Fighter *victim) {
  mk4_script_disarm_self(victim);
  jax_throw_grunt();
  mk4_script_shake(victim, 0);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  fighter_start_move(victim, S_KNOCKDOWN, &s_slam_fall);
  victim->sample_override = 0;
  native_begin(victim, 0, s_slam_fall.last, slam_victim_getup);
  for (int frame = 0; frame <= s_slam_fall.last; ++frame) {
    native_frame(victim, frame, frame, NULL);
  }
  native_play(victim, false);
}

#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: native_throws. */
void fujin_enter_throw(Fighter *fighter) {
  retail_lp_throw(fighter);
}

/* Native service cluster: 42c820, 42c8c0, 42c8f0, 42c9f0. */
enum {
  n42c820_SLAM_LIMIT = 4,
  n42c820_SLAM_START = 55
};

/* LP throw 42c560 -> 4de798 -> 42c7f0 -> 4de4f0 (victim 145, Jax 83, pair to
 * 55) is Jax's multi-slam. 42cab0 counts slams in task +68: below four it
 * continues for a CPU-owned player or when the 45f2b0 chord for that count
 * is held (4de588/5a8/5c8/5e8). Each extra slam (4de518 to 82, 4de538 to
 * 116, then 55..116 again) subtracts 5%; 4de568/4de558 release the victim
 * with reaction 100/125 and a last 5%. */

/* 48a220 -> 489e80(4ea828): one of tokens 760, 765, 770, 775, 780. */

/* 45f2b0 rows: switches 4d00a4 (HP 0x10, BLK 0x20, HK 0x40, Up 0x01) and
 * 4d00a8 (LP 1, LK 2, RUN 4) under masks 0x71/7 must equal the chord. */
static bool n42c820_slam_chord(const Fighter *fighter, int count) {
  static const unsigned chords[n42c820_SLAM_LIMIT] = {
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
static bool n42c820_slam_continue(Fighter *fighter) {
  if (fighter->native_variant >= n42c820_SLAM_LIMIT) {
    return false;
  }
  return n42c820_slam_chord(fighter, fighter->native_variant++);
}

void jax_lp_slam_first(Fighter *fighter) {
  if (!n42c820_slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de568, fighter_ready, 13);
    return;
  }
  /* 42c83f saves the victim group position in task +5c..+64. */
  memcpy(fighter->saved_motion, fighter_opponent(fighter)->body.pos, sizeof fighter->saved_motion);
  retail_vm_enter_owned(fighter, 0x4de518, fighter_ready, 13);
}

void jax_lp_slam_second(Fighter *fighter) {
  if (n42c820_slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de538, fighter_ready, 13);
  } else {
    retail_vm_enter_owned(fighter, 0x4de558, fighter_ready, 13);
  }
}

void jax_lp_slam_third(Fighter *fighter) {
  if (!n42c820_slam_continue(fighter)) {
    retail_vm_enter_owned(fighter, 0x4de568, fighter_ready, 13);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  memcpy(victim->body.pos, fighter->saved_motion, sizeof victim->body.pos);
  fighter->sample_override = n42c820_SLAM_START;
  victim->sample_override = n42c820_SLAM_START;
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

/* Native service cluster: 485930. */
static void n485930_lk_thrown_release(Fighter *victim);
static void n485930_lk_thrown_turn(Fighter *victim);

void liu_kang_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void n485930_lk_thrown_release(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->reaction = 80;
  retail_vm_enter_owned(victim, 0x4e9a90, fighter_ready, 3);
}

void liu_kang_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  victim->sample_override = 30;
  native_hold(victim, 2, n485930_lk_thrown_release);
}

void liu_kang_throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void n485930_lk_thrown_turn(Fighter *victim) {
  victim->facing = -victim->facing;
  liu_kang_thrown_finish(victim);
}

/* 485930 reverses the clip to pose 0 and holds it one visit before turning. */
void liu_kang_liu_script_lk_thrown_tail(Fighter *victim) {
  fighter_set_speed(victim, -0x3333);
  victim->body.physics = fighter_friction;
  int first = fighter_sample(victim);
  native_begin(victim, 1, first + 1, n485930_lk_thrown_turn);
  for (int frame = 1; frame <= first + 1; ++frame) {
    native_frame(victim, frame, maxi(0, first - frame), NULL);
  }
  native_play(victim, false);
}

/* Native service cluster: 470420. */
void raiden_script_thrown_return(Fighter *fighter) {
  if (fighter->reaction == 102 && fighter_move(fighter)->clip == 143) {
    retail_getup_flip(fighter);
    return;
  }
  fighter_finish_mmove(fighter, fighter_ready);
}

void raiden_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 169, 43, 0);
  fighter_set_speed(victim, 0xf5c); /* 490ab0 consumes 4e99bc's speed. */
  retail_vm_enter_owned(victim, 0x4e99c0, fighter_ready, 1);
}

/* Native service cluster: native_throws_2. */

/* Native service cluster: native_throws_3. */
static void native_throws_3_lk_victim_land(Fighter *victim);

static void native_throws_3_lk_victim_land(Fighter *victim) {
  native_await(victim, native_grounded, mk4_mmove_thrown_finish);
}

/* The victim advances seven poses and then completes clip 203 in one run. */
void sonya_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 203, 112, 75);
  native_finish_clip(victim, native_throws_3_lk_victim_land);
}

/* Native service cluster: 487220. */
static void n487220_thrown_finish(Fighter *victim);
static void n487220_lk_thrown_release(Fighter *victim);
static const MoveDef n487220_s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef n487220_s_lp_thrown_getup = {.clip = 328, .last = 14, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const mframe_t n487220_lk_thrown_release_phase_frames[5] = {{0}};
static const mmove_t n487220_lk_thrown_release_phase = {
    .firstframe = 42, .lastframe = 46, .frame = n487220_lk_thrown_release_phase_frames, .endfunc = n487220_lk_thrown_release};
static const mframe_t n487220_lk_rise_frames[41] = {{0}};
static const mmove_t n487220_lk_rise = {.firstframe = 0, .lastframe = 40, .frame = n487220_lk_rise_frames, .endfunc = n487220_thrown_finish};

/* A newly installed peer can take its P1/P2 turn in the installation tick.
 * A displayed victim sample must still last one simulation tick. Keep the
 * first wait on that sample as well; subsequent waits follow the same rule. */
static void n487220_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void sub_zero_script_thrown_return(Fighter *victim) {
  if (fighter_move(victim)->clip != 143) {
    mk4_script_thrown_ready(victim);
    return;
  }
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_move(victim, S_KNOCKDOWN, &n487220_s_lp_thrown_getup);
  victim->sample_override = 0;
  sub_zero_script_lp_getup(victim);
}

static void n487220_lk_thrown_release(Fighter *victim) {
  victim->sample_override = 46;
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e9ee0, fighter_ready, 4);
}

void sub_zero_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  fighter_mmove_phase(victim, &n487220_lk_thrown_release_phase);
}

void sub_zero_script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &n487220_s_lk_thrown_rise, &n487220_lk_rise);
}

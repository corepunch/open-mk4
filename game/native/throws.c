#include "native/capabilities.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: native_throws. */
void fujin_enter_throw(Fighter *fighter) {
  action_lp_throw(fighter);
}

/* Native service cluster: 42c820, 42c8c0, 42c8f0, 42c9f0. */
enum {
  SLAM_FIRST_SCRIPT = MK4_LINK_THROWS_SLAM_FIRST_SCRIPT,
  SLAM_SECOND_SCRIPT = MK4_LINK_THROWS_SLAM_SECOND_SCRIPT,
  SLAM_SECOND_RELEASE_SCRIPT = MK4_LINK_THROWS_SLAM_SECOND_RELEASE_SCRIPT,
  SLAM_FIRST_RELEASE_SCRIPT = MK4_LINK_THROWS_SLAM_FIRST_RELEASE_SCRIPT,
  SLAM_CHORD_ROW_BYTES = 32,
  JAX_THROWS_SLAM_LIMIT = 4,
  JAX_THROWS_SLAM_START = 55
};

/* LP throw 42c560 -> 4de798 -> 42c7f0 -> 4de4f0 (victim 145, Jax 83, pair to
 * 55) is Jax's multi-slam. 42cab0 counts slams in task +68: below four it
 * continues for a CPU-owned player or when the 45f2b0 chord for that count
 * is held (4de588/5a8/5c8/5e8). Each extra slam (4de518 to 82, 4de538 to
 * 116, then 55..116 again) subtracts 5%; 4de568/4de558 release the victim
 * with reaction 100/125 and a last 5%. */

/* 48a220 -> 489e80(4ea828): one of tokens 760, 765, 770, 775, 780. */

/* 42cab0. The simulation has no CPU-ownership flag (48f2f0); a CPU player
 * continues only when its input frame supplies the chord. */
static bool jax_throws_slam_continue(Fighter *fighter) {
  if (fighter->native_variant >= JAX_THROWS_SLAM_LIMIT) {
    return false;
  }
  uint32_t row = MK4_DATA_SLAM_CONTINUE_CHORDS + (uint32_t)fighter->native_variant++ * SLAM_CHORD_ROW_BYTES;
  return action_native_button_chord(fighter, row);
}

void jax_lp_slam_first(Fighter *fighter) {
  if (!jax_throws_slam_continue(fighter)) {
    script_vm_enter(fighter, SLAM_FIRST_RELEASE_SCRIPT, fighter_ready);
    return;
  }
  /* 42c83f saves the victim group position in task +5c..+64. */
  memcpy(fighter->saved_motion, fighter_opponent(fighter)->body.pos, sizeof fighter->saved_motion);
  script_vm_enter(fighter, SLAM_FIRST_SCRIPT, fighter_ready);
}

void jax_lp_slam_second(Fighter *fighter) {
  if (jax_throws_slam_continue(fighter)) {
    script_vm_enter(fighter, SLAM_SECOND_SCRIPT, fighter_ready);
  } else {
    script_vm_enter(fighter, SLAM_SECOND_RELEASE_SCRIPT, fighter_ready);
  }
}

void jax_lp_slam_third(Fighter *fighter) {
  if (!jax_throws_slam_continue(fighter)) {
    script_vm_enter(fighter, SLAM_FIRST_RELEASE_SCRIPT, fighter_ready);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  memcpy(victim->body.pos, fighter->saved_motion, sizeof victim->body.pos);
  fighter->sample_override = JAX_THROWS_SLAM_START;
  victim->sample_override = JAX_THROWS_SLAM_START;
  script_vm_enter(fighter, SLAM_FIRST_SCRIPT, fighter_ready);
}

/* 42c9f0 blends into the stance (48fde0, 0xccc) and returns via 470420. */
void jax_lp_throw_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_execution_finish(fighter, fighter_ready);
}

/* Native service cluster: 485930. */
static void liu_kang_throws_lk_thrown_turn(Fighter *victim);

void liu_kang_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_execution_finish(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void liu_kang_throws_lk_thrown_turn(Fighter *victim) {
  victim->facing = -victim->facing;
  liu_kang_thrown_finish(victim);
}

/* 485930 reverses the clip to pose 0 and holds it one visit before turning. */
void liu_kang_liu_script_lk_thrown_tail(Fighter *victim) {
  fighter_set_speed(victim, -0x3333);
  victim->body.physics = fighter_friction;
  int first = fighter_sample(victim);
  native_begin(victim, 1, first + 1, liu_kang_throws_lk_thrown_turn);
  for (int frame = 1; frame <= first + 1; ++frame) {
    native_frame(victim, frame, maxi(0, first - frame), NULL);
  }
  native_play(victim, false);
}

/* Native service cluster: 487220. */
static void sub_zero_throws_thrown_finish(Fighter *victim);
static void sub_zero_throws_lk_thrown_release(Fighter *victim);
static const MoveDef sub_zero_throws_s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_CODE_4003};
static const mframe_t sub_zero_throws_lk_thrown_release_phase_frames[5] = {{0}};
static const mmove_t sub_zero_throws_lk_thrown_release_phase = {
    .firstframe = 42, .lastframe = 46, .frame = sub_zero_throws_lk_thrown_release_phase_frames, .endfunc = sub_zero_throws_lk_thrown_release};
static const mframe_t sub_zero_throws_lk_rise_frames[41] = {{0}};
static const mmove_t sub_zero_throws_lk_rise = {
    .firstframe = 0, .lastframe = 40, .frame = sub_zero_throws_lk_rise_frames, .endfunc = sub_zero_throws_thrown_finish};

/* A newly installed peer can take its P1/P2 turn in the installation tick.
 * A displayed victim sample must still last one simulation tick. Keep the
 * first wait on that sample as well; subsequent waits follow the same rule. */
static void sub_zero_throws_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_execution_finish(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void sub_zero_throws_lk_thrown_release(Fighter *victim) {
  victim->sample_override = 46;
  victim->body.physics = fighter_friction;
  script_vm_enter(victim, MK4_SCRIPT_LK_THROWN_RELEASE, fighter_ready);
}

void sub_zero_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  fighter_mmove_phase(victim, &sub_zero_throws_lk_thrown_release_phase);
}

void sub_zero_script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &sub_zero_throws_s_lk_thrown_rise, &sub_zero_throws_lk_rise);
}

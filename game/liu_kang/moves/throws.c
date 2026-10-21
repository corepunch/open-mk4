#include "retail_vm.h"
#include "../character.h"

static void lk_thrown_release(Fighter *victim);
static void lk_thrown_turn(Fighter *victim);

void liu_kang_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void lk_thrown_release(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->reaction = 80;
  retail_vm_enter_owned(victim, 0x4e9a90, fighter_ready, 3);
}

void liu_kang_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  victim->sample_override = 30;
  native_hold(victim, 2, lk_thrown_release);
}

void liu_kang_throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void lk_thrown_turn(Fighter *victim) {
  victim->facing = -victim->facing;
  liu_kang_thrown_finish(victim);
}

/* 485930 reverses the clip to pose 0 and holds it one visit before turning. */
void liu_kang_liu_script_lk_thrown_tail(Fighter *victim) {
  fighter_set_speed(victim, -0x3333);
  victim->body.physics = fighter_friction;
  int first = fighter_sample(victim);
  native_begin(victim, 1, first + 1, lk_thrown_turn);
  for (int frame = 1; frame <= first + 1; ++frame) {
    native_frame(victim, frame, maxi(0, first - frame), NULL);
  }
  native_play(victim, false);
}

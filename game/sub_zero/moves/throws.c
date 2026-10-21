#include "retail_vm.h"
#include "../character.h"

static void thrown_finish(Fighter *victim);
static void lk_thrown_release(Fighter *victim);
static const MoveDef s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown_getup = {.clip = 328, .last = 14, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const mframe_t lk_thrown_release_phase_frames[5] = {{0}};
static const mmove_t lk_thrown_release_phase = {.firstframe = 42, .lastframe = 46, .frame = lk_thrown_release_phase_frames, .endfunc = lk_thrown_release};
static const mframe_t lk_rise_frames[41] = {{0}};
static const mmove_t lk_rise = {.firstframe = 0, .lastframe = 40, .frame = lk_rise_frames, .endfunc = thrown_finish};

/* A newly installed peer can take its P1/P2 turn in the installation tick.
 * A displayed victim sample must still last one simulation tick. Keep the
 * first wait on that sample as well; subsequent waits follow the same rule. */
static void thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void sub_zero_lp_thrown_cleanup(Fighter *victim) {
  if (victim->body.physics == mk4_script_landing_motion) {
    victim->body.physics = NULL;
  }
  if (victim->body.on_land == mk4_script_impact) {
    victim->body.on_land = NULL;
  }
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
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown_getup);
  victim->sample_override = 0;
  sub_zero_script_lp_getup(victim);
}

static void lk_thrown_release(Fighter *victim) {
  victim->sample_override = 46;
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e9ee0, fighter_ready, 4);
}

void sub_zero_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  fighter_mmove_phase(victim, &lk_thrown_release_phase);
}

void sub_zero_throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

void sub_zero_script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_lk_thrown_rise, &lk_rise);
}

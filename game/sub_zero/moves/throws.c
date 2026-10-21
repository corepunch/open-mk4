#include "../character.h"

static void thrown_finish(Fighter *victim);
static void lk_thrown_release(Fighter *victim);
static void throw_cleanup(Fighter *owner);
static void ground_throw(Fighter *fighter, bool lk);
static const MoveDef s_lk_throw = {.clip = 41, .last = 74, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 165, .last = 46, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown_getup = {.clip = 328, .last = 14, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const mframe_t lk_thrown_release_phase_frames[5] = {{0}};
static const mmove_t lk_thrown_release_phase = {.firstframe = 42, .lastframe = 46, .frame = lk_thrown_release_phase_frames, .endfunc = lk_thrown_release};
static const mframe_t lk_rise_frames[41] = {{0}};
static const mmove_t lk_rise = {.firstframe = 0, .lastframe = 40, .frame = lk_rise_frames, .endfunc = thrown_finish};
static const mframe_t lp_capture_frames[1] = {
    [0] = {.thinkfunc = sub_zero_lp_capture_frames_frame_0},
};
static const mmove_t lp_capture_phase = {.firstframe = 0, .lastframe = 0, .frame = lp_capture_frames, .endfunc = sub_zero_lp_capture_done};

bool sub_zero_can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

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
  sub_zero_entry_retail_script_004e9ee0(victim);
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

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = fighter_peer(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 153 || fighter_move(victim)->clip == 165)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->health <= 0 && victim->script_phase == 1) {
      return;
    }
    mk4_script_stop_body(victim);
    fighter_cancel_mmove(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

void sub_zero_script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_lk_thrown_rise, &lk_rise);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!sub_zero_can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &sub_zero_s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &sub_zero_s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  if (!lk) {
    memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
    victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
    victim->facing = fighter->facing;
    memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  }
  if (lk) {
    fighter_set_speed(fighter, -0x1999);
    mk4_script_save_motion(fighter);
  }
  fighter_mmove_phase(victim, &lp_capture_phase);
  fighter->mmove_cleanup = throw_cleanup;
  (lk ? sub_zero_entry_retail_script_004de490 : sub_zero_entry_retail_script_004de798)(fighter);
}

void sub_zero_enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

void sub_zero_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

void sub_zero_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, sub_zero_entry_retail_script_004e5e28, sub_zero_entry_retail_script_004e9910);
}

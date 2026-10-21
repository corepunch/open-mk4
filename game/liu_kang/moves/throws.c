#include "../character.h"

static void lk_thrown_release(Fighter *victim);
static void lk_thrown_turn(Fighter *victim);
static void ground_thrown_frame(Fighter *victim);
static void ground_thrown_script(Fighter *victim);
static void throw_cleanup(Fighter *owner);
static void ground_throw(Fighter *fighter, bool lk);
static const MoveDef s_lk_throw = {.clip = 35, .last = 43, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 151, .last = 45, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 133, .last = 35, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

bool liu_kang_can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

void liu_kang_thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void lk_thrown_release(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->reaction = 80;
  liu_kang_entry_retail_script_004e9a90(victim);
}

void liu_kang_lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  victim->sample_override = 30;
  native_hold(victim, 2, lk_thrown_release);
}

static void ground_thrown_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    liu_kang_thrown_finish(victim);
    return;
  }
  victim->mmove_hold = true;
}

static void ground_thrown_script(Fighter *victim) {
  native_begin(victim, 0, 0, NULL);
  victim->native.frames[0].thinkfunc = ground_thrown_frame;
  native_play(victim, false);
}

void liu_kang_throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = fighter_peer(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 151 || fighter_move(victim)->clip == 133)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->script_phase == 1) {
      return;
    }
    mk4_script_stop_body(victim);
    fighter_cancel_mmove(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!liu_kang_can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &liu_kang_s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.pos[axis] = fighter->body.pos[axis];
  }
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  fighter->mmove_cleanup = throw_cleanup;
  (lk ? liu_kang_entry_retail_script_004de3b0 : liu_kang_entry_retail_script_004de798)(fighter);
  ground_thrown_script(victim);
}

void liu_kang_enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

void liu_kang_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
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

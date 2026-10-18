static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

static void thrown_play_to(Fighter *victim, int sample, int step) {
  native_play_to(victim, sample, step);
}

static void thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_fiber_finish(victim->health > 0 ? fighter_ready : NULL);
}

static void lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  victim->sample_override = 30;
  native_delay(victim, 2);
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->reaction = 80;
  retail_script_004e9a90(victim);
}

static void ground_thrown_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    fighter_stop_mmove(victim);
    return;
  }
  victim->mmove_hold = true;
}

static void ground_thrown_script(Fighter *victim) {
  native_begin(victim, 0, 0, NULL);
  victim->native.frames[0].thinkfunc = ground_thrown_frame;
  native_wait(victim);
  thrown_finish(victim);
}

static void throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = opponent(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 151 || fighter_move(victim)->clip == 133)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->script_phase == 1) {
      return;
    }
    mk4_script_stop_body(victim);
    fighter_cancel_fiber(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    liu_kang_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
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
  fighter_start_fiber(fighter, lk ? retail_script_004de3b0 : retail_script_004de798, throw_cleanup);
  fighter_start_fiber(victim, ground_thrown_script, NULL);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void liu_script_lk_thrown_tail(Fighter *victim) {
  fighter_set_speed(victim, -0x3333);
  victim->body.physics = fighter_friction;
  thrown_play_to(victim, 0, -1);
  native_delay(victim, 1);
  victim->facing = -victim->facing;
  thrown_finish(victim);
}

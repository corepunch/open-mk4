static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

static void thrown_end_script(Fighter *victim) {
  native_play_to(victim, 130, 1);
  native_delay(victim, 1);
  fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
}

static void ground_thrown_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    fighter_stop_mmove(victim);
    return;
  }
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->mmove_hold = true;
}

static void ground_thrown_script(Fighter *victim) {
  native_begin(victim, 0, 0, NULL);
  victim->native.frames[0].thinkfunc = ground_thrown_frame;
  native_wait(victim);
  fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  fighter_start_fiber(fighter, lk ? retail_script_004de2f8 : retail_script_004de798, NULL);
  fighter_start_fiber(victim, ground_thrown_script, NULL);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void scorpion_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

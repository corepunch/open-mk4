static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
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
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void lp_thrown_cleanup(Fighter *victim) {
  if (victim->body.physics == mk4_script_landing_motion) {
    victim->body.physics = NULL;
  }
  if (victim->body.on_land == mk4_script_impact) {
    victim->body.on_land = NULL;
  }
}

static void script_thrown_return(Fighter *victim) {
  if (fighter_move(victim)->clip != 143) {
    mk4_script_thrown_ready(victim);
    return;
  }
  stop_body(victim);
  if (victim->health <= 0) {
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown_getup);
  victim->sample_override = 0;
  script_lp_getup(victim);
}

static void lk_thrown_release(Fighter *victim) {
  victim->sample_override = 46;
  victim->body.physics = fighter_friction;
  retail_script_004e9ee0(victim);
}

static void lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  fighter_mmove_phase(victim, &lk_thrown_release_phase);
}

static void throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = opponent(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 153 || fighter_move(victim)->clip == 165)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->health <= 0 && victim->script_phase == 1) {
      return;
    }
    stop_body(victim);
    fighter_cancel_fiber(victim);
    fighter_cancel_mmove(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

static void script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_lk_thrown_rise, &lk_rise);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    sub_zero_ready(fighter);
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
  (lk ? retail_script_004de490 : retail_script_004de798)(fighter);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void sub_zero_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, retail_script_004e5e28, retail_script_004e9910);
}

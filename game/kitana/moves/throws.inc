static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

static void script_throw_getup(Fighter *victim) {
  native_until(victim, native_grounded);
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  mk4_script_bind(victim, 328, 15, 0);
  mk4_script_finish_clip(victim, 1);
  native_delay(victim, 1);
  mk4_script_thrown_finish(victim);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_throw_start(fighter, lk ? &s_lk_throw : &s_throw, lk ? &s_lk_thrown : &s_lp_thrown, lk ? retail_script_004de360 : retail_script_004de798);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void lk_victim(Fighter *victim) {
  mk4_script_bind(victim, 203, 112, 75);
  mk4_script_advance(victim, 7, 1);
  mk4_script_finish_clip(victim, 1);
  native_until(victim, native_grounded);
  mk4_script_thrown_finish(victim);
}

static void kitana_air_throw(Fighter *fighter) {
  if (!mk4_can_air_throw(fighter)) {
    return;
  }
  fighter_upright(fighter);
  fighter_upright(opponent(fighter));
  mk4_script_throw_start(fighter, &s_air_owner, &s_air_victim, retail_script_004e5ed0);
}

/* Native controller return used by generated VM programs and native runners. */
static void noob_saibot_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void noob_saibot_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    retail_script_004de6b8(fighter);
  } else {
    retail_script_004de640(fighter);
  }
}

static void noob_saibot_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004de664, NULL);
  retail_script_004de668(fighter);
}

static void noob_saibot_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 102 || reaction == 107) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
    fighter_start_fiber(victim, reaction == 102 ? retail_script_004e9bd0 : retail_script_004e9b40, NULL);
    fighter_fiber_finish(noob_saibot_ready);
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(noob_saibot_ready);
    return;
  }
  require(false, "Unexpected Noob Saibot opponent reaction");
}

static void noob_saibot_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void noob_saibot_shadow_low(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_low, retail_script_004ed268);
}

static void noob_saibot_shadow_up(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_up, retail_script_004ed2c0);
}

static bool noob_saibot_shadow_up_enabled(const Fighter *fighter) {
  /* 45ef20 returns true exactly when current-group +0x4c (gravity) is nonzero. */
  return fighter->body.gravity != 0;
}

static void noob_saibot_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

static void noob_saibot_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

static void noob_saibot_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_fiber_finish(noob_saibot_ready);
}

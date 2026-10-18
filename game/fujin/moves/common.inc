static void fujin_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void fujin_script_lk_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_lp_select(Fighter *fighter) {
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
    retail_script_004de610(fighter);
  }
}

static void fujin_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static void fujin_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(fujin_ready);
    return;
  }
  require(false, "Unexpected Fujin opponent reaction");
}

static void fujin_script_lp_extended_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 109;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  battle_react(victim, 109);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_lp_short_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 102;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  battle_react(victim, 102);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void fujin_state_move(Fighter *fighter, int state) {
  fighter_start_move(fighter, state, moves[state]);
}

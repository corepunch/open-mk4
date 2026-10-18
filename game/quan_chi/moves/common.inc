static void quan_chi_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void quan_chi_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(quan_chi_ready);
    return;
  }
  require(false, "Unexpected Quan Chi opponent reaction");
}

static void quan_chi_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void quan_chi_entry_469c80(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void quan_chi_entry_46a020(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_47a370(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_47cf30(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_499740(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void shinnok_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

static void shinnok_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void shinnok_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void shinnok_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void shinnok_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void shinnok_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    shinnok_fatality_contact_hold(fighter);
    return;
  }
  battle_react(fighter, reaction);
}

static void shinnok_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Shinnok paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_input_target(Fighter *fighter, Button button) {
  fighter_press(fighter, button);
}

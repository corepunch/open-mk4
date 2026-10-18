static void jax_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

static bool jax_contact_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (event->phase == JAX_EVENT_JAB_CONTACT) {
    ContactResult result = battle_contact(owner, &s_jax_jab_record);
    native_hold(owner, result == CONTACT_MISS ? 8 : 17, jax_ready);
  } else {
    native_hold(owner, 1, jax_ready);
  }
  return false;
}

static void jax_script_opponent_reaction(Fighter *fighter, int reaction) {
  battle_react((Fighter *)fighter_opponent(fighter), reaction);
}

static void jax_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 183) {
    fighter_start_fiber(fighter, retail_script_004e5b10, NULL);
    return;
  }
  battle_react(fighter, reaction);
}

static void jax_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void jax_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void jax_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void jax_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void jax_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(jax_ready);
}

static void jax_script_input_47ab10(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &s_jab, retail_script_004e8150);
}

static void jax_script_input_47ab50(Fighter *fighter) {
  retail_script_004e8174(fighter);
}

static void jax_script_input_46a990(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004e5dd8);
}

static void jax_script_input_46a9f0(Fighter *fighter) {
  retail_script_004e5df8(fighter);
}

static void jax_script_lk_return(Fighter *fighter) {
  retail_script_004de358(fighter);
}

static void jax_script_lk_finish(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(jax_ready);
}

/* VM control flow is generated; native adapters reuse C fibers. Playback need not
 * match stored clip length. See docs/liu-kang-scripts.md for the evidence. */
static void liu_kang_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

static Fighter *opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void liu_kang_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void liu_kang_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    liu_kang_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    liu_kang_kick(fighter);
  }
}

static void liu_script_lp_getup(Fighter *victim) {
  if (victim->health <= 0) {
    thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  fighter_fiber_move(victim, S_KNOCKDOWN, &s_lp_getup);
  victim->sample_override = 0;
  fighter_set_speed(victim, -0x147a);
  thrown_play_to(victim, 15, 1);
  native_delay(victim, 1);
  thrown_finish(victim);
}

static void liu_script_lk_continue(Fighter *fighter) {
  retail_script_004de410(fighter);
}

static void liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  fighter_start_fiber(victim, lk_thrown_script, NULL);
  battle_damage(victim, 0x2e14);
  retail_script_004de428(fighter);
  retail_script_004de468(fighter);
}

static void liu_script_spawn_low(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang low-fireball task type");
  fireball_spawn(fighter, 1);
}

static void liu_script_spawn_high(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang high-fireball task type");
  fireball_spawn(fighter, 0);
}

static void liu_script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = opponent(fighter);
  victim->reaction = reaction;
  if (reaction == 104) {
    throw_disarm(victim);
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_release);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    fighter_start_fiber(victim, retail_script_004e9b68, NULL);
  } else if (reaction == 80) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
  } else if (reaction == 176) {
    victim->facing = -victim->facing;
  }
}

static void liu_script_motion(Fighter *fighter, int speed, int delay, int duration) {
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

static void liu_script_back_lp_tail(Fighter *fighter) {
  const WeaponStrike *strike = &sword_attacks[3];
  native_queries(fighter, &strike->record, strike->queries, 1);
  fighter->action = ACTION_RETAIL_1006;
  fighter_set_speed(fighter, 0);
  fighter->body.physics = NULL;
  native_play_to(fighter, strike->move.last, 1);
  native_delay(fighter, 1);
  fighter_fiber_finish(fighter_ready);
}

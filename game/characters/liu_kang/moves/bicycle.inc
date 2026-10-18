static void bicycle_release_script(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  bool air = victim->body.ground - victim->body.pos[AXIS_Y] > 0x3333;
  fighter_fiber_move(victim, S_KNOCKDOWN, air ? &s_bicycle_air_release : &s_bicycle_release);
  victim->reaction = 81;
  victim->sample_override = 0;
  if (air) {
    retail_script_004e9ae8(victim);
  } else {
    victim->body.physics = fighter_friction;
    retail_script_004e9ad0(victim);
  }
}

static void bicycle_release(Fighter *victim) {
  fighter_start_fiber(victim, bicycle_release_script, NULL);
}

static void bicycle_victim_guard(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (victim->script_phase != 0 || owner->action != ACTION_RETAIL_0206 || owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
    fighter_finish_mmove(victim, bicycle_release);
  }
}

static void bicycle_victim_cycle(Fighter *victim) {
  native_begin(victim, 0, 19, NULL);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(victim, frame, 4 + (frame + 1) % 20, bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void bicycle_victim_script(Fighter *victim) {
  native_begin(victim, 1, 24, bicycle_victim_cycle);
  for (int frame = 1; frame <= 24; ++frame) {
    native_frame(victim, frame, frame < 24 ? frame : 4, bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void bicycle_cleanup(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  if (victim->current_move == &s_bicycle_victim && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
}

static void bicycle_query(Fighter *fighter) {
  if (!fighter_airborne(fighter) || battle_contact(fighter, &bicycle_record) != CONTACT_MISS) {
    fighter_stop_mmove(fighter);
  }
}

static void bicycle_pair(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  if (victim->current_move != &s_bicycle_victim || victim->pending != NULL || victim->current_mmove == NULL || !(fighter->flags & FF_PAIRED) ||
      !(victim->flags & FF_PAIRED)) {
    fighter_stop_mmove(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xe666);
  }
}

static bool bicycle_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void bicycle_script(Fighter *fighter) {
  native_begin(fighter, 0, 59, NULL);
  for (int frame = 0; frame < 60; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, bicycle_query);
  }
  native_wait(fighter);
  if (fighter->contact == CONTACT_BLOCK) {
    retail_script_004e8688(fighter);
  }
  if (fighter->contact == CONTACT_HIT) {
    Fighter *victim = opponent(fighter);
    throw_disarm(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_bicycle_victim);
    victim->sample_override = 0;
    victim->reaction = 84;
    victim->flags |= FF_PAIRED | FF_REACTING;
    fighter->flags |= FF_PAIRED;
    fighter->body.pos[AXIS_Y] = mini(fighter->body.pos[AXIS_Y], -0x15999);
    fighter_set_speed(fighter, 0x10a3);
    fighter->body.vel[AXIS_Y] = fighter->body.gravity = 0;
    fighter_start_fiber(victim, bicycle_victim_script, NULL);
    native_begin(fighter, 0, 57, NULL);
    for (int frame = 0; frame < 58; ++frame) {
      native_frame(fighter, frame, 7 + frame % 16, bicycle_pair);
    }
    native_wait(fighter);
    if (victim->current_move == &s_bicycle_victim && victim->pending == NULL && (victim->flags & FF_PAIRED)) {
      victim->flags &= ~(unsigned)FF_PAIRED;
      victim->script_phase = 1;
    }
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    mk4_script_stop_body(fighter);
  }
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0xc4;
    native_until(fighter, bicycle_grounded);
  }
  retail_script_004e8678(fighter);
}

static bool can_bicycle(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action != ACTION_RETAIL_100B && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 120;
}

static void liu_kang_bicycle_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_opponent(fighter)->action == ACTION_RETAIL_100B) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_bicycle);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, retail_script_004e8610, bicycle_cleanup);
}

static void liu_script_bicycle_getup(Fighter *victim) {
  if (victim->health > 0) {
    fighter_fiber_move(victim, S_KNOCKDOWN, &s_bicycle_getup);
    victim->sample_override = 0;
    thrown_play_to(victim, 15, 1);
  }
  thrown_finish(victim);
}

static void bicycle_motion_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

static void liu_script_bicycle_motion(Fighter *fighter) {
  if (fighter->native_motion.binding != liu_script_bicycle_motion) {
    native_motion_begin(fighter, 16, NULL);
    for (int frame = 0; frame < 16; ++frame) {
      native_motion_frame(fighter, frame, bicycle_motion_pose, (float)(7 + (frame + 1) % 16), NULL);
    }
    fighter->native_motion.binding = liu_script_bicycle_motion;
    fighter->native_motion.loop = true;
    native_motion_play(fighter);
    fighter->body.physics = liu_script_bicycle_motion;
  }
  native_motion_step(fighter);
}

/* Native controller return used by generated VM programs and native runners. */
static void sub_zero_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

static void hold_phase(Fighter *fighter, const mmove_t *phase) {
  int sample = fighter_sample(fighter);
  fighter_mmove_phase(fighter, phase);
  fighter->sample_override = sample;
}

static Fighter *opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void sub_zero_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void sub_zero_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    sub_zero_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    sub_zero_kick(fighter);
  }
}

static void script_ready(Fighter *fighter) {
  FighterFn next = fighter->reaction == 102 && fighter_move(fighter)->clip == 143 ? script_thrown_return : fighter_ready;
  fighter_finish_mmove(fighter, next);
}

static void lp_getup_done(Fighter *victim) {
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void script_lp_getup(Fighter *victim) {
  fighter_mmove_phase(victim, &lp_getup_phase);
}

static void script_lk_rebase(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  stop_body(fighter);
  stop_body(victim);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  retail_script_004de4b8(fighter);
}

static void script_lk_damage(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  memcpy(fighter->body.vel, fighter->saved_motion, sizeof fighter->body.vel);
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.vel[axis] = -fighter->saved_motion[axis];
  }
  fighter->body.physics = fighter_friction;
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  retail_script_004de4e0(fighter);
}

static void script_lk_owner_return(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_mmove(fighter, S_THROW, &s_lk_throw_idle, &lk_owner_idle_phase);
}

static void script_lp_owner_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, sub_zero_ready);
}

static void lp_capture_done(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

/* Owner submits paired samples; the victim only watches capture lifetime. */
static void lp_capture_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = !lp_pair_released(fighter);
}

static bool owned_object(const Fighter *fighter, int kind) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == kind && &g_battle.fighters[p->owner] == fighter) {
      return true;
    }
  }
  return false;
}

static void script_spawn_ice(Fighter *fighter, int task_type) {
  require(task_type == 66, "Invalid ice projectile task type");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_ICE, 180, ice_projectile);
  if (p != NULL) {
    p->mesh_animation = &ice_meshes;
    p->initial_mesh = 74;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      q16 heading = fighter_axis(fighter, axis);
      p->pos[axis] += q16_mul(heading, 0xccc);
      p->vel[axis] = q16_mul(heading, 0x3333);
    }
    p->pos[AXIS_Y] -= 0x28f;
  }
}

static bool can_ice(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->hits <= 1 && !owned_object(fighter, PROJECTILE_CLONE);
}

static void sub_zero_ice(Fighter *fighter) {
  bool followup = fighter_fast_start(fighter);
  stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_ice);
  fighter->script_phase = followup;
  fighter->sample_override = 0;
  retail_script_004ed588(fighter);
}

static void native_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end) {
  fighter->mmove_cleanup = scene_blend_cleanup;
  native_scene_blends(fighter, clips, count, end);
}

static void script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return; /* Spike's native arena continuation owns the victim. */
  }
  require(reaction == 102, "Unexpected Sub-Zero generated reaction");
  Fighter *victim = opponent(fighter);
  throw_disarm(victim);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  victim->reaction = reaction;
  victim->sample_override = 25;
  victim->flags |= FF_REACTING;
  victim->mmove_cleanup = lp_thrown_cleanup;
  retail_script_004e9bd0(victim);
}

/* Native controller return used by generated VM programs and native runners. */
static void scorpion_ready(Fighter *fighter) {
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

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static void special_ready(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  scorpion_ready(fighter);
}

static bool fighting(const Fighter *fighter) {
  (void)fighter;
  return g_battle.phase == PHASE_FIGHT;
}

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void scorpion_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void scorpion_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    scorpion_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    scorpion_kick(fighter);
  }
}

static void script_lk_rebase(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  retail_script_004de318(fighter);
}

static void script_lk_damage(Fighter *fighter) {
  retail_script_004de328(fighter);
}

static void script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(scorpion_ready);
}

static void script_lp_return(Fighter *fighter) {
  fighter_fiber_finish(scorpion_ready);
  (void)fighter;
}

static void script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Scorpion pair release");
  Fighter *victim = opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown_end);
  victim->sample_override = 65;
  fighter_start_fiber(victim, thrown_end_script, NULL);
}

static void scorpion_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

static void scorpion_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

static void scorpion_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

static void spin_script(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, NULL);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, frame % 12, frame == 12 ? sword_spin_query : sword_spin_control);
  }
  fighter->native.poses[0] = 0;
  native_play(fighter, false);
  fighter->sample_override = pose;
}

static void script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = opponent(fighter);
  if (reaction == 175) {
    return; /* Native arena continuation owns the impalement and completion. */
  }
  if (reaction == 71) {
    if (victim->action == ACTION_RETAIL_1008) {
      victim->reaction = 71;
    }
    return; /* Pull arrival already started the independently interruptible stun. */
  }
  require(reaction == 100, "Unexpected generated Scorpion reaction");
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  battle_react(victim, reaction);
}

void scorpion_generated_reaction(Fighter *victim) {
  fighter_start_fiber(victim, victim->reaction == 173 ? retail_script_004e97f8 : retail_script_004e9938, NULL);
}

static void native_blend(Fighter *fighter, const SceneClip *blend) {
  native_scene_blends(fighter, blend, 1, NULL);
}

static void script_transform_effect(Fighter *fighter, int type) {
  require(type == 0, "Unexpected transform effect");
  mk4_script_effect(fighter, 18);
}

/* Native controller return used by generated VM programs and native runners. */
static void sonya_ready(Fighter *fighter) {
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

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void sonya_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void sonya_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    sonya_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    sonya_kick(fighter);
  }
}

static void script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 184) {
    Fighter *victim = opponent(fighter);
    mk4_script_scene_pose(victim, 561, 79, 0);
    fighter_start_fiber(victim, retail_script_004e59c8, NULL);
    return;
  }
  if (reaction == 175) {
    return;
  }
  require(reaction == 78 || reaction == 79 || reaction == 103, "Unexpected Sonya paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_fiber(victim, reaction == 78 ? lk_victim : reaction == 79 ? retail_script_004e9a68 : retail_script_004e9b88, NULL);
}

static void script_lk_return(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  native_delay(fighter, 20); /* Native stance blend, step 0xccc. */
  fighter_fiber_finish(sonya_ready);
}

static void special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static void script_reaction_side_getup(Fighter *victim) {
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  mk4_script_bind(victim, 185, 16, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

void sonya_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 24:
      script = retail_script_004e9110;
      break;
    case 55:
      script = retail_script_004e8b38;
      break;
    case 56:
      script = retail_script_004e8f50;
      break;
    case 68:
      script = retail_script_004e9de8;
      break;
    case 86:
      script = retail_script_004ea130;
      break;
    case 87:
      script = square_victim;
      break;
    case 121:
      script = retail_script_004ea0b8;
      battle_wall_recoil(victim);
      break;
    default:
      require(false, "Unexpected Sonya record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  fighter_start_fiber(victim, script, NULL);
}

#include "../character.h"

static void torpedo_pose(Fighter *fighter, float pose);
static void torpedo_cycle(Fighter *fighter);
static void torpedo_pair(Fighter *fighter);
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const NativeHPProfile hp_profile = {&hp_record, &raiden_s_hp, &s_hp_second, raiden_ready};
static const MoveDef s_torpedo = {.clip = 94, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x202};
static const MoveDef s_torpedo_victim = {.clip = 135, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord torpedo_record = {85852, -3932, 62914, 20971, 82, 82, 0, 0, 9830};

/* Native controller return used by generated VM programs and native runners. */
void raiden_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

int raiden_victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

Fighter *raiden_opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

void raiden_stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

void raiden_hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

void raiden_lk(Fighter *fighter) {
  raiden_ground_attack(fighter, G_LOW_KICK);
}

void raiden_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    raiden_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    raiden_ground_attack(fighter, G_CLOSE_HK);
  } else {
    raiden_kick(fighter);
  }
}

void raiden_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 187) {
    Fighter *victim = raiden_opponent(fighter);
    mk4_script_scene_pose(victim, 574, 39, 0);
    fighter_start_fiber(victim, raiden_entry_retail_script_004e5770, NULL);
    return;
  }
  if (reaction == 175) {
    return; /* Arena spike coordinator owns the victim. */
  }
  require(reaction == 102 || reaction == 107 || reaction == 111, "Unexpected Raiden paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_fiber(victim,
      reaction == 111   ? raiden_lk_victim
      : reaction == 107 ? raiden_entry_retail_script_004e9b40
                        : raiden_entry_retail_script_004e9bd0,
      NULL);
}

void raiden_script_lp_select(Fighter *fighter) {
  /* 42d1b0 compares camera-side ordering and squared pair range (40).
   * Resolve camera-side ordering along our fighter axis. */
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    raiden_entry_retail_script_004de6b8(fighter);
  } else {
    raiden_entry_retail_script_004de640(fighter);
  }
}

void raiden_script_lp_end(Fighter *fighter) {
  raiden_entry_retail_script_004de668(fighter);
}

void raiden_script_lp_return(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(raiden_ready);
}

void raiden_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static void torpedo_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

static void torpedo_cycle(Fighter *fighter) {
  native_motion_begin(fighter, 22, NULL);
  for (int frame = 0; frame < 22; ++frame) {
    native_motion_frame(fighter, frame, torpedo_pose, (float)(20 + frame / 2), NULL);
  }
  fighter->native_motion.loop = true;
  native_motion_play(fighter);
}

void raiden_script_torpedo_motion(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  int delay = fighter->ticks & 1;
  int count = delay + (30 - pose) * 2;
  if (count <= 0) {
    torpedo_cycle(fighter);
  } else {
    native_motion_begin(fighter, count, torpedo_cycle);
    for (int frame = 0; frame < count; ++frame) {
      int sample = pose + (frame >= delay ? 1 + (frame - delay) / 2 : 0);
      native_motion_frame(fighter, frame, torpedo_pose, (float)sample, NULL);
    }
    native_motion_play(fighter);
  }
  native_motion_step(fighter);
}

unsigned raiden_script_torpedo_query(Fighter *fighter) {
  return battle_contact(fighter, &torpedo_record) == CONTACT_MISS ? 0u : 1u;
}

void raiden_script_torpedo_miss(Fighter *fighter) {
  fighter->body.physics = NULL;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.vel[axis] = q16_mul(fighter->body.vel[axis], 0x4000);
  }
  fighter->body.gravity = 0x28f;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(raiden_ready);
}

void raiden_script_torpedo_step(Fighter *fighter) {
  int64_t radius2 = 0, next2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int64_t pos = fighter->body.pos[axis];
    int64_t next = pos + fighter->body.vel[axis];
    radius2 += pos * pos;
    next2 += next * next;
  }
  if (radius2 >= (int64_t)10 * Q16_ONE * 10 * Q16_ONE && next2 >= radius2) {
    raiden_script_torpedo_miss(fighter);
  }
}

static void torpedo_pair(Fighter *fighter) {
  Fighter *victim = raiden_opponent(fighter);
  if (victim->pending != NULL || !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
    fighter_stop_mmove(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y];
  if (fighter_distance(fighter) > 10.f) {
    fighter_stop_mmove(fighter);
  }
}

void raiden_script_torpedo_contact(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (fighter->contact != CONTACT_BLOCK) {
    Fighter *victim = raiden_opponent(fighter);
    mk4_script_pair_victim(fighter, &s_torpedo_victim);
    fighter_start_fiber(victim, raiden_entry_retail_script_004ea108, NULL);
    native_calls(fighter, 50, 0, torpedo_pair);
    if (victim->pending == NULL && (fighter->flags & FF_PAIRED) && (victim->flags & FF_PAIRED)) {
      mk4_script_release_pair(fighter);
      battle_react(victim, 83);
    } else {
      fighter->flags &= ~(unsigned)FF_PAIRED;
    }
  }
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(raiden_ready);
}

void raiden_torpedo(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  bool fast = fighter_fast_start(fighter);
  fighter_start_move(fighter, S_KICK, &s_torpedo);
  fighter_set_speed(fighter, 0x2666);
  mk4_script_save_motion(fighter);
  fighter_start_fiber(fighter, fast ? raiden_entry_retail_script_004e8440 : raiden_entry_retail_script_004e8458, raiden_special_cleanup);
}

void raiden_script_reaction_side_getup(Fighter *victim) {
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  mk4_script_bind(victim, 185, 16, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

void raiden_script_torpedo_victim_motion(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (!(victim->flags & FF_PAIRED)) {
    victim->body.physics = NULL;
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
}

void raiden_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 47:
      script = raiden_entry_retail_script_004ea068;
      battle_wall_recoil(victim);
      break;
    case 53:
      script = raiden_entry_retail_script_004e8e18;
      break;
    case 66:
      script = raiden_entry_retail_script_004e9d50;
      break;
    case 82:
      script = raiden_entry_retail_script_004ea108;
      break;
    case 83:
      script = fighter_airborne(victim) ? raiden_entry_retail_script_004e9ae8 : raiden_entry_retail_script_004e9ad0;
      break;
    default:
      require(false, "Unexpected Raiden record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &raiden_s_knockdown);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  fighter_start_fiber(victim, script, NULL);
}

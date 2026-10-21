#include "../character.h"

static void lp_getup_done(Fighter *victim);
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const NativeHPProfile hp_profile = {&hp_record, &sub_zero_s_hp, &s_hp_second, fighter_ready};
static const MoveDef s_lk_throw_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_0501};
static const mframe_t lp_getup_frames[15] = {{0}};
static const mmove_t lp_getup_phase = {.firstframe = 0, .lastframe = 14, .frame = lp_getup_frames, .endfunc = lp_getup_done};
// clang-format off: Keep table entries on one row, including long rows.
static const int lk_owner_idle_frames_samples[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};
// clang-format on
static const mframe_t lk_owner_idle_frames[60] = {{0}};
static const mmove_t lk_owner_idle_phase = {
    .firstframe = 0, .lastframe = 59, .frame = lk_owner_idle_frames, .endfunc = fighter_ready, .samples = lk_owner_idle_frames_samples};
static const MoveDef s_ice = {.clip = 16, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0304};
static const MeshAnimation ice_meshes = {.first = 75, .count = 8, .period = 1, .scale = Q16_ONE};

void sub_zero_hold_phase(Fighter *fighter, const mmove_t *phase) {
  int sample = fighter_sample(fighter);
  fighter_mmove_phase(fighter, phase);
  fighter->sample_override = sample;
}

void sub_zero_hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

void sub_zero_lk(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_LOW_KICK);
}

void sub_zero_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    sub_zero_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    sub_zero_ground_attack(fighter, G_CLOSE_HK);
  } else {
    sub_zero_kick(fighter);
  }
}

void sub_zero_script_ready(Fighter *fighter) {
  FighterFn next = fighter->reaction == 102 && fighter_move(fighter)->clip == 143 ? sub_zero_script_thrown_return : fighter_ready;
  fighter_finish_mmove(fighter, next);
}

static void lp_getup_done(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void sub_zero_script_lp_getup(Fighter *victim) {
  fighter_mmove_phase(victim, &lp_getup_phase);
}

void sub_zero_script_lk_rebase(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  sub_zero_entry_retail_script_004de4b8(fighter);
}

void sub_zero_script_lk_damage(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  memcpy(fighter->body.vel, fighter->saved_motion, sizeof fighter->body.vel);
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.vel[axis] = -fighter->saved_motion[axis];
  }
  fighter->body.physics = fighter_friction;
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  sub_zero_lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  sub_zero_entry_retail_script_004de4e0(fighter);
}

void sub_zero_script_lk_owner_return(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_mmove(fighter, S_THROW, &s_lk_throw_idle, &lk_owner_idle_phase);
}

void sub_zero_script_lp_owner_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void sub_zero_lp_capture_done(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

/* Owner submits paired samples; the victim only watches capture lifetime. */
void sub_zero_lp_capture_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = !sub_zero_lp_pair_released(fighter);
}

bool sub_zero_owned_object(const Fighter *fighter, int kind) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == kind && &g_battle.fighters[p->owner] == fighter) {
      return true;
    }
  }
  return false;
}

void sub_zero_script_spawn_ice(Fighter *fighter, int task_type) {
  require(task_type == 66, "Invalid ice projectile task type");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_ICE, 180, sub_zero_ice_projectile);
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

bool sub_zero_can_ice(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->hits <= 1 && !sub_zero_owned_object(fighter, PROJECTILE_CLONE);
}

void sub_zero_ice(Fighter *fighter) {
  bool followup = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_ice);
  fighter->script_phase = followup;
  fighter->sample_override = 0;
  sub_zero_entry_retail_script_004ed588(fighter);
}

void sub_zero_native_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end) {
  fighter->mmove_cleanup = sub_zero_scene_blend_cleanup;
  native_scene_blends(fighter, clips, count, end);
}

void sub_zero_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return; /* Spike's native arena continuation owns the victim. */
  }
  require(reaction == 102, "Unexpected Sub-Zero generated reaction");
  Fighter *victim = fighter_peer(fighter);
  sub_zero_throw_disarm(victim);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &sub_zero_s_lp_thrown);
  victim->reaction = reaction;
  victim->sample_override = 25;
  victim->flags |= FF_REACTING;
  victim->mmove_cleanup = sub_zero_lp_thrown_cleanup;
  sub_zero_entry_retail_script_004e9bd0(victim);
}

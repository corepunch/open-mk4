#include "../character.h"

/* BLOCK record 4e43c8 (B,F,F,D, window 50, flags 0x41, gate 45e570) ->
 * 467cd0 -> 43bd70 -> 4dfe90[13] = 4669c0: Jax claps the victim's head.
 * After the shared 41aaf0 scene setup, task 466dd0 finishes the victim's
 * current clip (4e5b38) and parks it (466df0), while 4e5a80 binds clip 284
 * on Jax, waits 20 ticks and enters 466a90: 475530 places the victim 0.85
 * units ahead, 15 ticks later 4e5a98 runs the clap. */
static const MoveDef second_owner_pose = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef second_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 412ec0 -> 4d1748: share2 mesh 281 for 30 ticks at joint 10, scale 0x4ccc. */
static const MeshAnimation second_flash_mesh = {
    .first = 281, .count = 1, .period = 1, .scale = 0x4ccc, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

bool jax_can_fatality_2(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static void second_flash_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  Vec3 hand;
  Mat3 unused;
  fighter_joint_position(&g_battle.fighters[object->owner], 10, &hand, &unused);
  object->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
}

/* 412d80: 413070 sparks from joint 10 on the clap approach. */
void jax_fatality_2_sparks(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Jax clap spark task");
  battle_particles(fighter, 10, 3, false);
}

/* 412ec0: the flash mesh follows joint 10 for its 30-tick track. */
void jax_fatality_2_flash(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Jax clap flash task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 30, second_flash_step);
  if (object != NULL) {
    object->mesh_animation = &second_flash_mesh;
    object->initial_mesh = second_flash_mesh.first;
    second_flash_step(object);
  }
}

/* 45b1f0 in 4e5a98: reaction 183 is installed on the task peer. */
void jax_script_peer_reaction(Fighter *fighter, int reaction) {
  require(reaction == 183, "Unexpected Jax scene reaction");
  battle_react((Fighter *)fighter_opponent(fighter), reaction);
}

/* 466a90: 475530 at 0xd999, then 15 ticks before 4e5a98. */
void jax_fatality_2_approach(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xd999);
  }
  native_hold(fighter, 15, jax_entry_retail_script_004e5a98);
}

/* 466df0 finishes only the auxiliary victim task; the body keeps its pose
 * until the clap installs reaction 183. */
void jax_fatality_2_victim_pause(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
}

/* 4ea4e8[183] -> 466b40: 476d80 hides node 2 (head), effect 4, the neck
 * blood task 455ea0 and 4e5b10 (B[8], clip 557, keepalive, clip 206). */
void jax_fatality_2_head(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &second_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(victim, 2, &neck, &unused);
  victim->hidden_joints[0] |= UINT64_C(1) << 2;
  mk4_script_effect(victim, 4);
  battle_blood_hit(fighter_opponent(victim), victim, neck, 0x3333);
  jax_entry_retail_script_004e5b10(victim);
}

static void second_scene_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  battle_end_finisher();
}

/* 466bb0 waits 20 ticks, removes the type-0x26b scene tasks and parks in
 * 4314e0; that last own action is the portable completion boundary. */
void jax_fatality_2_finish(Fighter *victim) {
  native_hold(victim, 20, second_scene_end);
}

void jax_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &second_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  jax_entry_retail_script_004e5b38(victim);
  fighter_start_move(fighter, S_PUNCH, &second_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  jax_entry_retail_script_004e5a80(fighter);
}

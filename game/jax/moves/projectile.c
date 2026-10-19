#include "../character.h"

/* LP D,F record 4e4388 (flags 0x2, gate 45e7e0) and the projectile CPU row
 * 4df9dc both select 496f80: 49c610 snapshots the pair heading into task
 * +68/+6c, action 0x314 is stored and 4ed090 plays clip 29, queues C[93] at
 * pose 10 and spawns independent task 80 (496fc0) before finishing. */
static const MoveDef s_missile = {.clip = 29, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x314};
/* C[4] record 4ebf60: 12%, reactions 68/10/2. */
static const AttackRecord missile_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 68, 10, 2, 2, 0x1eb8};
/* 4ed0b8 -> rig 5066ac (mesh 42); track 5047ac loops meshes 43..48 (504790),
 * three ticks each, in shared_g.geo. */
static const MeshAnimation missile_mesh = {.first = 43, .count = 6, .period = 3, .scale = Q16_ONE};

/* 45e7e0 requires 80 ticks since stamp 0xf. No routine writes that stamp in
 * this EXE (only the 422540 round reset clears it), so the gate reduces to
 * the battle clock that 48bb90 resets with the random seed. */
bool jax_can_missile(const Fighter *fighter) {
  (void)fighter;
  return g_battle.ticks >= 80;
}

/* Descriptor 4ed0c8: 49c6b0 skips contact for action 2001, probes C[4] and
 * calls 49bb00 on any contact (C[0x5e], explosion, removal). Otherwise
 * 49bbd0 removes the object outside the squared arena radius plus 12. */
static void missile_step(Projectile *object) {
  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[object->owner]);
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(object, &missile_record) != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x5e);
    object->kind = 0;
    return;
  }
  int64_t x = object->pos[AXIS_X], z = object->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    object->kind = 0;
  }
}

/* 496fc0: 49c300 spawns 4ed0b8 (forward 0x7333, Y -0x1999); 404af0 scales
 * the snapshotted heading by 0x3333 into X/Z velocity. The retail object
 * has no tick limit; the cap only bounds the shared pool if bounds fail. */
void jax_missile_spawn(Fighter *fighter, int task_type) {
  require(task_type == 80, "Unexpected Jax missile task");
  Projectile *object = mk4_script_projectile_spawn(fighter, missile_step, &missile_mesh, 0x7333, -0x1999, 600);
  if (object == NULL) {
    return;
  }
  object->initial_mesh = 42;
  object->vel[AXIS_X] = q16_mul(fighter->saved_motion[0], 0x3333);
  object->vel[AXIS_Z] = q16_mul(fighter->saved_motion[1], 0x3333);
}

void jax_missile(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_missile);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  jax_entry_retail_script_004ed090(fighter);
}

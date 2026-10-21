#include "retail_vm.h"
#include "../character.h"

static const MoveDef cast = {.clip = 28, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x311};
static const AttackRecord orb_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 64, 10, 2, ATTACK_MARK, 0x1999};
/* 4d1788 -> ten roots50e4dc..50e5d8, all share2_g.geo with the same
 * authored root translation.49726d sets the root's .75 scale. */
static const MeshAnimation orb_mesh = {
    .first = 271, .count = 10, .period = 1, .scale = 0xc000, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

/* 4ed108 is a native callback descriptor (selector0,497340,4973a0,
 * 4973f0), not a VM program. The independent object owns motion and contact. */
static void orb_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  projectile->vel[AXIS_Y] += projectile->gravity;
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  /* 497375 ->49bc00: squared arena radius plus twelve square units. */
  int64_t x = projectile->pos[AXIS_X], z = projectile->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    projectile->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  /* 49c6b0's selector-zero projectile descriptor skips action2001. */
  ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(projectile, &orb_record);
  if (result != CONTACT_MISS) {
    battle_sound(SOUND_C, result == CONTACT_BLOCK ? 0x61 : 0x60);
    projectile->kind = 0;
  }
}

void cage_orb_spawn(Fighter *fighter, int task_type) {
  require(task_type == 79, "Unexpected Cage projectile task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 180, orb_step);
  if (projectile == NULL) {
    return;
  }
  projectile->phase = fighter->action == 0x311 ? 0 : 1;
  projectile->mesh_animation = &orb_mesh;
  projectile->initial_mesh = orb_mesh.first;
  projectile->heading[0] = fighter->saved_motion[0];
  projectile->heading[1] = fighter->saved_motion[1];
  projectile->pos[AXIS_Y] += 0x1999;
  projectile->vel[AXIS_Y] = projectile->phase == 0 ? -0x1999 : -0x3333;
  projectile->gravity = projectile->phase == 0 ? 0x189 : 0x28f;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->pos[axis] += q16_mul(fighter->saved_motion[axis == AXIS_X ? 0 : 1], 0x3333);
    projectile->vel[axis] = q16_mul(fighter->saved_motion[axis == AXIS_X ? 0 : 1], 0x3333);
  }
}

static void cast_orb(Fighter *fighter, int action) {
  if (!fighter_airborne(fighter)) {
    mk4_script_stop_body(fighter);
  }
  fighter_start_move(fighter, S_PUNCH, &cast);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = action;
  retail_vm_enter_owned(fighter, 0x4ed0d8, fighter_ready, 14);
}

void cage_high_orb(Fighter *fighter) {
  cast_orb(fighter, 0x312);
}

void cage_low_orb(Fighter *fighter) {
  cast_orb(fighter, 0x311);
}

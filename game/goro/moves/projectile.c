#include "native_services.h"

static void fireball_step(Projectile *projectile);
static const MoveDef s_fireball = {.clip = 592, .last = 24, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
/* 4ed7e0 selects C[22]; 49c670 queries it every visit without the
 * selector-zero projectile's crouching (2001) exemption. */
static const AttackRecord fireball_record = {0x3d70, -0x51eb, 0x7d70, 0x7333, 64, 10, 7, ATTACK_MARK, 0x23d7};
static const MeshAnimation fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

static void fireball_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  if (battle_projectile_contact(projectile, &fireball_record) != CONTACT_MISS) {
    projectile->kind = 0;
  }
}

/* 49ca90 is Liu Kang's 49c110 task with descriptor 4ed7d0 (rig 50678c,
 * mesh 50, forward 0x2e14, Y -0x1999, track 5047f8) and contact block
 * 4ed7e0: speed 0x3333 along the 49c610 heading snapshot, 48-tick life. */
void goro_fireball_spawn(Fighter *fighter, int task_type) {
  require(task_type == 83, "Unexpected Goro fireball task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, fireball_step);
  if (projectile == NULL) {
    return;
  }
  projectile->mesh_animation = &fireball_mesh;
  projectile->initial_mesh = 50;
  projectile->pos[AXIS_Y] -= 0x1999;
  projectile->pos[AXIS_X] += q16_mul(fighter->saved_motion[0], 0x2e14);
  projectile->pos[AXIS_Z] += q16_mul(fighter->saved_motion[1], 0x2e14);
  projectile->vel[AXIS_X] = q16_mul(fighter->saved_motion[0], 0x3333);
  projectile->vel[AXIS_Z] = q16_mul(fighter->saved_motion[1], 0x3333);
}

/* HP F,B (4ed828[0]) and the CPU projectile row both enter 49ca70: 49c610
 * snapshots the pair heading, then clip-592 script 4ed790 launches task 83. */
void goro_fireball(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_fireball);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter_owned(fighter, 0x4ed790, fighter_ready, 15);
}

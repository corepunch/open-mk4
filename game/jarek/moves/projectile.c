#include "../character.h"

/* D,B+LP and the CPU projectile row -> 497540: 49c610 snapshots the pair
 * heading, action 0x310, then generated 4ed118 spawns task 78 (497580). */
static const MoveDef cast_move = {.clip = 27, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x310};
/* Descriptor 4ed180 selects C[4] (0x4ebf60): 12%, reactions 68/10/2. */
static const AttackRecord blade_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 68, 10, 2, 2, 0x1eb8};
/* 4ed170 rig 0x50c770: share2 geometry, mesh 34, no authored offset. */
static const MeshAnimation blade_mesh = {.first = 34, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  BLADE_FLIGHT,
  BLADE_STUCK,
  BLADE_DEFLECTED
};

static void blade_spin(Projectile *projectile) {
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
    projectile->angle[axis] = (projectile->angle[axis] + projectile->spin[axis]) % 0x6487e;
  }
}

static void blade_hit(Projectile *projectile, ContactResult result) {
  if (result == CONTACT_BLOCK) {
    /* 4978e0 -> 497730: scatter by 4ab690(.1) on X and Z, rise .14 with
     * gravity .0055 and tumble for at most 90 visits. */
    projectile->phase = BLADE_DEFLECTED;
    projectile->vel[AXIS_X] = (q16)battle_random(2 * 0x1999) - 0x1999;
    projectile->vel[AXIS_Z] = (q16)battle_random(2 * 0x1999) - 0x1999;
    projectile->vel[AXIS_Y] = -0x23d7;
    projectile->gravity = 0x168;
    projectile->spin[0] = projectile->spin[1] = 0x28f;
    projectile->spin[2] = 0x4ccc;
    projectile->lifetime = 91;
    return;
  }
  /* 4978e0 stage 0: bank C 8, then ten three-visit stab cycles and a final
   * twenty-visit wait before 497ab0 removes the object. */
  battle_sound(SOUND_C, 8);
  projectile->phase = BLADE_STUCK;
  projectile->ticks = 0;
  projectile->lifetime = 50;
}

static void blade_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  if (projectile->phase == BLADE_FLIGHT) {
    blade_spin(projectile);
    /* 49c690 clears 53c440, so 49c6b0 skips a crouching (2001) victim. */
    ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(projectile, &blade_record);
    if (result != CONTACT_MISS) {
      blade_hit(projectile, result);
    }
    return;
  }
  if (projectile->phase == BLADE_STUCK) {
    for (int axis = 0; axis < 3; ++axis) {
      projectile->pos[axis] += projectile->vel[axis];
    }
    /* Every third visit 4978e0 places the blade two velocity steps short
     * of the victim, retaining its height and heading. */
    if (projectile->ticks % 3 == 1 && projectile->ticks <= 30) {
      projectile->pos[AXIS_X] = victim->body.pos[AXIS_X] - 2 * projectile->vel[AXIS_X];
      projectile->pos[AXIS_Z] = victim->body.pos[AXIS_Z] - 2 * projectile->vel[AXIS_Z];
    }
    return;
  }
  projectile->vel[AXIS_Y] += projectile->gravity;
  blade_spin(projectile);
  /* 497730 removes the blade once it falls .1 below the floor. */
  if (projectile->pos[AXIS_Y] >= 0x1999) {
    projectile->pos[AXIS_Y] = -0x1999;
    projectile->kind = 0;
  }
}

/* 497580 spawns 4ed170 .4 forward and .22 up, at .2 per visit along the
 * snapshotted heading, spinning about Z; its 4976b0 waiter expires after 48. */
void jarek_blade_spawn(Fighter *fighter, int task_type) {
  require(task_type == 78, "Unexpected Jarek blade task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, blade_step);
  if (projectile == NULL) {
    return;
  }
  projectile->phase = BLADE_FLIGHT;
  projectile->mesh_animation = &blade_mesh;
  projectile->initial_mesh = blade_mesh.first;
  projectile->heading[0] = fighter->saved_motion[0];
  projectile->heading[1] = fighter->saved_motion[1];
  projectile->pos[AXIS_Y] += -0x3851;
  projectile->spin[2] = -0x9999;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int component = axis == AXIS_X ? 0 : 1;
    projectile->pos[axis] += q16_mul(fighter->saved_motion[component], 0x6666);
    projectile->vel[axis] = q16_mul(fighter->saved_motion[component], 0x3333);
  }
}

void jarek_blade(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &cast_move);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = 0x310;
  jarek_entry_retail_script_004ed118(fighter);
}

#include "../character.h"

/* LP [F,F] record 4e4818 and the CPU projectile row select native 499740:
 * 49c610 snapshots the pair heading, action 306 enters 4ed3a0 (clip 23). */
static const MoveDef s_skull = {.clip = 23, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = 0x306};
/* 4998d0 copies C[7]/4ebe88 through contact descriptor 4ed3f8. */
static const AttackRecord skull_record = {0x824d, -0x6978, 0xc000, 0x947a, 128, 10, 6, ATTACK_MARK, 0x1999};
/* 53d6b8 spawns rig 50c364 from share2_g.geo at the owner origin: cranium
 * mesh 4 and jaw mesh 5 at its authored (0, 0x1020, -0x324d) offset. */
static const MeshAnimation skull_mesh = {
    .first = 4, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .child = true, .child_mesh = 5, .child_offset = {0, 0x1020, -0x324d}};
/* 40c760: shared smoke track 508e28..508f08, meshes 191..199 at two ticks each. */
static const ParticleVisual smoke_visual = {.animation = {.first = 191, .count = 9, .period = 2, .scale = 0xa666}, .billboard = true, .additive = true};
static const ParticleSpray smoke = {.visual = &smoke_visual, .count = 1, .lifetime = 20, .offset = {0, -0x3333, 0}, .position_spread = {0x1e78, 0, 0x1e78}};
static const MoveDef s_bitten = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION};

enum {
  SKULL_FLIGHT,
  SKULL_BITE,
  /* 4ed930 holds the bite for 2 + 6 * 4 + 2 ticks, then 499850 multiplies the
   * slowed velocity by five for its final 30-tick entry before 405dd0. */
  SKULL_BITE_TICKS = 28,
  SKULL_ESCAPE_TICKS = 30,
};

/* Root track 4ed910 alternates roll -/+0x3333 for 2/4/2 ticks. */
static q16 skull_roll(const Projectile *skull) {
  int tick = skull->ticks % 8;
  return tick >= 2 && tick < 6 ? 0x3333 : -0x3333;
}

static void skull_bite(Projectile *skull) {
  /* 499b40: both hit and blocked contacts slow the skull to 0.4 and install
   * the bite track; the contact waiter task then finishes. */
  skull->phase = SKULL_BITE;
  skull->vel[AXIS_X] = q16_mul(skull->vel[AXIS_X], 0x6666);
  skull->vel[AXIS_Z] = q16_mul(skull->vel[AXIS_Z], 0x6666);
  skull->lifetime = SKULL_BITE_TICKS + SKULL_ESCAPE_TICKS + 1;
}

/* 49c690(4ed3f8) waits for contact each tick; 499ae0 tests the arena bound
 * from 49bc00 and emits smoke on its first visit and then every third visit. */
static void skull_step(Projectile *skull) {
  if (g_battle.contacts_off) {
    skull->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    skull->pos[axis] += skull->vel[axis];
  }
  skull->angle[2] = skull_roll(skull);
  if (skull->phase == SKULL_BITE) {
    if (skull->lifetime == SKULL_ESCAPE_TICKS + 1) {
      skull->vel[AXIS_X] *= 5;
      skull->vel[AXIS_Z] *= 5;
    }
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[skull->owner]);
  /* 49c6b0 skips the C[7] query while the victim action is 2001. */
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(skull, &skull_record) != CONTACT_MISS) {
    skull_bite(skull);
    return;
  }
  int64_t x = skull->pos[AXIS_X], z = skull->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    skull->kind = 0; /* 499c00 frees the object directly. */
    return;
  }
  if (skull->ticks % 3 == 1) {
    battle_spray(&smoke, (Vec3){q16_float(skull->pos[0]), q16_float(skull->pos[1]), q16_float(skull->pos[2])}, (Vec3){0}, 0, 0);
  }
}

/* 499780: spawn task 73 (4998d0), then enter the 4ed3c0 recovery. */
void quan_chi_script_skull_release(Fighter *fighter) {
  Projectile *skull = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, skull_step);
  if (skull != NULL) {
    skull->mesh_animation = &skull_mesh;
    skull->initial_mesh = skull_mesh.first;
    skull->heading[0] = fighter->saved_motion[0];
    skull->heading[1] = fighter->saved_motion[1];
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      skull->vel[axis] = q16_mul(fighter->saved_motion[axis == AXIS_X ? 0 : 1], 0x3333);
    }
  }
  quan_chi_entry_retail_script_004ed3c0(fighter);
}

void quan_chi_skull(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_skull);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  quan_chi_entry_retail_script_004ed3a0(fighter);
}

/* Reaction 128 (485f90): 494520 drops a weapon, then 4e9cd8 knocks the
 * victim back through clip 182 and returns through its own controller. */
void quan_chi_skull_reaction(Fighter *victim) {
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_bitten);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  quan_chi_entry_retail_script_004e9cd8(victim);
}

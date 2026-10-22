#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Named byte addresses of native attack records, checked against the local EXE.
 * Decode the original data through the VM; do not keep copied balance tables. */
enum {
  FIRE_ATTACK_RECORD = 0x4ebf90,
  ORB_ATTACK_RECORD = 0x4ebed0,
  FIREBALL_ATTACK_RECORD = 0x4ec290,
  BLADE_ATTACK_RECORD = 0x4ebf60,
  MISSILE_ATTACK_RECORD = 0x4ebf60,
  DROP_ATTACK_RECORD = 0x4ebe70,
  RISE_ATTACK_RECORD = 0x4ebe58,
  FIREBALL_HIGH_ATTACK_RECORD = 0x4ebed0,
  FIREBALL_LOW_ATTACK_RECORD = 0x4ebee8,
  SKULL_ATTACK_RECORD = 0x4ebe88,
  GENERIC_PROJECTILE_ATTACK_RECORD = 0x4ebf00,
  STAR_ATTACK_RECORD = 0x4ebe40,
  ACID_ATTACK_RECORD = 0x4ebe28,
  RING_ATTACK_RECORD = 0x4ebf60,
  ICE_PROJECTILE_ATTACK_RECORD = 0x4ebf18
};

/* Native service cluster: 4970d0. */
static const MoveDef cage_projectile_cast = {.clip = 28, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x311};

/* 4d1788 -> ten roots50e4dc..50e5d8, all share2_g.geo with the same
 * authored root translation.49726d sets the root's .75 scale. */
static const MeshAnimation cage_projectile_orb_mesh = {
    .first = 271, .count = 10, .period = 1, .scale = 0xc000, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

/* 4ed108 is a native callback descriptor (selector0,497340,4973a0,
 * 4973f0), not a VM program. The independent object owns motion and contact. */
static void cage_projectile_orb_step(Projectile *projectile) {
  AttackRecord cage_projectile_orb_record = retail_vm_attack_record(retail_vm_image(), ORB_ATTACK_RECORD);

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
  ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(projectile, &cage_projectile_orb_record);
  if (result != CONTACT_MISS) {
    battle_sound(SOUND_C, result == CONTACT_BLOCK ? 0x61 : 0x60);
    projectile->kind = 0;
  }
}

void cage_orb_spawn(Fighter *fighter, int task_type) {
  require(task_type == 79, "Unexpected Cage projectile task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 180, cage_projectile_orb_step);
  if (projectile == NULL) {
    return;
  }
  projectile->phase = fighter->action == 0x311 ? 0 : 1;
  projectile->mesh_animation = &cage_projectile_orb_mesh;
  projectile->initial_mesh = cage_projectile_orb_mesh.first;
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

static void cage_projectile_cast_orb(Fighter *fighter, int action) {
  if (!fighter_airborne(fighter)) {
    mk4_script_stop_body(fighter);
  }
  fighter_start_move(fighter, S_PUNCH, &cage_projectile_cast);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = action;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAST_ORB, fighter_ready);
}

void cage_high_orb(Fighter *fighter) {
  cage_projectile_cast_orb(fighter, 0x312);
}

void cage_low_orb(Fighter *fighter) {
  cage_projectile_cast_orb(fighter, 0x311);
}

/* Native service cluster: 46a650. */
static const MoveDef fujin_wind_caster = {.clip = 98, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x403};
static const MoveDef fujin_wind_slam_move = {.clip = 99, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x402};
static const MoveDef fujin_wind_caught = {.clip = 8, .last = 28, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x1001};
static void fujin_wind_vortex_lift(Fighter *fighter);
static void fujin_wind_vortex_next(Fighter *fighter);
static void fujin_wind_wind_effect_step(Projectile *effect);
// clang-format off: Keep table entries on one row, including long rows.
static const ParticleVisual fujin_wind_wind_visuals[] = {
  {.animation = {.first = 84, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 85, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 86, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 87, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 88, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 89, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 90, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 91, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
};
// clang-format on
static const q16 fujin_wind_wind_root_y[] = {0, -0x10a3, -0x2147, -0x3c28, -0x5ae1, -0x71eb, -0x8666, -0xa28f};

bool fujin_can_wind(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && victim->action != 0x100d && victim->action != 0x1001 && victim->action != 0x1002 && victim->hits <= 2;
}

bool fujin_can_wind_slam(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action == 0x1001;
}

static void fujin_wind_wind_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  int slot = fighter == &g_battle.fighters[0] ? 0 : 1;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *effect = &g_battle.projectiles[i];
    if (effect->step == fujin_wind_wind_effect_step && effect->owner == slot) {
      effect->kind = 0;
    }
  }
}

static void fujin_wind_wind_effect_step(Projectile *effect) {
  const Fighter *victim = &g_battle.fighters[effect->owner];
  if (victim->action != 0x1001 || g_battle.contacts_off) {
    effect->kind = 0;
    return;
  }
  /* 414950 emits three additive wind meshes per visit from4d1a50.
   * Reuse the bounded particle pool for their random spread and lifetime. */
  Vec3 origin = {fighter_x(victim), 0, fighter_z(victim)};
  for (int i = 0; i < 3; ++i) {
    int mesh = (int)((g_battle.ticks * 3 + (unsigned)i) % 8);
    ParticleSpray spray = {.visual = &fujin_wind_wind_visuals[mesh],
        .count = 1,
        .lifetime = 10,
        .velocity = {0, -0x23d7, 0},
        .spread = {0, 0x7ae, 0},
        .position_spread = {0xd999, 0, 0xd999},
        .offset = {0, fujin_wind_wind_root_y[mesh], 0}};
    battle_spray(&spray, origin, (Vec3){0}, 0, 0);
  }
}

static void fujin_wind_wind_motion_hold(Fighter *fighter) {
  fighter->mmove_hold = !fighter->native_condition(fighter);
}

static void fujin_wind_wind_wait(Fighter *fighter, FighterTest done, FighterFn end) {
  fighter->native_condition = done;
  native_begin(fighter, 0, 0, end);
  native_frame(fighter, 0, fighter_sample(fighter), fujin_wind_wind_motion_hold);
  native_play(fighter, false);
}

static bool fujin_wind_at_apex(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static bool fujin_wind_rising(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] < 0;
}

static void fujin_wind_vortex_up_complete(Fighter *fighter) {
  native_hold(fighter, 8, fujin_wind_vortex_next);
}

static void fujin_wind_vortex_reverse(Fighter *fighter) {
  battle_sound(SOUND_C, 0x40);
  fighter->body.gravity = -0x83;
  fujin_wind_wind_wait(fighter, fujin_wind_rising, fujin_wind_vortex_up_complete);
}

static void fujin_wind_vortex_apex(Fighter *fighter) {
  native_hold(fighter, 8, fujin_wind_vortex_reverse);
}

static void fujin_wind_vortex_lift(Fighter *fighter) {
  /* 4881b0/4881f0: apex, eight visits, reverse acceleration until
   * rising, then eight visits before returning to the next of four lifts. */
  fighter->body.gravity = 0x83;
  fujin_wind_wind_wait(fighter, fujin_wind_at_apex, fujin_wind_vortex_apex);
}

static void fujin_wind_vortex_landed(Fighter *fighter) {
  mk4_mmove_bind(fighter, 195, 29, 28);
  if (fighter->health > 0) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VORTEX_LANDED, fighter_ready);
  } else {
    fighter_finish_mmove(fighter, NULL);
  }
}

static void fujin_wind_vortex_next(Fighter *fighter) {
  if (++fighter->native_variant < 4) {
    fujin_wind_vortex_lift(fighter);
  } else {
    fighter->action = 0x1002;
    fighter->body.gravity = 0xccc;
    fujin_wind_wind_wait(fighter, native_grounded, fujin_wind_vortex_landed);
  }
}

static void fujin_wind_vortex_lifts_begin(Fighter *fighter) {
  battle_sound(SOUND_C, 0x40);
  fighter->native_variant = 0;
  fujin_wind_vortex_lift(fighter);
}

static void fujin_wind_vortex_damp(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = q16_mul(fighter->body.vel[AXIS_Y], 0xe666);
}

static void fujin_wind_vortex_freeze(Fighter *fighter) {
  fighter->body.gravity = 0;
  native_begin(fighter, 0, 19, fujin_wind_vortex_lifts_begin);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(fighter, frame, 0, fujin_wind_vortex_damp);
  }
  native_play(fighter, false);
}

void fujin_wind_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &fujin_wind_caught);
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = fujin_wind_wind_cleanup;
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 5);
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 400, fujin_wind_wind_effect_step);
  fighter->body.gravity = -0x28f;
  native_hold(fighter, 10, fujin_wind_vortex_freeze);
}

static void fujin_wind_slam_landed(Fighter *fighter) {
  mk4_mmove_bind(fighter, 195, 29, 28);
  fighter->hits++;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SLAM_LANDED, fighter_ready);
}

static void fujin_wind_slam_drop(Fighter *fighter) {
  fighter->body.gravity = 0xf5c;
  fujin_wind_wind_wait(fighter, native_grounded, fujin_wind_slam_landed);
}

void fujin_wind_slam_victim(Fighter *fighter) {
  /* Reaction97 changes gravity while retaining the captured vortex velocity.
   * battle_react preserves that velocity across the previous action cleanup. */
  fighter_start_move(fighter, S_KNOCKDOWN, &fujin_wind_caught);
  fighter->action = 0x1002;
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = fujin_wind_wind_cleanup;
  fighter->sample_override = 0;
  fighter->body.gravity = -0x11eb;
  native_hold(fighter, 4, fujin_wind_slam_drop);
}

void fujin_wind_recovery(Fighter *fighter) {
  fighter->action = 0x1015;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_WIND_RECOVERY, fighter_ready);
}

void fujin_wind(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &fujin_wind_caster);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_WIND, fighter_ready);
}

void fujin_wind_slam(Fighter *fighter) {
  if (!fujin_can_wind_slam(fighter)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &fujin_wind_slam_move);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_WIND_SLAM, fighter_ready);
}

/* Native service cluster: 49ca90. */
static void goro_projectile_fireball_step(Projectile *projectile);
static const MoveDef goro_projectile_s_fireball = {.clip = 592, .last = 24, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

/* 4ed7e0 selects C[22]; 49c670 queries it every visit without the
 * selector-zero projectile's crouching (2001) exemption. */

static const MeshAnimation goro_projectile_fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

static void goro_projectile_fireball_step(Projectile *projectile) {
  AttackRecord goro_projectile_fireball_record = retail_vm_attack_record(retail_vm_image(), FIREBALL_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  if (battle_projectile_contact(projectile, &goro_projectile_fireball_record) != CONTACT_MISS) {
    projectile->kind = 0;
  }
}

/* 49ca90 is Liu Kang's 49c110 task with descriptor 4ed7d0 (rig 50678c,
 * mesh 50, forward 0x2e14, Y -0x1999, track 5047f8) and contact block
 * 4ed7e0: speed 0x3333 along the 49c610 heading snapshot, 48-tick life. */
void goro_fireball_spawn(Fighter *fighter, int task_type) {
  require(task_type == 83, "Unexpected Goro fireball task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, goro_projectile_fireball_step);
  if (projectile == NULL) {
    return;
  }
  projectile->mesh_animation = &goro_projectile_fireball_mesh;
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
  fighter_start_move(fighter, S_PUNCH, &goro_projectile_s_fireball);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_FIREBALL, fighter_ready);
}

/* Native service cluster: 43b8c0. */
static void goro_quake_quake_weight(Fighter *fighter, float weight);
static void goro_quake_quake_jump(Fighter *fighter);
static void goro_quake_quake_cleanup(Fighter *fighter);
static const MoveDef goro_quake_s_quake = {.clip = 602, .last = 29, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

enum {
  GORO_QUAKE_QUAKE_BLEND_STEP = 0x3333,
  GORO_QUAKE_QUAKE_DAMAGE = 0x3d70,
  GORO_QUAKE_QUAKE_REACTION = 136
};

static void goro_quake_quake_weight(Fighter *fighter, float weight) {
  fighter->blend_weight = (q16)weight;
}

static void goro_quake_quake_cleanup(Fighter *fighter) {
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

static void goro_quake_quake_jump(Fighter *fighter) {
  goro_quake_quake_cleanup(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_QUAKE_JUMP, fighter_ready);
}

/* 43b7e0 (HK B,F,D,D): voice 0x604, then 48fde0 blends from the current
 * pose to clip 602 (descriptor 4fc9b4) sample 0 in 0x3333 steps before
 * 4dfc50 plays the jump to its landing pose 21. */
void goro_quake_stomp(Fighter *fighter) {
  int source = fighter_move(fighter)->clip, sample = fighter_sample(fighter);
  goro_begin_special(fighter, S_KICK, &goro_quake_s_quake, 0x604);
  fighter->mmove_cleanup = goro_quake_quake_cleanup;
  mk4_mmove_bind(fighter, 602, 30, 0);
  fighter->blend_clip = source;
  fighter->blend_sample = sample;
  fighter->blend_duration = 1;
  fighter->blend_weight = 0;
  int frames = (Q16_ONE + GORO_QUAKE_QUAKE_BLEND_STEP - 1) / GORO_QUAKE_QUAKE_BLEND_STEP;
  native_begin(fighter, 1, frames, goro_quake_quake_jump);
  for (int frame = 1; frame <= frames; ++frame) {
    fighter->native.frames[frame - 1] = (mframe_t){goro_quake_quake_weight, (float)mini(Q16_ONE, frame * GORO_QUAKE_QUAKE_BLEND_STEP), NULL};
  }
  native_play(fighter, false);
}

/* 43b8c0: 48ee60 skips an airborne victim; otherwise the landing deals
 * 24% and installs reaction 136 on the victim without a contact query.
 * Either way 43b920 disturbs resting Goro objects and enters 4dfc60. */
void goro_script_quake_land(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (!fighter_airborne(victim)) {
    battle_damage(victim, GORO_QUAKE_QUAKE_DAMAGE);
    battle_react(victim, GORO_QUAKE_QUAKE_REACTION);
  }
  retail_disturb_resting_weapons();
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_QUAKE_LAND, fighter_ready);
}

/* Native service cluster: 497580. */
/* D,B+LP and the CPU projectile row -> 497540: 49c610 snapshots the pair
 * heading, action 0x310, then retail 4ed118 spawns task 78 (497580). */
static const MoveDef jarek_projectile_cast_move = {.clip = 27, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x310};

/* Descriptor 4ed180 selects C[4] (0x4ebf60): 12%, reactions 68/10/2. */

/* 4ed170 rig 0x50c770: share2 geometry, mesh 34, no authored offset. */
static const MeshAnimation jarek_projectile_blade_mesh = {.first = 34, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  JAREK_PROJECTILE_BLADE_FLIGHT,
  JAREK_PROJECTILE_BLADE_STUCK,
  JAREK_PROJECTILE_BLADE_DEFLECTED
};

static void jarek_projectile_blade_spin(Projectile *projectile) {
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
    projectile->angle[axis] = (projectile->angle[axis] + projectile->spin[axis]) % 0x6487e;
  }
}

static void jarek_projectile_blade_hit(Projectile *projectile, ContactResult result) {
  if (result == CONTACT_BLOCK) {
    /* 4978e0 -> 497730: scatter by 4ab690(.1) on X and Z, rise .14 with
     * gravity .0055 and tumble for at most 90 visits. */
    projectile->phase = JAREK_PROJECTILE_BLADE_DEFLECTED;
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
  projectile->phase = JAREK_PROJECTILE_BLADE_STUCK;
  projectile->ticks = 0;
  projectile->lifetime = 50;
}

static void jarek_projectile_blade_step(Projectile *projectile) {
  AttackRecord jarek_projectile_blade_record = retail_vm_attack_record(retail_vm_image(), BLADE_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  if (projectile->phase == JAREK_PROJECTILE_BLADE_FLIGHT) {
    jarek_projectile_blade_spin(projectile);
    /* 49c690 clears 53c440, so 49c6b0 skips a crouching (2001) victim. */
    ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(projectile, &jarek_projectile_blade_record);
    if (result != CONTACT_MISS) {
      jarek_projectile_blade_hit(projectile, result);
    }
    return;
  }
  if (projectile->phase == JAREK_PROJECTILE_BLADE_STUCK) {
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
  jarek_projectile_blade_spin(projectile);
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
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, jarek_projectile_blade_step);
  if (projectile == NULL) {
    return;
  }
  projectile->phase = JAREK_PROJECTILE_BLADE_FLIGHT;
  projectile->mesh_animation = &jarek_projectile_blade_mesh;
  projectile->initial_mesh = jarek_projectile_blade_mesh.first;
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
  fighter_start_move(fighter, S_PUNCH, &jarek_projectile_cast_move);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = 0x310;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAREK_BLADE, fighter_ready);
}

/* Native service cluster: 47b600. */
/* B,D,B+HK -> 47b5d0: action 0x105, then retail 4e81a8 stamps clip 107
 * to pose 20 with indexed 33, bank C 68 and shake 3 before tail 47b600. */
static const MoveDef jarek_quake_quake_move = {.clip = 107, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x105};

void jarek_quake(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &jarek_quake_quake_move);
  fighter->sample_override = 0;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAREK_QUAKE, fighter_ready);
}

/* 47b600: action 0x200f, 415180 ground-shock decoration, then 48ee60/48ef40
 * test the peer: an airborne peer is unaffected, a grounded one takes the
 * unblockable 48de80 damage and reaction 136. 47b680 enters 4e81c8. */
void jarek_quake_tail(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->action = 0x200f;
  if (!fighter_airborne(victim)) {
    battle_damage(victim, 0x23d7);
    /* 4ea4e8[136] -> 4834d0 is roster-generic: the shared reaction table
     * runs Goro's port of it (disarm, 4ea8f8 voice, victim script 4e95c8). */
    battle_react(victim, 136);
  }
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAREK_QUAKE_TAIL, fighter_ready);
}

/* Native service cluster: native_ground_pound. */
/* LK F,F,D record 4e43a8 (no gate) and the medium CPU row 4df95c select the
 * phase machine 468f00. Phase 0 stores action 0x101, binds 53ce14 (clip
 * 108, 57 poses) through 428750 and plays to pose 15 (429230); phase 1
 * spawns effect task 0x8b and plays to 35 (428d70); phase 2 spawns task
 * 0x8a, queues C[0x50], starts shake 3 and, unless 48ee60 finds the
 * opponent airborne, subtracts 0x23d7 and dispatches reaction 136 before
 * playing to 38. Phases 3..5 alternate poses 37/38 for ten cycles, 428d10
 * finishes the clip and 46f650 returns to the controller. */
static const MoveDef ground_pound_move = {.clip = 108, .last = 56, .flags = SF_LOCK | SF_RECORD, .action = 0x101};

enum {
  GROUND_POUND_RAISE = 15,
  GROUND_POUND_IMPACT = 35,
  GROUND_POUND_SETTLE = 38,
  GROUND_POUND_SHAKE_CYCLES = 10
};

static void ground_pound_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void ground_pound_impact(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_sound(SOUND_C, 0x50);
  mk4_script_shake(fighter, 3);
  if (!fighter_airborne(victim)) {
    battle_damage(victim, 0x23d7);
    /* 4ea4e8[136] -> 4834d0 is roster-generic: the shared reaction table
     * runs Goro's port of it (disarm, 4ea8f8 voice, victim script 4e95c8). */
    battle_react(victim, 136);
  }
}

void jax_ground_pound(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &ground_pound_move);
  fighter->sample_override = 0;
  int shake = 2 * GROUND_POUND_SHAKE_CYCLES;
  int last = GROUND_POUND_SETTLE + shake + (ground_pound_move.last - GROUND_POUND_SETTLE) - 1;
  native_begin(fighter, 0, last, ground_pound_ready);
  int frame = 0;
  for (int pose = 1; pose <= GROUND_POUND_SETTLE; ++pose, ++frame) {
    native_frame(fighter, frame, pose, pose == GROUND_POUND_IMPACT ? ground_pound_impact : NULL);
  }
  for (int cycle = 0; cycle < shake; ++cycle, ++frame) {
    native_frame(fighter, frame, cycle % 2 ? GROUND_POUND_SETTLE : GROUND_POUND_SETTLE - 1, NULL);
  }
  for (int pose = GROUND_POUND_SETTLE + 1; pose <= ground_pound_move.last; ++pose, ++frame) {
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 496fc0. */
/* LP D,F record 4e4388 (flags 0x2, gate 45e7e0) and the projectile CPU row
 * 4df9dc both select 496f80: 49c610 snapshots the pair heading into task
 * +68/+6c, action 0x314 is stored and 4ed090 plays clip 29, queues C[93] at
 * pose 10 and spawns independent task 80 (496fc0) before finishing. */
static const MoveDef jax_projectile_s_missile = {.clip = 29, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x314};

/* C[4] record 4ebf60: 12%, reactions 68/10/2. */

/* 4ed0b8 -> rig 5066ac (mesh 42); track 5047ac loops meshes 43..48 (504790),
 * three ticks each, in shared_g.geo. */
static const MeshAnimation jax_projectile_missile_mesh = {.first = 43, .count = 6, .period = 3, .scale = Q16_ONE};

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
static void jax_projectile_missile_step(Projectile *object) {
  AttackRecord jax_projectile_missile_record = retail_vm_attack_record(retail_vm_image(), MISSILE_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[object->owner]);
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(object, &jax_projectile_missile_record) != CONTACT_MISS) {
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
  Projectile *object = mk4_script_projectile_spawn(fighter, jax_projectile_missile_step, &jax_projectile_missile_mesh, 0x7333, -0x1999, 600);
  if (object == NULL) {
    return;
  }
  object->initial_mesh = 42;
  object->vel[AXIS_X] = q16_mul(fighter->saved_motion[0], 0x3333);
  object->vel[AXIS_Z] = q16_mul(fighter->saved_motion[1], 0x3333);
}

void jax_missile(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &jax_projectile_s_missile);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_MISSILE, fighter_ready);
}

/* Native service cluster: 499000, 499450, 499470. */
/* Kai's two skull specials share task code at 498e50..499734.
 * B,B+HP 499410: action 307, 4ed330 clip 21, task 499490 (type 48) rises
 * above the arena and drops onto the peer's launch position, C[8].
 * F,F+LP 498fc0: action 308, 4ed2f8 clip 22 hop, task 4990f0 (type 4a)
 * sinks under the floor and rises under the peer's launch position, C[9].
 * 49c610 snapshots the owner heading; 49c540 selects the peer group. */
static const MoveDef kai_skull_rising_cast = {.clip = 21, .last = 6, .flags = SF_LOCK | SF_RECORD, .action = 0x307};
static const MoveDef kai_skull_ground_cast = {.clip = 22, .last = 4, .flags = SF_LOCK | SF_RECORD, .action = 0x308};

/* C[8] 0x4ebe70 and C[9] 0x4ebe58: 13% damage, reactions 129/130, no block
 * reaction, flags 2 (mark). */

/* Spawn rigs 4ed390/4ed320 both select rig 50c8f8: share2 mesh 48 with no
 * root translation. */
static const MeshAnimation kai_skull_skull_mesh = {.first = 48, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  KAI_SKULL_SKULL_COOLDOWN = 0x30,
  KAI_SKULL_SKULL_SPEED = 0x3333,
  KAI_SKULL_SKULL_TOP = -0x40000,
  KAI_SKULL_SKULL_DROP_FLOOR = -0x6666,
  KAI_SKULL_SKULL_SINK = 0x10000,
  KAI_SKULL_SKULL_LIFETIME = 120,
  /* Object +68 pitch: 0x191eb rising, 0x4b5c2 falling. */
  KAI_SKULL_SKULL_UP = 0x191eb,
  KAI_SKULL_SKULL_DOWN = 0x4b5c2,
  /* Object task phases. */
  KAI_SKULL_SKULL_ASCEND = 0,
  KAI_SKULL_SKULL_DESCEND,
  KAI_SKULL_SKULL_SURFACE
};

bool kai_can_rising_skull(const Fighter *fighter) {
  /* 45eb40: 48e3f0 slot 3 elapsed since the last completed cast >= 0x30. */
  return g_battle.ticks >= fighter->skull_ready;
}

bool kai_can_ground_skull(const Fighter *fighter) {
  /* 45eb10: reject a peer in action 100a (45f540), then require its
   * +7c hit count <= 3 (45ebb0/45f5b0). */
  const Fighter *victim = fighter_opponent(fighter);
  return victim->action != 0x100a && victim->hits <= 3;
}

static void kai_skull_skull_impact(Projectile *object) {
  /* 489f90 C[0x34], camera shake row 0; 40e860/40e190 and 40bde0 burst
   * decorations remain omitted, then 406740/41f830 retire the object. */
  battle_sound(SOUND_C, 0x34);
  battle_camera_shake(3, 4, 0x2666);
  object->kind = 0;
}

static void kai_skull_skull_move(Projectile *object) {
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
}

static void kai_skull_skull_relocate(Projectile *object, q16 vel_y, q16 pitch) {
  object->pos[AXIS_X] = object->target[0];
  object->pos[AXIS_Z] = object->target[1];
  object->vel[AXIS_X] = object->vel[AXIS_Z] = 0;
  object->vel[AXIS_Y] = vel_y;
  object->angle[AXIS_Z] = pitch;
}

static void kai_skull_skull_spawn_from(Fighter *fighter, ProjectileFn step, q16 forward, q16 y, q16 vel_y, q16 pitch) {
  /* 49c300 places the rig on the snapshotted heading; failure is a real
   * allocation branch (53c44c bit 2) and leaves the owner's move intact. */
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, KAI_SKULL_SKULL_LIFETIME, step);
  if (object == NULL) {
    return;
  }
  const Fighter *victim = fighter_opponent(fighter);
  object->mesh_animation = &kai_skull_skull_mesh;
  object->initial_mesh = kai_skull_skull_mesh.first;
  object->heading[0] = fighter->saved_motion[0];
  object->heading[1] = fighter->saved_motion[1];
  object->pos[AXIS_X] += q16_mul(fighter->saved_motion[0], forward);
  object->pos[AXIS_Z] += q16_mul(fighter->saved_motion[1], forward);
  object->pos[AXIS_Y] += y;
  object->vel[AXIS_Y] = vel_y;
  object->angle[AXIS_Z] = pitch;
  object->target[0] = victim->body.pos[AXIS_X];
  object->target[1] = victim->body.pos[AXIS_Z];
  object->phase = KAI_SKULL_SKULL_ASCEND;
}

static void kai_skull_rising_step(Projectile *object) {
  AttackRecord kai_skull_drop_record = retail_vm_attack_record(retail_vm_image(), DROP_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  kai_skull_skull_move(object);
  if (object->phase == KAI_SKULL_SKULL_ASCEND) {
    /* 4995b0: rise until Y <= -4, then drop over the saved peer position. */
    if (object->pos[AXIS_Y] <= KAI_SKULL_SKULL_TOP) {
      kai_skull_skull_relocate(object, KAI_SKULL_SKULL_SPEED, KAI_SKULL_SKULL_DOWN);
      object->phase = KAI_SKULL_SKULL_DESCEND;
    }
    return;
  }
  /* 4994ca: query C[8] each falling visit; contact or Y >= -.4 explodes. */
  if (battle_projectile_contact(object, &kai_skull_drop_record) != CONTACT_MISS || object->pos[AXIS_Y] >= KAI_SKULL_SKULL_DROP_FLOOR) {
    kai_skull_skull_impact(object);
  }
}

static void kai_skull_ground_step(Projectile *object) {
  AttackRecord kai_skull_rise_record = retail_vm_attack_record(retail_vm_image(), RISE_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  kai_skull_skull_move(object);
  if (object->phase == KAI_SKULL_SKULL_ASCEND) {
    /* 48f570: sink until Y >= 1, then 499227 rises under the saved peer position. */
    if (object->pos[AXIS_Y] >= KAI_SKULL_SKULL_SINK) {
      kai_skull_skull_relocate(object, -KAI_SKULL_SKULL_SPEED, KAI_SKULL_SKULL_UP);
      object->phase = KAI_SKULL_SKULL_DESCEND;
    }
    return;
  }
  if (object->phase == KAI_SKULL_SKULL_DESCEND) {
    /* 48f5f0: no contact until the rig reaches the floor plane. */
    if (object->pos[AXIS_Y] <= 0) {
      object->phase = KAI_SKULL_SKULL_SURFACE;
    }
    return;
  }
  /* 499128: query C[9]; contact explodes, Y <= -4 retires silently. */
  if (battle_projectile_contact(object, &kai_skull_rise_record) != CONTACT_MISS) {
    kai_skull_skull_impact(object);
  } else if (object->pos[AXIS_Y] <= KAI_SKULL_SKULL_TOP) {
    object->kind = 0;
  }
}

static void kai_skull_skull_cast(Fighter *fighter, const MoveDef *move, uint32_t script) {
  fighter_start_move(fighter, S_PUNCH, move);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter(fighter, script, fighter_ready);
}

void kai_rising_skull(Fighter *fighter) {
  kai_skull_skull_cast(fighter, &kai_skull_rising_cast, MK4_RETAIL_DATA_KAI_RISING_SKULL);
}

void kai_rising_skull_spawn(Fighter *fighter) {
  /* 499450: task 499490 type 48 from 4ed390 (back .4, Y -.01), rising .2. */
  kai_skull_skull_spawn_from(fighter, kai_skull_rising_step, -0x6666, -0x28f, -KAI_SKULL_SKULL_SPEED, KAI_SKULL_SKULL_UP);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_KAI_RISING_SKULL_SPAWN, fighter_ready);
}

void kai_rising_skull_finish(Fighter *fighter) {
  /* 499470: 48e3a0 stamps slot 3 for 45eb40, then 46f650. */
  fighter->skull_ready = g_battle.ticks + KAI_SKULL_SKULL_COOLDOWN;
  mk4_mmove_thrown_finish(fighter);
}

void kai_ground_skull(Fighter *fighter) {
  kai_skull_skull_cast(fighter, &kai_skull_ground_cast, MK4_RETAIL_DATA_KAI_GROUND_SKULL);
}

static void kai_skull_ground_fall_landed(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void kai_skull_ground_fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    kai_skull_ground_fall_landed(fighter);
  }
}

static void kai_skull_ground_air_fall(Fighter *fighter) {
  /* 47e990: face the peer, gravity 147, clip 310 at sample 7 for eight
   * visits, then advance one sample per visit until 46fd90 lands. */
  enum {
    KAI_SKULL_FALL_FIRST = 7,
    KAI_SKULL_FALL_HOLD = 8,
    KAI_SKULL_FALL_LAST = 27
  };

  fighter->body.gravity = 0x147;
  retail_face_peer(fighter);
  mk4_mmove_bind(fighter, 310, KAI_SKULL_FALL_LAST + 1, KAI_SKULL_FALL_FIRST);
  int frames = KAI_SKULL_FALL_HOLD + KAI_SKULL_FALL_LAST - KAI_SKULL_FALL_FIRST;
  native_begin(fighter, 1, frames, kai_skull_ground_fall_landed);
  for (int frame = 1; frame <= frames; ++frame) {
    int sample = frame <= KAI_SKULL_FALL_HOLD ? KAI_SKULL_FALL_FIRST : KAI_SKULL_FALL_FIRST + frame - KAI_SKULL_FALL_HOLD;
    FighterFn action = frame <= KAI_SKULL_FALL_HOLD ? NULL : frame < frames ? kai_skull_ground_fall_check : mk4_mmove_wait_grounded;
    native_frame(fighter, frame, sample, action);
  }
  native_play(fighter, false);
}

static void kai_skull_ground_cast_return(Fighter *fighter) {
  /* 499000 state 2: 48ef30 selects 47e990 in the air, otherwise 46f650. */
  if (fighter_airborne(fighter)) {
    kai_skull_ground_air_fall(fighter);
  } else {
    mk4_mmove_thrown_finish(fighter);
  }
}

void kai_ground_skull_spawn(Fighter *fighter) {
  /* 499000: task 4990f0 type 4a from 4ed320 (forward .2, Y -.2), sinking
   * .2; the owner sleeps 20 visits, then 429460 rewinds clip 22 to 0. */
  kai_skull_skull_spawn_from(fighter, kai_skull_ground_step, 0x3333, -0x3333, KAI_SKULL_SKULL_SPEED, KAI_SKULL_SKULL_DOWN);
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 1, 20 + last + 1, kai_skull_ground_cast_return);
  for (int frame = 1; frame <= 20; ++frame) {
    native_frame(fighter, frame, last, NULL);
  }
  for (int sample = last; sample >= 0; --sample) {
    native_frame(fighter, 20 + last + 1 - sample, sample, NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 47ed40, 47ef10, 49c010, 49c0d0. */
const MeshAnimation liu_kang_fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

static void liu_kang_projectile_fireball_projectile(Projectile *p);
static void liu_kang_projectile_fireball(Fighter *fighter, bool low);
static const MoveDef liu_kang_projectile_s_fireball_high = {.clip = 18, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030C};
static const MoveDef liu_kang_projectile_s_fireball_low = {.clip = 19, .last = 46, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030D};
static const MoveDef liu_kang_projectile_s_fireball_air = {.clip = 20, .last = 5, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030E};

static void liu_kang_projectile_fireball_projectile(Projectile *p) {
  AttackRecord liu_kang_projectile_fireball_high_record = retail_vm_attack_record(retail_vm_image(), FIREBALL_HIGH_ATTACK_RECORD);
  AttackRecord liu_kang_projectile_fireball_low_record = retail_vm_attack_record(retail_vm_image(), FIREBALL_LOW_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  bool low = p->phase != 0;
  if ((low || fighter_peer(owner)->action != ACTION_RETAIL_2001) &&
      battle_projectile_contact(p, low ? &liu_kang_projectile_fireball_low_record : &liu_kang_projectile_fireball_high_record) != CONTACT_MISS) {
    p->kind = 0;
  }
}

void liu_kang_fireball_spawn(Fighter *fighter, int type) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, liu_kang_projectile_fireball_projectile);
  if (p == NULL) {
    return;
  }
  p->phase = type == 1;
  p->mesh_animation = &liu_kang_fireball_mesh;
  p->initial_mesh = 50; /* 4ed730/740/760 -> rig 50678c +24. */
  q16 forward = type == 2 ? 0x9999 : type == 1 ? 0x28f5 : 0x1999;
  p->pos[AXIS_Y] += type == 2 ? -0x2147 : type == 1 ? 0x35c2 : -0x7333;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    p->pos[axis] += q16_mul(fighter_axis(fighter, axis), forward);
    p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void liu_kang_projectile_fireball(Fighter *fighter, bool low) {
  bool air = !low && fighter_airborne(fighter);
  if (g_battle.phase != PHASE_FIGHT || (low && fighter_airborne(fighter))) {
    return;
  }
  if (!air) {
    mk4_script_stop_body(fighter);
  }
  fighter_start_move(fighter, S_PUNCH,
      air   ? &liu_kang_projectile_s_fireball_air
      : low ? &liu_kang_projectile_s_fireball_low
            : &liu_kang_projectile_s_fireball_high);
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter_upright(fighter);
  fighter->script_phase = air ? 2 : low ? 1 : 0;
  fighter->sample_override = 0;
  RetailScriptContinuation script = air   ? (RetailScriptContinuation){MK4_RETAIL_SCRIPT_LIU_KANG_AIR_FIREBALL_AIR, fighter_ready}
                                    : low ? (RetailScriptContinuation){MK4_RETAIL_DATA_LIU_KANG_LOW_FIREBALL_LOW, fighter_ready}
                                          : (RetailScriptContinuation){MK4_RETAIL_DATA_LIU_KANG_HIGH_FIREBALL_HIGH, fighter_ready};
  retail_vm_enter(fighter, script.entry, script.finish);
}

void liu_kang_high_fireball(Fighter *fighter) {
  liu_kang_projectile_fireball(fighter, false);
}

void liu_kang_low_fireball(Fighter *fighter) {
  liu_kang_projectile_fireball(fighter, true);
}

void liu_kang_liu_script_spawn_air(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang air-fireball task type");
  liu_kang_fireball_spawn(fighter, 2);
}

static void liu_kang_projectile_air_return_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

void liu_kang_liu_script_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

void liu_kang_liu_script_air_return(Fighter *fighter) {
  fighter->body.gravity = 0x20c;
  native_await(fighter, native_grounded, liu_kang_projectile_air_return_land);
}

/* Native service cluster: 499780. */
/* LP [F,F] record 4e4818 and the CPU projectile row select native 499740:
 * 49c610 snapshots the pair heading, action 306 enters 4ed3a0 (clip 23). */
static const MoveDef quan_chi_skull_s_skull = {.clip = 23, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = 0x306};

/* 4998d0 copies C[7]/4ebe88 through contact descriptor 4ed3f8. */

/* 53d6b8 spawns rig 50c364 from share2_g.geo at the owner origin: cranium
 * mesh 4 and jaw mesh 5 at its authored (0, 0x1020, -0x324d) offset. */
static const MeshAnimation quan_chi_skull_skull_mesh = {
    .first = 4, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .child = true, .child_mesh = 5, .child_offset = {0, 0x1020, -0x324d}};
/* 40c760: shared smoke track 508e28..508f08, meshes 191..199 at two ticks each. */
static const ParticleVisual quan_chi_skull_smoke_visual = {
    .animation = {.first = 191, .count = 9, .period = 2, .scale = 0xa666}, .billboard = true, .additive = true};
static const ParticleSpray quan_chi_skull_smoke = {
    .visual = &quan_chi_skull_smoke_visual, .count = 1, .lifetime = 20, .offset = {0, -0x3333, 0}, .position_spread = {0x1e78, 0, 0x1e78}};
static const MoveDef quan_chi_skull_s_bitten = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION};

enum {
  QUAN_CHI_SKULL_SKULL_FLIGHT,
  QUAN_CHI_SKULL_SKULL_BITE,
  /* 4ed930 holds the bite for 2 + 6 * 4 + 2 ticks, then 499850 multiplies the
   * slowed velocity by five for its final 30-tick entry before 405dd0. */
  QUAN_CHI_SKULL_SKULL_BITE_TICKS = 28,
  QUAN_CHI_SKULL_SKULL_ESCAPE_TICKS = 30,
};

/* Root track 4ed910 alternates roll -/+0x3333 for 2/4/2 ticks. */
static q16 quan_chi_skull_skull_roll(const Projectile *skull) {
  int tick = skull->ticks % 8;
  return tick >= 2 && tick < 6 ? 0x3333 : -0x3333;
}

static void quan_chi_skull_skull_bite(Projectile *skull) {
  /* 499b40: both hit and blocked contacts slow the skull to 0.4 and install
   * the bite track; the contact waiter task then finishes. */
  skull->phase = QUAN_CHI_SKULL_SKULL_BITE;
  skull->vel[AXIS_X] = q16_mul(skull->vel[AXIS_X], 0x6666);
  skull->vel[AXIS_Z] = q16_mul(skull->vel[AXIS_Z], 0x6666);
  skull->lifetime = QUAN_CHI_SKULL_SKULL_BITE_TICKS + QUAN_CHI_SKULL_SKULL_ESCAPE_TICKS + 1;
}

/* 49c690(4ed3f8) waits for contact each tick; 499ae0 tests the arena bound
 * from 49bc00 and emits smoke on its first visit and then every third visit. */
static void quan_chi_skull_skull_step(Projectile *skull) {
  AttackRecord quan_chi_skull_skull_record = retail_vm_attack_record(retail_vm_image(), SKULL_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    skull->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    skull->pos[axis] += skull->vel[axis];
  }
  skull->angle[2] = quan_chi_skull_skull_roll(skull);
  if (skull->phase == QUAN_CHI_SKULL_SKULL_BITE) {
    if (skull->lifetime == QUAN_CHI_SKULL_SKULL_ESCAPE_TICKS + 1) {
      skull->vel[AXIS_X] *= 5;
      skull->vel[AXIS_Z] *= 5;
    }
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[skull->owner]);
  /* 49c6b0 skips the C[7] query while the victim action is 2001. */
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(skull, &quan_chi_skull_skull_record) != CONTACT_MISS) {
    quan_chi_skull_skull_bite(skull);
    return;
  }
  int64_t x = skull->pos[AXIS_X], z = skull->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    skull->kind = 0; /* 499c00 frees the object directly. */
    return;
  }
  if (skull->ticks % 3 == 1) {
    battle_spray(&quan_chi_skull_smoke, (Vec3){q16_float(skull->pos[0]), q16_float(skull->pos[1]), q16_float(skull->pos[2])}, (Vec3){0}, 0, 0);
  }
}

/* 499780: spawn task 73 (4998d0), then enter the 4ed3c0 recovery. */
void quan_chi_script_skull_release(Fighter *fighter) {
  Projectile *skull = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, quan_chi_skull_skull_step);
  if (skull != NULL) {
    skull->mesh_animation = &quan_chi_skull_skull_mesh;
    skull->initial_mesh = quan_chi_skull_skull_mesh.first;
    skull->heading[0] = fighter->saved_motion[0];
    skull->heading[1] = fighter->saved_motion[1];
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      skull->vel[axis] = q16_mul(fighter->saved_motion[axis == AXIS_X ? 0 : 1], 0x3333);
    }
  }
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_QUAN_CHI_SKULL_RELEASE, fighter_ready);
}

void quan_chi_skull(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &quan_chi_skull_s_skull);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_QUAN_CHI_SKULL, fighter_ready);
}

/* Reaction 128 (485f90): 494520 drops a weapon, then 4e9cd8 knocks the
 * victim back through clip 182 and returns through its own controller. */
void quan_chi_skull_reaction(Fighter *victim) {
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &quan_chi_skull_s_bitten);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_QUAN_CHI_SKULL_REACTION, fighter_ready);
}

/* Native service cluster: 49be10. */
static void raiden_projectile_projectile_step(Projectile *object);
static const MoveDef raiden_projectile_s_projectile = {.clip = 17, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x305};

static const MeshAnimation raiden_projectile_projectile_mesh = {.first = 41, .count = 1, .period = 1, .scale = Q16_ONE};

static void raiden_projectile_projectile_step(Projectile *object) {
  AttackRecord raiden_projectile_projectile_record = retail_vm_attack_record(retail_vm_image(), GENERIC_PROJECTILE_ATTACK_RECORD);

  mk4_script_projectile_step(object, &raiden_projectile_projectile_record, false);
}

void raiden_script_lightning_spawn(Fighter *fighter, int task_type) {
  require(task_type == 65, "Incorrect retail projectile task");
  mk4_script_projectile_spawn(fighter, raiden_projectile_projectile_step, &raiden_projectile_projectile_mesh, 0x28f5, 0, 48);
}

void raiden_lightning(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &raiden_projectile_s_projectile);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_RAIDEN_LIGHTNING, fighter_ready);
}

/* Native service cluster: 483df0, 496e00. */
/* D,F+LP 496db0 (also the projectile CPU row): stamp slot 18, snapshot the
 * pair axis (49c610), action 0x315, then 4ed038 throws on clip 437 and
 * spawns task 82 (496e00) at pose 13. */
static const MoveDef reiko_star_s_star_throw = {.clip = 437, .last = 23, .flags = SF_LOCK | SF_RECORD, .action = 0x315};

/* 4ed080: C[21] contact, bounds 496f10, contact 496f40, expiry 496f60. */

/* 4deae0 is an invisible carrier node. The thrown star drawn by 415620 uses
 * rig 50cf34: share2 mesh 73 translated (0x2000, 0x2000, 0), spun 0xb333. */
static const MeshAnimation reiko_star_star_mesh = {
    .first = 73, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .offset = {0x2000, 0x2000, 0}};

/* 45e6d0: slot 18 must be at least 80 ticks old. */
bool reiko_can_star(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && mk4_special_elapsed(fighter, 0x12, 80);
}

/* 49c690/49c6b0 with 4ed080: the query skips a crouching (2001) peer. A
 * contact plays C[0x65] and retires the object; otherwise 49bc00 retires it
 * beyond the arena radius squared plus 12. The object list then moves it. */
static void reiko_star_star_step(Projectile *star) {
  AttackRecord reiko_star_star_record = retail_vm_attack_record(retail_vm_image(), STAR_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    star->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[star->owner]);
  ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(star, &reiko_star_star_record);
  if (result != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x65);
    star->kind = 0;
    return;
  }
  int64_t x = star->pos[AXIS_X], z = star->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (((x * x) >> 16) + ((z * z) >> 16) > ((radius * radius) >> 16) + 12 * Q16_ONE) {
    star->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    star->pos[axis] += star->vel[axis];
  }
  star->angle[AXIS_Z] = (star->angle[AXIS_Z] + 0xb333) % 0x6487e;
}

/* 496e00: the object starts at the owner's origin and travels 0x30a3 per
 * visit along the axis snapshot taken by the entry. */
void reiko_star_spawn(Fighter *fighter, int task_type) {
  require(task_type == 82, "Unexpected Reiko star task");
  Projectile *star = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, reiko_star_star_step);
  if (star == NULL) {
    return;
  }
  star->mesh_animation = &reiko_star_star_mesh;
  star->initial_mesh = reiko_star_star_mesh.first;
  star->heading[0] = fighter->saved_motion[AXIS_X];
  star->heading[1] = fighter->saved_motion[AXIS_Z];
  star->vel[AXIS_X] = q16_mul(fighter->saved_motion[AXIS_X], 0x30a3);
  star->vel[AXIS_Z] = q16_mul(fighter->saved_motion[AXIS_Z], 0x30a3);
}

void reiko_star(Fighter *fighter) {
  mk4_special_stamp(fighter, 0x12);
  fighter_start_move(fighter, S_PUNCH, &reiko_star_s_star_throw);
  fighter->saved_motion[AXIS_X] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[AXIS_Z] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REIKO_STAR, fighter_ready);
}

/* Native service cluster: 40eb80, 40ec40, 40ed20, 40ee00, 483df0, 498920. */
/* Acid spit: D,F+HP record 4e4570 and the CPU projectile row both enter
 * 4988d0. It stamps action family 7 (48e3a0), snapshots the pair heading
 * (49c610), sets action 30b and runs retail 4ed1e8 on clip 25. */
static const MoveDef reptile_projectile_spit = {.clip = 25, .last = 41, .flags = SF_LOCK | SF_RECORD, .action = 0x30b};

/* 4ed258 selector 0x11: C[17], 10% damage, reactions 41/41/2. */

/* 40ee30 droplets: 4d16d0 lists shared rigs 509e74..509ee4, meshes 340..344
 * for two ticks each, with the 509e90 authored origin. */
static const ParticleVisual reptile_projectile_drool_visual = {
    .animation = {.first = 340, .count = 5, .period = 2, .scale = 0x51eb, .source = MESH_SHARED, .offset = {0x1100, 0xd80, 0}}};
static const ParticleSpray reptile_projectile_drool_spray = {
    .visual = &reptile_projectile_drool_visual, .count = 1, .lifetime = 40, .gravity = 0x2e1, .velocity = {0, -0xe14, 0}, .spread = {0x51e, 0x666, 0x51e}};

/* 483d60 grounded victim: action 1013, clip 182 (65 samples). */

enum {
  REPTILE_PROJECTILE_HEAD_JOINT = 2,
  REPTILE_PROJECTILE_HEAD_OPEN = 4, /* 40ec40: rig record 517c24 */
  REPTILE_PROJECTILE_HEAD_WIDE = 5  /* 40ed20: rig record 517c40 */
};

bool reptile_can_spit(const Fighter *fighter) {
  /* 45e9d0: at least 0x50 ticks since the last family-7 stamp. */
  return g_battle.ticks >= fighter->projectile_ready;
}

/* 40ec40/40ed20/40eb80 write the head node's +0x24 descriptor through
 * 408cb0. Retail skips the write while 53c1a8[player] is set or when a
 * morphing Shinnok (48e2d0, id 6) runs the script; neither state exists in
 * this engine. The default costume selects the 5172b8 rig records. */
static void reptile_projectile_head_mesh(Fighter *fighter, int mesh) {
  fighter->mesh_swap_joint = REPTILE_PROJECTILE_HEAD_JOINT;
  fighter->mesh_swap = mesh;
}

void reptile_script_head_open(Fighter *fighter) {
  reptile_projectile_head_mesh(fighter, REPTILE_PROJECTILE_HEAD_OPEN);
}

void reptile_script_head_wide(Fighter *fighter) {
  reptile_projectile_head_mesh(fighter, REPTILE_PROJECTILE_HEAD_WIDE);
}

/* 40eb80 restores rig record 517a9c, the walked head node and its mesh 2. */
void reptile_script_head_close(Fighter *fighter) {
  fighter->mesh_swap_joint = 0;
  fighter->mesh_swap = 0;
}

static void reptile_projectile_spit_cleanup(Fighter *fighter) {
  /* Portable adaptation: an interrupted retail task never reaches 40eb80
   * and leaves the swapped head descriptor; action cleanup restores it. */
  reptile_script_head_close(fighter);
}

static bool reptile_projectile_drool_drop(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)event;
  (void)opponent;
  Vec3 head;
  Mat3 basis;
  if (!fighter_joint_position(owner, REPTILE_PROJECTILE_HEAD_JOINT, &head, &basis)) {
    return true;
  }
  /* 40ee30 offsets the head-local point (.01, -.05, -.1) before 4b8140. */
  Vec3 mouth = vec_add(head, mat3_apply(basis, (Vec3){q16_float(0x28f), q16_float(-0xccc), q16_float(-0x1999)}));
  Vec3 forward = {q16_float(owner->saved_motion[0]), 0, q16_float(owner->saved_motion[1])};
  battle_spray(&reptile_projectile_drool_spray, mouth, forward, 0x2e14, 0x51e);
  return true;
}

static bool reptile_projectile_drool_done(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)event;
  (void)owner;
  (void)opponent;
  return false;
}

/* 40ee00 -> task 0x85 40ee30: nine droplet visits two ticks apart from
 * the head joint. Its randomized records are decorative; the portable
 * spray keeps their forward 0.16-0.2 speed, lift and gravity ranges. */
void reptile_script_acid_drool(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, reptile_projectile_drool_done, false, false);
  if (event == NULL) {
    return;
  }
  battle_event_program(event, 18);
  for (int visit = 0; visit < 9; ++visit) {
    battle_event_frame(event, visit * 2, 0, reptile_projectile_drool_drop);
  }
}

static void reptile_projectile_acid_step(Projectile *acid) {
  AttackRecord reptile_projectile_acid_record = retail_vm_attack_record(retail_vm_image(), ACID_ATTACK_RECORD);

  if (acid->phase > 0) {
    /* 498ac0: the contact visit retires the object and plays C[48]; ten
     * ticks later C[49] and the task finishes. */
    if (--acid->phase == 0) {
      battle_sound(SOUND_C, 0x49);
      acid->kind = 0;
    }
    return;
  }
  if (g_battle.contacts_off) {
    acid->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    acid->pos[axis] += acid->vel[axis];
  }
  /* 498a90 -> 49bc00: squared arena radius plus twelve square units. */
  int64_t x = acid->pos[AXIS_X], z = acid->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    acid->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[acid->owner]);
  /* 49c6b0 skips the C[17] query against action 2001. */
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(acid, &reptile_projectile_acid_record) != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x48);
    acid->phase = 10;
  }
}

/* 498920: C[71] cue, invisible 4deae0 carrier (node flags 0x8c are culled)
 * at the owner's position, 0x23d7 along the 49c610 heading snapshot. */
void reptile_acid_spawn(Fighter *fighter, int task_type) {
  require(task_type == 76, "Unexpected Reptile acid task");
  battle_sound(SOUND_C, 0x47);
  Projectile *acid = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, reptile_projectile_acid_step);
  if (acid == NULL) {
    return;
  }
  acid->heading[0] = fighter->saved_motion[0];
  acid->heading[1] = fighter->saved_motion[1];
  acid->vel[AXIS_X] = q16_mul(fighter->saved_motion[0], 0x23d7);
  acid->vel[AXIS_Z] = q16_mul(fighter->saved_motion[1], 0x23d7);
}

void reptile_acid_spit(Fighter *fighter) {
  fighter->projectile_ready = g_battle.ticks + 0x50;
  fighter_start_move(fighter, S_PUNCH, &reptile_projectile_spit);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->mmove_cleanup = reptile_projectile_spit_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REPTILE_ACID_SPIT, fighter_ready);
}

/* Native service cluster: 416ae0, 416d40, 468640. */
enum {
  FIRE_SCRIPT_STARTUP = 0x4e5b58,
  FIRE_SCRIPT_RECOVERY = 0x4e5b98,
  FIRE_NATIVE_NORMAL_END = 47,
  FIRE_NATIVE_FAST_END = 31,
  FIRE_NATIVE_NORMAL_CONTACT = 27,
  FIRE_NATIVE_FAST_CONTACT = 19
};

static void scorpion_fire_fire_contact(Fighter *fighter);
static const MoveDef scorpion_fire_s_fire = {.clip = 448, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0115};

static void scorpion_fire_fire_contact(Fighter *fighter) {
  AttackRecord record = retail_vm_attack_record(retail_vm_image(), FIRE_ATTACK_RECORD);
  /* Native fire bypasses guarding; this explicit flag belongs to our contact
   * boundary, while the box, reactions and damage come from retail data. */
  record.flags |= ATTACK_UNBLOCKABLE;
  battle_contact(fighter, &record);
}

/*468640 finishes its native contact phase at sample28, then enters words. */
static void fire_recovery_script(Fighter *fighter) {
  retail_vm_enter(fighter, FIRE_SCRIPT_RECOVERY, fighter_ready);
}

/*4688e0 uses428cf0 to finish the inherited descriptor, then46f650. */
void retail_fire_recovery_finish(Fighter *fighter) {
  native_finish_clip(fighter, scorpion_special_ready);
}

void scorpion_fire_script(Fighter *fighter) {
  bool fast = fighter->script_phase != 0;
  int first = fighter->ticks + 1, last = fast ? FIRE_NATIVE_FAST_END : FIRE_NATIVE_NORMAL_END;
  native_begin(fighter, first, last, fire_recovery_script);
  for (int frame = first; frame <= last; ++frame) {
    int pose = fast ? frame - (frame < FIRE_NATIVE_FAST_CONTACT ? 2 : 3) : 11 + (frame - 13) / 2;
    native_frame(fighter, frame, pose, frame == (fast ? FIRE_NATIVE_FAST_CONTACT : FIRE_NATIVE_NORMAL_CONTACT) ? scorpion_fire_fire_contact : NULL);
  }
  native_play(fighter, false);
}

void scorpion_fire(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &scorpion_fire_s_fire);
  fighter->sample_override = 1;
  fighter->script_phase = (fighter_opponent(fighter)->flags & FF_REACTING) != 0;
  retail_vm_enter(fighter, FIRE_SCRIPT_STARTUP, fighter_ready);
}

void scorpion_script_fire_decor(Fighter *fighter) {
  battle_particles(fighter, 8, 4, true);
}

void scorpion_script_fire_flame(Fighter *fighter) {
  battle_particles(fighter, 9, 4, true);
}

/* Native service cluster: 49b9e0, 49ba50. */
static void sonya_projectile_projectile_step(Projectile *object);
static const MoveDef sonya_projectile_s_projectile = {.clip = 15, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x313};

static const MeshAnimation sonya_projectile_projectile_mesh = {.first = 42, .count = 2, .period = 1, .scale = Q16_ONE};

static void sonya_projectile_projectile_step(Projectile *object) {
  AttackRecord sonya_projectile_projectile_record = retail_vm_attack_record(retail_vm_image(), RING_ATTACK_RECORD);

  mk4_script_projectile_step(object, &sonya_projectile_projectile_record, false);
}

void sonya_script_ring_spawn(Fighter *fighter, int task_type) {
  require(task_type == 68, "Incorrect retail projectile task");
  mk4_script_projectile_spawn(fighter, sonya_projectile_projectile_step, &sonya_projectile_projectile_mesh, 0xccc, -0x1999, 48);
}

void sonya_ring(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &sonya_projectile_s_projectile);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SONYA_RING, fighter_ready);
}

void sonya_script_ring_flash(Fighter *fighter, int task_type) {
  require(task_type == 38, "Incorrect ring flash task");
  battle_particles(fighter, 2, 3, true);
}

/* Native service cluster: native_projectile. */

static const ParticleVisual ice_projectile_trail_visual = {
    .animation = {.first = 339, .count = 1, .period = 1, .scale = 0x4000}, .billboard = true, .additive = true, .shrink = 0x51e};
static const ParticleSpray ice_projectile_trail = {.visual = &ice_projectile_trail_visual,
    .count = 10,
    .lifetime = 10,
    .velocity = {0, -0x7ae, 0},
    .spread = {0, 0x28f, 0},
    .offset = {0x1000, 0xc00, 0},
    .position_spread = {0x6666, 0x6666, 0x6666}};

void sub_zero_ice_projectile(Projectile *p) {
  AttackRecord ice_projectile_record = retail_vm_attack_record(retail_vm_image(), ICE_PROJECTILE_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int i = 0; i < 3; ++i) {
    p->pos[i] += p->vel[i];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  if (fighter_peer(owner)->action != ACTION_RETAIL_2001 && battle_projectile_contact(p, &ice_projectile_record) != CONTACT_MISS) {
    p->kind = 0;
  }
  int64_t radius2 = (int64_t)g_battle.arena_radius * g_battle.arena_radius + (int64_t)0xc0000 * Q16_ONE;
  if ((int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] > radius2) {
    p->kind = 0;
  }
  if (p->kind != 0) {
    battle_spray(&ice_projectile_trail, (Vec3){q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])}, (Vec3){0}, 0, 0);
  }
}

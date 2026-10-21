#include "retail_vm.h"
#include "../character.h"

/* Acid spit: D,F+HP record 4e4570 and the CPU projectile row both enter
 * 4988d0. It stamps action family 7 (48e3a0), snapshots the pair heading
 * (49c610), sets action 30b and runs generated 4ed1e8 on clip 25. */
static const MoveDef spit = {.clip = 25, .last = 41, .flags = SF_LOCK | SF_RECORD, .action = 0x30b};
/* 4ed258 selector 0x11: C[17], 10% damage, reactions 41/41/2. */
static const AttackRecord acid_record = {0x1072b, -0x82d0, 0xcf1a, 0x6147, 41, 41, 2, ATTACK_MARK, 0x1999};
/* 40ee30 droplets: 4d16d0 lists shared rigs 509e74..509ee4, meshes 340..344
 * for two ticks each, with the 509e90 authored origin. */
static const ParticleVisual drool_visual = {
    .animation = {.first = 340, .count = 5, .period = 2, .scale = 0x51eb, .source = MESH_SHARED, .offset = {0x1100, 0xd80, 0}}};
static const ParticleSpray drool_spray = {
    .visual = &drool_visual, .count = 1, .lifetime = 40, .gravity = 0x2e1, .velocity = {0, -0xe14, 0}, .spread = {0x51e, 0x666, 0x51e}};
/* 483d60 grounded victim: action 1013, clip 182 (65 samples). */
static const MoveDef acid_victim = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = 0x1013};

enum {
  REPTILE_HEAD_JOINT = 2,
  REPTILE_HEAD_OPEN = 4, /* 40ec40: rig record 517c24 */
  REPTILE_HEAD_WIDE = 5  /* 40ed20: rig record 517c40 */
};

bool reptile_can_spit(const Fighter *fighter) {
  /* 45e9d0: at least 0x50 ticks since the last family-7 stamp. */
  return g_battle.ticks >= fighter->projectile_ready;
}

/* 40ec40/40ed20/40eb80 write the head node's +0x24 descriptor through
 * 408cb0. Retail skips the write while 53c1a8[player] is set or when a
 * morphing Shinnok (48e2d0, id 6) runs the script; neither state exists in
 * this engine. The default costume selects the 5172b8 rig records. */
static void head_mesh(Fighter *fighter, int mesh) {
  fighter->mesh_swap_joint = REPTILE_HEAD_JOINT;
  fighter->mesh_swap = mesh;
}

void reptile_script_head_open(Fighter *fighter) {
  head_mesh(fighter, REPTILE_HEAD_OPEN);
}

void reptile_script_head_wide(Fighter *fighter) {
  head_mesh(fighter, REPTILE_HEAD_WIDE);
}

/* 40eb80 restores rig record 517a9c, the walked head node and its mesh 2. */
void reptile_script_head_close(Fighter *fighter) {
  fighter->mesh_swap_joint = 0;
  fighter->mesh_swap = 0;
}

static void spit_cleanup(Fighter *fighter) {
  /* Portable adaptation: an interrupted retail task never reaches 40eb80
   * and leaves the swapped head descriptor; action cleanup restores it. */
  reptile_script_head_close(fighter);
}

static bool drool_drop(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)event;
  (void)opponent;
  Vec3 head;
  Mat3 basis;
  if (!fighter_joint_position(owner, REPTILE_HEAD_JOINT, &head, &basis)) {
    return true;
  }
  /* 40ee30 offsets the head-local point (.01, -.05, -.1) before 4b8140. */
  Vec3 mouth = vec_add(head, mat3_apply(basis, (Vec3){q16_float(0x28f), q16_float(-0xccc), q16_float(-0x1999)}));
  Vec3 forward = {q16_float(owner->saved_motion[0]), 0, q16_float(owner->saved_motion[1])};
  battle_spray(&drool_spray, mouth, forward, 0x2e14, 0x51e);
  return true;
}

static bool drool_done(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)event;
  (void)owner;
  (void)opponent;
  return false;
}

/* 40ee00 -> task 0x85 40ee30: nine droplet visits two ticks apart from
 * the head joint. Its randomized records are decorative; the portable
 * spray keeps their forward 0.16-0.2 speed, lift and gravity ranges. */
void reptile_script_acid_drool(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, drool_done, false, false);
  if (event == NULL) {
    return;
  }
  battle_event_program(event, 18);
  for (int visit = 0; visit < 9; ++visit) {
    battle_event_frame(event, visit * 2, 0, drool_drop);
  }
}

static void acid_step(Projectile *acid) {
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
  if (victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(acid, &acid_record) != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x48);
    acid->phase = 10;
  }
}

/* 498920: C[71] cue, invisible 4deae0 carrier (node flags 0x8c are culled)
 * at the owner's position, 0x23d7 along the 49c610 heading snapshot. */
void reptile_acid_spawn(Fighter *fighter, int task_type) {
  require(task_type == 76, "Unexpected Reptile acid task");
  battle_sound(SOUND_C, 0x47);
  Projectile *acid = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, acid_step);
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
  fighter_start_move(fighter, S_PUNCH, &spit);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->mmove_cleanup = spit_cleanup;
  retail_vm_enter_owned(fighter, 0x4ed1e8, fighter_ready, 10);
}

/* 483df0/483ed0/483fa0: twenty samples, ten four-sample chunks, 48e080
 * neutral reset, then 428810 plays the rest and 46f650 returns. The CPU
 * state hook 4339b0 between chunks belongs to the CPU controller. */
void reptile_acid_victim_tail(Fighter *victim) {
  native_begin(victim, 1, 65, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= 64; ++frame) {
    native_frame(victim, frame, frame, frame == 61 ? fighter_neutral : NULL);
  }
  native_frame(victim, 65, 64, NULL);
  native_play(victim, false);
}

/* Reaction 41. battle_react sends airborne victims to reaction 10, as
 * 483d60 does through 48ef30 and 481320. Grounded victims install the
 * 4e97f0 friction callback and enter 4e97f8. */
void reptile_acid_victim(Fighter *victim) {
  fighter_start_move(victim, S_HIT, &acid_victim);
  victim->sample_override = 0;
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e97f8, fighter_ready, 10);
}

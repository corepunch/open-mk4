#include "retail_vm.h"
#include "retail_native.h"
#include "native.h"
#include "script.h"
#include "native_services.h"
#include "math_util.h"

/* T2 actions 10/11 (498b70/498e70) set 309/30a and enter 4ed268/4ed2c0. */
static const MoveDef cast = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x309};
static const MoveDef air_cast = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x30a};
/* 480cf0 binds clip 310 at sample 7 and advances to 18 while falling. */
static const MoveDef fall = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK, .action = 0x1005};
/* Both waiter descriptors (4ed2b0/4ed2e8) select contact C[0]. */
/* 4d1718 -> ten share2 roots 50e3c4..50e4c0, meshes 261..270, two ticks
 * each. They carry the same authored origin as Cage's orb roots, and
 * 498be0 sets the root's .75 scale. */
static const MeshAnimation fireball_mesh = {
    .first = 261, .count = 10, .period = 2, .scale = 0xc000, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

static void fireball_move(Projectile *projectile) {
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
}

/* 49c6b0 probes the C[0] contact before the step callback. 49c690's
 * standing waiter skips a victim in action 2001; 49c670's air waiter does not.
 * The 40fe40 burst particles and the 413760 trail are not replicated. */
static bool fireball_contact(Projectile *projectile, bool air) {
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  if (!air && victim->action == ACTION_RETAIL_2001) {
    return false;
  }
  const Mk4Image *image = battle_data();
  uint32_t waiter = air ? MK4_RETAIL_DATA_FIREBALL_AIR_CONTACT_ROW : MK4_RETAIL_DATA_FIREBALL_STANDING_CONTACT_ROW;
  unsigned selector = mk4_image_u32(image, waiter);
  require(selector < 256, "Invalid native projectile contact selector");
  AttackRecord record = retail_vm_attack_record(image, mk4_image_u32(image, MK4_RETAIL_DATA_PROJECTILE_CONTACT_TABLE + selector * 4) * 4);
  if (battle_projectile_contact(projectile, &record) == CONTACT_MISS) {
    return false;
  }
  projectile->kind = 0;
  return true;
}

/* 498df0: retire once 49bc00 places the object beyond the arena radius
 * squared plus twelve square units. */
static void fireball_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  fireball_move(projectile);
  if (fireball_contact(projectile, false)) {
    return;
  }
  int64_t x = projectile->pos[AXIS_X], z = projectile->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    projectile->kind = 0;
  }
}

/* 498f20: floor contact starts camera shake 0 and sound C 0x1a. */
static void air_fireball_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  fireball_move(projectile);
  if (fireball_contact(projectile, true)) {
    return;
  }
  if (projectile->pos[AXIS_Y] > 0) {
    mk4_script_shake(&g_battle.fighters[projectile->owner], 0);
    battle_sound(SOUND_C, 0x1a);
    projectile->kind = 0;
  }
}

/* 498be0: 4ed808 places the object .2 along the current heading and .1
 * lower; 498d90 gives it .2 per tick along that heading. */
static Projectile *spawn(Fighter *fighter, ProjectileFn step) {
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 180, step);
  if (projectile == NULL) {
    return NULL;
  }
  projectile->mesh_animation = &fireball_mesh;
  projectile->initial_mesh = fireball_mesh.first;
  projectile->heading[0] = fighter_axis(fighter, AXIS_X);
  projectile->heading[1] = fighter_axis(fighter, AXIS_Z);
  const Mk4Image *image = battle_data();
  q16 offset = (q16)mk4_image_u32(image, MK4_RETAIL_DATA_FIREBALL_SPAWN_FORWARD_OFFSET);
  projectile->pos[AXIS_Y] += (q16)mk4_image_u32(image, MK4_RETAIL_DATA_FIREBALL_SPAWN_Y_OFFSET);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->pos[axis] += q16_mul(fighter_axis(fighter, axis), offset);
    projectile->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
  return projectile;
}

void retail_fireball_spawn(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Tanya fireball task");
  spawn(fighter, fireball_step);
}

/* 498ed0 adds a 0x23d7 descent to the same object. */
void retail_air_fireball_spawn(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Tanya air fireball task");
  Projectile *projectile = spawn(fighter, air_fireball_step);
  if (projectile != NULL) {
    projectile->phase = 1;
    projectile->vel[AXIS_Y] = 0x23d7;
  }
}

static void fall_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 480d46: landing binds sample 18 and 428d10 plays the rest of clip 310. */
static void fall_land(Fighter *fighter) {
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 18, last, fall_ready);
  for (int pose = 18; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    fall_land(fighter);
  }
}

static void fall_hold(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 18, fall_check);
  native_play(fighter, false);
}

/* 498eb0 -> 480cf0: gravity 0x20c with no vertical velocity, action 1005
 * and the peer-facing update 490bd0; one fall sample per visit until 18. */
void retail_fireball_fall(Fighter *fighter) {
  fighter_start_move(fighter, S_JUMP, &fall);
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_310, 7);
  fighter->body.vel[AXIS_Y] = 0;
  fighter->body.gravity = 0x20c;
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  native_begin(fighter, 1, 11, fall_hold);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, 7 + frame, fall_check);
  }
  native_play(fighter, false);
}

/* 45f5f0 stops all motion before every T2 action. */
void retail_fireball(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &cast);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_TANYA_FIREBALL_CASTER, fighter_ready);
}

/* Command flag 0x10 and 45ef20 require an airborne fighter with gravity. */
void retail_air_fireball(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &air_cast);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_AIR_FIREBALL, fighter_ready);
}

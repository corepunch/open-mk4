#include "native_services.h"

/* Kai's two skull specials share task code at 498e50..499734.
 * B,B+HP 499410: action 307, 4ed330 clip 21, task 499490 (type 48) rises
 * above the arena and drops onto the peer's launch position, C[8].
 * F,F+LP 498fc0: action 308, 4ed2f8 clip 22 hop, task 4990f0 (type 4a)
 * sinks under the floor and rises under the peer's launch position, C[9].
 * 49c610 snapshots the owner heading; 49c540 selects the peer group. */
static const MoveDef rising_cast = {.clip = 21, .last = 6, .flags = SF_LOCK | SF_RECORD, .action = 0x307};
static const MoveDef ground_cast = {.clip = 22, .last = 4, .flags = SF_LOCK | SF_RECORD, .action = 0x308};
/* C[8] 0x4ebe70 and C[9] 0x4ebe58: 13% damage, reactions 129/130, no block
 * reaction, flags 2 (mark). */
static const AttackRecord drop_record = {0x4000, 0x1958, 0x7d70, 0x96c8, 129, 129, 0, ATTACK_MARK, 0x2147};
static const AttackRecord rise_record = {0x3d70, -0xb851, 0x7d70, 0x96c8, 130, 130, 0, ATTACK_MARK, 0x2147};
/* Spawn rigs 4ed390/4ed320 both select rig 50c8f8: share2 mesh 48 with no
 * root translation. */
static const MeshAnimation skull_mesh = {.first = 48, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  SKULL_COOLDOWN = 0x30,
  SKULL_SPEED = 0x3333,
  SKULL_TOP = -0x40000,
  SKULL_DROP_FLOOR = -0x6666,
  SKULL_SINK = 0x10000,
  SKULL_LIFETIME = 120,
  /* Object +68 pitch: 0x191eb rising, 0x4b5c2 falling. */
  SKULL_UP = 0x191eb,
  SKULL_DOWN = 0x4b5c2,
  /* Object task phases. */
  SKULL_ASCEND = 0,
  SKULL_DESCEND,
  SKULL_SURFACE
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

static void skull_impact(Projectile *object) {
  /* 489f90 C[0x34], camera shake row 0; 40e860/40e190 and 40bde0 burst
   * decorations remain omitted, then 406740/41f830 retire the object. */
  battle_sound(SOUND_C, 0x34);
  battle_camera_shake(3, 4, 0x2666);
  object->kind = 0;
}

static void skull_move(Projectile *object) {
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
}

static void skull_relocate(Projectile *object, q16 vel_y, q16 pitch) {
  object->pos[AXIS_X] = object->target[0];
  object->pos[AXIS_Z] = object->target[1];
  object->vel[AXIS_X] = object->vel[AXIS_Z] = 0;
  object->vel[AXIS_Y] = vel_y;
  object->angle[AXIS_Z] = pitch;
}

static void skull_spawn_from(Fighter *fighter, ProjectileFn step, q16 forward, q16 y, q16 vel_y, q16 pitch) {
  /* 49c300 places the rig on the snapshotted heading; failure is a real
   * allocation branch (53c44c bit 2) and leaves the owner's move intact. */
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, SKULL_LIFETIME, step);
  if (object == NULL) {
    return;
  }
  const Fighter *victim = fighter_opponent(fighter);
  object->mesh_animation = &skull_mesh;
  object->initial_mesh = skull_mesh.first;
  object->heading[0] = fighter->saved_motion[0];
  object->heading[1] = fighter->saved_motion[1];
  object->pos[AXIS_X] += q16_mul(fighter->saved_motion[0], forward);
  object->pos[AXIS_Z] += q16_mul(fighter->saved_motion[1], forward);
  object->pos[AXIS_Y] += y;
  object->vel[AXIS_Y] = vel_y;
  object->angle[AXIS_Z] = pitch;
  object->target[0] = victim->body.pos[AXIS_X];
  object->target[1] = victim->body.pos[AXIS_Z];
  object->phase = SKULL_ASCEND;
}

static void rising_step(Projectile *object) {
  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  skull_move(object);
  if (object->phase == SKULL_ASCEND) {
    /* 4995b0: rise until Y <= -4, then drop over the saved peer position. */
    if (object->pos[AXIS_Y] <= SKULL_TOP) {
      skull_relocate(object, SKULL_SPEED, SKULL_DOWN);
      object->phase = SKULL_DESCEND;
    }
    return;
  }
  /* 4994ca: query C[8] each falling visit; contact or Y >= -.4 explodes. */
  if (battle_projectile_contact(object, &drop_record) != CONTACT_MISS || object->pos[AXIS_Y] >= SKULL_DROP_FLOOR) {
    skull_impact(object);
  }
}

static void ground_step(Projectile *object) {
  if (g_battle.contacts_off) {
    object->kind = 0;
    return;
  }
  skull_move(object);
  if (object->phase == SKULL_ASCEND) {
    /* 48f570: sink until Y >= 1, then 499227 rises under the saved peer position. */
    if (object->pos[AXIS_Y] >= SKULL_SINK) {
      skull_relocate(object, -SKULL_SPEED, SKULL_UP);
      object->phase = SKULL_DESCEND;
    }
    return;
  }
  if (object->phase == SKULL_DESCEND) {
    /* 48f5f0: no contact until the rig reaches the floor plane. */
    if (object->pos[AXIS_Y] <= 0) {
      object->phase = SKULL_SURFACE;
    }
    return;
  }
  /* 499128: query C[9]; contact explodes, Y <= -4 retires silently. */
  if (battle_projectile_contact(object, &rise_record) != CONTACT_MISS) {
    skull_impact(object);
  } else if (object->pos[AXIS_Y] <= SKULL_TOP) {
    object->kind = 0;
  }
}

static void skull_cast(Fighter *fighter, const MoveDef *move, uint32_t script) {
  fighter_start_move(fighter, S_PUNCH, move);
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  retail_vm_enter_owned(fighter, script, fighter_ready, 11);
}

void kai_rising_skull(Fighter *fighter) {
  skull_cast(fighter, &rising_cast, 0x4ed330);
}

void kai_rising_skull_spawn(Fighter *fighter) {
  /* 499450: task 499490 type 48 from 4ed390 (back .4, Y -.01), rising .2. */
  skull_spawn_from(fighter, rising_step, -0x6666, -0x28f, -SKULL_SPEED, SKULL_UP);
  retail_vm_enter_owned(fighter, 0x4ed348, fighter_ready, 11);
}

void kai_rising_skull_finish(Fighter *fighter) {
  /* 499470: 48e3a0 stamps slot 3 for 45eb40, then 46f650. */
  fighter->skull_ready = g_battle.ticks + SKULL_COOLDOWN;
  mk4_mmove_thrown_finish(fighter);
}

void kai_ground_skull(Fighter *fighter) {
  skull_cast(fighter, &ground_cast, 0x4ed2f8);
}

static void ground_fall_landed(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void ground_fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    ground_fall_landed(fighter);
  }
}

static void ground_air_fall(Fighter *fighter) {
  /* 47e990: face the peer, gravity 147, clip 310 at sample 7 for eight
   * visits, then advance one sample per visit until 46fd90 lands. */
  enum {
    FALL_FIRST = 7,
    FALL_HOLD = 8,
    FALL_LAST = 27
  };

  fighter->body.gravity = 0x147;
  retail_face_peer(fighter);
  mk4_mmove_bind(fighter, 310, FALL_LAST + 1, FALL_FIRST);
  int frames = FALL_HOLD + FALL_LAST - FALL_FIRST;
  native_begin(fighter, 1, frames, ground_fall_landed);
  for (int frame = 1; frame <= frames; ++frame) {
    int sample = frame <= FALL_HOLD ? FALL_FIRST : FALL_FIRST + frame - FALL_HOLD;
    FighterFn action = frame <= FALL_HOLD ? NULL : frame < frames ? ground_fall_check : mk4_mmove_wait_grounded;
    native_frame(fighter, frame, sample, action);
  }
  native_play(fighter, false);
}

static void ground_cast_return(Fighter *fighter) {
  /* 499000 state 2: 48ef30 selects 47e990 in the air, otherwise 46f650. */
  if (fighter_airborne(fighter)) {
    ground_air_fall(fighter);
  } else {
    mk4_mmove_thrown_finish(fighter);
  }
}

void kai_ground_skull_spawn(Fighter *fighter) {
  /* 499000: task 4990f0 type 4a from 4ed320 (forward .2, Y -.2), sinking
   * .2; the owner sleeps 20 visits, then 429460 rewinds clip 22 to 0. */
  skull_spawn_from(fighter, ground_step, 0x3333, -0x3333, SKULL_SPEED, SKULL_DOWN);
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 1, 20 + last + 1, ground_cast_return);
  for (int frame = 1; frame <= 20; ++frame) {
    native_frame(fighter, frame, last, NULL);
  }
  for (int sample = last; sample >= 0; --sample) {
    native_frame(fighter, 20 + last + 1 - sample, sample, NULL);
  }
  native_play(fighter, false);
}

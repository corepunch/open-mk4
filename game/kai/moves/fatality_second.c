#include "native_services.h"

enum {
  KAI_EVENT_FATALITY_1 = 23,
  KAI_EVENT_FATALITY_2
};

#include <math.h>

/* Second fatality: 467cd0 -> 43bd70 -> roster table 4dfe90[11] = 43bfe0.
 * The owner levitates on clips 493/486/487 (4dff88) and throws the skull
 * object 43c140; within 1.3 units it installs reaction 178 (4814c0), whose
 * head task 455bf0 and script 4ea240 -> 46a500 -> 4e5be0 fell the victim. */
static const MoveDef owner_pose = {.clip = 493, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4dffe8 selects the skull rig 50c8f8 (share2 mesh 48): forward .2, Y -.2. */
static const MeshAnimation skull_mesh = {.first = 48, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  HOVER_RISE,
  HOVER_EASE_START,
  HOVER_EASE,
  HOVER_DESCEND,
  HOVER_APEX_HOLD,
  HOVER_ASCEND,
  HOVER_LOW_HOLD,
  SKULL_FLIGHT = 0,
  SKULL_HIT,
  /* 4e5be0 plays clip 206 for 100 visits; then the scene settles for one
   * 60-visit park interval before the result. */
  SECOND_SETTLE = 160
};

void kai_second_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector == 45, "Unexpected Kai second scene event");
  /* 4315b0 queues the scene camera event; the finisher owns its input lock. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

void kai_second_child_walk(Fighter *fighter) {
  /* 406ba0 walks the node's +10 child list with selector 3. The finisher
   * disarmed both fighters and attached no child objects before these
   * calls, so the portable actors have no child nodes to visit. */
  require(!fighter->armed, "Kai second fatality actors must not carry a weapon");
}

static void hover_step(Projectile *object) {
  /* 46a280/46a340/46a380 drive the owner's group gravity. Visit counts are
   * this independent task's own state, not an actor clock. */
  Fighter *owner = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  object->lifetime = 2;
  Body *body = &owner->body;
  switch (object->phase) {
    case HOVER_RISE:
      body->gravity = -0x28f;
      object->phase = HOVER_EASE_START;
      break;
    case HOVER_EASE_START:
      object->bounce = 10;
      object->phase = HOVER_EASE;
      break;
    case HOVER_EASE:
      body->gravity += 0x41;
      if (--object->bounce == 0) {
        body->gravity = 0x83;
        object->phase = HOVER_DESCEND;
      }
      break;
    case HOVER_DESCEND:
      /* 48f490 waits until the rise stops, then 46a380 holds ten visits. */
      if (body->vel[AXIS_Y] >= 0) {
        object->bounce = 10;
        object->phase = HOVER_APEX_HOLD;
      }
      break;
    case HOVER_APEX_HOLD:
      if (--object->bounce == 0) {
        body->gravity = -0x83;
        object->phase = HOVER_ASCEND;
      }
      break;
    case HOVER_ASCEND:
      /* 48f510 waits until the body rises again, then holds ten visits. */
      if (body->vel[AXIS_Y] < 0) {
        object->bounce = 10;
        object->phase = HOVER_LOW_HOLD;
      }
      break;
    case HOVER_LOW_HOLD:
      if (--object->bounce == 0) {
        body->gravity = 0x83;
        object->phase = HOVER_DESCEND;
      }
      break;
    default:
      require(false, "Invalid Kai hover phase");
  }
}

void kai_second_hover(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected Kai hover task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, hover_step);
  if (object != NULL) {
    object->phase = HOVER_RISE;
    hover_step(object);
  }
}

static void skull_step(Projectile *object) {
  Fighter *victim = (Fighter *)fighter_opponent(&g_battle.fighters[object->owner]);
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
  if (object->phase == SKULL_FLIGHT) {
    object->lifetime = 2;
    /* 43c17e: within 1.3 units on X, 480f80 installs reaction 178 on the
     * peer; the object retires after a further 15 visits (406740). */
    q16 dx = victim->body.pos[AXIS_X] - object->pos[AXIS_X];
    if ((dx < 0 ? -dx : dx) <= 0x14ccc) {
      battle_react(victim, 178);
      object->phase = SKULL_HIT;
      object->lifetime = 16;
    }
  }
}

void kai_second_skull(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected Kai skull task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 2, skull_step);
  if (object == NULL) {
    return;
  }
  const Fighter *victim = fighter_opponent(fighter);
  object->mesh_animation = &skull_mesh;
  object->initial_mesh = skull_mesh.first;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    object->pos[axis] += q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
  object->pos[AXIS_Y] -= 0x3333;
  object->angle[AXIS_Z] = 0x5e147;
  /* 431110 normalizes the X/Z direction to the peer; 43c29d moves .3 along
   * it with Y velocity .08. */
  double dx = (double)victim->body.pos[AXIS_X] - object->pos[AXIS_X];
  double dz = (double)victim->body.pos[AXIS_Z] - object->pos[AXIS_Z];
  double length = sqrt(dx * dx + dz * dz);
  if (length > 0.) {
    object->vel[AXIS_X] = (q16)(dx / length * 0x4ccc);
    object->vel[AXIS_Z] = (q16)(dz / length * 0x4ccc);
  }
  object->vel[AXIS_Y] = 0x147a;
  object->phase = SKULL_FLIGHT;
}

static void second_complete(Fighter *victim) {
  (void)victim;
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

void kai_second_victim_fall(Fighter *victim) {
  /* 46a500: 406ba0, then 4e5be0 drops the victim on clip 206 and parks.
   * An independent settle row ends the scene after one park interval. */
  kai_second_child_walk(victim);
  native_motion_begin(victim, SECOND_SETTLE, second_complete);
  native_motion_play(victim);
  retail_vm_enter_owned(victim, 0x4e5be0, fighter_ready, 11);
}

void kai_second_victim(Fighter *victim) {
  /* 4814c0 -> 455bf0: hide head joint 2 and launch its piece (gravity 20c,
   * Y -.205, spin .4/-.18/-.7) with neck blood, then script 4ea240. */
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(victim, 2, &neck, &unused);
  ScenePiece *head = battle_detach_piece(victim, 2, (q16[3]){0, -0x347a, 0}, (q16[3]){0x6666, -0x2e14, -0xb333});
  if (head != NULL) {
    head->gravity = 0x20c;
  }
  battle_blood_hit(owner, victim, neck, 0x3333);
  retail_vm_enter_owned(victim, 0x4ea240, fighter_ready, 11);
}

void kai_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, KAI_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* 43bfe0 (after 41aaf0): 48f030 keeps the victim's side, then the owner
   * stands at the X/Z origin and the victim five units along X. The
   * pair-axis heading replaces the retail absolute yaw 4b5c2 and facing
   * flags (43c3a0/43c330); the scene camera stays portable. */
  q16 side = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? Q16_ONE : -Q16_ONE;
  fighter->body.pos[AXIS_X] = fighter->body.pos[AXIS_Z] = 0;
  victim->body.pos[AXIS_X] = 5 * side;
  victim->body.pos[AXIS_Z] = 0;
  mk4_script_scene_pose(victim, 13, 0);
  native_hold(victim, 1, NULL);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(fighter, 0x4dff88, fighter_ready, 11);
}

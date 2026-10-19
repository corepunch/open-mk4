#include "../character.h"

/* Reiko's D,F+LP target 496db0: stamp slot 18, snapshot the
 * pair axis (49c610), action 0x315, then 4ed038 throws on clip 437 and
 * spawns task 82 (496e00) at pose 13. */
static const MoveDef s_star_throw = {.clip = 437, .last = 23, .flags = SF_LOCK | SF_RECORD, .action = 0x315};
/* 4ed080: C[21] contact, bounds 496f10, contact 496f40, expiry 496f60. */
static const AttackRecord star_record = {0x1072b, -0x82d0, 0xcf1a, 0x6147, 45, 45, 2, ATTACK_MARK, 0x2147};
/* 4deae0 is an invisible carrier node. The thrown star drawn by 415620 uses
 * rig 50cf34: share2 mesh 73 translated (0x2000, 0x2000, 0), spun 0xb333. */
static const MeshAnimation star_mesh = {.first = 73, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .offset = {0x2000, 0x2000, 0}};

/* Noob reaches this only through his projectile CPU row (4df9c4 word
 * 0x20018), which enters the native without LP record gate 45e6d0. Reaction
 * 45 is the shared table entry owned by Reiko's port. */
/* 49c690/49c6b0 with 4ed080: the query skips a crouching (2001) peer. A
 * contact plays C[0x65] and retires the object; otherwise 49bc00 retires it
 * beyond the arena radius squared plus 12. The object list then moves it. */
static void star_step(Projectile *star) {
  if (g_battle.contacts_off) {
    star->kind = 0;
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[star->owner]);
  ContactResult result = victim->action == ACTION_RETAIL_2001 ? CONTACT_MISS : battle_projectile_contact(star, &star_record);
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
void noob_saibot_star_spawn(Fighter *fighter, int task_type) {
  require(task_type == 82, "Unexpected Noob Saibot star task");
  Projectile *star = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 600, star_step);
  if (star == NULL) {
    return;
  }
  star->mesh_animation = &star_mesh;
  star->initial_mesh = star_mesh.first;
  star->heading[0] = fighter->saved_motion[AXIS_X];
  star->heading[1] = fighter->saved_motion[AXIS_Z];
  star->vel[AXIS_X] = q16_mul(fighter->saved_motion[AXIS_X], 0x30a3);
  star->vel[AXIS_Z] = q16_mul(fighter->saved_motion[AXIS_Z], 0x30a3);
}

void noob_saibot_star(Fighter *fighter) {
  mk4_special_stamp(fighter, 0x12);
  fighter_start_move(fighter, S_PUNCH, &s_star_throw);
  fighter->saved_motion[AXIS_X] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[AXIS_Z] = fighter_axis(fighter, AXIS_Z);
  noob_saibot_entry_retail_script_004ed038(fighter);
}

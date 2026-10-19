#include "../character.h"

/* D,F+LP 496db0 (also the projectile CPU row): stamp slot 18, snapshot the
 * pair axis (49c610), action 0x315, then 4ed038 throws on clip 437 and
 * spawns task 82 (496e00) at pose 13. */
static const MoveDef s_star_throw = {.clip = 437, .last = 23, .flags = SF_LOCK | SF_RECORD, .action = 0x315};
/* 4ed080: C[21] contact, bounds 496f10, contact 496f40, expiry 496f60. */
static const AttackRecord star_record = {0x1072b, -0x82d0, 0xcf1a, 0x6147, 45, 45, 2, ATTACK_MARK, 0x2147};
/* 4deae0 is an invisible carrier node. The thrown star drawn by 415620 uses
 * rig 50cf34: share2 mesh 73 translated (0x2000, 0x2000, 0), spun 0xb333. */
static const MeshAnimation star_mesh = {.first = 73, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .offset = {0x2000, 0x2000, 0}};

/* 45e6d0: slot 18 must be at least 80 ticks old. */
bool reiko_can_star(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && mk4_special_elapsed(fighter, 0x12, 80);
}

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
void reiko_star_spawn(Fighter *fighter, int task_type) {
  require(task_type == 82, "Unexpected Reiko star task");
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

void reiko_star(Fighter *fighter) {
  mk4_special_stamp(fighter, 0x12);
  fighter_start_move(fighter, S_PUNCH, &s_star_throw);
  fighter->saved_motion[AXIS_X] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[AXIS_Z] = fighter_axis(fighter, AXIS_Z);
  reiko_entry_retail_script_004ed038(fighter);
}

static const MoveDef s_star_victim = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = 0x2013};

static void star_victim_neutral(Fighter *fighter) {
  /* 483fa0 -> 48e080: the stagger returns to neutral before its last poses. */
  fighter_neutral(fighter);
}

static void star_victim_end(Fighter *fighter) {
  if (fighter->health > 0) {
    mk4_return_to_stance(fighter);
  } else {
    fighter_stop_mmove(fighter);
  }
}

/* 483df0: play twenty visits, then 483ec0 ten blocks of four (429490), each
 * advancing one pose per visit. 48e080 returns to neutral, 428810 finishes
 * clip 182 and 483fa0 enters the controller. */
void reiko_script_star_victim_tail(Fighter *fighter) {
  int pose = fighter_sample(fighter), last = s_star_victim.last;
  native_begin(fighter, 1, last - pose, star_victim_end);
  for (int frame = 1; frame <= last - pose; ++frame) {
    native_frame(fighter, frame, pose + frame, frame == 60 ? star_victim_neutral : NULL);
  }
  native_play(fighter, false);
}

/* Reaction 45 (483d40): 481010 resets the peer and stores action 0x2013.
 * Airborne victims continue as reaction 10 (481320 -> 4e89a0). Otherwise
 * 494520 disarms, 4906c0 snaps to the floor and stops, 4e97f0 installs
 * friction and 4e97f8 pushes back on clip 182. */
void reiko_star_reaction(Fighter *victim) {
  if (fighter_airborne(victim)) {
    battle_react(victim, 10);
    return;
  }
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  fighter_start_move(victim, S_HIT, &s_star_victim);
  victim->flags |= FF_REACTING;
  victim->body.pos[AXIS_Y] = victim->body.ground;
  mk4_script_stop_body(victim);
  victim->body.physics = fighter_friction;
  reiko_entry_retail_script_004e97f8(victim);
}

#include "retail_vm.h"
#include "../character.h"

/* F,B,F,F+LK -> 467ce0 -> 43cbb0 -> 4e0498[12] = 446d00 (heart rip). */
static const MoveDef owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip owner_female[] = {
  {{.clip = 403, .last = 237, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip owner_male[] = {
  {{.clip = 402, .last = 237, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 404, .last = 204, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on
/* 446fa0 -> 406dd0(50831c): ja_fat_g.geo mesh 0 on the owner's joint 10,
 * placed by 408cb0/408510 at (-0xf5c, -0xf5c, 0x7ae). */
static const MeshAnimation heart_mesh = {
    .first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_JAREK_FATALITY, .offset = {-0xf5c, -0xf5c, 0x7ae}};

static bool female_victim(const Fighter *victim) {
  /* 474fa0 -> 475160 reads the peer roster; list 4e7b28 begins 9, 2. */
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  return roster == 9 || roster == 2;
}

static void heart_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  const Fighter *owner = &g_battle.fighters[object->owner];
  Vec3 hand;
  Mat3 unused;
  fighter_joint_position(owner, 10, &hand, &unused);
  object->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
  object->heading[0] = owner->heading[0];
  object->heading[1] = owner->heading[1];
  object->lifetime = 2;
}

/* 446f80: 446fa0 attaches the heart, then generated 4e1248 finishes the
 * owner clip, binds 405 and parks. */
void jarek_first_heart(Fighter *fighter) {
  Projectile *heart = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, heart_step);
  if (heart != NULL) {
    heart->mesh_animation = &heart_mesh;
    heart->initial_mesh = heart_mesh.first;
    heart_step(heart);
  }
  retail_vm_enter_owned(fighter, 0x4e1248, fighter_ready, 12);
}

static void owner_stream(Fighter *fighter) {
  if (female_victim(fighter_opponent(fighter))) {
    retail_vm_enter_owned(fighter, 0x4e1220, fighter_ready, 12);
  } else {
    retail_vm_enter_owned(fighter, 0x4e1230, fighter_ready, 12);
  }
}

static void owner_blend_done(Fighter *fighter) {
  /* Stage 2/4 waits 15 visits before stage 3/5 enters the owner root. */
  native_hold(fighter, 15, owner_stream);
}

static void owner_blend(Fighter *fighter) {
  /* Stage 1: bank B 0x2f, then 475250 blends into 403 or 402. */
  battle_sound(SOUND_B, 0x2f);
  native_scene_blends(fighter, female_victim(fighter_opponent(fighter)) ? owner_female : owner_male, 1, owner_blend_done);
}

static void victim_reach(Fighter *victim) {
  native_hold(victim, 15, jarek_entry_retail_script_004e1260);
}

static void victim_aligned(Fighter *victim) {
  /* 4756f0 copies the owner's X/Z: clip 404 carries the paired offset. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  victim_reach(victim);
}

static void victim_blend(Fighter *victim) {
  native_scene_blends(victim, victim_blends, 2, victim_aligned);
}

static void chest_blood(Fighter *victim) {
  Vec3 chest;
  Mat3 unused;
  fighter_joint_position(victim, 3, &chest, &unused);
  battle_blood_hit(fighter_opponent(victim), victim, chest, 0x3333);
}

/* 447310: cursor 44, blood task 447450, bank B 0x23/0x16, then 4e1270. */
void jarek_first_victim_rip(Fighter *victim) {
  victim->sample_override = 44;
  chest_blood(victim);
  battle_sound(SOUND_B, 0x23);
  battle_sound(SOUND_B, 0x16);
  retail_vm_enter_owned(victim, 0x4e1270, fighter_ready, 12);
}

/* 4473b0: bank B 0x12 and shake 5, cursor 148, then 4e1280. */
void jarek_first_victim_impact(Fighter *victim) {
  battle_sound(SOUND_B, 0x12);
  mk4_script_shake(victim, 5);
  victim->sample_override = 148;
  retail_vm_enter_owned(victim, 0x4e1280, fighter_ready, 12);
}

/* 447410: shake 5 before the final 4e1290 playback. */
void jarek_first_victim_end(Fighter *victim) {
  mk4_script_shake(victim, 5);
  retail_vm_enter_owned(victim, 0x4e1290, fighter_ready, 12);
}

static void scene_wait(Fighter *victim) {
  victim->mmove_hold = fighter_move(fighter_opponent(victim))->clip != 405;
}

static void scene_end(Fighter *victim) {
  fighter_stop_mmove(victim);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 49cb60 retires the victim task with its last pose. The owner parks on
 * clip 405 in 4e1248; the portable scene completes once both are done. */
void jarek_first_victim_done(Fighter *victim) {
  native_begin(victim, 0, 0, scene_end);
  native_frame(victim, 0, fighter_sample(victim), scene_wait);
  native_play(victim, false);
}

void jarek_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, JAREK_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* The owner keeps its stance, locked, through the 16-visit stage-0 wait. */
  fighter_start_move(fighter, S_PUNCH, &owner_wait);
  /* Stage 0: 475530 aligns the scene pair one unit apart. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), Q16_ONE);
  }
  mk4_script_scene_pose(victim, 13, 0);
  /* 404ef0(447150): the victim task waits 11 visits before its blends. */
  native_hold(victim, 11, victim_blend);
  native_hold(fighter, 16, owner_blend);
}

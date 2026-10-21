#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: 44f5e0, 44f620, 44f6f0, 49cb60. */
static const MoveDef n44f5e0_first_owner = {.clip = 398, .last = 26, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n44f5e0_first_victim = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n44f5e0_first_intro[] = {
  {{.clip = 398, .last = 26, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n44f5e0_first_bow[] = {
  {{.clip = 210, .last = 36, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const ParticleVisual n44f5e0_tornado[] = {
  {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 1, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 2, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 3, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 4, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 5, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
};
// clang-format on
static const MeshAnimation n44f5e0_first_bolt = {.first = 106, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED};

bool fujin_can_first_fatality(const Fighter *fighter) {
  if (!retail_can_finish_range(fighter)) {
    return false;
  }
  unsigned pair = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned forbidden =
      BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  if ((fighter->held & pair) != pair || (fighter->held & forbidden)) {
    return false;
  }
  /*45e3b0/4e4138 increments a native input-event counter, not a clock.
   *Both eligible press tables visit this gate while BL+RUN are held. */
  Fighter *owner = &g_battle.fighters[fighter == &g_battle.fighters[1]];
  return ++owner->fujin_finish_inputs == 14;
}

static void n44f5e0_first_wind_start(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  for (int part = 0; part < 6; ++part) {
    ParticleSpray spray = {.visual = &n44f5e0_tornado[part], .count = 1, .lifetime = 600};
    Vec3 point = {fighter_x(victim), q16_float(victim->body.ground - part * 0x6666), fighter_z(victim)};
    battle_spray(&spray, point, (Vec3){0}, 0, 0);
  }
  /*450090/4501e0: rise for216 visits, approaching the point2.3 ahead
   *of the caster at.005 of the initial displacement for200 visits. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 target = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x24ccc);
    victim->body.vel[axis] = q16_mul(target - victim->body.pos[axis], 0x147);
  }
}

static void n44f5e0_first_spin_start(Fighter *victim) {
  victim->body.spin[AXIS_Y] = victim->facing > 0 ? -0x51e : 0x51e;
}

static void n44f5e0_first_spin_accelerate(Fighter *victim) {
  victim->body.spin[AXIS_Y] = q16_mul(victim->body.spin[AXIS_Y], 0x110c4);
}

static void n44f5e0_first_rise(Fighter *victim) {
  victim->body.pos[AXIS_Y] -= 0x28f;
}

static void n44f5e0_first_rise_and_spin(Fighter *victim) {
  n44f5e0_first_rise(victim);
  n44f5e0_first_spin_accelerate(victim);
}

static void n44f5e0_first_start_rise(Fighter *victim) {
  n44f5e0_first_wind_start(victim);
  n44f5e0_first_rise_and_spin(victim);
}

static void n44f5e0_first_stop_drift(Fighter *victim) {
  victim->body.vel[AXIS_X] = 0;
  victim->body.vel[AXIS_Z] = 0;
  n44f5e0_first_rise(victim);
}

static bool n44f5e0_first_sound_loop(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)victim;
  if (!owner->native_signal || g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  battle_sound(SOUND_B, 0x26);
  battle_event_delay(event, 60);
  return true;
}

void fujin_first_loop_start(Fighter *fighter) {
  /*44f5e0 sets13 before the initial399 cycle:14 complete passes. */
  fighter->native_variant = 13;
  BattleEvent *sound = battle_event_start(fighter, n44f5e0_first_sound_loop, false, false);
  if (sound != NULL) {
    battle_event_delay(sound, 25);
  }
  retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
}

void fujin_first_cycle(Fighter *fighter) {
  if (fighter->native_variant-- > 0) {
    retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
    return;
  }
  retail_weapon_equip(fighter);
  native_scene_blends(fighter, n44f5e0_first_bow, 1, NULL);
  native_script_end(fighter, 0x4e1f80, fighter_ready, 5);
}

static void n44f5e0_first_explode(Fighter *victim, Fighter *owner) {
  /*44e090 hides the body eight visits after the bolt's ten-visit arrival.
   *44eb00's closed head/hand debris uses the2/9/10 list at4e1cd0. */
  Fighter source = *victim;
  source.appearance = APPEARANCE_NORMAL;
  memset(source.hidden_joints, 0, sizeof source.hidden_joints);
  if (source.assets != NULL) {
    source.scene_model = &source.assets->cut_characters.data[roster_def(source.roster)->asset];
  }
  static const int joints[] = {2, 9, 10};
  static const q16 scales[] = {0x1999, 0x147a, 0x147a};
  for (int part = 0; part < 3; ++part) {
    ScenePiece *piece = battle_detach_piece(&source, joints[part], (q16[3]){part == 1 ? -0x2147 : 0x2147, -0x2147, 0}, (q16[3]){0, 0x2147, 0});
    if (piece != NULL) {
      piece->scale = scales[part];
      piece->gravity = 0x189;
    }
  }
  battle_blood_hit(owner, victim, (Vec3){fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]), fighter_z(victim)}, Q16_ONE);
  victim->appearance = APPEARANCE_HIDDEN;
  mk4_script_stop_body(victim);
  fighter_stop_mmove(victim);
}

static void n44f5e0_first_bolt_step(Projectile *bolt) {
  if (g_battle.phase != PHASE_FATALITY) {
    bolt->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    bolt->pos[axis] += bolt->vel[axis];
  }
  if (bolt->ticks == 10) {
    battle_sound(SOUND_C, 0x43);
    bolt->mesh_animation = NULL;
  }
  if (bolt->ticks == 18) {
    n44f5e0_first_explode(&g_battle.fighters[1 - bolt->owner], &g_battle.fighters[bolt->owner]);
    bolt->kind = 0;
  }
}

static bool n44f5e0_first_shot(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)victim;
  ++owner->native_variant;
  owner->native_signal = false;
  for (int i = 0; i < MAX_PARTICLES; ++i) {
    Particle *particle = &g_battle.particles[i];
    if (particle->visual != NULL && particle->visual->animation.source == MESH_FUJIN_WIND) {
      particle->ticks = 0;
    }
  }
  Projectile *bolt = battle_spawn_projectile(owner, PROJECTILE_EFFECT, 19, n44f5e0_first_bolt_step);
  if (bolt != NULL) {
    bolt->mesh_animation = &n44f5e0_first_bolt;
    bolt->initial_mesh = 106;
    bolt->angle[1] = 0x3243f;
    bolt->angle[2] = -Q16_ONE;
    bolt->pos[AXIS_Y] = -0x18000;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      bolt->pos[axis] += q16_mul(fighter_axis(owner, axis), 0xccc);
      bolt->vel[axis] = q16_mul(fighter_axis(owner, axis), 0x3333);
    }
    bolt->vel[AXIS_Y] = -0x2e14;
  }
  battle_sound(SOUND_C, 0x29);
  retail_vm_enter_owned(owner, 0x4e1f98, fighter_ready, 5);
  return true;
}

static bool n44f5e0_first_shots_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)owner;
  (void)victim;
  return false;
}

void fujin_first_shoot(Fighter *fighter) {
  fighter->native_variant = 0;
  BattleEvent *event = battle_event_start(fighter, n44f5e0_first_shots_done, false, false);
  require(event != NULL, "Fujin first scene needs its bounded shot coordinator");
  battle_event_program(event, 86);
  battle_event_frame(event, 0, 0, n44f5e0_first_shot);
  battle_event_frame(event, 40, 0, n44f5e0_first_shot);
  battle_event_frame(event, 85, 0, n44f5e0_first_shot);
}

void fujin_first_fire_end(Fighter *fighter) {
  fighter_stop_mmove(fighter);
  if (fighter->native_variant == 3) {
    battle_end_finisher();
  }
}

void fujin_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FUJIN_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n44f5e0_first_victim);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  native_begin(victim, 0, 305, fighter_stop_mmove);
  for (int frame = 0; frame <= 305; ++frame) {
    FighterFn action = frame == 80                 ? n44f5e0_first_spin_start
                       : frame == 90               ? n44f5e0_first_start_rise
                       : frame == 290              ? n44f5e0_first_stop_drift
                       : frame > 90 && frame < 140 ? n44f5e0_first_rise_and_spin
                       : frame >= 90               ? n44f5e0_first_rise
                       : frame > 80 && frame < 140 ? n44f5e0_first_spin_accelerate
                                                   : NULL;
    native_frame(victim, frame, 0, action);
  }
  native_play(victim, false);
  fighter_start_move(fighter, S_PUNCH, &n44f5e0_first_owner);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_signal = true;
  g_battle.camera.follow_height = true;
  native_scene_blends(fighter, n44f5e0_first_intro, 1, NULL);
  native_script_end(fighter, 0x4e1f58, fighter_ready, 5);
}

/* Native service cluster: 446f80, 447310, 4473b0, 447410, 49cb60. */
enum {
  n446f80_JAREK_EVENT_FATALITY_1 = 25,
  n446f80_JAREK_EVENT_FATALITY_2
};

/* F,B,F,F+LK -> 467ce0 -> 43cbb0 -> 4e0498[12] = 446d00 (heart rip). */
static const MoveDef n446f80_owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n446f80_owner_female[] = {
  {{.clip = 403, .last = 237, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n446f80_owner_male[] = {
  {{.clip = 402, .last = 237, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n446f80_victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 404, .last = 204, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on
/* 446fa0 -> 406dd0(50831c): ja_fat_g.geo mesh 0 on the owner's joint 10,
 * placed by 408cb0/408510 at (-0xf5c, -0xf5c, 0x7ae). */
static const MeshAnimation n446f80_heart_mesh = {
    .first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_JAREK_FATALITY, .offset = {-0xf5c, -0xf5c, 0x7ae}};

static bool n446f80_female_victim(const Fighter *victim) {
  /* 474fa0 -> 475160 reads the peer roster; list 4e7b28 begins 9, 2. */
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  return roster == 9 || roster == 2;
}

static void n446f80_heart_step(Projectile *object) {
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
  Projectile *heart = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, n446f80_heart_step);
  if (heart != NULL) {
    heart->mesh_animation = &n446f80_heart_mesh;
    heart->initial_mesh = n446f80_heart_mesh.first;
    n446f80_heart_step(heart);
  }
  retail_vm_enter_owned(fighter, 0x4e1248, fighter_ready, 12);
}

static void n446f80_owner_stream(Fighter *fighter) {
  if (n446f80_female_victim(fighter_opponent(fighter))) {
    retail_vm_enter_owned(fighter, 0x4e1220, fighter_ready, 12);
  } else {
    retail_vm_enter_owned(fighter, 0x4e1230, fighter_ready, 12);
  }
}

static void n446f80_owner_blend_done(Fighter *fighter) {
  /* Stage 2/4 waits 15 visits before stage 3/5 enters the owner root. */
  native_hold(fighter, 15, n446f80_owner_stream);
}

static void n446f80_owner_blend(Fighter *fighter) {
  /* Stage 1: bank B 0x2f, then 475250 blends into 403 or 402. */
  battle_sound(SOUND_B, 0x2f);
  native_scene_blends(fighter, n446f80_female_victim(fighter_opponent(fighter)) ? n446f80_owner_female : n446f80_owner_male, 1, n446f80_owner_blend_done);
}

static void n446f80_victim_reach(Fighter *victim) {
  native_hold(victim, 15, NULL);
  native_script_end(victim, 0x4e1260, fighter_ready, 12);
}

static void n446f80_victim_aligned(Fighter *victim) {
  /* 4756f0 copies the owner's X/Z: clip 404 carries the paired offset. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  n446f80_victim_reach(victim);
}

static void n446f80_victim_blend(Fighter *victim) {
  native_scene_blends(victim, n446f80_victim_blends, 2, n446f80_victim_aligned);
}

static void n446f80_chest_blood(Fighter *victim) {
  Vec3 chest;
  Mat3 unused;
  fighter_joint_position(victim, 3, &chest, &unused);
  battle_blood_hit(fighter_opponent(victim), victim, chest, 0x3333);
}

/* 447310: cursor 44, blood task 447450, bank B 0x23/0x16, then 4e1270. */
void jarek_first_victim_rip(Fighter *victim) {
  victim->sample_override = 44;
  n446f80_chest_blood(victim);
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

static void n446f80_scene_wait(Fighter *victim) {
  victim->mmove_hold = fighter_move(fighter_opponent(victim))->clip != 405;
}

static void n446f80_scene_end(Fighter *victim) {
  fighter_stop_mmove(victim);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 49cb60 retires the victim task with its last pose. The owner parks on
 * clip 405 in 4e1248; the portable scene completes once both are done. */
void jarek_first_victim_done(Fighter *victim) {
  native_begin(victim, 0, 0, n446f80_scene_end);
  native_frame(victim, 0, fighter_sample(victim), n446f80_scene_wait);
  native_play(victim, false);
}

void jarek_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n446f80_JAREK_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* The owner keeps its stance, locked, through the 16-visit stage-0 wait. */
  fighter_start_move(fighter, S_PUNCH, &n446f80_owner_wait);
  /* Stage 0: 475530 aligns the scene pair one unit apart. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), Q16_ONE);
  }
  mk4_script_scene_pose(victim, 13, 0);
  /* 404ef0(447150): the victim task waits 11 visits before its blends. */
  native_hold(victim, 11, n446f80_victim_blend);
  native_hold(fighter, 16, n446f80_owner_blend);
}

/* Native service cluster: 449620, 449890, 4498d0, 449940, 49cb60. */
/* LK-release record 4e43e8 (F,F,D,F, window 70, gate 45e1b0) -> 467ce0 ->
 * 43cbb0 -> 4e0498[13] = 4494c0: Jax tears off both of the victim's arms.
 * Owner: 475530 places the victim 1.1 units ahead, waits 16 ticks, B[0x30]
 * and a 0x32f1/0x3333 blend into 384 (4fbf7c), 4e1390 plays 384, then 449620
 * blends into 443 (4fc240, step 0xccc) and 4e13a0 loops it. Victim task
 * 4496d0: 11 ticks, blend into 389 (4fbfb8), 475690 copies the owner's
 * heading, blend into 204 (4fb70c), 4756f0 copies the owner's X/Z, then
 * 4e13b0 (clip 204, roster list 4e7b08) or 4e13c0 (clip 400). */
#define FIRST_OWNER_READY 1
#define FIRST_VICTIM_DONE 2

static const MoveDef n449620_first_owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n449620_first_owner_blend[] = {
  {{.clip = 384, .last = 249, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n449620_first_victory_blend[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0xccc},
};
static const SceneClip n449620_first_victim_stand[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n449620_first_victim_scene[] = {
  {{.clip = 204, .last = 385, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

/* 449b00/44a080: 476d80 hides victim node 5/6, 44a530/44a790 rebuild that
 * arm as a three-node object placed at the owner's origin and heading, and
 * 4e1400/4e1420 play clip 446 to 145 or 447 to 120 on it. */
static const AttachmentDef n449620_first_left_arm = {.clip = 446, .first = 0, .last = 171, .loop_from = 171, .scene_space = true, .limb = true};
static const AttachmentDef n449620_first_right_arm = {.clip = 447, .first = 0, .last = 120, .loop_from = 120, .scene_space = true, .limb = true};

enum {
  n449620_FIRST_SECOND_ARM = 129,
  n449620_FIRST_SLAM = 306,
  n449620_FIRST_DROP = 376,
  /* The first arm advances on its own task clock from victim pose 79;
   * 4e13e0 spends 49 poses before the native rows start at pose 129. */
  n449620_FIRST_ARM_OFFSET = 49,
  n449620_FIRST_ARM_IMPACT = 146,
  n449620_FIRST_ARM_END = 171
};

/* 45e1b0: the 45e570 close gate plus 240 ticks since the LK press. */
bool jax_can_fatality_1(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 240;
}

static bool n449620_first_listed_victim(const Fighter *victim) {
  static const int rosters[] = {4, 0, 12, 8, 7, 13, 14};
  for (size_t i = 0; i < sizeof rosters / sizeof rosters[0]; ++i) {
    if (victim->roster == rosters[i]) {
      return true;
    }
  }
  return false;
}

static void n449620_first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static void n449620_first_tear(Fighter *victim, int joint, const AttachmentDef *def) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  Vec3 shoulder;
  Mat3 unused;
  fighter_joint_position(victim, joint, &shoulder, &unused);
  battle_attach_piece_model(battle_detach_piece(victim, joint, (q16[3]){0}, (q16[3]){0}), def, owner);
  battle_blood_hit(owner, victim, shoulder, 0x3333);
}

static void n449620_first_owner_loop(Fighter *owner) {
  owner->state = S_VICTORY;
  owner->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter_owned(owner, 0x4e13a0, fighter_ready, 13);
  n449620_first_maybe_complete(owner);
}

/* 449620 -> 475430: blend into 443 before the 4e13a0 loop. */
void jax_fatality_1_victory(Fighter *owner) {
  native_scene_blends(owner, n449620_first_victory_blend, 1, n449620_first_owner_loop);
}

static void n449620_first_owner_start(Fighter *owner) {
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(owner, n449620_first_owner_blend, 1, NULL);
  native_script_end(owner, 0x4e1390, fighter_ready, 13);
}

static void n449620_first_victim_stream(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  if (n449620_first_listed_victim(victim)) {
    retail_vm_enter_owned(victim, 0x4e13b0, fighter_ready, 13);
  } else {
    retail_vm_enter_owned(victim, 0x4e13c0, fighter_ready, 13);
  }
}

static void n449620_first_victim_face(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  native_scene_blends(victim, n449620_first_victim_scene, 1, n449620_first_victim_stream);
}

static void n449620_first_victim_start(Fighter *victim) {
  native_scene_blends(victim, n449620_first_victim_stand, 1, n449620_first_victim_face);
}

/* 449890: cursor 50, B[0x29], 4e13d0 to 78. */
void jax_fatality_1_grab(Fighter *victim) {
  victim->sample_override = 50;
  battle_sound(SOUND_B, 0x29);
  retail_vm_enter_owned(victim, 0x4e13d0, fighter_ready, 13);
}

/* 4498d0: first arm (449b00) and blood fall (474580), B[0], the 489ff0
 * throw voice, cursor 79 and 4e13e0 to 128. */
void jax_fatality_1_first_arm(Fighter *victim) {
  n449620_first_tear(victim, 5, &n449620_first_left_arm);
  battle_sound(SOUND_B, 0);
  mk4_script_roster_sound(victim, 8);
  victim->sample_override = 79;
  retail_vm_enter_owned(victim, 0x4e13e0, fighter_ready, 13);
}

static void n449620_first_second_arm(Fighter *victim) {
  n449620_first_tear(victim, 6, &n449620_first_right_arm);
  battle_sound(SOUND_B, 0);
}

/* 449ca0/449cf0 on the first arm's own clock: B[0x17] and shake 7. */
static void n449620_first_arm_impact(Fighter *victim) {
  battle_sound(SOUND_B, 0x17);
  mk4_script_shake(victim, 7);
}

static void n449620_first_slam(Fighter *victim) {
  battle_sound(SOUND_B, 0x12);
  mk4_script_shake(victim, 5);
}

/* 472800: B[2 + random(2)], then shake 5 before 4e13f0. */
static void n449620_first_drop(Fighter *victim) {
  battle_sound(SOUND_B, 2 + (int)battle_random(2));
  mk4_script_shake(victim, 5);
}

/* 449940: cursor 129 and the second arm (44a080), 428d70 to 305; cursor
 * 306 with B[0x12] and shake 5, to 375; cursor 376, then 4e13f0. */
void jax_fatality_1_second_arm(Fighter *victim) {
  native_begin(victim, n449620_FIRST_SECOND_ARM, n449620_FIRST_DROP, NULL);
  native_script_end(victim, 0x4e13f0, fighter_ready, 13);
  for (int sample = n449620_FIRST_SECOND_ARM; sample <= n449620_FIRST_DROP; ++sample) {
    FighterFn action = NULL;
    int arm = sample - n449620_FIRST_SECOND_ARM + n449620_FIRST_ARM_OFFSET;
    if (sample == n449620_FIRST_SECOND_ARM) {
      action = n449620_first_second_arm;
    } else if (sample == n449620_FIRST_SLAM) {
      action = n449620_first_slam;
    } else if (sample == n449620_FIRST_DROP) {
      action = n449620_first_drop;
    } else if (arm == n449620_FIRST_ARM_IMPACT || arm == n449620_FIRST_ARM_END) {
      action = n449620_first_arm_impact;
    }
    native_frame(victim, sample, sample, action);
  }
  native_play(victim, false);
}

/* 4e13f0 finishes clip 204/400 and 49cb60 retires the task; the body stays. */
void jax_fatality_task_end(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  fighter_stop_mmove(victim);
  if (g_battle.finisher == FINISH_JAX_1) {
    owner->native_variant |= FIRST_VICTIM_DONE;
    n449620_first_maybe_complete(owner);
  }
}

void jax_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 4494c0 phase 0 leaves the owner's current pose untouched for 16 ticks. */
  fighter_start_move(fighter, S_PUNCH, &n449620_first_owner_wait);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x11999);
  }
  mk4_script_scene_pose(victim, 13, 0);
  native_hold(victim, 11, n449620_first_victim_start);
  native_hold(fighter, 16, n449620_first_owner_start);
}

#undef FIRST_OWNER_READY
#undef FIRST_VICTIM_DONE

/* Native service cluster: 44aeb0, 44af20, 44af30, 44b260, 44b290, 44b2c0, 44b600, 44b650, 49cb60. */
enum {
  n44aeb0_KAI_EVENT_FATALITY_1 = 23,
  n44aeb0_KAI_EVENT_FATALITY_2
};

/* First fatality: 467ce0 -> 43cbb0 -> roster table 4e0498[11] = 44ad70.
 * Owner clip 440 with spin clip 445 and victory loop 443; victim task
 * 44b0e0 blends into clip 441 with turn clip 444; 44b2c0 severs the
 * waist and 44b4a0 starts the independent torso on clip 442. */
#define FIRST_OWNER_READY 1
#define FIRST_VICTIM_DONE 2
#define FIRST_TORSO_DONE 4
static const MoveDef n44aeb0_first_torso = {.clip = 442, .last = 335, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 44ad70 holds the owner's current standing pose for sixteen visits. */
static const MoveDef n44aeb0_first_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n44aeb0_owner_blend[] = {
  {{.clip = 440, .last = 439, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n44aeb0_victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 441, .last = 401, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

bool kai_can_first_fatality(const Fighter *fighter) {
  /* 45e220: 45e570 range <= 1 and finish window, then mask 4e40b8:
   * BLK+HK held; HP, LP, LK and RUN rejected; Up is outside the mask. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && (fighter->held & mask) == chord;
}

static bool n44aeb0_female_victim(const Fighter *fighter) {
  /* 474e20 selects the second roster pose for assets 2 and 9. */
  int asset = roster_def(fighter->roster)->asset;
  return asset == 2 || asset == 9;
}

static Fighter *n44aeb0_first_scene_owner(Fighter *actor) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    if (&slot->actor == actor) {
      return &g_battle.fighters[slot->owner];
    }
  }
  require(false, "Kai torso has no scene owner");
  return NULL;
}

static void n44aeb0_first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE | FIRST_TORSO_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 428d70: advance one stored sample per visit to the target, then hold
 * one visit before the continuation. */
static void n44aeb0_first_play_to(Fighter *fighter, int target, FighterFn end) {
  int first = fighter_sample(fighter);
  require(target > first && target <= fighter_move(fighter)->last, "Invalid Kai first-fatality playback range");
  native_begin(fighter, 1, target - first + 1, end);
  for (int frame = 1; frame <= target - first; ++frame) {
    native_frame(fighter, frame, first + frame, NULL);
  }
  native_frame(fighter, target - first + 1, target, NULL);
  native_play(fighter, false);
}

static void n44aeb0_first_impact(int shake_row) {
  /* 472800: random B[1] or B[2]; 457b50 shake rows 5 {2,3,.18} and 7 {2,3,.06}. */
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  battle_camera_shake(2, 3, shake_row == 5 ? 0x2e14 : 0xf5c);
}

void kai_first_owner_grab(Fighter *fighter) {
  /* 44aeb0: victim voice list 2 (4727e0, no portable bank), B[31], B[2f],
   * then 4e1440 resumes clip 440 at the next stored sample. */
  battle_sound(SOUND_B, 0x31);
  battle_sound(SOUND_B, 0x2f);
  fighter->sample_override = fighter_sample(fighter) + 1;
  retail_vm_enter_owned(fighter, 0x4e1440, fighter_ready, 11);
}

void kai_first_owner_spin(Fighter *fighter) {
  /* 44af20 enters 4e1450, two passes of clip 445. */
  retail_vm_enter_owned(fighter, 0x4e1450, fighter_ready, 11);
}

static void n44aeb0_first_owner_victory(Fighter *fighter) {
  /* 44af5a: second impact, then 4e1468 finishes 440 and loops 443. */
  n44aeb0_first_impact(7);
  fighter->state = S_VICTORY;
  fighter->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter_owned(fighter, 0x4e1468, fighter_ready, 11);
  n44aeb0_first_maybe_complete(fighter);
}

static void n44aeb0_first_owner_pull(Fighter *fighter) {
  /* 44af9c: impact, a female victim rises .1 (474e20), then 365..393. */
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  n44aeb0_first_impact(5);
  if (n44aeb0_female_victim(victim)) {
    victim->body.pos[AXIS_Y] -= 0x1999;
  }
  fighter->sample_override = 365;
  n44aeb0_first_play_to(fighter, 393, n44aeb0_first_owner_victory);
}

void kai_first_owner_tear(Fighter *fighter) {
  /* 44b043: rebind clip 440 at 124 and play to 364. */
  mk4_mmove_bind(fighter, 440, 440, 124);
  n44aeb0_first_play_to(fighter, 364, n44aeb0_first_owner_pull);
}

void kai_first_victim_71(Fighter *victim) {
  /* 44b260 sets cursor 71, then 4e1490. */
  victim->sample_override = 71;
  retail_vm_enter_owned(victim, 0x4e1490, fighter_ready, 11);
}

void kai_first_victim_turn(Fighter *victim) {
  /* 44b290: B[29], then the second clip-444 pass 4e14a8. */
  battle_sound(SOUND_B, 0x29);
  retail_vm_enter_owned(victim, 0x4e14a8, fighter_ready, 11);
}

static void n44aeb0_first_torso_start(Fighter *torso) {
  /* 44b4a0: torso B[0] (its voice list 12 has no portable bank), then 4e14c8. */
  battle_sound(SOUND_B, 0);
  retail_vm_enter_owned(torso, 0x4e14c8, fighter_ready, 11);
}

static void n44aeb0_first_sever(Fighter *victim) {
  /* 476eb0(3) hides the victim waist subtree; 476120 copies the body,
   * 476d80 hides legs 11/12 on the copy; 44b680's blood stream is the
   * shared waist blood hit. The victim then finishes 441 through 4e14b8. */
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->hidden_joints[0] |= UINT64_C(1) << 3;
  Fighter *torso = battle_scene_actor_start(owner, victim, &n44aeb0_first_torso, n44aeb0_first_torso_start);
  require(torso != NULL, "Kai torso needs its bounded independent scene slot");
  torso->hidden_joints[0] = (UINT64_C(1) << 11) | (UINT64_C(1) << 12);
  Vec3 waist;
  Mat3 unused;
  fighter_joint_position(victim, 3, &waist, &unused);
  battle_blood_hit(owner, victim, waist, 0x3333);
  retail_vm_enter_owned(victim, 0x4e14b8, fighter_ready, 11);
}

static void n44aeb0_first_victim_rip(Fighter *victim) {
  /* 44b346: B[2b], 145..161. */
  battle_sound(SOUND_B, 0x2b);
  n44aeb0_first_play_to(victim, 161, n44aeb0_first_sever);
}

void kai_first_victim_sever(Fighter *victim) {
  /* 44b3dd: B[2a], rebind clip 441 at 124 and play to 145. */
  battle_sound(SOUND_B, 0x2a);
  mk4_mmove_bind(victim, 441, 402, 124);
  n44aeb0_first_play_to(victim, 145, n44aeb0_first_victim_rip);
}

void kai_first_torso_203(Fighter *torso) {
  /* 44b600: a female torso rises .1, cursor 203, then 4e14d8. */
  if (n44aeb0_female_victim(torso)) {
    torso->body.pos[AXIS_Y] -= 0x1999;
  }
  torso->sample_override = 203;
  retail_vm_enter_owned(torso, 0x4e14d8, fighter_ready, 11);
}

void kai_first_torso_impact(Fighter *torso) {
  /* 44b650: B[12], then 4e14e8 finishes clip 442. */
  battle_sound(SOUND_B, 0x12);
  retail_vm_enter_owned(torso, 0x4e14e8, fighter_ready, 11);
}

void kai_first_task_end(Fighter *actor) {
  /* 49cb60 retires the victim or torso task; its final pose remains. */
  bool torso = battle_is_scene_actor(actor);
  Fighter *owner = torso ? n44aeb0_first_scene_owner(actor) : (Fighter *)fighter_opponent(actor);
  fighter_stop_mmove(actor);
  owner->native_variant |= torso ? FIRST_TORSO_DONE : FIRST_VICTIM_DONE;
  n44aeb0_first_maybe_complete(owner);
}

static void n44aeb0_first_victim_start(Fighter *victim) {
  /* 4756f0 copies the owner's X/Z to the victim before 4e1480: clips 440
   * and 441 share one authored origin, so the pair is not separated. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  victim->flags |= FF_PAIRED;
  retail_vm_enter_owned(victim, 0x4e1480, fighter_ready, 11);
}

static void n44aeb0_first_victim_blend(Fighter *victim) {
  /* 44b0e0 after eleven visits: 475190 common blend, then 475340 into 441. */
  native_scene_blends(victim, n44aeb0_victim_blends, 2, n44aeb0_first_victim_start);
}

static void n44aeb0_first_owner_blend(Fighter *fighter) {
  /* 44adae after sixteen visits: 475250 blends into 440, then 4e1430. */
  native_scene_blends(fighter, n44aeb0_owner_blend, 1, NULL);
  native_script_end(fighter, 0x4e1430, fighter_ready, 11);
}

void kai_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n44aeb0_KAI_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int pose = fighter_sample(fighter);
  fighter_start_move(fighter, S_PUNCH, &n44aeb0_first_wait);
  fighter->sample_override = pose;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_variant = 0;
  mk4_script_scene_pose(victim, 13, 0);
  /* 44ad70 -> 475530 places the victim 1.15 units along the pair axis.
   * Camera tasks 44bad0/44bbf0 remain portable fight framing. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x12666);
  }
  native_hold(victim, 11, n44aeb0_first_victim_blend);
  native_hold(fighter, 16, n44aeb0_first_owner_blend);
}

#undef FIRST_OWNER_READY
#undef FIRST_VICTIM_DONE
#undef FIRST_TORSO_DONE

/* Native service cluster: native_fatality_first. */
bool liu_kang_can_first_fatality(const Fighter *fighter) {
  /* 45e410 / 4e4158: word A requires BLK+HK, word B requires LK;
   * reject Up/HP/LP/RUN. Directions other than Up are outside the mask. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_RUN);
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f && (fighter->held & mask) == chord;
}

void liu_kang_first_fatality(Fighter *fighter) {
  liu_kang_start_fatality(fighter, FINISH_LIU_KANG_1);
}

/* Native service cluster: 44be10, 44be40, 44be60, 44beb0, 44bf10. */
enum {
  n44be10_QUAN_CHI_FINISH_LEG = 31
};

#define QC_NATIVE_COUNT(array) ((int)(sizeof(array) / sizeof((array)[0])))

/* LK-release [F,D,F] record 4e4878 (predicate 45e2f0) enters 467ce0, whose
 * table 4e0498[8] selects 44bcd0: Quan Chi tears off the victim's leg and
 * beats the victim with it. Owner clip 420 then 421 loops; the victim
 * blends 389 -> 424 then loops 417; the leg runs 422 then loops 423. */
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n44be10_owner_blends[] = {
  {{.clip = 420, .last = 251, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n44be10_victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 424, .last = 251, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 48a910 camera tracks 4e1a80 (3 + 4 controls) and 4e1ad8 (4 + 4 controls),
 * both stepped at 0xccc; 44c7dc holds 45 ticks between them. Retail X,Z,Y
 * records are stored here as X,Y,Z. */
static const q16 n44be10_camera_side[][3] = {
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
};
static const q16 n44be10_camera_orbit[][3] = {
  {0x88000, -0x1b333, -0x80000},
  {0x60000, -0x18000, 0x60000},
  {0x48000, -0x14ccc, 0x40000},
  {-0x80000, -0x14ccc, 0x80000},
  {-0x80000, -0x14ccc, -0x80000},
  {0x80000, -0x14ccc, -0x80000},
  {0x80000, -0x14ccc, 0x80000},
  {-0x80000, -0x14ccc, 0x80000},
};
static const CameraShot n44be10_camera_shots[] = {
  {n44be10_camera_side, QC_NATIVE_COUNT(n44be10_camera_side), 45, 0xccc, 0, 0, 0},
  {n44be10_camera_orbit, QC_NATIVE_COUNT(n44be10_camera_orbit), 0, 0xccc, 0, 0, 0},
};
// clang-format on
/* The owner keeps his stance pose through 44bd9b's sixteen-tick setup. */
static const MoveDef n44be10_owner_hold = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n44be10_leg_move = {.clip = 422, .last = 134, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 44c550 -> 473ed0: stump blood from joint 12 for 0x8a ticks after ten. */
static const ParticleVisual n44be10_stump_blood_visual = {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE}};
static const ParticleSpray n44be10_stump_blood = {
    .visual = &n44be10_stump_blood_visual, .count = 1, .lifetime = 120, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase n44be10_stump_phases[] = {
  {10, 10, -1, 0},
  {138, 2, -1, 1},
};
// clang-format on
static const JointSprayDef n44be10_stump_spray = {.spray = &n44be10_stump_blood,
    .phases = n44be10_stump_phases,
    .count = QC_NATIVE_COUNT(n44be10_stump_phases),
    .origin_joint = 12,
    .base_joint = 3,
    .speed = 0x23d7,
    .variation = 0x7ae};

enum {
  /* Camera task: one tick, side track (5 segments), hold 45, orbit track
   * (6 segments); the beating loops continue until that program ends. */
  n44be10_SCENE_TICKS = 1 + 5 * 20 + 45 + 6 * 20,
};

/* 472800: hit sound B[1|2] and 457b50 shake. The 4727e0 victim voice lists
 * 5 and 3 are not reproduced; the portable roster voices cover lists 1/8. */
static void n44be10_beat_cue(Fighter *fighter, int shake) {
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  mk4_script_shake(fighter, shake);
}

void quan_chi_fatality_cue_40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1508, fighter_ready, 8);
}

void quan_chi_fatality_hit_179(Fighter *fighter) {
  n44be10_beat_cue(fighter, 5);
  retail_vm_enter_owned(fighter, 0x4e1528, fighter_ready, 8);
}

void quan_chi_fatality_hit_241(Fighter *fighter) {
  n44be10_beat_cue(fighter, 7);
  retail_vm_enter_owned(fighter, 0x4e1538, fighter_ready, 8);
}

void quan_chi_fatality_beat(Fighter *fighter) {
  n44be10_beat_cue(fighter, 7);
  retail_vm_enter_owned(fighter, 0x4e1558, fighter_ready, 8);
}

static void n44be10_leg_entry(Fighter *leg) {
  if (leg->assets != NULL) {
    leg->scene_model = &leg->assets->scene_models.data[SCENE_MODEL_SEVERED_LEGS + roster_def(leg->roster)->asset];
  }
  retail_vm_enter_owned(leg, 0x4e1580, fighter_ready, 8);
}

/* 44be40 -> task 44c0f0: sound B[0], victim voice list 8, 476d80 detaches
 * victim joint 12 into the copied leg (44ccf0) at the victim origin, whose
 * VM 4e1580 plays clip 422 and loops 423; 44c280/44c550 bleed. */
void quan_chi_fatality_leg(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_sound(SOUND_B, 0);
  mk4_script_roster_sound(victim, 8);
  victim->hidden_joints[0] |= UINT64_C(1) << 12;
  Fighter *leg = battle_scene_actor_start(fighter, victim, &n44be10_leg_move, n44be10_leg_entry);
  require(leg != NULL, "Quan Chi's torn leg needs its bounded scene slot");
  Vec3 hip;
  Mat3 unused;
  fighter_joint_position(victim, 12, &hip, &unused);
  battle_blood_hit(fighter, victim, hip, 0x3333);
  battle_joint_spray(victim, &n44be10_stump_spray);
  retail_vm_enter_owned(fighter, 0x4e1518, fighter_ready, 8);
}

static void n44be10_victim_stream(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4e1568, fighter_ready, 8);
}

static void n44be10_victim_blend(Fighter *victim) {
  native_scene_blends(victim, n44be10_victim_blends, QC_NATIVE_COUNT(n44be10_victim_blends), n44be10_victim_stream);
}

static void n44be10_owner_stream(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14f8, fighter_ready, 8);
}

static void n44be10_owner_blend(Fighter *fighter) {
  native_scene_blends(fighter, n44be10_owner_blends, QC_NATIVE_COUNT(n44be10_owner_blends), n44be10_owner_stream);
}

static void n44be10_scene_camera(Fighter *fighter) {
  battle_camera_sequence(fighter, fighter_opponent(fighter), n44be10_camera_shots, QC_NATIVE_COUNT(n44be10_camera_shots));
}

/* The native beating loops never end themselves. The portable result is
 * declared when the 44c700 camera program finishes; owner, victim and leg
 * keep looping under it, as the retail tasks do. */
static void n44be10_scene_complete(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

void quan_chi_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n44be10_QUAN_CHI_FINISH_LEG)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &n44be10_owner_hold);
  mk4_script_scene_pose(victim, 13, 0);
  /* 44bd9b -> 475530 aligns the scene pair at 1.1 units. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x11999);
  }
  native_hold(victim, 11, n44be10_victim_blend);
  native_hold(fighter, 16, n44be10_owner_blend);
  /* Camera task 44c700 and the scene clock run beside actor playback. */
  native_motion_begin(fighter, n44be10_SCENE_TICKS, n44be10_scene_complete);
  native_motion_frame(fighter, 1, NULL, 0, n44be10_scene_camera);
  native_motion_play(fighter);
}

bool quan_chi_can_first_fatality(const Fighter *fighter) {
  /* 45e2f0: 45e570's close gate, then LK held for at least 0xf0 ticks. */
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 0xf0;
}

#undef QC_NATIVE_COUNT

/* Native service cluster: 441140, 441180. */
static void n441140_first_end_shake(Fighter *fighter);
static void n441140_first_end_particles(Fighter *fighter);
static void n441140_first_end_finish(Fighter *fighter);

void raiden_script_first_release(Fighter *fighter) {
  mk4_script_scene_pose(fighter_peer(fighter), 393, 0);
  battle_particles(fighter_peer(fighter), 2, 12, true);
  retail_vm_enter_owned(fighter, 0x4e0220, fighter_ready, 1);
}

static void n441140_first_end_shake(Fighter *fighter) {
  mk4_script_shake(fighter, 8);
}

static void n441140_first_end_particles(Fighter *fighter) {
  battle_particles(fighter_peer(fighter), 2, 24, true);
}

static void n441140_first_end_finish(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  battle_end_finisher();
  raiden_script_victory(fighter);
}

void raiden_script_first_end(Fighter *fighter) {
  n441140_first_end_shake(fighter);
  native_begin(fighter, 1, 60, n441140_first_end_finish);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter),
        frame == 30                  ? n441140_first_end_shake
        : frame == 28 || frame == 58 ? n441140_first_end_particles
                                     : NULL);
  }
  native_play(fighter, false);
}

void raiden_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 392, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 393, 0);
  retail_vm_enter_owned(fighter, 0x4e0210, fighter_ready, 1);
}

/* Native service cluster: 4528a0, 452ac0, 452bb0, 452c10, 452c50, 452e50, 452e90, 452f00, 452f40, 452f90, 452fe0, 453020, 453040, 453090, 4530d0, 4530f0. */
enum {
  n4528a0_REIKO_FINISH_1 = 27,
  n4528a0_REIKO_FINISH_2
};

/* F,D,F+BLK 467ce0 -> first-fatality table 4e0498[7] = 452730: Reiko and
 * the dizzy victim share one origin. Owner clip 425 tears the victim's
 * clip 428 apart at pose 37 (452ac0). Torso (joint 3), head (2) and both
 * arms (5, 6) then continue as detached rig subtrees with their own local
 * clips 429/430/427/426 (407330), while the victim keeps its pelvis and
 * legs. The owner ends in the 443 victory loop. */
#define FIRST_OWNER_DONE 1
#define FIRST_VICTIM_DONE 2
#define FIRST_PIECES_DONE (4 | 8 | 16 | 32)

static const MoveDef n4528a0_first_owner = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n4528a0_owner_intro[] = {
  {{.clip = 425, .last = 93, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n4528a0_owner_victory[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
static const SceneClip n4528a0_victim_reach_clip[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n4528a0_victim_body[] = {
  {{.clip = 428, .last = 193, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 4e3638 camera shake rows used by this scene: pulses, half cycle, Y. */
static const int n4528a0_shake_rows[][3] = {
  [2] = {2, 2, 0x1999},
  [6] = {3, 3, 0x3333},
  [7] = {2, 3, 0x0f5c},
};
// clang-format on
static const MoveDef n4528a0_torso_piece = {.clip = 429, .last = 50, .action = 0x700};
static const MoveDef n4528a0_head_piece = {.clip = 430, .last = 124, .action = 0x700};
static const MoveDef n4528a0_left_arm_piece = {.clip = 427, .last = 69, .action = 0x700};
static const MoveDef n4528a0_right_arm_piece = {.clip = 426, .last = 88, .action = 0x700};

static void n4528a0_shake(int row) {
  battle_camera_shake(n4528a0_shake_rows[row][0], n4528a0_shake_rows[row][1], n4528a0_shake_rows[row][2]);
}

/* 45e360: the 45e570 close-finish gate and held switches 4e4118: BLK, HK,
 * LP and LK down; Up, HP and RUN released. */
bool reiko_can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_RUN);
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && (fighter->held & mask) == chord;
}

static Fighter *n4528a0_first_owner_of(Fighter *actor) {
  if (!battle_is_scene_actor(actor)) {
    return (Fighter *)fighter_opponent(actor);
  }
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (&g_battle.scene_actors[i].actor == actor) {
      return &g_battle.fighters[g_battle.scene_actors[i].owner];
    }
  }
  require(false, "Reiko scene piece has no owner");
  return NULL;
}

static void n4528a0_first_done(Fighter *owner, int flag) {
  owner->native_variant |= flag;
  if (owner->native_variant == (FIRST_OWNER_DONE | FIRST_VICTIM_DONE | FIRST_PIECES_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static void n4528a0_random_hit_sound(void) {
  /* 472800: B[1 + random(2)]. */
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
}

static void n4528a0_random_piece_sound(void) {
  /* 453170: B[0x3b + random(4)]. */
  battle_sound(SOUND_B, 0x3b + (int)battle_random(4));
}

static void n4528a0_owner_victory_loop(Fighter *fighter) {
  fighter->state = S_VICTORY;
  retail_vm_enter_owned(fighter, 0x4e2d18, fighter_ready, 7);
  n4528a0_first_done(fighter, FIRST_OWNER_DONE);
}

/* 4528a0: 475430 blends into victory clip 443 at step 0xccc, then 4e2d18. */
void reiko_first_victory(Fighter *fighter) {
  native_scene_blends(fighter, n4528a0_owner_victory, 1, n4528a0_owner_victory_loop);
}

static void n4528a0_owner_tear(Fighter *fighter) {
  /* 452730 state 3 enters 4e2cf0 after 43d7d0 and twenty visits. */
  native_hold(fighter, 20, NULL);
  native_script_end(fighter, 0x4e2cf0, fighter_ready, 7);
}

static void n4528a0_owner_blend(Fighter *fighter) {
  /* 452730 state 1: 475250 blends into 425. 43d720/43d7d0 swap stage
   * objects 0x45/0x7a; the portable arena keeps its objects. */
  native_scene_blends(fighter, n4528a0_owner_intro, 1, n4528a0_owner_tear);
}

static void n4528a0_victim_place(Fighter *victim) {
  /* 452950 state 3: 4756f0 puts the victim at the owner's X/Z. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  retail_vm_enter_owned(victim, 0x4e2d28, fighter_ready, 7);
}

static void n4528a0_victim_body_blend(Fighter *victim) {
  /* 475190 -> 475690 copies the owner's facing and angles; 475340 blends
   * into 428 while 475510 keeps the body on the floor. */
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  native_scene_blends(victim, n4528a0_victim_body, 1, n4528a0_victim_place);
}

static void n4528a0_victim_reach(Fighter *victim) {
  native_scene_blends(victim, n4528a0_victim_reach_clip, 1, n4528a0_victim_body_blend);
}

static void n4528a0_piece_place(Fighter *piece, int joint) {
  /* 453740 copies the victim's X/Z, angles and facing; Y is -0x12c49. */
  piece->subtree_joint = joint + 1;
  piece->body.pos[AXIS_Y] = -0x12c49;
  piece->sample_override = 0;
}

static void n4528a0_torso_start(Fighter *piece) {
  /* 4534e0: joint 3 without the separately detached head and arms. */
  n4528a0_piece_place(piece, 3);
  piece->hidden_joints[0] |= (UINT64_C(1) << 2) | (UINT64_C(1) << 5) | (UINT64_C(1) << 6);
  retail_vm_enter_owned(piece, 0x4e2d68, fighter_ready, 7);
}

static void n4528a0_head_start(Fighter *piece) {
  n4528a0_piece_place(piece, 2);
  retail_vm_enter_owned(piece, 0x4e2d88, fighter_ready, 7);
}

static void n4528a0_left_arm_start(Fighter *piece) {
  n4528a0_piece_place(piece, 5);
  retail_vm_enter_owned(piece, 0x4e2de0, fighter_ready, 7);
}

static void n4528a0_right_arm_start(Fighter *piece) {
  n4528a0_piece_place(piece, 6);
  retail_vm_enter_owned(piece, 0x4e2e18, fighter_ready, 7);
}

/* The independent 452c90 sound task plays only while this scene runs. */
static bool n4528a0_tear_sounds(BattleEvent *event, Fighter *owner, Fighter *victim) {
  static const int sounds[] = {0x38, 0x34, 0x39, 0x34, 0x34};
  battle_sound(SOUND_B, sounds[event->frame_value]);
  return g_battle.phase == PHASE_FATALITY && fighter_opponent(owner) == victim;
}

static bool n4528a0_tear_sequence_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* The program has played its last row; 452de5 retires the task. */
  return event->frame_count > 0 && fighter_opponent(owner) == victim;
}

/* 452ac0 at victim pose 37: four detached subtree tasks, the 452c90 sound
 * sequence (B 0x38, 0x34, 0x39, 0x34, 0x34 at 0/4/15/19/34), B[0x18] and
 * B[0x3a], shake row 7, and 476eb0 hides joint 3's subtree on the body. */
void reiko_first_tear(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  static const MoveDef *const moves[] = {&n4528a0_torso_piece, &n4528a0_head_piece, &n4528a0_left_arm_piece, &n4528a0_right_arm_piece};
  static const FighterFn starts[] = {n4528a0_torso_start, n4528a0_head_start, n4528a0_left_arm_start, n4528a0_right_arm_start};
  for (int i = 0; i < 4; ++i) {
    Fighter *piece = battle_scene_actor_start(owner, victim, moves[i], starts[i]);
    require(piece != NULL, "Reiko's first fatality needs its four bounded scene pieces");
  }
  BattleEvent *sounds = battle_event_start(owner, n4528a0_tear_sequence_done, false, false);
  if (sounds != NULL) {
    static const int at[] = {0, 4, 15, 19, 34};
    battle_event_program(sounds, 35);
    for (int i = 0; i < 5; ++i) {
      battle_event_frame(sounds, at[i], i, n4528a0_tear_sounds);
    }
  }
  battle_sound(SOUND_B, 0x18);
  battle_sound(SOUND_B, 0x3a);
  n4528a0_shake(7);
  Vec3 waist;
  Mat3 unused;
  fighter_joint_position(victim, 3, &waist, &unused);
  battle_blood_hit(owner, victim, waist, 0x3333);
  victim->hidden_joints[0] |= UINT64_C(1) << 3;
  retail_vm_enter_owned(victim, 0x4e2d38, fighter_ready, 7);
}

void reiko_first_victim_118(Fighter *victim) {
  /* 452bb0: B[0x40], shake row 6, pose 118, then 4e2d48 to 165. */
  battle_sound(SOUND_B, 0x40);
  n4528a0_shake(6);
  victim->sample_override = 118;
  retail_vm_enter_owned(victim, 0x4e2d48, fighter_ready, 7);
}

void reiko_first_victim_165(Fighter *victim) {
  /* 452c10: random hit sound, shake row 2, then 4e2d58 finishes 428. */
  n4528a0_random_hit_sound();
  n4528a0_shake(2);
  retail_vm_enter_owned(victim, 0x4e2d58, fighter_ready, 7);
}

/* 452c50 / 452e90 / 452f90 / 453040 / 4530f0: a floor pool (475730 /
 * 475930) and task_finish; the final pose stays in the scene. */
void reiko_first_piece_end(Fighter *actor) {
  Fighter *owner = n4528a0_first_owner_of(actor);
  fighter_stop_mmove(actor);
  int flag = FIRST_VICTIM_DONE;
  if (battle_is_scene_actor(actor)) {
    int joint = actor->subtree_joint - 1;
    flag = joint == 3 ? 4 : joint == 2 ? 8 : joint == 5 ? 16 : 32;
  }
  n4528a0_first_done(owner, flag);
}

void reiko_first_torso_29(Fighter *piece) {
  /* 452e50: random hit sound, shake row 7, then 4e2d78 finishes 429. */
  n4528a0_random_hit_sound();
  n4528a0_shake(7);
  retail_vm_enter_owned(piece, 0x4e2d78, fighter_ready, 7);
}

void reiko_first_torso_end(Fighter *piece) {
  /* 452e90: random hit sound and shake row 7 before the shared end. */
  n4528a0_random_hit_sound();
  n4528a0_shake(7);
  reiko_first_piece_end(piece);
}

void reiko_first_head_59(Fighter *piece) {
  /* 452f00: B[0x3f], pose 59, then 4e2dc0 to 82. */
  battle_sound(SOUND_B, 0x3f);
  piece->sample_override = 59;
  retail_vm_enter_owned(piece, 0x4e2dc0, fighter_ready, 7);
}

void reiko_first_head_83(Fighter *piece) {
  /* 452f40: two random piece sounds around pose 83, then 4e2dd0. */
  n4528a0_random_piece_sound();
  piece->sample_override = 83;
  n4528a0_random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2dd0, fighter_ready, 7);
}

void reiko_first_left_arm_56(Fighter *piece) {
  /* 452fe0: random piece sound, pose 56, then 4e2df8 to 66. */
  n4528a0_random_piece_sound();
  piece->sample_override = 56;
  retail_vm_enter_owned(piece, 0x4e2df8, fighter_ready, 7);
}

void reiko_first_left_arm_land(Fighter *piece) {
  /* 453020: random piece sound, then 4e2e08 finishes 427. */
  n4528a0_random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2e08, fighter_ready, 7);
}

void reiko_first_right_arm_44(Fighter *piece) {
  /* 453090: random piece sound, pose 44, then 4e2e40 to 68. */
  n4528a0_random_piece_sound();
  piece->sample_override = 44;
  retail_vm_enter_owned(piece, 0x4e2e40, fighter_ready, 7);
}

void reiko_first_right_arm_land(Fighter *piece) {
  /* 4530d0: random piece sound, then 4e2e50 finishes 426. */
  n4528a0_random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2e50, fighter_ready, 7);
}

void reiko_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n4528a0_REIKO_FINISH_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 467ce0 stops the owner in its stance clip (4611c0 -> 284); the peer
   * keeps the dizzy finish pose. 475530 aligns the victim 1.3 units ahead. */
  fighter_start_move(fighter, S_PUNCH, &n4528a0_first_owner);
  fighter->sample_override = 0;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  mk4_script_scene_pose(victim, 13, 0);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14ccc);
  }
  /* 452730 state 0 sleeps sixteen visits; 452950 sleeps sixty-three. */
  native_hold(fighter, 16, n4528a0_owner_blend);
  native_hold(victim, 63, n4528a0_victim_reach);
}

#undef FIRST_OWNER_DONE
#undef FIRST_VICTIM_DONE
#undef FIRST_PIECES_DONE

/* Native service cluster: 455110, 455140, 4558b0, 4558d0, 455ae0, 455b60, 455b80, 455ba0, 49cb60. */
enum {
  n455110_SHINNOK_FINISH_1 = 21,
  n455110_SHINNOK_FINISH_2
};

enum {
  n455110_SHINNOK_FATALITY_OWNER_READY = 1,
  n455110_SHINNOK_FATALITY_VICTIM_DONE = 2,
  n455110_SHINNOK_FATALITY_HAND_DONE = 4,
  n455110_SHINNOK_FATALITY_DONE = 7
};

/* Fatality I: 467ce0 -> 43cbb0 -> 4e0498[6] = 455000. Shinnok vanishes
 * through clip 12 while a giant hand (rig 5083c4, nf_fat_g.geo) rises in
 * front of him, squeezes the victim and drops it; he then leaps back in. */
static const MoveDef n455110_grab_hand_pose = {.clip = 434, .last = 379, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 4558f0 states 2/3: 475190 blends descriptor 4fbfb8 (clip 389), then
 * 475340 blends 4fc1e0 (clip 435), both with 475250's .2 start and step. */
static const SceneClip n455110_victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 435, .last = 280, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

typedef struct SoundCue {
  int selector, wait;
} SoundCue;

// clang-format off: Keep table entries on one row, including long rows.
/* 456990: table 4e3208 (B selector, wait) pairs until zero, then 455bf0,
 * B 0x46 and the 5/3/4-tick tail states. */
static const SoundCue n455110_victim_cues[] = {
  {0x29, 10}, {0x2a, 20}, {0x1b, 10}, {0x33, 10}, {0x1c, 20}, {0x2b, 3}, {0x2a, 4}, {0x2b, 3}, {0x29, 4}, {0x2a, 3}, {0x29, 4},
  {0x2b, 4}, {0x29, 4}, {0x29, 4}, {0x2a, 3}, {0x29, 4}, {0x2b, 3}, {0x2a, 4}, {0x2b, 3}, {0x2a, 4}, {0x1b, 6}, {0x33, 13},
  {0x46, 5}, {0x2b, 3}, {0x2a, 4}, {0x2b, 0},
};
/* 4564b0 -> 456630: the hand's B 0x49, 0x4a, 0x4a and 0x4b cues. */
static const SoundCue n455110_hand_cues[] = {
  {0x49, 125}, {0x4a, 189}, {0x4a, 189}, {0x4b, 0},
};
// clang-format on

static bool n455110_play_cues(BattleEvent *event, const SoundCue *cues) {
  const SoundCue *cue = &cues[event->phase++];
  battle_sound(SOUND_B, cue->selector);
  if (cue->wait == 0) {
    return false;
  }
  battle_event_delay(event, cue->wait);
  return true;
}

static bool n455110_victim_cue(BattleEvent *event, Fighter *victim, Fighter *owner) {
  if (n455110_victim_cues[event->phase].selector == 0x46) {
    /* 455bf0 launches a type 0x92 piece from the victim's head joint. */
    Vec3 head;
    Mat3 unused;
    fighter_joint_position(victim, 2, &head, &unused);
    battle_blood_hit(owner, victim, head, 0x4000);
  }
  return n455110_play_cues(event, n455110_victim_cues);
}

static bool n455110_hand_cue(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)owner;
  (void)victim;
  return n455110_play_cues(event, n455110_hand_cues);
}

static void n455110_start_cues(Fighter *owner, BattleEventFn step) {
  BattleEvent *event = battle_event_start(owner, step, false, false);
  require(event != NULL, "Shinnok fatality needs a bounded cue sequence");
  event->phase = 0;
  event->due_tick = g_battle.ticks;
}

static void n455110_victim_sounds(Fighter *victim) {
  /* 456990 first queues roster voice list 0xb (4727e0); the portable bank
   * resolves only voice lists 1 and 8, so the sampled B cues follow. */
  n455110_start_cues(victim, n455110_victim_cue);
}

static Fighter *n455110_hand_owner(Fighter *hand) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (&g_battle.scene_actors[i].actor == hand) {
      return &g_battle.fighters[g_battle.scene_actors[i].owner];
    }
  }
  require(false, "Shinnok hand has no scene owner");
  return NULL;
}

static void n455110_hand_entry(Fighter *hand) {
  if (hand->assets != NULL) {
    hand->scene_model = &hand->assets->shinnok_hand;
  }
  retail_vm_enter_owned(hand, 0x4e2f40, fighter_ready, 6);
}

/* 4556d0 state 1: rig 5083c4 is placed 1.4 along the owner's heading at
 * his floor height, with his yaw and facing; 4e2f40 animates it. */
static bool n455110_hand_spawn(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)victim;
  Fighter *hand = battle_scene_actor_start(owner, owner, &n455110_grab_hand_pose, n455110_hand_entry);
  require(hand != NULL, "Shinnok hand needs its bounded scene slot");
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    hand->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x16666);
  }
  hand->body.pos[AXIS_Y] = owner->body.ground;
  n455110_start_cues(owner, n455110_hand_cue);
  return false;
}

static void n455110_first_vanish(Fighter *fighter) {
  /* 455000 state 1 after 16 ticks: 4556d0 sleeps 65, then the hand. */
  BattleEvent *event = battle_event_start(fighter, n455110_hand_spawn, false, false);
  require(event != NULL, "Shinnok hand needs its delayed spawner");
  battle_event_delay(event, 65);
  shinnok_fatality_vanish_cue(fighter);
  retail_vm_enter_owned(fighter, 0x4e2ef0, fighter_ready, 6);
}

static void n455110_victim_lift(Fighter *victim) {
  /* 4558f0 state 4 rechecks the 1.4 placement before 4e2f78. */
  const Fighter *owner = fighter_opponent(victim);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x16666);
  }
  retail_vm_enter_owned(victim, 0x4e2f78, fighter_ready, 6);
}

static void n455110_victim_blend(Fighter *victim) {
  native_scene_blends(victim, n455110_victim_blends, 2, n455110_victim_lift);
}

void shinnok_first_vanished(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  retail_vm_enter_owned(fighter, 0x4e2f00, fighter_ready, 6);
}

void shinnok_first_hidden(Fighter *fighter) {
  native_hold(fighter, 0x235, shinnok_fatality_return);
}

void shinnok_first_hand_squeeze(Fighter *hand) {
  retail_vm_repeat(hand, 0x4e2f58, 23, 0x4e2f68);
}

void shinnok_first_squeeze_cycle(Fighter *fighter) {
  /* 4558d0/455b80 decrement their native counter and re-enter retail. */
  retail_vm_repeat_next(fighter);
}

void shinnok_first_task_end(Fighter *actor) {
  /* 49cb60 retires the task; the body keeps its final rendered pose. */
  fighter_stop_mmove(actor);
  bool hand = battle_is_scene_actor(actor);
  Fighter *owner = hand ? n455110_hand_owner(actor) : (Fighter *)fighter_opponent(actor);
  owner->native_variant |= hand ? n455110_SHINNOK_FATALITY_HAND_DONE : n455110_SHINNOK_FATALITY_VICTIM_DONE;
  shinnok_fatality_maybe_complete(owner);
}

void shinnok_first_victim_held(Fighter *victim) {
  /* 455ae0 marks the group 0x1000 and 4773a0 detaches it from its player
   * slot; the portable victim simply remains scene-owned. */
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(victim, 0x4e2f88, fighter_ready, 6);
}

void shinnok_first_victim_squeeze(Fighter *victim) {
  retail_vm_repeat(victim, 0x4e2f98, 23, 0x4e2fa8);
}

void shinnok_first_victim_crush(Fighter *victim) {
  /* 455ba0: cursor 139, a random grunt (472800: B 1 or 2) and pulse 5. */
  victim->sample_override = 0x8b;
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  mk4_script_shake(victim, 5);
  Vec3 chest;
  Mat3 unused;
  fighter_joint_position(victim, 1, &chest, &unused);
  battle_blood_hit(fighter_opponent(victim), victim, chest, 0x8000);
  retail_vm_enter_owned(victim, 0x4e2fb8, fighter_ready, 6);
}

void shinnok_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n455110_SHINNOK_FINISH_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  shinnok_fatality_begin(fighter, n455110_SHINNOK_FATALITY_DONE);
  /* 475530 aligns the pair 1.4 units apart (53c444 = 0x16666). */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x16666);
  }
  /* 4558f0: sleep 71, start 456990, sleep 100, then the two blends. */
  native_begin(victim, 0, 170, n455110_victim_blend);
  for (int frame = 0; frame <= 170; ++frame) {
    native_frame(victim, frame, 0, frame == 70 ? n455110_victim_sounds : NULL);
  }
  native_play(victim, false);
  native_hold(fighter, 16, n455110_first_vanish);
}

/* Native service cluster: 43f510. */
void sonya_script_first_release(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  fighter->body.physics = sonya_scene_end_watch;
  retail_vm_enter_owned(fighter, 0x4e01d0, fighter_ready, 2);
}

void sonya_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 389, 0);
  retail_vm_enter_owned(fighter, 0x4e01c0, fighter_ready, 2);
}

bool sonya_can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

/* Native service cluster: 445150, 445220, 445240, 445280, 4452b0, 4452e0, 445310, 445510, 4456c0, 445700. */
#define BLEND(c, sample, step, first)                                                                                                                          \
  {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, sample, sample, 1, step, first}
static void n445150_first_camera_start(Fighter *fighter);
static void n445150_first_victim_start(Fighter *victim);
static const SceneClip n445150_first_actor_blend[] = {BLEND(385, 0, 0x32f1, 0x3333)};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n445150_first_victim_setup[] = {
  BLEND(389, 0, 0x32f1, 0x3333),
  BLEND(387, 0, 0x32f1, 0x3333),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n445150_first_victim_blends[] = {
  BLEND(387, 46, 0x666, 0x666),
  BLEND(387, 49, 0x43f, 0x43f),
  BLEND(387, 50, 0x43f, 0x886),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const q16 n445150_first_camera[][3] = {
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const q16 n445150_orbit_camera[][3] = {
  {-327680, -163840, -458752},
  {-393216, -183500, -393216},
  {-393216, -255590, 393216},
  {393216, -216268, 393216},
  {393216, -190054, -393216},
  {-458752, -170393, -458752},
  {-524288, -157286, 524288},
  {589824, -144179, 589824},
  {655360, -117964, -655360},
  {-655360, -104857, -655360},
  {-655360, -104857, 655360},
  {655360, -104857, 655360},
  {655360, -117964, -655360},
  {-655360, -131072, -655360},
  {-655360, -144179, 655360},
  {655360, -150732, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const CameraShot n445150_first_camera_shots[] = {
  {n445150_first_camera, NATIVE_COUNT(n445150_first_camera), 50, 0xccc, 0, 0, 0},
  {n445150_orbit_camera, NATIVE_COUNT(n445150_orbit_camera), 0, 0x45a, 3, 80, -0x4000},
};
// clang-format on
static const AttachmentDef n445150_first_spine = {.model = SCENE_MODEL_SPINE,
    .clip = 386,
    .first = 0,
    .last = 96,
    .loop_from = 52,
    .group_offset = {0, -0xccc / 65536.f, 0},
    .piece_offset = {0, 0x2b43 / 65536.f, 0},
    .piece_kind = 0,
    .scene_space = true};
static const ParticleVisual n445150_neck_blood_visual = {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE}};
static const ParticleSpray n445150_neck_blood = {
    .visual = &n445150_neck_blood_visual, .count = 1, .lifetime = 180, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase n445150_neck_blood_phases[] = {
  {11, 1, -1, 2},
  {11, 1, -1, 1},
  {39, 3, 2, 1},
  {142, 2, -1, 1},
};
// clang-format on
static const JointSprayDef n445150_first_blood = {.spray = &n445150_neck_blood,
    .phases = n445150_neck_blood_phases,
    .count = NATIVE_COUNT(n445150_neck_blood_phases),
    .origin_joint = 2,
    .base_joint = 3,
    .speed = 0x23d7,
    .variation = 0x7ae};

/* 45e460 -> mask 4e4178: HP + BLOCK + RUN; no Up/HK/LP/LK. */
bool sub_zero_can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  return sub_zero_can_close_fatality(fighter) && (fighter->held & mask) == chord;
}

static void n445150_first_camera_start(Fighter *fighter) {
  battle_camera_sequence(fighter, fighter_peer(fighter), n445150_first_camera_shots, NATIVE_COUNT(n445150_first_camera_shots));
  retail_vm_enter_owned(fighter, 0x4e0368, fighter_ready, 4);
}

void sub_zero_first_fatality_script(Fighter *fighter) {
  native_hold(fighter, maxi(1, 8 - g_battle.phase_ticks), n445150_first_camera_start);
}

static void n445150_first_victim_start(Fighter *victim) {
  sub_zero_native_blends(victim, n445150_first_victim_setup, NATIVE_COUNT(n445150_first_victim_setup), NULL);
  native_script_end(victim, 0x4e03f0, fighter_ready, 4);
}

void sub_zero_script_first_begin(Fighter *fighter) {
  fighter->script_phase = 1;
  sub_zero_native_blends(fighter, n445150_first_actor_blend, NATIVE_COUNT(n445150_first_actor_blend), NULL);
  native_script_end(fighter, 0x4e0378, fighter_ready, 4);
}

void sub_zero_script_first_pull(Fighter *fighter) {
  n445150_first_victim_start(fighter_peer(fighter));
  retail_vm_enter_owned(fighter, 0x4e0388, fighter_ready, 4);
}

void sub_zero_script_first_cues1(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  battle_sound(SOUND_B, 0x29);
  retail_vm_enter_owned(fighter, 0x4e0398, fighter_ready, 4);
}

void sub_zero_script_first_cues2(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2b);
  retail_vm_enter_owned(fighter, 0x4e03a8, fighter_ready, 4);
}

void sub_zero_script_first_cues3(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2a);
  retail_vm_enter_owned(fighter, 0x4e03b8, fighter_ready, 4);
}

void sub_zero_script_first_cues4(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  retail_vm_enter_owned(fighter, 0x4e03c8, fighter_ready, 4);
}

void sub_zero_script_first_head(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  battle_sound(SOUND_B, 0x1c);
  battle_sound(SOUND_B, 0);
  ScenePiece *head = battle_attach_head(victim, fighter, 10);
  battle_attach_piece_model(head, &n445150_first_spine, fighter);
  battle_joint_spray(victim, &n445150_first_blood);
  retail_vm_enter_owned(fighter, 0x4e03d8, fighter_ready, 4);
}

void sub_zero_script_first_victim_blends(Fighter *victim) {
  sub_zero_native_blends(victim, n445150_first_victim_blends, NATIVE_COUNT(n445150_first_victim_blends), NULL);
  native_script_end(victim, 0x4e0420, fighter_ready, 4);
}

void sub_zero_script_first_victim_collapse(Fighter *victim) {
  battle_sound(SOUND_B, 0x18);
  battle_camera_shake(2, 3, 0x2e14);
  retail_vm_enter_owned(victim, 0x4e0430, fighter_ready, 4);
}

void sub_zero_script_first_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, sub_zero_finish_scene);
}

void sub_zero_first_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SUB_ZERO_1);
}

#undef BLEND

/* Native service cluster: 44d4a0, 44d4e0, 44d510, 44d530, 44d6e0, 44d750, 44d7a0. */
enum {
  n44d4a0_TANYA_EVENT_FATALITY_1 = 35,
  n44d4a0_TANYA_EVENT_FATALITY_2
};

/* BLK D,D,U,D record 4e4520 -> 467ce0 -> 43cbb0 -> 4e0498[9] = 44d340. */
static const MoveDef n44d4a0_owner_pose = {.clip = 439, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 475250/475190/475340 blend at step 0x32f1 from weight 0x3333. */
static const SceneClip n44d4a0_owner_kiss[] = {
  {{.clip = 395, .last = 140, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n44d4a0_victim_turn[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n44d4a0_victim_swell[] = {
  {{.clip = 397, .last = 195, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

/* 45e2a0: the close gate 45e570 plus held mask 4e40f8: HP + BLOCK, with
 * no Up/HK/LP/LK/RUN. */
bool tanya_can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return retail_can_finish_close(fighter) && (fighter->held & mask) == chord;
}

/* 44d8d0 jump table 44dc80: per state its B sounds, 489ff0 roster voice,
 * camera shake row 7 and the following sleep. Only voice list 8 has a
 * portable bank (SOUND_THROW_VOICE); lists 1/2/3 stay silent. */
typedef struct VictimCue {
  int sounds[2], voice;
  bool shake;
  int sleep;
} VictimCue;

// clang-format off: Keep table entries on one row, including long rows.
static const VictimCue n44d4a0_victim_cues[] = {
  {{0, 0}, 8, false, 3},
  {{0x1c, 0}, 0, true, 33},
  {{0x32, 0}, 2, true, 38},
  {{0x33, 0}, 3, true, 38},
  {{0x1c, 0x2b}, 1, true, 30},
  {{0x32, 0}, 8, true, 29},
  {{0x33, 0}, 3, false, 30},
  {{0x29, 0}, 0, false, 25},
  {{0x2a, 0}, 0, false, 0},
};
// clang-format on

static bool n44d4a0_cue_next(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  int index = event->data[1]++;
  if (index >= (int)(sizeof n44d4a0_victim_cues / sizeof n44d4a0_victim_cues[0])) {
    return false;
  }
  const VictimCue *cue = &n44d4a0_victim_cues[index];
  for (int i = 0; i < 2; ++i) {
    if (cue->sounds[i] != 0) {
      battle_sound(SOUND_B, cue->sounds[i]);
    }
  }
  if (cue->voice == 8) {
    mk4_script_roster_sound(owner, 8);
  }
  if (cue->shake) {
    mk4_script_shake(owner, 7);
  }
  if (cue->sleep == 0) {
    return false;
  }
  battle_event_program(event, cue->sleep);
  return true;
}

/* 44d7c0: after 25 visits the victim's Y oscillates by .07 as its phase
 * advances 0xccc per visit. */
static bool n44d4a0_bob_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  event->data[1] += 0xccc;
  float phase = q16_float(event->data[1]);
  owner->body.pos[AXIS_Y] = event->data[0] + (q16)(-0x11eb * sinf(phase));
  return true;
}

static bool n44d4a0_bob_begin(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  event->data[0] = owner->body.pos[AXIS_Y];
  event->data[1] = 0;
  event->step = n44d4a0_bob_step;
  return true;
}

static void n44d4a0_first_end(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

/* 44dd90's six tasks: shake row 8 (44e090), B 0x15/0x1c (44e160) and the
 * 4e1cd0 joint list {2, .1}, {9, .08}, {10, .08} (44eb00/407030), which
 * leaves the swollen body. Their particle and mesh tasks (472fb0, 474580,
 * 4b8140) use the portable blood spray. */
static bool n44d4a0_burst_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (event->phase == 0) {
    event->phase = 1;
    mk4_script_shake(owner, 8);
    battle_sound(SOUND_B, 0x15);
    static const int joints[][2] = {{2, 0x1999}, {9, 0x147a}, {10, 0x147a}};
    const Fighter *peer = fighter_opponent(owner);
    Vec3 neck;
    Mat3 unused;
    fighter_joint_position(owner, 2, &neck, &unused);
    for (size_t i = 0; i < sizeof joints / sizeof joints[0]; ++i) {
      q16 speed = joints[i][1];
      q16 velocity[3] = {q16_mul(fighter_axis(peer, AXIS_X), speed), -speed, q16_mul(fighter_axis(peer, AXIS_Z), speed)};
      ScenePiece *piece = battle_detach_piece(owner, joints[i][0], velocity, (q16[3]){0});
      if (piece != NULL) {
        piece->gravity = 0x7ae;
      }
    }
    battle_blood_hit((Fighter *)peer, owner, neck, 0x3333);
    battle_event_program(event, 15);
    return true;
  }
  if (event->phase == 1) {
    event->phase = 2;
    battle_sound(SOUND_B, 0x1c);
    battle_event_program(event, 105);
    return true;
  }
  n44d4a0_first_end(owner);
  return false;
}

/* 44d7a0: 44dd90 waits 88 visits before the burst; 4e16d0 keeps shaking. */
void tanya_first_victim_burst(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, n44d4a0_burst_step, false, false);
  battle_event_program(event, 88);
  retail_vm_enter_owned(fighter, 0x4e16d0, fighter_ready, 9);
}

/* 44d750: one more 4e15f0 pass, then cue task 44d8d0 and clip 409. */
void tanya_first_victim_repeat(Fighter *fighter) {
  if (fighter->native_variant-- > 0) {
    retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
    return;
  }
  BattleEvent *event = battle_event_start(fighter, n44d4a0_cue_next, false, false);
  event->data[1] = 0;
  n44d4a0_cue_next(event, fighter, (Fighter *)fighter_opponent(fighter));
  retail_vm_enter_owned(fighter, 0x4e1600, fighter_ready, 9);
}

/* 44d6e0: the 44d7c0 bob task, sound B 0x2b and a two-pass counter. */
void tanya_first_victim_swell(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, n44d4a0_bob_begin, false, false);
  battle_event_program(event, 25);
  battle_sound(SOUND_B, 0x2b);
  fighter->native_variant = 1;
  retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
}

/* 44d560 state 3: 4756f0 places the victim on the owner's X/Z. */
static void n44d4a0_victim_scene(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  owner->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED;
  retail_vm_enter_owned(victim, 0x4e15e0, fighter_ready, 9);
}

/* 475190 state 1: 475690 copies the owner's facing and angles. */
static void n44d4a0_victim_turned(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->body.angle, owner->body.angle, sizeof victim->body.angle);
  native_scene_blends(victim, n44d4a0_victim_swell, 1, n44d4a0_victim_scene);
}

static void n44d4a0_victim_start(Fighter *victim) {
  native_scene_blends(victim, n44d4a0_victim_turn, 1, n44d4a0_victim_turned);
}

void tanya_first_owner_53(Fighter *fighter) {
  fighter->sample_override = 53;
  battle_sound(SOUND_B, 0x27);
  retail_vm_enter_owned(fighter, 0x4e15a8, fighter_ready, 9);
}

/* 44d4e0 -> 44dd20 scales the 44dca0 scene vector by -0x439. That vector
 * comes from an unresolved camera object; the owner keeps its place. */
void tanya_first_owner_drift(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15b8, fighter_ready, 9);
}

/* 44d510 arms nine more 4e15d0 replays of clip 396. */
void tanya_first_owner_replays(Fighter *fighter) {
  fighter->native_variant = 9;
  retail_vm_enter_owned(fighter, 0x4e15d0, fighter_ready, 9);
}

/* 44d530 counts the replays, then stops motion and parks the owner. */
void tanya_first_owner_loop(Fighter *fighter) {
  if (--fighter->native_variant >= 0) {
    retail_vm_enter_owned(fighter, 0x4e15d0, fighter_ready, 9);
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, NULL);
}

static void n44d4a0_owner_kiss_start(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1598, fighter_ready, 9);
}

/* 44d340 state 1 after sixteen visits: sound B 0x30, then 475250 blends
 * into clip 395. 44dca0/43d7d0 camera and object cleanup are presentation. */
static void n44d4a0_owner_intro(Fighter *fighter) {
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(fighter, n44d4a0_owner_kiss, 1, n44d4a0_owner_kiss_start);
}

void tanya_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n44d4a0_TANYA_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &n44d4a0_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  mk4_script_scene_pose(victim, 13, 0);
  /* 44d340 state 0 -> 475530 aligns the victim 1.15 ahead. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x12666);
  }
  /* Task 44d560 waits eleven visits; the owner waits sixteen. */
  native_hold(victim, 11, n44d4a0_victim_start);
  native_hold(fighter, 16, n44d4a0_owner_intro);
}

#include "retail_layout.h"
#include "native_services.h"
#include "retail_data.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: 448480, 4487b0, 4487f0, 448830, 448870, 4488b0, 4488f0, 448930, 448c80, 448d20, 448d70, 49cb60. */
enum {
  n448480_CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  n448480_CAGE_EVENT_POWERED_RECOVERY,
  n448480_CAGE_EVENT_FINISH_RECOVERY,
  n448480_CAGE_EVENT_SCENE_LOOP,
  n448480_CAGE_EVENT_FATALITY_1 = 17,
  n448480_CAGE_EVENT_FATALITY_2
};

#define FIRST_OWNER_READY 1
#define FIRST_VICTIM_DONE 2
#define FIRST_TORSO_DONE 4
static const MoveDef n448480_first_body = {.clip = 408, .last = 142, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n448480_owner_male[] = {
  {{.clip = 406, .last = 376, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n448480_owner_female[] = {
  {{.clip = 407, .last = 376, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n448480_victim_male[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 410, .last = 309, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n448480_victim_female[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 411, .last = 309, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n448480_first_victory[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
// clang-format on

static bool n448480_female_victim(const Fighter *fighter) {
  int asset = roster_def(fighter->roster)->asset;
  return asset == 2 || asset == 9;
}

static void n448480_first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE | FIRST_TORSO_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static Fighter *n448480_first_body_owner(Fighter *actor) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    if (&slot->actor == actor) {
      return &g_battle.fighters[slot->owner];
    }
  }
  require(false, "Cage torso has no scene owner");
  return NULL;
}

/* Native playback helpers keep the selected clip; VM bodies and their
 * branches remain compiler generated. Period2 is428f60/429120 playback. */
static void n448480_first_native_to(Fighter *fighter, int target, int period, FighterFn end) {
  int first = fighter_sample(fighter), count = (target - first) * period;
  require(target >= first && target <= fighter_move(fighter)->last && (period == 1 || period == 2), "Invalid Cage scene continuation range");
  count = count > 0 ? count : 1;
  native_begin(fighter, 1, count, end);
  for (int row = 1; row <= count; ++row) {
    native_frame(fighter, row, first + row / period < target ? first + row / period : target, NULL);
  }
  native_play(fighter, false);
}

static void n448480_first_victim_stream(Fighter *victim) {
  if (n448480_female_victim(victim)) {
    retail_vm_enter(victim, 0x4e12d0, fighter_ready);
  } else {
    retail_vm_enter(victim, 0x4e12c0, fighter_ready);
  }
}

static void n448480_first_victim_blends(Fighter *victim) {
  native_scene_blends(victim, n448480_female_victim(victim) ? n448480_victim_female : n448480_victim_male, 2, n448480_first_victim_stream);
}

static void n448480_first_intro_complete(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* 448160 ->475530 uses.95 units for the selected paired scene. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xf333);
  }
  native_hold(victim, 11, n448480_first_victim_blends);
  native_hold(fighter, 16, NULL);
  native_script_end(fighter, 0x4e12a0, fighter_ready);
}

static void n448480_first_intro(Fighter *fighter) {
  require((fighter_move(fighter)->clip == 406 || fighter_move(fighter)->clip == 407) && fighter_sample(fighter) == 0,
      "Cage first scene needs its native selected actor clip");
  /* 448160: ordinary playback to283, one hold, half-speed to334. */
  native_begin(fighter, 1, 386, n448480_first_intro_complete);
  int row = 1;
  for (int sample = 1; sample <= 283; ++sample) {
    native_frame(fighter, row++, sample, NULL);
  }
  native_frame(fighter, row++, 283, NULL);
  for (int sample = 284; sample <= 334; ++sample) {
    native_frame(fighter, row++, sample - 1, NULL);
    native_frame(fighter, row++, sample, NULL);
  }
  native_play(fighter, false);
}

static void n448480_first_owner_victory(Fighter *fighter) {
  fighter->state = S_VICTORY;
  fighter->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter(fighter, 0x4e12b0, fighter_ready);
  n448480_first_maybe_complete(fighter);
}

void cage_script_fatality_1_signal(Fighter *fighter) {
  /* 448480 blends into443 at step.05, then4e12b0 owns the loop. */
  native_scene_blends(fighter, n448480_first_victory, 1, n448480_first_owner_victory);
}

void cage_first_victim_59(Fighter *victim) {
  victim->sample_override = 59;
  battle_sound(SOUND_B, 0x1c);
  retail_vm_enter(victim, 0x4e12e0, fighter_ready);
}

void cage_first_victim_131(Fighter *victim) {
  victim->sample_override = 131;
  battle_sound(SOUND_B, 0x29);
  retail_vm_enter(victim, 0x4e12f0, fighter_ready);
}

void cage_first_victim_148(Fighter *victim) {
  victim->sample_override = 148;
  battle_sound(SOUND_B, 0x1b);
  retail_vm_enter(victim, 0x4e1300, fighter_ready);
}

void cage_first_victim_166(Fighter *victim) {
  victim->sample_override = 166;
  battle_sound(SOUND_B, 0x2a);
  retail_vm_enter(victim, 0x4e1310, fighter_ready);
}

void cage_first_victim_185(Fighter *victim) {
  victim->sample_override = 185;
  battle_sound(SOUND_B, 0x1b);
  retail_vm_enter(victim, 0x4e1320, fighter_ready);
}

void cage_first_victim_206(Fighter *victim) {
  victim->sample_override = 206;
  battle_sound(SOUND_B, 0x2b);
  retail_vm_enter(victim, 0x4e1330, fighter_ready);
}

static void n448480_first_lower_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  battle_camera_shake(5, 3, 0x2e14);
  retail_vm_enter(victim, 0x4e1340, retail_scene_448480_complete);
}

static void n448480_first_lower_impact(Fighter *victim) {
  victim->sample_override = 278;
  battle_sound(SOUND_B, 0x12);
  battle_camera_shake(5, 3, 0x2e14);
  n448480_first_native_to(victim, 307, 1, n448480_first_lower_finish);
}

void cage_first_sever(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->sample_override = 225;
  victim->hidden_joints[0] |= UINT64_C(1) << 3;
  Fighter *torso = battle_scene_actor_start_script(owner, victim, &n448480_first_body, 0x4e1350, fighter_ready);
  require(torso != NULL, "Cage torso needs its bounded independent scene slot");
  /* 448af0's copied body hides the two leg subtrees11/12. The original
   * body keeps those legs and continues410/411 independently. */
  torso->hidden_joints[0] = (UINT64_C(1) << 11) | (UINT64_C(1) << 12);
  battle_sound(SOUND_B, 0x16);
  Vec3 waist;
  Mat3 unused;
  fighter_joint_position(victim, 3, &waist, &unused);
  battle_blood_hit(owner, victim, waist, 0x3333);
  n448480_first_native_to(victim, 277, 1, n448480_first_lower_impact);
}

void cage_first_torso_half(Fighter *torso) {
  n448480_first_native_to(torso, 109, 2, NULL);
  native_script_end(torso, 0x4e1360, fighter_ready);
}

void cage_first_torso_123(Fighter *torso) {
  torso->sample_override = 123;
  battle_sound(SOUND_B, 0x17);
  battle_camera_shake(9, 3, 0x2e14);
  retail_vm_enter(torso, 0x4e1370, fighter_ready);
}

void cage_first_torso_finish(Fighter *torso) {
  battle_sound(SOUND_B, 0x17);
  battle_camera_shake(9, 3, 0x2e14);
  retail_vm_enter(torso, 0x4e1380, retail_scene_448480_complete);
}

void retail_scene_448480_complete(Fighter *actor) {
  Fighter *owner = battle_is_scene_actor(actor) ? n448480_first_body_owner(actor) : (Fighter *)fighter_opponent(actor);
  bool torso = battle_is_scene_actor(actor);
  /* Retiring the retail task leaves its final rendered body in the scene. */
  owner->native_variant |= torso ? FIRST_TORSO_DONE : FIRST_VICTIM_DONE;
  n448480_first_maybe_complete(owner);
}

void cage_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n448480_CAGE_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  fighter->state = S_PUNCH;
  fighter->action = 0x700;
  mk4_script_scene_pose(victim, 13, 0);
  native_hold(victim, 1, NULL);
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(fighter, n448480_female_victim(victim) ? n448480_owner_female : n448480_owner_male, 1, n448480_first_intro);
}

#undef FIRST_OWNER_READY
#undef FIRST_VICTIM_DONE
#undef FIRST_TORSO_DONE

/* Native service cluster: 413970, 413a10, 4658a0, 4658f0, 465b10, 466df0. */
enum {
  n413970_CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  n413970_CAGE_EVENT_POWERED_RECOVERY,
  n413970_CAGE_EVENT_FINISH_RECOVERY,
  n413970_CAGE_EVENT_SCENE_LOOP,
  n413970_CAGE_EVENT_FATALITY_1 = 17,
  n413970_CAGE_EVENT_FATALITY_2
};

static const MoveDef n413970_owner_pose = {.clip = 293, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n413970_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 413ce0 uses4d17b8: the additive variants of the same ten share2
 * roots as Cage's orb, attached to joint10 at.7 scale. */
static const MeshAnimation n413970_charge_mesh = {
    .first = 271, .count = 10, .period = 1, .scale = 0xb333, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

static void n413970_charge_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  Vec3 hand;
  Mat3 unused;
  fighter_joint_position(&g_battle.fighters[object->owner], 10, &hand, &unused);
  object->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
}

static void n413970_charge_spawn(Fighter *fighter, int task_type, int lifetime) {
  require(task_type == 619, "Unexpected Cage charge task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, lifetime, n413970_charge_step);
  if (object != NULL) {
    object->mesh_animation = &n413970_charge_mesh;
    object->initial_mesh = n413970_charge_mesh.first;
    n413970_charge_step(object);
  }
}

void cage_second_charge(Fighter *fighter, int task_type) {
  n413970_charge_spawn(fighter, task_type, 19);
}

void cage_second_flash(Fighter *fighter, int task_type) {
  n413970_charge_spawn(fighter, task_type, 8);
}

void cage_script_fatality_2_continue(Fighter *fighter) {
  /* 4658a0 -> standing dispatch ->4658d0 ->4e5880. Both actors
   * already have locked scene poses; retain this actor's frame ownership. */
  retail_vm_enter(fighter, 0x4e5880, fighter_ready);
}

void cage_second_repeated_uppercuts(Fighter *fighter) {
  int sample = fighter_sample(fighter), frame = 1;
  require(fighter_move(fighter)->clip == 293 && sample == 6, "Cage repeated uppercuts require their generated six-pose startup");
  native_begin(fighter, 1, 45, NULL);
  native_script_end(fighter, 0x4e58d8, fighter_ready);
  /* 4658f0 repeats rewind-to6/advance8 three times, then finishes293. */
  for (int repeat = 0; repeat < 3; ++repeat) {
    while (sample > 6) {
      native_frame(fighter, frame++, --sample, NULL);
    }
    for (int advance = 0; advance < 8; ++advance) {
      native_frame(fighter, frame++, ++sample, NULL);
    }
  }
  while (sample < 19) {
    native_frame(fighter, frame++, ++sample, NULL);
  }
  require(frame == 46, "Cage repeated uppercut frame count differs from the native loop");
  native_play(fighter, false);
}

static void n413970_second_scene_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  battle_end_finisher();
}

void cage_second_victim_finish(Fighter *fighter) {
  /* 465b10 leaves joint2 hidden, holds the fallen victim80 visits,
   * then returns to the common completed-finisher controller. */
  fighter->hidden_joints[0] |= UINT64_C(1) << 2;
  native_hold(fighter, 80, n413970_second_scene_end);
}

void cage_second_victim_hit(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &n413970_victim_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  /* 465a80/466e10 extract joint2;465ba0 launches its independent body
   * away from the attacker at.425, up.15 with gravity.03. */
  ScenePiece *head = battle_detach_piece(fighter, 2, (q16[3]){-0x6ccc, -0x2666, 0}, (q16[3]){0});
  if (head != NULL) {
    const Fighter *owner = fighter_opponent(fighter);
    head->vel[AXIS_X] = q16_mul(fighter_axis(owner, AXIS_X), 0x6ccc);
    head->vel[AXIS_Z] = q16_mul(fighter_axis(owner, AXIS_Z), 0x6ccc);
    head->gravity = 0x7ae;
  }
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(fighter, 2, &neck, &unused);
  battle_blood_hit(fighter_opponent(fighter), fighter, neck, 0x3333);
  retail_vm_enter(fighter, 0x4e5900, fighter_stop_mmove);
}

void cage_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n413970_CAGE_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n413970_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(victim, 0x4e5b38, fighter_ready);
  fighter_start_move(fighter, S_PUNCH, &n413970_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4e5870, fighter_ready);
}

/* Native service cluster: 41ef20, 45ac30, 4816f0, 4817c0. */
static const MoveDef n41ef20_owner_pose = {.clip = 291, .last = 72, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n41ef20_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/*4818f0 reads4ea420 in this order, separating one joint every five visits. */
static const int n41ef20_torn_joints[] = {2, 10, 9, 8, 7, 6, 5, 3, 12, 11, 14, 13, 16, 15};

void fujin_second_blood(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected wind fatality decoration task");
  const Fighter *victim = fighter_opponent(fighter);
  Vec3 point = {fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]), fighter_z(victim)};
  battle_blood_hit(fighter, victim, point, 0x1eb8);
}

static void n41ef20_second_voice(Fighter *victim) {
  (void)victim;
  battle_sound(SOUND_C, 0x3f);
}

static void n41ef20_second_tear_joint(Fighter *victim) {
  int index = victim->native_variant++;
  require(index >= 0 && (size_t)index < sizeof n41ef20_torn_joints / sizeof n41ef20_torn_joints[0], "Invalid wind tear phase");
  q16 velocity[3] = {victim->saved_motion[AXIS_X] * 10 * victim->facing, 0, victim->saved_motion[AXIS_Z] * 10};
  ScenePiece *piece = battle_detach_piece(victim, n41ef20_torn_joints[index], velocity, (q16[3]){0});
  if (piece != NULL) {
    /*406ce0 exchanges the detached joint for its closed share2 part
     *from4ea420, retaining the original joint transform. */
    piece->model = &victim->assets->scene_models.data[SCENE_MODEL_WIND_PIECES + index];
    piece->bone = 0;
    piece->gravity = 0x1999;
    piece->lifetime = 45;
  }
}

static void n41ef20_second_flayed(Fighter *victim) {
  /*481880 ->41d770 swaps the now-torn skin for the complete flesh body,
   *then enters550. Detached pieces retain their authored closed part geometry. */
  native_motion_stop(victim);
  mk4_script_stop_body(victim);
  memset(victim->hidden_joints, 0, sizeof victim->hidden_joints);
  if (victim->assets != NULL) {
    victim->scene_model = &victim->assets->scene_models.data[SCENE_MODEL_FLAYED_BODY];
  }
  retail_vm_enter(victim, 0x4ea2a8, fighter_ready);
}

void fujin_second_burst(Fighter *victim) {
  /*4816f0 saves drift;4818f0 schedules14 skin joints five visits apart.
   *The independent wind oscillation remains active until replacement. */
  victim->saved_motion[AXIS_X] = victim->body.vel[AXIS_X];
  victim->saved_motion[AXIS_Z] = victim->body.vel[AXIS_Z];
  victim->native_variant = 0;
  int visits = (int)(sizeof n41ef20_torn_joints / sizeof n41ef20_torn_joints[0]) * 5;
  native_begin(victim, 1, visits, n41ef20_second_flayed);
  for (int visit = 1; visit <= visits; ++visit) {
    native_frame(victim, visit, 0, (visit - 1) % 5 == 0 ? n41ef20_second_tear_joint : NULL);
  }
  native_play(victim, false);
}

static void n41ef20_second_vibrate(Fighter *victim) {
  victim->body.vel[AXIS_Y] = -victim->body.vel[AXIS_Y];
}

void fujin_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &n41ef20_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  /*481670 turns the actor perpendicular to the pair;4819b0 alternates
   *vertical speed.03 every five visits during the90-visit wind hold. */
  victim->body.angle[AXIS_Y] = -0x191eb;
  victim->body.vel[AXIS_Y] = 0x7ae;
  n41ef20_second_voice(victim);
  native_motion_begin(victim, 160, NULL);
  for (int visit = 0; visit < 160; ++visit) {
    FighterFn action = visit % 5 == 0 ? n41ef20_second_vibrate : (visit == 59 || visit == 118) ? n41ef20_second_voice : NULL;
    native_motion_frame(victim, visit, NULL, 0, action);
  }
  native_motion_play(victim);
  retail_vm_enter(victim, 0x4ea2d8, fighter_ready);
}

void fujin_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FUJIN_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n41ef20_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter_start_move(fighter, S_PUNCH, &n41ef20_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4dfed0, fighter_ready);
}

/* Native service cluster: native_fatality. */
/* The Jarek fatality contact uses reaction 175 with zero damage. The stage
 * coordinator owns the victim pose and recovery for that scene. */

/* Native service cluster: 41de30, 41dfc0, 41e0a0, 41e0e0. */
enum {
  n41de30_JAREK_EVENT_FATALITY_1 = 25,
  n41de30_JAREK_EVENT_FATALITY_2
};

/* U,U,F,F+BLK -> 467cd0 -> 43bd70 -> 4dfe90[12] = 41df20. After the shared
 * 41aaf0 preamble the owner runs generated 4d2db0 on clip 577. */
static const MoveDef n41de30_owner_pose = {.clip = 577, .last = 114, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n41de30_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n41de30_victim_scene = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 41de30 -> 406dd0(50c604): share2 mesh 0, .113 forward of the node. */
static const MeshAnimation n41de30_glow_mesh = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .offset = {0, 0, 0x1ced}};

static void n41de30_glow_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  const Fighter *owner = &g_battle.fighters[object->owner];
  Vec3 head;
  Mat3 unused;
  fighter_joint_position(owner, 2, &head, &unused);
  object->pos[AXIS_X] = (q16)(head.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(head.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(head.z * Q16_ONE);
  object->heading[0] = owner->heading[0];
  object->heading[1] = owner->heading[1];
  object->lifetime = 2;
}

/* 41de30: bank C 11, then the 50c604 model rides the owner's joint 2. */
void jarek_second_glow(Fighter *fighter) {
  battle_sound(SOUND_C, 11);
  Projectile *glow = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, n41de30_glow_step);
  if (glow != NULL) {
    glow->mesh_animation = &n41de30_glow_mesh;
    glow->initial_mesh = n41de30_glow_mesh.first;
    n41de30_glow_step(glow);
  }
}

static void n41de30_neck_blood(Fighter *victim) {
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(victim, 2, &neck, &unused);
  battle_blood_hit(fighter_opponent(victim), victim, neck, 0x3333);
}

static ScenePiece *n41de30_victim_head(const Fighter *victim) {
  int index = victim == &g_battle.fighters[0] ? 0 : 1;
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    ScenePiece *piece = &g_battle.pieces[i];
    if (piece->active && piece->attached_to == index && piece->joint == 2) {
      return piece;
    }
  }
  return NULL;
}

/* 41dff0 stage 0: context sound 0x12, then 41ad60 sprays 40a470 gore 0/8
 * and separates joint 2 as its own spinning node. Stage 1 enters 4d2e10. */
static void n41de30_victim_start(Fighter *victim) {
  battle_sound(SOUND_CONTEXT, 0x12);
  n41de30_neck_blood(victim);
  /* The separated head stays on the neck until 41e0e0 drops it. */
  battle_attach_head(victim, victim, 2);
  fighter_start_move(victim, S_KNOCKDOWN, &n41de30_victim_scene);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  retail_vm_enter(victim, 0x4d2e10, fighter_ready);
}

/* 41dfc0: 48f880 replaces the victim task with 41dff0; the owner enters
 * 4d2de0 and keeps lasing on clip 577. */
void jarek_second_handoff(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  n41de30_victim_start(victim);
  retail_vm_enter(fighter, 0x4d2de0, fighter_ready);
}

/* 41e0e0: the separated head falls under gravity 0x20c, spinning about Z
 * by the victim's side (41ad60 0x147a), with gore 8/4/3 at release. */
void jarek_second_head_fall(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Jarek head task");
  battle_sound(SOUND_CONTEXT, 0x12);
  battle_sound(SOUND_C, 11);
  n41de30_neck_blood(fighter);
  ScenePiece *head = n41de30_victim_head(fighter);
  if (head != NULL) {
    head->attached_to = -1;
    head->gravity = 0x20c;
    head->spin[2] = fighter == &g_battle.fighters[0] ? 0x147a : -0x147a;
  }
}

static void n41de30_second_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 41e0a0 -> 41f1b0 parks the fallen victim in 60-visit waits; the portable
 * scene completes after the first one. */
void jarek_second_victim_hold(Fighter *fighter) {
  native_hold(fighter, 60, n41de30_second_end);
}

void jarek_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n41de30_JAREK_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n41de30_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  /* The victim keeps its defeated stance until 41dfc0 replaces its task. */
  native_begin(victim, 0, n41de30_victim_pose.last, NULL);
  for (int frame = 0; frame <= n41de30_victim_pose.last; ++frame) {
    native_frame(victim, frame, frame, NULL);
  }
  native_play(victim, false);
  fighter_start_move(fighter, S_PUNCH, &n41de30_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4d2db0, fighter_ready);
}

/* Native service cluster: 412d80, 412ec0, 466a90, 466bb0, 466df0. */
/* BLOCK record 4e43c8 (B,F,F,D, window 50, flags 0x41, gate 45e570) ->
 * 467cd0 -> 43bd70 -> 4dfe90[13] = 4669c0: Jax claps the victim's head.
 * After the shared 41aaf0 scene setup, task 466dd0 finishes the victim's
 * current clip (4e5b38) and parks it (466df0), while 4e5a80 binds clip 284
 * on Jax, waits 20 ticks and enters 466a90: 475530 places the victim 0.85
 * units ahead, 15 ticks later 4e5a98 runs the clap. */
static const MoveDef n412d80_second_owner_pose = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n412d80_second_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 412ec0 -> 4d1748: share2 mesh 281 for 30 ticks at joint 10, scale 0x4ccc. */
static const MeshAnimation n412d80_second_flash_mesh = {
    .first = 281, .count = 1, .period = 1, .scale = 0x4ccc, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

static void n412d80_second_flash_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  Vec3 hand;
  Mat3 unused;
  fighter_joint_position(&g_battle.fighters[object->owner], 10, &hand, &unused);
  object->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
}

/* 412d80: 413070 sparks from joint 10 on the clap approach. */
void jax_fatality_2_sparks(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Jax clap spark task");
  battle_particles(fighter, 10, 3, false);
}

/* 412ec0: the flash mesh follows joint 10 for its 30-tick track. */
void jax_fatality_2_flash(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Jax clap flash task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 30, n412d80_second_flash_step);
  if (object != NULL) {
    object->mesh_animation = &n412d80_second_flash_mesh;
    object->initial_mesh = n412d80_second_flash_mesh.first;
    n412d80_second_flash_step(object);
  }
}

/* 466a90: 475530 at 0xd999, then 15 ticks before 4e5a98. */
void jax_fatality_2_approach(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xd999);
  }
  native_hold(fighter, 15, NULL);
  native_script_end(fighter, 0x4e5a98, fighter_ready);
}

/* 466df0 finishes only the auxiliary victim task; the body keeps its pose
 * until the clap installs reaction 183. */
/* 4ea4e8[183] -> 466b40: 476d80 hides node 2 (head), effect 4, the neck
 * blood task 455ea0 and 4e5b10 (B[8], clip 557, keepalive, clip 206). */
void jax_fatality_2_head(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &n412d80_second_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(victim, 2, &neck, &unused);
  victim->hidden_joints[0] |= UINT64_C(1) << 2;
  mk4_script_effect(victim, 4);
  battle_blood_hit(fighter_opponent(victim), victim, neck, 0x3333);
  retail_vm_enter(victim, 0x4e5b10, fighter_ready);
}

static void n412d80_second_scene_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  battle_end_finisher();
}

/* 466bb0 waits 20 ticks, removes the type-0x26b scene tasks and parks in
 * 4314e0; that last own action is the portable completion boundary. */
void jax_fatality_2_finish(Fighter *victim) {
  native_hold(victim, 20, n412d80_second_scene_end);
}

void jax_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n412d80_second_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(victim, 0x4e5b38, fighter_ready);
  fighter_start_move(fighter, S_PUNCH, &n412d80_second_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4e5a80, fighter_ready);
}

/* Native service cluster: 406ba0, 43c140, 45ac30, 46a280, 46a500. */
enum {
  n406ba0_KAI_EVENT_FATALITY_1 = 23,
  n406ba0_KAI_EVENT_FATALITY_2
};

/* Second fatality: 467cd0 -> 43bd70 -> roster table 4dfe90[11] = 43bfe0.
 * The owner levitates on clips 493/486/487 (4dff88) and throws the skull
 * object 43c140; within 1.3 units it installs reaction 178 (4814c0), whose
 * head task 455bf0 and script 4ea240 -> 46a500 -> 4e5be0 fell the victim. */
static const MoveDef n406ba0_owner_pose = {.clip = 493, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n406ba0_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4dffe8 selects the skull rig 50c8f8 (share2 mesh 48): forward .2, Y -.2. */
static const MeshAnimation n406ba0_skull_mesh = {.first = 48, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2};

enum {
  n406ba0_SKULL_FLIGHT = 0,
  n406ba0_SKULL_HIT,
  /* 4e5be0 plays clip 206 for 100 visits; then the scene settles for one
   * 60-visit park interval before the result. */
  n406ba0_SECOND_SETTLE = 160
};

void kai_second_child_walk(Fighter *fighter) {
  /* 406ba0 walks the node's +10 child list with selector 3. The finisher
   * disarmed both fighters and attached no child objects before these
   * calls, so the portable actors have no child nodes to visit. */
  require(!fighter->armed, "Kai second fatality actors must not carry a weapon");
}

static void n406ba0_skull_step(Projectile *object) {
  Fighter *victim = (Fighter *)fighter_opponent(&g_battle.fighters[object->owner]);
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    object->pos[axis] += object->vel[axis];
  }
  if (object->phase == n406ba0_SKULL_FLIGHT) {
    object->lifetime = 2;
    /* 43c17e: within 1.3 units on X, 480f80 installs reaction 178 on the
     * peer; the object retires after a further 15 visits (406740). */
    q16 dx = victim->body.pos[AXIS_X] - object->pos[AXIS_X];
    if ((dx < 0 ? -dx : dx) <= 0x14ccc) {
      battle_react(victim, 178);
      object->phase = n406ba0_SKULL_HIT;
      object->lifetime = 16;
    }
  }
}

void kai_second_skull(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected Kai skull task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 2, n406ba0_skull_step);
  if (object == NULL) {
    return;
  }
  const Fighter *victim = fighter_opponent(fighter);
  object->mesh_animation = &n406ba0_skull_mesh;
  object->initial_mesh = n406ba0_skull_mesh.first;
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
  object->phase = n406ba0_SKULL_FLIGHT;
}

static void n406ba0_second_complete(Fighter *victim) {
  (void)victim;
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

void kai_second_victim_fall(Fighter *victim) {
  /* 46a500: 406ba0, then 4e5be0 drops the victim on clip 206 and parks.
   * An independent settle row ends the scene after one park interval. */
  kai_second_child_walk(victim);
  native_motion_begin(victim, n406ba0_SECOND_SETTLE, n406ba0_second_complete);
  native_motion_play(victim);
  retail_vm_enter(victim, 0x4e5be0, fighter_ready);
}

void kai_second_victim(Fighter *victim) {
  /* 4814c0 -> 455bf0: hide head joint 2 and launch its piece (gravity 20c,
   * Y -.205, spin .4/-.18/-.7) with neck blood, then script 4ea240. */
  fighter_start_move(victim, S_KNOCKDOWN, &n406ba0_victim_pose);
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
  retail_vm_enter(victim, 0x4ea240, fighter_ready);
}

void kai_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n406ba0_KAI_EVENT_FATALITY_2)) {
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
  fighter_start_move(fighter, S_PUNCH, &n406ba0_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4dff88, fighter_ready);
}

/* Native service cluster: 43c500, 43c540, 45ac30, 48f9f0. */
#define SCENE(c, a, b) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, a, b, 1, 0, 0}
static void n43c500_dragon_stream(Fighter *fighter);
static void n43c500_dragon_spawn(Fighter *fighter);
static void n43c500_dragon_actor_start(Fighter *fighter);
static void n43c500_dragon_victim_start(Fighter *fighter);
static void n43c500_dragon_tear(Fighter *fighter);
static void n43c500_dragon_bite_sound(Fighter *fighter);
static void n43c500_dragon_delayed_sound(Fighter *fighter);
static void n43c500_dragon_detach(Fighter *fighter);
static void n43c500_dragon_fatality_script(Fighter *fighter);
static void n43c500_fatal_fireball_object(Projectile *p);
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n43c500_dragon_actor[] = {
  SCENE(418, 0, 159),
  SCENE(418, 160, 518),
  SCENE(418, 446, 518),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip n43c500_dragon_target[] = {
  {{.clip = 389, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 419, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

void liu_kang_finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void n43c500_dragon_stream(Fighter *fighter) {
  if (fighter->scene.clips != NULL) {
    scene_step(fighter);
  }
}

static void n43c500_dragon_spawn(Fighter *fighter) {
  fighter->appearance = APPEARANCE_DRAGON;
  fighter->body.pos[AXIS_Y] = -0x12c49;
  mk4_mmove_bind(fighter, 418, 519, 0);
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
  battle_sound(SOUND_B, 36);
  float x = q16_float(fighter->heading[0]), z = q16_float(fighter->heading[1]);
  Vec3 center = {fighter_x(fighter) + x, -2.7f, fighter_z(fighter) + z};
  g_battle.camera = (SceneCamera){.active = true, .focal_scale = 0x9999, .eye = {center.x - z * 7.5f, -2.9f, center.z + x * 7.5f}, .target = center};
}

static void n43c500_dragon_actor_start(Fighter *fighter) {
  battle_sound(SOUND_B, 46);
  scene_begin(fighter, n43c500_dragon_actor, NATIVE_COUNT(n43c500_dragon_actor), 2);
}

/*451190: native389/419 blends precede original4e1fa8 playback/retirement. */
static void n43c500_dragon_victim_script(Fighter *victim) {
  retail_vm_enter(victim, 0x4e1fa8, fighter_stop_mmove);
}

static void n43c500_dragon_victim_start(Fighter *fighter) {
  scene_step(fighter);
  native_scene_blends(fighter_peer(fighter), n43c500_dragon_target, NATIVE_COUNT(n43c500_dragon_target), n43c500_dragon_victim_script);
}

static void n43c500_dragon_tear(Fighter *fighter) {
  n43c500_dragon_stream(fighter);
  battle_particles(fighter_peer(fighter), 3, 3, false);
}

static void n43c500_dragon_bite_sound(Fighter *fighter) {
  n43c500_dragon_stream(fighter);
  battle_sound(SOUND_B, 37);
}

static void n43c500_dragon_delayed_sound(Fighter *fighter) {
  n43c500_dragon_stream(fighter);
  battle_sound(SOUND_B, 65);
}

static void n43c500_dragon_detach(Fighter *fighter) {
  n43c500_dragon_stream(fighter);
  battle_detach_piece(fighter_peer(fighter), 5, (q16[3]){0}, (q16[3]){0});
  battle_detach_piece(fighter_peer(fighter), 6, (q16[3]){0}, (q16[3]){0});
}

static void n43c500_dragon_fatality_script(Fighter *fighter) {
  int first = g_battle.phase_ticks + 1;
  int actor_end = 121 + scene_frame_count(n43c500_dragon_actor, 2);
  int victim_end = 269 + scene_frame_count(n43c500_dragon_target, NATIVE_COUNT(n43c500_dragon_target)) + (int)mk4_image_u32(retail_vm_image(), 0x4fc120 + 4);
  int last = maxi(actor_end, victim_end);
  native_motion_begin(fighter, last - first + 1, liu_kang_finish_scene);
  for (int frame = first; frame <= last; ++frame) {
    FighterFn action = n43c500_dragon_stream;
    if (frame == 101) {
      action = n43c500_dragon_spawn;
    } else if (frame == 121) {
      action = n43c500_dragon_actor_start;
    } else if (frame == 269) {
      action = n43c500_dragon_victim_start;
    } else if (frame == 281) {
      action = n43c500_dragon_bite_sound;
    } else if (frame == 418) {
      action = n43c500_dragon_delayed_sound;
    } else if (frame == 541) {
      action = n43c500_dragon_detach;
    } else {
      int tear = frame - 285;
      if ((tear >= 0 && tear < 92 && tear % 4 == 0) || (tear >= 92 && tear < 248 && (tear - 92) % 6 == 0)) {
        action = n43c500_dragon_tear;
      }
    }
    native_motion_frame(fighter, frame - first, NULL, 0, action);
  }
  native_motion_play(fighter);
}

static void n43c500_fatal_fireball_object(Projectile *p) {
  if (g_battle.phase != PHASE_FATALITY || g_battle.finisher != FINISH_LIU_KANG_2) {
    p->kind = 0;
    return;
  }
  Fighter *victim = fighter_peer(&g_battle.fighters[p->owner]);
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  q16 dx = victim->body.pos[AXIS_X] - p->pos[AXIS_X];
  q16 dz = victim->body.pos[AXIS_Z] - p->pos[AXIS_Z];
  if ((int64_t)dx * dx + (int64_t)dz * dz <= (int64_t)0x28000 * 0x28000 || p->ticks >= 47) {
    p->kind = 0;
    battle_react(victim, 177);
  }
}

void liu_kang_liu_script_fatal_setup(Fighter *fighter) {
  retail_scene_pose(fighter, 537, 0, true);
  retail_scene_pose(fighter_peer(fighter), 539, 0, false);
  fighter->flags |= FF_PAIRED;
  fighter_peer(fighter)->flags |= FF_PAIRED;
  retail_vm_enter(fighter, 0x4e0020, fighter_ready);
}

void liu_kang_liu_script_fatal_spawn(Fighter *fighter, int task_type) {
  require(task_type == 38, "Invalid fatality projectile task");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FATAL_FIREBALL, 48, n43c500_fatal_fireball_object);
  if (p != NULL) {
    p->mesh_animation = &liu_kang_fireball_mesh;
    p->initial_mesh = 50;
    p->pos[AXIS_Y] -= 0x7333;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      p->pos[axis] += q16_mul(fighter_axis(fighter, axis), 0x1999);
      p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
    }
  } else {
    battle_react(fighter_peer(fighter), 177);
  }
}

void liu_kang_liu_script_fatal_align(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
}

void liu_kang_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_LIU_KANG_1 || id == FINISH_LIU_KANG_2, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    return;
  }
  retail_scene_join_victory(fighter);
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, 119, 0, true);
  if (id == FINISH_LIU_KANG_1) {
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x20ccc);
    }
  }
  if (id == FINISH_LIU_KANG_1) {
    n43c500_dragon_fatality_script(fighter);
  } else {
    retail_vm_enter(fighter, 0x4dfff8, fighter_ready);
  }
}

/* Typed native boundaries used by generated VM code. These contain native
 * contact/controller work; all VM playback and branching lives in the include. */

#undef SCENE

/* Native service cluster: native_fatality_second. */
void liu_kang_second_fatality(Fighter *fighter) {
  liu_kang_start_fatality(fighter, FINISH_LIU_KANG_2);
}

/* Native service cluster: native_fatality_second_2. */
/* 45e530: the second-fatality range gate, 1.8..3 units. */
/* LP [U,U,D,D] record 4e4824 enters 467cd0, whose table 4dfe90[8] selects
 * 43be30: Quan Chi performs the victim roster's own second fatality
 * (4dfe90[victim], Kitana as Sonya and Noob as Reiko). Against another Quan
 * Chi, 43cbb0 runs his first fatality instead. The portable dispatch uses
 * the victim's original native table target, with the current actor as owner. */
void quan_chi_second_fatality(Fighter *fighter) {
  unsigned roster = retail_character_slot((unsigned)fighter_opponent(fighter)->roster);
  if (roster == 8) {
    retail_finish_first(fighter);
    return;
  }
  uint32_t target = retail_finish_target(retail_vm_image(), roster, true);
  if (target == 0) {
    fighter_ready(fighter);
    return;
  }
  retail_native_entry(target)(fighter);
}

/* Native service cluster: 410380. */
/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
void raiden_scene_end_watch(Fighter *fighter) {
  int clip = fighter_move(fighter)->clip;
  if (g_battle.phase != PHASE_FATALITY) {
    fighter->body.physics = NULL;
    return;
  }
  if (fighter_sample(fighter) == fighter_move(fighter)->last && (clip == 573 || clip == 388 || clip == 114)) {
    fighter->body.physics = NULL;
    battle_end_finisher();
  }
}

void raiden_script_victory(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4e6a40, fighter_ready);
}

void raiden_script_victory_effect(Fighter *fighter) {
  battle_particles(fighter, 10, 4, true);
  battle_particles(fighter, 13, 4, true);
}

/* Native service cluster: 427b70, 464960, 464a60, 464aa0, 464bb0, 464ee0. */
void raiden_script_second_blend(Fighter *fighter) {
  mk4_mmove_blend_to(fighter, 123, 101, 100, 0xccc, 0x1999, NULL);
  native_script_end(fighter, 0x4e56d8, fighter_ready);
}

void raiden_script_second_particles(Fighter *fighter) {
  battle_particles(fighter, 10, 8, true);
}

void raiden_script_second_effect(Fighter *fighter) {
  battle_particles(fighter_peer(fighter), 18, 12, true);
  retail_vm_enter(fighter, 0x4e56f0, fighter_ready);
}

void raiden_script_second_release(Fighter *fighter) {
  battle_particles(fighter_peer(fighter), 2, 24, true);
  retail_vm_enter(fighter, 0x4e5740, fighter_ready);
}

void raiden_script_second_end(Fighter *fighter) {
  fighter->body.physics = raiden_scene_end_watch;
  retail_vm_enter(fighter, 0x4e5760, fighter_ready);
}

void raiden_script_second_victim_end(Fighter *fighter) {
  battle_particles(fighter, 2, 24, true);
  retail_vm_enter(fighter, 0x4e5788, fighter_ready);
}

void raiden_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 123, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 13, 0);
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14000);
  }
  retail_vm_enter(fighter, 0x4e56c8, fighter_ready);
}

/* Native service cluster: 41c4b0, 41c4e0, 41d260, 41f1b0. */
enum {
  n41c4b0_REIKO_FINISH_1 = 27,
  n41c4b0_REIKO_FINISH_2
};

/* B,B,D,D+HK 467cd0 -> second-fatality table 4dfe90[7] = 41c360: Reiko throws
 * a 61-star barrage (41d260, table 4d3018) at the dizzy victim. The first
 * arrival starts the victim's clip-554 stagger with push-back (41c510) and
 * jolt (41c690) tasks; the final head star (#60) is the headshot signal
 * (5322a8 = -1) that ends them, and the victim falls. */
#define SECOND_ARRIVED 1
#define SECOND_HEADSHOT 2

static const MoveDef n41c4b0_second_owner = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

/* 45e530: 0x1cccc <= distance <= 0x30000, then the 45e5e0 finish window. */
/* A star leaves the owner's hand joint (9 or 10) and homes on its target
 * joint at 0.15 per visit until every component is within 0.2 (41cd60).
 * Portable adaptation: the flight is scheduled from the launch distance and
 * the stars themselves are not drawn or attached; only the arrival signals
 * that drive both actors are kept. */
static int n41c4b0_star_flight(const Fighter *owner, int hand, const Fighter *victim, int joint) {
  Vec3 from, to;
  Mat3 unused;
  if (!fighter_joint_position(owner, hand, &from, &unused) || !fighter_joint_position(victim, joint, &to, &unused)) {
    return 1;
  }
  double distance = sqrt((double)(to.x - from.x) * (to.x - from.x) + (double)(to.y - from.y) * (to.y - from.y) + (double)(to.z - from.z) * (to.z - from.z));
  return maxi(1, (int)ceil((distance - .2) / .15));
}

static bool n41c4b0_pushback_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* 41c510: -0.15 along the victim's own axis at arrival and every five
   * visits, twenty times; after the headshot, one more step. */
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->data[0] < 20 || ((owner->native_variant & SECOND_HEADSHOT) && event->data[0] == 20)) {
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] -= q16_mul(fighter_axis(victim, axis), 0x2666);
    }
    event->data[0]++;
    battle_event_delay(event, event->data[0] < 20 ? 5 : 1);
    return event->data[0] <= 20;
  }
  return event->data[0] == 20;
}

static bool n41c4b0_jolt_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* 41c690: one five-visit cycle of Y -0x1999, +0x28f, +0x51e, +0x7ae,
   * +0xa3d; 25 cycles, then one more after the headshot. */
  static const q16 jolt[] = {-0x1999, 0x28f, 0x51e, 0x7ae, 0xa3d};
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  int cycles = event->data[0] / 5;
  if (cycles >= 25 && !(cycles == 25 && (owner->native_variant & SECOND_HEADSHOT))) {
    return cycles == 25;
  }
  victim->body.pos[AXIS_Y] += jolt[event->data[0] % 5];
  event->data[0]++;
  return event->data[0] < 26 * 5;
}

static void n41c4b0_first_arrival(Fighter *owner, Fighter *victim) {
  owner->native_variant |= SECOND_ARRIVED;
  BattleEvent *push = battle_event_start(owner, n41c4b0_pushback_step, false, false);
  BattleEvent *jolt = battle_event_start(owner, n41c4b0_jolt_step, false, false);
  require(push != NULL && jolt != NULL, "Reiko's star barrage needs its bounded victim tasks");
  battle_blood_hit(owner, victim, (Vec3){fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]) - 1.2f, fighter_z(victim)}, 0x3333);
}

/* data[0]/data[1]: elapsed visit of the first and head-star arrivals,
 * data[2]: elapsed visits since the thrower started. */
static bool n41c4b0_arrivals_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  int elapsed = ++event->data[2];
  if (elapsed == event->data[0]) {
    n41c4b0_first_arrival(owner, victim);
  }
  if (elapsed == event->data[1]) {
    /* 41cd60 on the joint-2 star: 5322a8 = -1 and a 48a300 hit sound. */
    owner->native_variant |= SECOND_HEADSHOT;
    battle_scene_impact(victim);
  }
  return !(owner->native_variant & SECOND_HEADSHOT);
}

static BattleEvent *n41c4b0_arrivals_of(const Fighter *owner) {
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (event->active && event->step == n41c4b0_arrivals_step && &g_battle.fighters[event->owner] == owner) {
      return event;
    }
  }
  return NULL;
}

static bool n41c4b0_star_throw(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* Star k leaves at 3 * (k / 2) + (k & 1) from alternating hands, #60
   * after a further 25 visits (4d3018: joint 3 first, joint 2 last). */
  int star = event->frame_value;
  BattleEvent *arrivals = n41c4b0_arrivals_of(owner);
  if (arrivals != NULL && (star == 0 || star == 60)) {
    arrivals->data[star == 0 ? 0 : 1] = arrivals->data[2] + n41c4b0_star_flight(owner, star & 1 ? 10 : 9, victim, star == 0 ? 3 : 2);
  }
  return g_battle.phase == PHASE_FATALITY;
}

static bool n41c4b0_stars_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  return event->frame_count > 0 && fighter_opponent(owner) == victim;
}

/* 41d260, task type 0 at owner pose 29 of clip 553. */
void reiko_second_stars(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Reiko star barrage task");
  BattleEvent *event = battle_event_start(fighter, n41c4b0_stars_done, false, false);
  require(event != NULL, "Reiko's star barrage needs its bounded thrower task");
  battle_event_program(event, 116);
  for (int star = 0; star < 60; ++star) {
    battle_event_frame(event, 3 * (star / 2) + (star & 1), star, n41c4b0_star_throw);
  }
  battle_event_frame(event, 115, 60, n41c4b0_star_throw);
  BattleEvent *arrivals = battle_event_start(fighter, n41c4b0_arrivals_step, false, false);
  require(arrivals != NULL, "Reiko's star barrage needs its arrival signals");
  arrivals->data[0] = arrivals->data[1] = -1;
  arrivals->data[2] = 0;
}

/* 41f1b0 parks the owner on the final barrage pose. */
static void n41c4b0_arrival_gate(Fighter *victim) {
  victim->mmove_hold = !(fighter_opponent(victim)->native_variant & SECOND_ARRIVED);
}

static void n41c4b0_victim_stagger(Fighter *victim) {
  retail_vm_enter(victim, 0x4d2d60, fighter_ready);
}

/* 41c4b0: a floor pool at joint 4, then 4d2d90. */
void reiko_second_victim_fall(Fighter *victim) {
  retail_vm_enter(victim, 0x4d2d90, fighter_ready);
}

static void n41c4b0_second_end(Fighter *victim) {
  fighter_stop_mmove(victim);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 41c4e0 pools at joint 2 and parks. Retail never ends this scene; as in
 * Fujin's and Cage's ports the result follows after a 60-visit hold. */
void reiko_second_victim_end(Fighter *victim) {
  native_hold(victim, 60, n41c4b0_second_end);
}

void reiko_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n41c4b0_REIKO_FINISH_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 41c430 turns the dizzy victim towards its peer (490bd0) and polls the
   * first-arrival signal before 4d2d60. */
  mk4_script_scene_pose(victim, 13, 0);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  victim->facing = victim->body.pos[AXIS_X] <= fighter->body.pos[AXIS_X] ? 1 : -1;
  native_begin(victim, 0, 0, n41c4b0_victim_stagger);
  native_frame(victim, 0, 0, n41c4b0_arrival_gate);
  native_play(victim, false);
  /* 41c360 state 1: 4611c0 binds the stance clip 284, then 4d2d10. */
  fighter_start_move(fighter, S_PUNCH, &n41c4b0_second_owner);
  fighter->sample_override = 0;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4d2d10, fighter_ready);
}

#undef SECOND_ARRIVED
#undef SECOND_HEADSHOT

/* Native service cluster: 453ae0, 453b20, 453ba0, 454130, 4541b0, 4541e0. */
enum {
  n453ae0_REPTILE_EVENT_FATALITY_1 = 33,
  n453ae0_REPTILE_EVENT_FATALITY_2
};

/* UP record 4e4600 (gate 45e250) targets 467ce0, which selects
 * 4e0498[10] = 4539b0: Reptile's head-bite scene. */
static const MoveDef n453ae0_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4539b0 waits 31 visits before 475250; the stance pose is not advanced. */
static const MoveDef n453ae0_owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 475250 blends 4fc1bc (clip 432) at step 0x32f1 from 0x3333. */
static const SceneClip n453ae0_owner_blend[] = {
  {{.clip = 432, .last = 338, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 453fa0: 475190 blends 4fbfb8 (clip 389), 475340 blends 4fc1b0 (431). */
static const SceneClip n453ae0_victim_stagger[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip n453ae0_victim_bite[] = {
  {{.clip = 431, .last = 414, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

bool reptile_can_first_fatality(const Fighter *fighter) {
  /* 45e250: close gate 45e570 (at most one unit) and the 45e5e0 window,
   * then chord record 4e40d8: HP, HK, LP and LK held; BLOCK and RUN not.
   * Directions are outside the mask; the 53de40 override is not gameplay. */
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && (fighter->held & mask) == chord;
}

static void n453ae0_owner_entry(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4e2e60, fighter_ready);
}

static void n453ae0_owner_blend_start(Fighter *fighter) {
  /* 4539b0 state 1: 43d7d0 restores the stage objects 43d720 lifted,
   * then 475250 blends into 432 before VM 4e2e60. */
  native_scene_blends(fighter, n453ae0_owner_blend, 1, n453ae0_owner_entry);
}

/* 453ae0: roster voice list 4 (no portable bank), cursor 11, VM 4e2e70. */
void reptile_first_owner_cycle(Fighter *fighter) {
  fighter->sample_override = 11;
  retail_vm_enter(fighter, 0x4e2e70, fighter_ready);
}

/* 453b20: cursor 169, voice lists 4/12 and the 453bf0 hand-blood task,
 * then VM 4e2e98 plays out the clip. 4773a0 retires the victim's player
 * slot; the finisher already owns both actors' controls. */
void reptile_first_owner_bite(Fighter *fighter) {
  fighter->sample_override = 169;
  retail_vm_enter(fighter, 0x4e2e98, fighter_ready);
}

/* 453ba0: cursor 294 and VM 4e2ea8, which jumps back to 4e2e98. */
void reptile_first_owner_loop(Fighter *fighter) {
  fighter->sample_override = 294;
  retail_vm_enter(fighter, 0x4e2ea8, fighter_ready);
}

static void n453ae0_victim_entry(Fighter *victim) {
  retail_vm_enter(victim, 0x4e2eb0, fighter_ready);
}

static void n453ae0_victim_join(Fighter *victim) {
  /* 4756f0 places the victim on Reptile's X/Z: both clips share one
   * authored origin. The pair then waits thirty visits. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  native_hold(victim, 30, n453ae0_victim_entry);
}

static void n453ae0_victim_bite_blend(Fighter *victim) {
  /* 475690 copies Reptile's mirror bit and orientation; 475510 keeps
   * the victim on the floor during the 431 blend. */
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  native_scene_blends(victim, n453ae0_victim_bite, 1, n453ae0_victim_join);
}

static void n453ae0_victim_stagger_blend(Fighter *victim) {
  native_scene_blends(victim, n453ae0_victim_stagger, 1, n453ae0_victim_bite_blend);
}

static Vec3 n453ae0_neck(const Fighter *victim) {
  Vec3 point;
  Mat3 basis;
  fighter_joint_position(victim, 2, &point, &basis);
  return point;
}

/* 454130 at 431/86: C-bank cue B[0x48] (453bd0), 4544b0 removes the head
 * node and links a rp_fat_g.geo stump (5083a8, not loaded here), the
 * impact voice, the 454580 gore tasks, then VM 4e2ec0. */
void reptile_first_victim_bite(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->sample_override = 86;
  battle_sound(SOUND_B, 0x48);
  battle_blood_hit(owner, victim, n453ae0_neck(victim), 0x3333);
  victim->hidden_joints[0] |= UINT64_C(1) << 2;
  battle_sound(SOUND_IMPACT_VOICE, roster_def(victim->roster)->asset);
  retail_vm_enter(victim, 0x4e2ec0, fighter_ready);
}

/* 4541b0: B[0x30], then VM 4e2ed0 repeats 85..124. */
void reptile_first_victim_shake(Fighter *victim) {
  battle_sound(SOUND_B, 0x30);
  retail_vm_enter(victim, 0x4e2ed0, fighter_ready);
}

static void n453ae0_victim_spray(Fighter *victim) {
  /* 451ec0 walks 4e2b00: three neck (joint 2) bursts of four drops. */
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  for (int burst = 0; burst < 3; ++burst) {
    battle_blood_hit(owner, victim, n453ae0_neck(victim), 0x2666);
  }
}

static void n453ae0_victim_impact(Fighter *victim) {
  /* 454400, 24 visits after its spawn: voice list 6, a random B[1..2]
   * cue (472800), camera shake 6 (3 pulses, 3 ticks, 0x3333) and 451ec0. */
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  battle_camera_shake(3, 3, 0x3333);
  n453ae0_victim_spray(victim);
}

static void n453ae0_victim_done(Fighter *victim) {
  /* 4541e0 retires the victim task; the portable finisher completes once
   * the last native victim phase has played. */
  fighter_stop_mmove(victim);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 4541e0 from cursor 85: 429490 advances 113, cursor 146 and 100 more,
 * 429040 twenty more, spawns 454400 and advances 119, then 428ea0 plays
 * out clip 431 (415 samples). */
void reptile_first_victim_collapse(Fighter *victim) {
  require(fighter_move(victim)->clip == 431 && fighter_sample(victim) == 85, "Reptile bite victim needs its generated 431 cycles");

  enum {
    n453ae0_PLAYED = 113,
    n453ae0_RESUMED = n453ae0_PLAYED + 100,
    n453ae0_SPAWNED = n453ae0_RESUMED + 20,
    n453ae0_IMPACT = n453ae0_SPAWNED + 24,
    n453ae0_ROWS = n453ae0_SPAWNED + 119 + (414 - 385)
  };

  native_begin(victim, 1, n453ae0_ROWS, n453ae0_victim_done);
  for (int row = 1; row <= n453ae0_ROWS; ++row) {
    int sample = row <= n453ae0_PLAYED ? 85 + row : 146 + (row - n453ae0_PLAYED);
    native_frame(victim, row, sample, row == n453ae0_IMPACT ? n453ae0_victim_impact : NULL);
  }
  native_play(victim, false);
}

void reptile_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n453ae0_REPTILE_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int pose = fighter_sample(fighter);
  fighter_start_move(fighter, S_PUNCH, &n453ae0_owner_wait);
  fighter->sample_override = pose;
  fighter_start_move(victim, S_KNOCKDOWN, &n453ae0_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  /* 4539b0 state 0: 475530 aligns the pair 1.5 units apart; the 454a00
   * camera director and 43d720 stage-object launch are presentation. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  native_hold(fighter, 31, n453ae0_owner_blend_start);
  native_hold(victim, 11, n453ae0_victim_stagger_blend);
}

/* Native service cluster: 41e490, 45ac30, 46a280, 481590, 4817c0. */
enum {
  n41e490_REPTILE_EVENT_FATALITY_1 = 33,
  n41e490_REPTILE_EVENT_FATALITY_2
};

/* HP record 4e457c (U,D,D,D, window 50, gate 45e530) targets 467cd0, which
 * selects 4dfe90[10] = 43bf40: shared setup 41aaf0, then VM 4dff08. */
static const MoveDef n41e490_owner_pose = {.clip = 493, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n41e490_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4818f0 reads 4ea420 in this order, separating one joint every five visits. */
static const int n41e490_torn_joints[] = {2, 10, 9, 8, 7, 6, 5, 3, 12, 11, 14, 13, 16, 15};
/* 41e490 droplets reuse the 4d16d0 shared meshes 340..344; 40f380 adds
 * 0x106 gravity each visit. */
static const ParticleVisual n41e490_acid_visual = {
    .animation = {.first = 340, .count = 5, .period = 2, .scale = 0x51eb, .source = MESH_SHARED, .offset = {0x1100, 0xd80, 0}}};
static const ParticleSpray n41e490_acid_spray = {
    .visual = &n41e490_acid_visual, .count = 2, .lifetime = 30, .gravity = 0x106, .velocity = {0, 0x1708, 0}, .spread = {0x51e, 0x7ae, 0x51e}};

static bool n41e490_acid_drops(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)victim;
  if (event->data[0]-- <= 0 || g_battle.phase == PHASE_ROUND_OVER) {
    return false;
  }
  Vec3 head;
  Mat3 basis;
  if (fighter_joint_position(owner, 2, &head, &basis)) {
    /* 41e490 transforms the head-local point (.01, -.05, -.1) via 4b8140. */
    Vec3 mouth = vec_add(head, mat3_apply(basis, (Vec3){q16_float(0x28f), q16_float(-0xccc), q16_float(-0x1999)}));
    Vec3 forward = {q16_float(fighter_axis(owner, AXIS_X)), 0, q16_float(fighter_axis(owner, AXIS_Z))};
    battle_spray(&n41e490_acid_spray, mouth, forward, 0x1eb8, 0xa3d);
  }
  return true;
}

/* Task 0 (41e490): 121 visits, two droplets each, from Reptile's head
 * along the pair axis at 0x28f5 +/- 0xa3d. The 40f380 victim/floor
 * splash and smoke follow-ups are decorative and not reproduced. */
void reptile_second_acid(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Reptile acid emitter task");
  BattleEvent *event = battle_event_start(fighter, n41e490_acid_drops, false, false);
  if (event != NULL) {
    event->data[0] = 121;
  }
}

/* Reaction 180 (481530): hold 28 visits; 481670 clears the mirror bit
 * and turns the body perpendicular to the pair, then VM 4ea290. */
static void n41e490_second_turn(Fighter *victim) {
  victim->body.angle[AXIS_Y] = -0x191eb;
  retail_vm_enter(victim, 0x4ea290, fighter_ready);
}

void reptile_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &n41e490_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  native_hold(victim, 28, n41e490_second_turn);
}

static void n41e490_second_tear(Fighter *victim) {
  /* 481b80/481c10: closed share2 part (406ce0), ten times the saved
   * drift without facing, Z spin 0x1999, forty-five visits. */
  int index = victim->native_variant++;
  require(index >= 0 && (size_t)index < sizeof n41e490_torn_joints / sizeof n41e490_torn_joints[0], "Invalid Reptile tear phase");
  q16 velocity[3] = {victim->saved_motion[AXIS_X] * 10, 0, victim->saved_motion[AXIS_Z] * 10};
  ScenePiece *piece = battle_detach_piece(victim, n41e490_torn_joints[index], velocity, (q16[3]){0, 0, 0x1999});
  if (piece != NULL && victim->assets != NULL) {
    piece->model = &victim->assets->scene_models.data[SCENE_MODEL_WIND_PIECES + index];
    piece->bone = 0;
    piece->lifetime = 45;
  }
}

static void n41e490_second_flayed(Fighter *victim) {
  /* 481880: stop_motion and 41d770 install the share2 flesh rig 50c444,
   * then 4ea2d0 jumps to the shared fall 4ea2a8. */
  native_motion_stop(victim);
  mk4_script_stop_body(victim);
  memset(victim->hidden_joints, 0, sizeof victim->hidden_joints);
  if (victim->assets != NULL) {
    victim->scene_model = &victim->assets->scene_models.data[SCENE_MODEL_FLAYED_BODY];
  }
  retail_vm_enter(victim, 0x4ea2a8, fighter_ready);
}

/* 481590: save the -0x28f drift, stop motion and run 4818f0, one joint
 * every five visits; the last tear is followed by five more visits. */
void reptile_second_melt(Fighter *victim) {
  victim->saved_motion[AXIS_X] = victim->body.vel[AXIS_X];
  victim->saved_motion[AXIS_Z] = victim->body.vel[AXIS_Z];
  mk4_script_stop_body(victim);
  int count = (int)(sizeof n41e490_torn_joints / sizeof n41e490_torn_joints[0]);
  int pose = fighter_sample(victim);
  victim->native_variant = 0;
  native_begin(victim, 1, count * 5, n41e490_second_flayed);
  for (int visit = 1; visit <= count * 5; ++visit) {
    native_frame(victim, visit, pose, (visit - 1) % 5 == 0 ? n41e490_second_tear : NULL);
  }
  native_play(victim, false);
}

void reptile_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n41e490_REPTILE_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n41e490_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter_start_move(fighter, S_PUNCH, &n41e490_owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->sample_override = 0;
  retail_vm_enter(fighter, 0x4dff08, fighter_ready);
}

/* Native service cluster: 41ea50, 41ebd0, 41ec70, 41ed80, 41f1b0, 443930, 443a30, 443a70, 443b40, 443c10, 444000, 444030, 444060, 444100, 444150, 444190, 483df0, 48a1f0. */
#define BLEND(clip_id, cursor)                                                                                                                                 \
  {{.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, cursor, cursor, 1, 0x3333, 0x3333}
static void n41ea50_finish_scene(Fighter *fighter);
static void n41ea50_burn_start(Fighter *fighter);
static void n41ea50_beast_start(Fighter *fighter);
static void n41ea50_beast_watch(Fighter *fighter);
static void n41ea50_beast_gore_object(Projectile *object);
static const SceneClip n41ea50_burn_repeat_blend[] = {BLEND(390, 105)};
static const SceneClip n41ea50_burn_victory_blend[] = {BLEND(115, 0)};
static const SceneClip n41ea50_burn_victim_blend[] = {BLEND(206, 0)};

static void n41ea50_finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void n41ea50_burn_start(Fighter *fighter) {
  int frames = 8 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, NULL);
    native_script_end(fighter, 0x4e0278, fighter_ready);
  } else {
    retail_vm_enter(fighter, 0x4e0278, fighter_ready);
  }
}

static void n41ea50_beast_start(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4d2e58, fighter_ready);
}

void scorpion_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_BURN || id == FINISH_BEAST, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, id == FINISH_BEAST ? 12 : 390, 0, true);
  FighterFn start = id == FINISH_BURN ? n41ea50_burn_start : n41ea50_beast_start;
  start(fighter);
}

void scorpion_burn_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BURN);
}

void scorpion_beast_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BEAST);
}

void scorpion_script_burn_begin(Fighter *fighter) {
  battle_sound(SOUND_B, 14);
  battle_sound(SOUND_B, 46);
  retail_vm_enter(fighter, 0x4e0288, fighter_ready);
}

void scorpion_script_burn_victim_start(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  victim->appearance = APPEARANCE_BURNED;
  fighter_cancel_mmove(victim);
  retail_vm_enter(victim, 0x4e0300, fighter_ready);
  battle_sound(SOUND_B, 15);
  retail_vm_enter(fighter, 0x4e0298, fighter_ready);
}

void scorpion_script_burn_repeat1(Fighter *fighter) {
  scorpion_native_blend(fighter, n41ea50_burn_repeat_blend, NULL);
  native_script_end(fighter, 0x4e02a8, fighter_ready);
}

void scorpion_script_burn_repeat2(Fighter *fighter) {
  scorpion_native_blend(fighter, n41ea50_burn_repeat_blend, NULL);
  native_script_end(fighter, 0x4e02b8, fighter_ready);
}

void scorpion_script_burn_victory(Fighter *fighter) {
  scorpion_native_blend(fighter, n41ea50_burn_victory_blend, NULL);
  native_script_end(fighter, 0x4e02d0, fighter_ready);
}

void scorpion_script_burn_victim_cue(Fighter *victim) {
  battle_sound(SOUND_B, 16);
  retail_vm_enter(victim, 0x4e0310, fighter_ready);
}

void scorpion_script_burn_victim_repeat(Fighter *victim) {
  retail_vm_enter(victim, 0x4e0320, fighter_ready);
}

void scorpion_script_burn_victim_blend(Fighter *victim) {
  scorpion_native_blend(victim, n41ea50_burn_victim_blend, NULL);
  native_script_end(victim, 0x4e0338, fighter_ready);
}

void scorpion_script_burn_victim_impact(Fighter *victim) {
  mk4_script_shake(victim, 0);
  battle_particles(victim, 0, 4, true);
  retail_vm_enter(victim, 0x4e0348, fighter_ready);
}

void scorpion_script_burn_victim_settle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  retail_vm_enter(victim, 0x4e0358, fighter_ready);
}

void scorpion_script_burn_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, n41ea50_finish_scene);
}

/* Both scene actors have parked their frame programs; the parked poses
 * remain while the finisher completes. */
static void n41ea50_beast_watch(Fighter *fighter) {
  if (fighter->script_phase == 9 && fighter_peer(fighter)->script_phase == 9) {
    battle_end_finisher();
  }
}

void scorpion_script_beast_begin(Fighter *fighter) {
  fighter->appearance = APPEARANCE_BEAST;
  fighter->body.physics = n41ea50_beast_watch;
  retail_vm_enter(fighter, 0x4d2e88, fighter_ready);
}

void scorpion_script_beast_attack(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  fighter_cancel_mmove(victim);
  retail_vm_enter(victim, 0x4d2ef8, fighter_ready);
  retail_vm_enter(fighter, 0x4d2eb0, fighter_ready);
}

void scorpion_script_beast_victim_pieces(Fighter *victim) {
  battle_particles(victim, 0, 12, true);
  retail_vm_enter(victim, 0x4d2f10, fighter_ready);
}

void scorpion_script_beast_voice(Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

static void n41ea50_beast_gore_object(Projectile *object) {
  Fighter *victim = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 4 == 0) {
    battle_particles(victim, 0, 2, true);
  }
}

void scorpion_script_beast_gore(Fighter *victim, int type) {
  require(type == 0, "Unexpected beast gore task");
  battle_spawn_projectile(victim, PROJECTILE_EFFECT, 60, n41ea50_beast_gore_object);
}

#undef BLEND

/* Native service cluster: 4551b0, 455230. */
enum {
  n4551b0_SHINNOK_FATALITY_OWNER_READY = 1,
  n4551b0_SHINNOK_FATALITY_VICTIM_DONE = 2,
  n4551b0_SHINNOK_FATALITY_HAND_DONE = 4,
  n4551b0_SHINNOK_FATALITY_DONE = 7
};

/* Spike-fatality contact 0x4ebd80 uses reaction 175 with zero damage. The
 * stage coordinator owns the victim pose and recovery for that scene. */

/* 45e570: both fatality records accept distance <= 1 and 45e5e0's
 * finisher window. */
// clang-format off: Keep table entries on one row, including long rows.
/* 475430 with descriptor 4fc240 (clip 443) and step 0xccc. */
static const SceneClip n4551b0_victory_blend[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
// clang-format on

static void n4551b0_vanish_tick(Fighter *fighter) {
  /* 455590 then emits 41 visits of owner-joint sparkles (4e3170); the
   * portable scene keeps only this cue. The track ends on this row. */
  (void)fighter;
  battle_sound(SOUND_B, 0x4c);
}

/* 4554d0: a child task sleeps five ticks, then queues B 0x4c. */
static void n4551b0_vanish_cue(Fighter *fighter) {
  native_motion_begin(fighter, 5, NULL);
  native_motion_frame(fighter, 4, NULL, 0, n4551b0_vanish_tick);
  native_motion_play(fighter);
}

/* 455000 state 1 and 4552e0: B 0x5c and the 4554d0 child as clip 12 starts. */
void shinnok_fatality_vanish_cue(Fighter *fighter) {
  battle_sound(SOUND_B, 0x5c);
  n4551b0_vanish_cue(fighter);
}

/* 455190: the child cue again, then clip 436 to sample 18 (4e2f10). */
void shinnok_fatality_return(Fighter *fighter) {
  n4551b0_vanish_cue(fighter);
  retail_vm_enter(fighter, 0x4e2f10, fighter_ready);
}

static void n4551b0_reappear_land(Fighter *fighter) {
  /* 455370 clears gravity and restores the floor Y; 455460 then cues B 0x47. */
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  battle_sound(SOUND_B, 0x47);
}

/* 4551b0: cursor 19, clear hidden bit 4, then an independent leap with
 * gravity 0x28f and Y speed -0x2666 while 4e2f20 finishes clip 436. */
void shinnok_fatality_reappear(Fighter *fighter) {
  fighter->sample_override = 19;
  fighter->appearance = APPEARANCE_NORMAL;
  fighter->body.vel[AXIS_Y] = -0x2666;
  fighter->body.gravity = 0x28f;
  fighter->body.on_land = n4551b0_reappear_land;
  retail_vm_enter(fighter, 0x4e2f20, fighter_ready);
}

static void n4551b0_victory_loop(Fighter *fighter) {
  fighter->state = S_VICTORY;
  fighter->native_variant |= n4551b0_SHINNOK_FATALITY_OWNER_READY;
  retail_vm_enter(fighter, 0x4e2f30, fighter_ready);
  shinnok_fatality_maybe_complete(fighter);
}

void shinnok_fatality_victory(Fighter *fighter) {
  native_scene_blends(fighter, n4551b0_victory_blend, 1, n4551b0_victory_loop);
}

/* The retail scene leaves its final bodies posed; the portable finisher
 * completes once the owner loops 443 and every scene body has finished. */
void shinnok_fatality_maybe_complete(Fighter *owner) {
  if ((owner->native_variant & n4551b0_SHINNOK_FATALITY_DONE) == n4551b0_SHINNOK_FATALITY_DONE && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* Both fatalities start from a locked standing owner (stance clip 283,
 * 53d408[6]) and the dizzy victim. */
void shinnok_fatality_begin(Fighter *fighter, unsigned done) {
  static const MoveDef owner_pose = {.clip = 283, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_variant = (int)(n4551b0_SHINNOK_FATALITY_DONE & ~done);
  mk4_script_scene_pose(victim, 13, 0);
}

/* Native service cluster: 41db70, 41dba0, 41dbd0, 43c820, 43ca30, 455330, 455360, 45ac30. */
enum {
  n41db70_SHINNOK_FINISH_1 = 21,
  n41db70_SHINNOK_FINISH_2
};

enum {
  n41db70_SHINNOK_FATALITY_OWNER_READY = 1,
  n41db70_SHINNOK_FATALITY_VICTIM_DONE = 2,
  n41db70_SHINNOK_FATALITY_HAND_DONE = 4,
  n41db70_SHINNOK_FATALITY_DONE = 7
};

/* Fatality II: 467cd0 -> 43bd70 -> 4dfe90[6] = 43c6d0. The victim floats
 * up while Shinnok vanishes; two mirrored giant hands (rig 5083c4) clap
 * and burst it, then he leaps back in. */
static const MoveDef n41db70_clap_hand_pose = {.clip = 541, .last = 199, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef n41db70_victim_pose = {.clip = 542, .last = 11, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};

/* Rows of the independent 46a280/46a380 lift task. */
#define LIFT_ROWS 400

static void n41db70_lift_gravity(Fighter *victim, float gravity) {
  victim->body.gravity = (q16)gravity;
}

/* 46a280: gravity -0x28f, then +0x41 for nine visits; the tenth enters
 * 46a340 (gravity 0x83 until Y speed >= 0). 46a380 then holds 10 ticks,
 * reverses to -0x83 until the speed is upward, holds 10 and repeats.
 * The velocity tests are resolved here from the deterministic motion. */
static void n41db70_lift_victim(Fighter *victim) {
  native_motion_begin(victim, LIFT_ROWS, NULL);
  q16 gravity = -0x28f, speed = 0;
  int row = 0;
  native_motion_frame(victim, row++, n41db70_lift_gravity, (float)gravity, NULL);
  speed += gravity;
  native_motion_frame(victim, row++, n41db70_lift_gravity, (float)gravity, NULL);
  speed += gravity;
  for (int visit = 0; visit < 10; ++visit) {
    gravity = visit == 9 ? 0x83 : gravity + 0x41;
    native_motion_frame(victim, row++, n41db70_lift_gravity, (float)gravity, NULL);
    speed += gravity;
  }
  bool falling = true;
  while (row < LIFT_ROWS) {
    /* 48f490 / 48f510 yield once before testing the live speed. */
    for (int test = 0; row < LIFT_ROWS && (test == 0 || (falling ? speed < 0 : speed >= 0)); ++test) {
      native_motion_frame(victim, row++, n41db70_lift_gravity, (float)gravity, NULL);
      speed += gravity;
    }
    for (int hold = 0; hold < 10 && row < LIFT_ROWS; ++hold) {
      native_motion_frame(victim, row++, n41db70_lift_gravity, (float)gravity, NULL);
      speed += gravity;
    }
    falling = !falling;
    gravity = falling ? 0x83 : -0x83;
  }
  native_motion_play(victim);
}

/* 4ea4e8[179] = 481510: start the lift task and stream 4ea270. */
void shinnok_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &n41db70_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  n41db70_lift_victim(victim);
  retail_vm_enter(victim, 0x4ea270, fighter_ready);
}

void shinnok_second_vanish(Fighter *fighter) {
  /* 43c820 state 0 -> 4552e0. */
  shinnok_fatality_vanish_cue(fighter);
  retail_vm_enter(fighter, 0x4e2fc8, fighter_ready);
}

void shinnok_second_vanished(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  retail_vm_enter(fighter, 0x4e2fd8, fighter_ready);
}

static void n41db70_hand_model(Fighter *hand) {
  if (hand->assets != NULL) {
    hand->scene_model = &hand->assets->shinnok_hand;
  }
}

static void n41db70_first_hand_entry(Fighter *hand) {
  n41db70_hand_model(hand);
  retail_vm_enter(hand, 0x4e00b8, fighter_ready);
}

static void n41db70_second_hand_entry(Fighter *hand) {
  n41db70_hand_model(hand);
  /* 490350 toggles the second hand's facing bit. */
  hand->facing = -hand->facing;
  hand->heading[0] = -hand->heading[0];
  hand->heading[1] = -hand->heading[1];
  retail_vm_enter(hand, 0x4e00e0, fighter_ready);
}

/* 43caa0: rig 5083c4 at the victim's X, Z zero, with the owner's yaw and
 * facing; the group Y keeps the object default floor. */
static void n41db70_place_hand(Fighter *hand, const Fighter *victim) {
  hand->body.pos[AXIS_X] = victim->body.pos[AXIS_X];
  hand->body.pos[AXIS_Y] = 0;
  hand->body.pos[AXIS_Z] = 0;
  hand->body.ground = 0;
}

void shinnok_second_hands(Fighter *fighter) {
  /* 455360 returns to 43c820 state 1: 5322c4, both hand tasks, wait 300. */
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  Fighter *first = battle_scene_actor_start(fighter, fighter, &n41db70_clap_hand_pose, n41db70_first_hand_entry);
  Fighter *second = battle_scene_actor_start(fighter, fighter, &n41db70_clap_hand_pose, n41db70_second_hand_entry);
  require(first != NULL && second != NULL, "Shinnok clap needs both bounded scene slots");
  n41db70_place_hand(first, victim);
  n41db70_place_hand(second, victim);
  /* 43c820 state 2 enqueues camera context 0x34, then 455190. */
  native_hold(fighter, 300, shinnok_fatality_return);
}

/* 43ca30: 4900b0 hides the victim and 41d550 bursts it; the hand tail
 * 4e00f8 continues. The portable burst uses the shared blood particles. */
void shinnok_second_clap(Fighter *hand) {
  Fighter *victim = NULL;
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (&g_battle.scene_actors[i].actor == hand) {
      victim = &g_battle.fighters[1 - g_battle.scene_actors[i].owner];
    }
  }
  require(victim != NULL, "Shinnok clap needs its scene owner");
  battle_particles(victim, 1, 24, false);
  native_motion_stop(victim);
  mk4_script_stop_body(victim);
  victim->appearance = APPEARANCE_HIDDEN;
  retail_vm_enter(hand, 0x4e00f8, fighter_ready);
}

/* 41db70/41dba0/41dbd0 -> 41dc00 allocate generic flesh pieces from
 * tables 4d3a18/4d3c80/4d3d88 at the hand. */
void shinnok_second_gore(Fighter *hand) {
  battle_particles(hand, 3, 12, false);
}

void shinnok_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n41db70_SHINNOK_FINISH_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  shinnok_fatality_begin(fighter, n41db70_SHINNOK_FATALITY_OWNER_READY);
  /* 43c2f0/43c330: owner at the arena origin, victim one unit to the side
   * it already occupies (48f030), both on Z zero. */
  q16 side = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? Q16_ONE : -Q16_ONE;
  fighter->body.pos[AXIS_X] = fighter->body.pos[AXIS_Z] = 0;
  victim->body.pos[AXIS_X] = side;
  victim->body.pos[AXIS_Z] = 0;
  retail_vm_enter(fighter, 0x4e0088, fighter_ready);
}

#undef LIFT_ROWS

/* Native service cluster: native_fatality_2. */
/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
void sonya_scene_end_watch(Fighter *fighter) {
  int clip = fighter_move(fighter)->clip;
  if (g_battle.phase != PHASE_FATALITY) {
    fighter->body.physics = NULL;
    return;
  }
  if (fighter_sample(fighter) == fighter_move(fighter)->last && (clip == 573 || clip == 388 || clip == 114)) {
    fighter->body.physics = NULL;
    battle_end_finisher();
  }
}

/* Native service cluster: 465e10, 465e70, 465ea0, 465ed0, 466000, 466190. */
/* 465ed0 holds 15 visits before returning to the two-cycle coordinator. */
void sonya_script_second_pause(Fighter *fighter) {
  native_hold(fighter, 15, sonya_script_second_cycle_end);
}

/* 466000 returns to the native coordinator: script_phase counts the cycles
 * that remain, and the result pose follows the second. */
void sonya_script_second_cycle_end(Fighter *fighter) {
  if (--fighter->script_phase > 0) {
    retail_vm_enter(fighter, 0x4e5960, fighter_ready);
    return;
  }
  fighter->body.physics = sonya_scene_end_watch;
  retail_vm_enter(fighter, 0x4e59a8, fighter_ready);
}

void sonya_script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    retail_vm_enter(fighter, 0x4e5980, fighter_ready);
  } else {
    retail_vm_enter(fighter, 0x4e5990, fighter_ready);
  }
}

void sonya_script_second_repeat(Fighter *fighter) {
  fighter->script_phase = 2;
  retail_vm_enter(fighter, 0x4e5960, fighter_ready);
}

void sonya_script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  retail_vm_enter(fighter, 0x4e5940, fighter_ready);
}

void sonya_script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  retail_vm_enter(victim, 0x4e5a20, fighter_ready);
}

void sonya_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SONYA_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 558, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 13, 0);
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  retail_vm_enter(fighter, 0x4e5930, fighter_ready);
}

/* Native service cluster: native_fatality_3. */

bool sub_zero_can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

void sub_zero_finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

void sub_zero_scene_blend_cleanup(Fighter *fighter) {
  fighter->scene.clips = NULL;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

void sub_zero_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_SUB_ZERO_1 || id == FINISH_SUB_ZERO_2, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, id == FINISH_SUB_ZERO_1 ? 385 : 552, 0, true);
  if (id == FINISH_SUB_ZERO_1) {
    /* 445100 -> 475530 aligns the scene pair at 1.45 units. */
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x17333);
    }
  }
  FighterFn start = id == FINISH_SUB_ZERO_1 ? sub_zero_first_fatality_script : sub_zero_second_fatality_script;
  start(fighter);
}

/* Native service cluster: 41b080, 41b440, 41b570, 41b860, 41bbb0. */
static void n41b080_breath_object(Projectile *p);
static void n41b080_breath_freeze_object(Projectile *object);
static void n41b080_breath_resume(Fighter *fighter);

void sub_zero_script_breath_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
}

static void n41b080_breath_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (owner->action != ACTION_RETAIL_0700 || fighter_move(owner)->clip != 552) {
    p->kind = 0;
    return;
  }
  battle_particles(owner, 9 + (p->ticks & 1), 2, true);
}

void sub_zero_script_breath_object(Fighter *fighter, int type) {
  require(type == 0, "Unexpected breath task");
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 92, n41b080_breath_object);
}

static void n41b080_breath_freeze_object(Projectile *object) {
  Fighter *victim = fighter_peer(&g_battle.fighters[object->owner]);
  if (victim->appearance != APPEARANCE_FROZEN || g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 8 == 0) {
    battle_particles(victim, 9, 1, true);
  }
}

void sub_zero_script_breath_freeze(Fighter *fighter, int type) {
  require(type == 0, "Unexpected freeze task");
  fighter_peer(fighter)->appearance = APPEARANCE_FROZEN;
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 120, n41b080_breath_freeze_object);
}

void sub_zero_script_breath_approach(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4d2c78, fighter_ready);
}

static void n41b080_breath_resume(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->script_phase = 3;
  retail_vm_enter(fighter, 0x4d2ca0, fighter_ready);
}

void sub_zero_breath_travel(Fighter *fighter) {
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  int frames = maxi(0, q16_mul(distance - 0xcccc, 0x69999) / Q16_ONE) + 1;
  fighter->script_phase = 2;
  fighter->sample_override = 9;
  native_hold(fighter, frames, n41b080_breath_resume);
}

void sub_zero_script_breath_shatter(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  battle_particles(victim, 3, 32, true);
  battle_shatter(victim);
  retail_vm_enter(fighter, 0x4d2cc8, fighter_ready);
}

/* Native service cluster: native_fatality_second_3. */
void sub_zero_second_fatality_script(Fighter *fighter) {
  retail_vm_enter(fighter_peer(fighter), 0x4d2bf0, fighter_ready);
  retail_vm_enter(fighter, 0x4d2c10, fighter_ready);
}

void sub_zero_second_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SUB_ZERO_2);
}

/* Native service cluster: 45ac30, 4650b0, 465130, 465270, 465390, 466df0. */
enum {
  n45ac30_TANYA_EVENT_FATALITY_1 = 35,
  n45ac30_TANYA_EVENT_FATALITY_2
};

/* HK D,F,D,F record 4e453c -> 467cd0 -> 43bd70 -> 4dfe90[9] = 464f30. */
static const MoveDef n45ac30_victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 4611c0 selects the roster stance 53d408[9] = clip 439. */
static const MoveDef n45ac30_owner_pose = {.clip = 439, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

/* 45e570: close fatality range (<= 1 unit) and the shared finish window. */
/* Task-619 cue lists: {visits before the action, token or random list}. */
// clang-format off: Keep table entries on one row, including long rows.
static const int n45ac30_owner_cue[][2] = {{33, 0xd2}, {20, 0x281}};
static const int n45ac30_victim_cue[][2] = {{45, RETAIL_SOUND_FOOTSTEP_LIST}, {12, RETAIL_SOUND_FOOTSTEP_LIST}, {8, 0x14dc}, {20, 0x14e1}, {23, 0x14d7}, {13, 0xbe}, {15, 0x14dc}, {20, 0x14dc}, {33, 0x2f8}};
// clang-format on

static bool n45ac30_cue_frame(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)owner;
  (void)opponent;
  if (event->frame_value == RETAIL_SOUND_FOOTSTEP_LIST) {
    /* 48a2a0 -> 489e80: one of four list 4ea8b0 tokens. */
    retail_native_sound_list(RETAIL_SOUND_FOOTSTEP_LIST);
  } else {
    battle_sound(SOUND_TOKEN, event->frame_value);
  }
  return true;
}

/* Each task-619 state sleeps, plays its token and selects the next state. */
static bool n45ac30_cue_next(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)owner;
  (void)opponent;
  const int (*steps)[2] = event->data[0] ? n45ac30_victim_cue : n45ac30_owner_cue;
  int count = event->data[0] ? (int)(sizeof n45ac30_victim_cue / sizeof n45ac30_victim_cue[0]) : (int)(sizeof n45ac30_owner_cue / sizeof n45ac30_owner_cue[0]);
  int index = event->data[1]++;
  if (index >= count) {
    return false;
  }
  battle_event_program(event, steps[index][0]);
  battle_event_frame(event, steps[index][0] - 1, steps[index][1], n45ac30_cue_frame);
  return true;
}

static void n45ac30_start_cue(Fighter *fighter, bool victim) {
  BattleEvent *event = battle_event_start(fighter, n45ac30_cue_next, false, false);
  event->data[0] = victim;
  event->data[1] = 0;
  n45ac30_cue_next(event, fighter, (Fighter *)fighter_opponent(fighter));
}

/* 465270: wait 33, token 4dd8fc (0xd2), wait 20, token 4dd8f8 (0x281). */
void tanya_second_owner_cue(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Tanya fatality cue task");
  n45ac30_start_cue(fighter, false);
}

/* 465390 jump table 46576c: 45, list, 12, list, 8, 14dc, 20, 14e1, 23,
 * 14d7, 13, be, 15, 14dc, 20, 14dc, 33, then 2f8 before retiring. */
void tanya_second_victim_cue(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Tanya fatality victim cue task");
  n45ac30_start_cue(fighter, true);
}

/* 4650b0: list 4e33e8 (Sonya, Tanya, Kitana) lowers the owner 0x147a. */
void tanya_second_height(Fighter *fighter) {
  int roster = fighter_opponent(fighter)->roster;
  if (roster == 2 || roster == 9 || roster == 16) {
    fighter->body.pos[AXIS_Y] += 0x147a;
  }
  retail_vm_enter(fighter, 0x4e57c0, fighter_ready);
}

static void n45ac30_victory_finish(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void n45ac30_victory_watch(Fighter *fighter) {
  if (fighter_move(fighter)->clip == 124 && fighter->native_motion.count == 0) {
    native_motion_begin(fighter, 50, n45ac30_victory_finish);
    native_motion_play(fighter);
    native_motion_step(fighter);
  }
}

/* 4712e0 enters the looping 4e6a40/4e6b88 victory. The portable finisher
 * ends 50 visits into that loop, as in Sub-Zero's port. */
void tanya_script_victory(Fighter *fighter) {
  fighter->body.physics = n45ac30_victory_watch;
  retail_vm_enter(fighter, 0x4e6a40, fighter_ready);
}

static void n45ac30_second_victory(Fighter *fighter) {
  /* 490c00 faces the peer and flips the facing bit before 4712e0. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? -1 : 1;
  tanya_script_victory(fighter);
}

/* 465130: stance pose, face the peer, then 120 visits before victory. */
void tanya_second_finish(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  mk4_mmove_bind(fighter, n45ac30_owner_pose.clip, n45ac30_owner_pose.last + 1, 0);
  native_hold(fighter, 120, n45ac30_second_victory);
}

void tanya_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, n45ac30_TANYA_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &n45ac30_victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(victim, 0x4e5b38, fighter_ready);
  /* 475530 places the victim 1.0 ahead; 4906c0 grounds the owner. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), Q16_ONE);
  }
  fighter_start_move(fighter, S_PUNCH, &n45ac30_owner_pose);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter(fighter, 0x4e5798, fighter_ready);
}

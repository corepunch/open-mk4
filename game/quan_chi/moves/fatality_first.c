#include "../character.h"

#define QC_COUNT(array) ((int)(sizeof(array) / sizeof((array)[0])))

/* LK-release [F,D,F] record 4e4878 (predicate 45e2f0) enters 467ce0, whose
 * table 4e0498[8] selects 44bcd0: Quan Chi tears off the victim's leg and
 * beats the victim with it. Owner clip 420 then 421 loops; the victim
 * blends 389 -> 424 then loops 417; the leg runs 422 then loops 423. */
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip owner_blends[] = {
  {{.clip = 420, .last = 251, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_blends[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 424, .last = 251, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 48a910 camera tracks 4e1a80 (3 + 4 controls) and 4e1ad8 (4 + 4 controls),
 * both stepped at 0xccc; 44c7dc holds 45 ticks between them. Retail X,Z,Y
 * records are stored here as X,Y,Z. */
static const q16 camera_side[][3] = {
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
  {0, -0x20000, -0x60000},
};
static const q16 camera_orbit[][3] = {
  {0x88000, -0x1b333, -0x80000},
  {0x60000, -0x18000, 0x60000},
  {0x48000, -0x14ccc, 0x40000},
  {-0x80000, -0x14ccc, 0x80000},
  {-0x80000, -0x14ccc, -0x80000},
  {0x80000, -0x14ccc, -0x80000},
  {0x80000, -0x14ccc, 0x80000},
  {-0x80000, -0x14ccc, 0x80000},
};
static const CameraShot camera_shots[] = {
  {camera_side, QC_COUNT(camera_side), 45, 0xccc, 0, 0, 0},
  {camera_orbit, QC_COUNT(camera_orbit), 0, 0xccc, 0, 0, 0},
};
// clang-format on
/* The owner keeps his stance pose through 44bd9b's sixteen-tick setup. */
static const MoveDef owner_hold = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef leg_move = {.clip = 422, .last = 134, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 44c550 -> 473ed0: stump blood from joint 12 for 0x8a ticks after ten. */
static const ParticleVisual stump_blood_visual = {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE}};
static const ParticleSpray stump_blood = {
    .visual = &stump_blood_visual, .count = 1, .lifetime = 120, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase stump_phases[] = {
  {10, 10, -1, 0},
  {138, 2, -1, 1},
};
// clang-format on
static const JointSprayDef stump_spray = {
    .spray = &stump_blood, .phases = stump_phases, .count = QC_COUNT(stump_phases), .origin_joint = 12, .base_joint = 3, .speed = 0x23d7, .variation = 0x7ae};

enum {
  /* Camera task: one tick, side track (5 segments), hold 45, orbit track
   * (6 segments); the beating loops continue until that program ends. */
  SCENE_TICKS = 1 + 5 * 20 + 45 + 6 * 20,
};

/* 472800: hit sound B[1|2] and 457b50 shake. The 4727e0 victim voice lists
 * 5 and 3 are not reproduced; the portable roster voices cover lists 1/8. */
static void beat_cue(Fighter *fighter, int shake) {
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  mk4_script_shake(fighter, shake);
}

void quan_chi_fatality_cue_40(Fighter *fighter) {
  quan_chi_entry_retail_script_004e1508(fighter);
}

void quan_chi_fatality_hit_179(Fighter *fighter) {
  beat_cue(fighter, 5);
  quan_chi_entry_retail_script_004e1528(fighter);
}

void quan_chi_fatality_hit_241(Fighter *fighter) {
  beat_cue(fighter, 7);
  quan_chi_entry_retail_script_004e1538(fighter);
}

void quan_chi_fatality_beat(Fighter *fighter) {
  beat_cue(fighter, 7);
  quan_chi_entry_retail_script_004e1558(fighter);
}

static void leg_entry(Fighter *leg) {
  if (leg->assets != NULL) {
    leg->scene_model = &leg->assets->scene_models.data[SCENE_MODEL_SEVERED_LEGS + roster_def(leg->roster)->asset];
  }
  quan_chi_entry_retail_script_004e1580(leg);
}

/* 44be40 -> task 44c0f0: sound B[0], victim voice list 8, 476d80 detaches
 * victim joint 12 into the copied leg (44ccf0) at the victim origin, whose
 * VM 4e1580 plays clip 422 and loops 423; 44c280/44c550 bleed. */
void quan_chi_fatality_leg(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_sound(SOUND_B, 0);
  mk4_script_roster_sound(victim, 8);
  victim->hidden_joints[0] |= UINT64_C(1) << 12;
  Fighter *leg = battle_scene_actor_start(fighter, victim, &leg_move, leg_entry);
  require(leg != NULL, "Quan Chi's torn leg needs its bounded scene slot");
  Vec3 hip;
  Mat3 unused;
  fighter_joint_position(victim, 12, &hip, &unused);
  battle_blood_hit(fighter, victim, hip, 0x3333);
  battle_joint_spray(victim, &stump_spray);
  quan_chi_entry_retail_script_004e1518(fighter);
}

static void victim_stream(Fighter *victim) {
  quan_chi_entry_retail_script_004e1568(victim);
}

static void victim_blend(Fighter *victim) {
  native_scene_blends(victim, victim_blends, QC_COUNT(victim_blends), victim_stream);
}

static void owner_stream(Fighter *fighter) {
  quan_chi_entry_retail_script_004e14f8(fighter);
}

static void owner_blend(Fighter *fighter) {
  native_scene_blends(fighter, owner_blends, QC_COUNT(owner_blends), owner_stream);
}

static void scene_camera(Fighter *fighter) {
  battle_camera_sequence(fighter, fighter_opponent(fighter), camera_shots, QC_COUNT(camera_shots));
}

/* The native beating loops never end themselves. The portable result is
 * declared when the 44c700 camera program finishes; owner, victim and leg
 * keep looping under it, as the retail tasks do. */
static void scene_complete(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

void quan_chi_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, QUAN_CHI_FINISH_LEG)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &owner_hold);
  mk4_script_scene_pose(victim, 13, 56, 0);
  /* 44bd9b -> 475530 aligns the scene pair at 1.1 units. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x11999);
  }
  native_hold(victim, 11, victim_blend);
  native_hold(fighter, 16, owner_blend);
  /* Camera task 44c700 and the scene clock run beside actor playback. */
  native_motion_begin(fighter, SCENE_TICKS, scene_complete);
  native_motion_frame(fighter, 1, NULL, 0, scene_camera);
  native_motion_play(fighter);
}

bool quan_chi_can_first_fatality(const Fighter *fighter) {
  /* 45e2f0: 45e570's close gate, then LK held for at least 0xf0 ticks. */
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 0xf0;
}

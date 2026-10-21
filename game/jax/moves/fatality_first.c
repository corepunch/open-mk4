#include "retail_vm.h"
#include "../character.h"

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

static const MoveDef first_owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip first_owner_blend[] = {
  {{.clip = 384, .last = 249, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip first_victory_blend[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0xccc},
};
static const SceneClip first_victim_stand[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip first_victim_scene[] = {
  {{.clip = 204, .last = 385, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
// clang-format on

/* 449b00/44a080: 476d80 hides victim node 5/6, 44a530/44a790 rebuild that
 * arm as a three-node object placed at the owner's origin and heading, and
 * 4e1400/4e1420 play clip 446 to 145 or 447 to 120 on it. */
static const AttachmentDef first_left_arm = {.clip = 446, .first = 0, .last = 171, .loop_from = 171, .scene_space = true, .limb = true};
static const AttachmentDef first_right_arm = {.clip = 447, .first = 0, .last = 120, .loop_from = 120, .scene_space = true, .limb = true};

enum {
  FIRST_SECOND_ARM = 129,
  FIRST_SLAM = 306,
  FIRST_DROP = 376,
  /* The first arm advances on its own task clock from victim pose 79;
   * 4e13e0 spends 49 poses before the native rows start at pose 129. */
  FIRST_ARM_OFFSET = 49,
  FIRST_ARM_IMPACT = 146,
  FIRST_ARM_END = 171
};

/* 45e1b0: the 45e570 close gate plus 240 ticks since the LK press. */
bool jax_can_fatality_1(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 240;
}

static bool first_listed_victim(const Fighter *victim) {
  static const int rosters[] = {4, 0, 12, 8, 7, 13, 14};
  for (size_t i = 0; i < sizeof rosters / sizeof rosters[0]; ++i) {
    if (victim->roster == rosters[i]) {
      return true;
    }
  }
  return false;
}

static void first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static void first_tear(Fighter *victim, int joint, const AttachmentDef *def) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  Vec3 shoulder;
  Mat3 unused;
  fighter_joint_position(victim, joint, &shoulder, &unused);
  battle_attach_piece_model(battle_detach_piece(victim, joint, (q16[3]){0}, (q16[3]){0}), def, owner);
  battle_blood_hit(owner, victim, shoulder, 0x3333);
}

static void first_owner_loop(Fighter *owner) {
  owner->state = S_VICTORY;
  owner->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter_owned(owner, 0x4e13a0, fighter_ready, 13);
  first_maybe_complete(owner);
}

/* 449620 -> 475430: blend into 443 before the 4e13a0 loop. */
void jax_fatality_1_victory(Fighter *owner) {
  native_scene_blends(owner, first_victory_blend, 1, first_owner_loop);
}

static void first_owner_start(Fighter *owner) {
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(owner, first_owner_blend, 1, jax_entry_retail_script_004e1390);
}

static void first_victim_stream(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  if (first_listed_victim(victim)) {
    retail_vm_enter_owned(victim, 0x4e13b0, fighter_ready, 13);
  } else {
    retail_vm_enter_owned(victim, 0x4e13c0, fighter_ready, 13);
  }
}

static void first_victim_face(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  native_scene_blends(victim, first_victim_scene, 1, first_victim_stream);
}

static void first_victim_start(Fighter *victim) {
  native_scene_blends(victim, first_victim_stand, 1, first_victim_face);
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
  first_tear(victim, 5, &first_left_arm);
  battle_sound(SOUND_B, 0);
  mk4_script_roster_sound(victim, 8);
  victim->sample_override = 79;
  retail_vm_enter_owned(victim, 0x4e13e0, fighter_ready, 13);
}

static void first_second_arm(Fighter *victim) {
  first_tear(victim, 6, &first_right_arm);
  battle_sound(SOUND_B, 0);
}

/* 449ca0/449cf0 on the first arm's own clock: B[0x17] and shake 7. */
static void first_arm_impact(Fighter *victim) {
  battle_sound(SOUND_B, 0x17);
  mk4_script_shake(victim, 7);
}

static void first_slam(Fighter *victim) {
  battle_sound(SOUND_B, 0x12);
  mk4_script_shake(victim, 5);
}

/* 472800: B[2 + random(2)], then shake 5 before 4e13f0. */
static void first_drop(Fighter *victim) {
  battle_sound(SOUND_B, 2 + (int)battle_random(2));
  mk4_script_shake(victim, 5);
}

/* 449940: cursor 129 and the second arm (44a080), 428d70 to 305; cursor
 * 306 with B[0x12] and shake 5, to 375; cursor 376, then 4e13f0. */
void jax_fatality_1_second_arm(Fighter *victim) {
  native_begin(victim, FIRST_SECOND_ARM, FIRST_DROP, jax_entry_retail_script_004e13f0);
  for (int sample = FIRST_SECOND_ARM; sample <= FIRST_DROP; ++sample) {
    FighterFn action = NULL;
    int arm = sample - FIRST_SECOND_ARM + FIRST_ARM_OFFSET;
    if (sample == FIRST_SECOND_ARM) {
      action = first_second_arm;
    } else if (sample == FIRST_SLAM) {
      action = first_slam;
    } else if (sample == FIRST_DROP) {
      action = first_drop;
    } else if (arm == FIRST_ARM_IMPACT || arm == FIRST_ARM_END) {
      action = first_arm_impact;
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
    first_maybe_complete(owner);
  }
}

void jax_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 4494c0 phase 0 leaves the owner's current pose untouched for 16 ticks. */
  fighter_start_move(fighter, S_PUNCH, &first_owner_wait);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x11999);
  }
  mk4_script_scene_pose(victim, 13, 0);
  native_hold(victim, 11, first_victim_start);
  native_hold(fighter, 16, first_owner_start);
}

#include "native_services.h"

enum {
  REPTILE_EVENT_FATALITY_1 = 33,
  REPTILE_EVENT_FATALITY_2
};

/* UP record 4e4600 (gate 45e250) targets 467ce0, which selects
 * 4e0498[10] = 4539b0: Reptile's head-bite scene. */
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4539b0 waits 31 visits before 475250; the stance pose is not advanced. */
static const MoveDef owner_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 475250 blends 4fc1bc (clip 432) at step 0x32f1 from 0x3333. */
static const SceneClip owner_blend[] = {
  {{.clip = 432, .last = 338, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 453fa0: 475190 blends 4fbfb8 (clip 389), 475340 blends 4fc1b0 (431). */
static const SceneClip victim_stagger[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_bite[] = {
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

static void owner_entry(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e60, fighter_ready, 10);
}

static void owner_blend_start(Fighter *fighter) {
  /* 4539b0 state 1: 43d7d0 restores the stage objects 43d720 lifted,
   * then 475250 blends into 432 before VM 4e2e60. */
  native_scene_blends(fighter, owner_blend, 1, owner_entry);
}

/* 453ae0: roster voice list 4 (no portable bank), cursor 11, VM 4e2e70. */
void reptile_first_owner_cycle(Fighter *fighter) {
  fighter->sample_override = 11;
  retail_vm_enter_owned(fighter, 0x4e2e70, fighter_ready, 10);
}

/* 453b20: cursor 169, voice lists 4/12 and the 453bf0 hand-blood task,
 * then VM 4e2e98 plays out the clip. 4773a0 retires the victim's player
 * slot; the finisher already owns both actors' controls. */
void reptile_first_owner_bite(Fighter *fighter) {
  fighter->sample_override = 169;
  retail_vm_enter_owned(fighter, 0x4e2e98, fighter_ready, 10);
}

/* 453ba0: cursor 294 and VM 4e2ea8, which jumps back to 4e2e98. */
void reptile_first_owner_loop(Fighter *fighter) {
  fighter->sample_override = 294;
  retail_vm_enter_owned(fighter, 0x4e2ea8, fighter_ready, 10);
}

static void victim_entry(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4e2eb0, fighter_ready, 10);
}

static void victim_join(Fighter *victim) {
  /* 4756f0 places the victim on Reptile's X/Z: both clips share one
   * authored origin. The pair then waits thirty visits. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  native_hold(victim, 30, victim_entry);
}

static void victim_bite_blend(Fighter *victim) {
  /* 475690 copies Reptile's mirror bit and orientation; 475510 keeps
   * the victim on the floor during the 431 blend. */
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  native_scene_blends(victim, victim_bite, 1, victim_join);
}

static void victim_stagger_blend(Fighter *victim) {
  native_scene_blends(victim, victim_stagger, 1, victim_bite_blend);
}

static Vec3 neck(const Fighter *victim) {
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
  battle_blood_hit(owner, victim, neck(victim), 0x3333);
  victim->hidden_joints[0] |= UINT64_C(1) << 2;
  battle_sound(SOUND_IMPACT_VOICE, roster_def(victim->roster)->asset);
  retail_vm_enter_owned(victim, 0x4e2ec0, fighter_ready, 10);
}

/* 4541b0: B[0x30], then VM 4e2ed0 repeats 85..124. */
void reptile_first_victim_shake(Fighter *victim) {
  battle_sound(SOUND_B, 0x30);
  retail_vm_enter_owned(victim, 0x4e2ed0, fighter_ready, 10);
}

static void victim_spray(Fighter *victim) {
  /* 451ec0 walks 4e2b00: three neck (joint 2) bursts of four drops. */
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  for (int burst = 0; burst < 3; ++burst) {
    battle_blood_hit(owner, victim, neck(victim), 0x2666);
  }
}

static void victim_impact(Fighter *victim) {
  /* 454400, 24 visits after its spawn: voice list 6, a random B[1..2]
   * cue (472800), camera shake 6 (3 pulses, 3 ticks, 0x3333) and 451ec0. */
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
  battle_camera_shake(3, 3, 0x3333);
  victim_spray(victim);
}

static void victim_done(Fighter *victim) {
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
    PLAYED = 113,
    RESUMED = PLAYED + 100,
    SPAWNED = RESUMED + 20,
    IMPACT = SPAWNED + 24,
    ROWS = SPAWNED + 119 + (414 - 385)
  };

  native_begin(victim, 1, ROWS, victim_done);
  for (int row = 1; row <= ROWS; ++row) {
    int sample = row <= PLAYED ? 85 + row : 146 + (row - PLAYED);
    native_frame(victim, row, sample, row == IMPACT ? victim_impact : NULL);
  }
  native_play(victim, false);
}

void reptile_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, REPTILE_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int pose = fighter_sample(fighter);
  fighter_start_move(fighter, S_PUNCH, &owner_wait);
  fighter->sample_override = pose;
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  /* 4539b0 state 0: 475530 aligns the pair 1.5 units apart; the 454a00
   * camera director and 43d720 stage-object launch are presentation. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  native_hold(fighter, 31, owner_blend_start);
  native_hold(victim, 11, victim_stagger_blend);
}

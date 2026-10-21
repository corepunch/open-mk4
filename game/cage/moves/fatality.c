#include "retail_vm.h"
#include "../character.h"

#define FIRST_OWNER_READY 1
#define FIRST_VICTIM_DONE 2
#define FIRST_TORSO_DONE 4
static const MoveDef first_body = {.clip = 408, .last = 142, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip owner_male[] = {
  {{.clip = 406, .last = 376, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip owner_female[] = {
  {{.clip = 407, .last = 376, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_male[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 410, .last = 309, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_female[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 411, .last = 309, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip first_victory[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
// clang-format on

static bool female_victim(const Fighter *fighter) {
  int asset = roster_def(fighter->roster)->asset;
  return asset == 2 || asset == 9;
}

static void first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE | FIRST_TORSO_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static Fighter *first_body_owner(Fighter *actor) {
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
static void first_native_to(Fighter *fighter, int target, int period, FighterFn end) {
  int first = fighter_sample(fighter), count = (target - first) * period;
  require(target >= first && target <= fighter_move(fighter)->last && (period == 1 || period == 2), "Invalid Cage scene continuation range");
  count = count > 0 ? count : 1;
  native_begin(fighter, 1, count, end);
  for (int row = 1; row <= count; ++row) {
    native_frame(fighter, row, first + row / period < target ? first + row / period : target, NULL);
  }
  native_play(fighter, false);
}

static void first_victim_stream(Fighter *victim) {
  if (female_victim(victim)) {
    retail_vm_enter_owned(victim, 0x4e12d0, fighter_ready, 14);
  } else {
    retail_vm_enter_owned(victim, 0x4e12c0, fighter_ready, 14);
  }
}

static void first_victim_blends(Fighter *victim) {
  native_scene_blends(victim, female_victim(victim) ? victim_female : victim_male, 2, first_victim_stream);
}

static void first_intro_complete(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* 448160 ->475530 uses.95 units for the selected paired scene. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xf333);
  }
  native_hold(victim, 11, first_victim_blends);
  native_hold(fighter, 16, NULL);
  native_script_end(fighter, 0x4e12a0, fighter_ready, 14);
}

static void first_intro(Fighter *fighter) {
  require((fighter_move(fighter)->clip == 406 || fighter_move(fighter)->clip == 407) && fighter_sample(fighter) == 0,
      "Cage first scene needs its native selected actor clip");
  /* 448160: ordinary playback to283, one hold, half-speed to334. */
  native_begin(fighter, 1, 386, first_intro_complete);
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

static void first_owner_victory(Fighter *fighter) {
  fighter->state = S_VICTORY;
  fighter->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter_owned(fighter, 0x4e12b0, fighter_ready, 14);
  first_maybe_complete(fighter);
}

void cage_script_fatality_1_signal(Fighter *fighter) {
  /* 448480 blends into443 at step.05, then4e12b0 owns the loop. */
  native_scene_blends(fighter, first_victory, 1, first_owner_victory);
}

void cage_first_victim_59(Fighter *victim) {
  victim->sample_override = 59;
  battle_sound(SOUND_B, 0x1c);
  retail_vm_enter_owned(victim, 0x4e12e0, fighter_ready, 14);
}

void cage_first_victim_131(Fighter *victim) {
  victim->sample_override = 131;
  battle_sound(SOUND_B, 0x29);
  retail_vm_enter_owned(victim, 0x4e12f0, fighter_ready, 14);
}

void cage_first_victim_148(Fighter *victim) {
  victim->sample_override = 148;
  battle_sound(SOUND_B, 0x1b);
  retail_vm_enter_owned(victim, 0x4e1300, fighter_ready, 14);
}

void cage_first_victim_166(Fighter *victim) {
  victim->sample_override = 166;
  battle_sound(SOUND_B, 0x2a);
  retail_vm_enter_owned(victim, 0x4e1310, fighter_ready, 14);
}

void cage_first_victim_185(Fighter *victim) {
  victim->sample_override = 185;
  battle_sound(SOUND_B, 0x1b);
  retail_vm_enter_owned(victim, 0x4e1320, fighter_ready, 14);
}

void cage_first_victim_206(Fighter *victim) {
  victim->sample_override = 206;
  battle_sound(SOUND_B, 0x2b);
  retail_vm_enter_owned(victim, 0x4e1330, fighter_ready, 14);
}

static void first_lower_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  battle_camera_shake(5, 3, 0x2e14);
  retail_vm_enter_owned(victim, 0x4e1340, fighter_ready, 14);
}

static void first_lower_impact(Fighter *victim) {
  victim->sample_override = 278;
  battle_sound(SOUND_B, 0x12);
  battle_camera_shake(5, 3, 0x2e14);
  first_native_to(victim, 307, 1, first_lower_finish);
}

void cage_first_sever(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->sample_override = 225;
  victim->hidden_joints[0] |= UINT64_C(1) << 3;
  Fighter *torso = battle_scene_actor_start(owner, victim, &first_body, cage_entry_retail_script_004e1350);
  require(torso != NULL, "Cage torso needs its bounded independent scene slot");
  /* 448af0's copied body hides the two leg subtrees11/12. The original
   * body keeps those legs and continues410/411 independently. */
  torso->hidden_joints[0] = (UINT64_C(1) << 11) | (UINT64_C(1) << 12);
  battle_sound(SOUND_B, 0x16);
  Vec3 waist;
  Mat3 unused;
  fighter_joint_position(victim, 3, &waist, &unused);
  battle_blood_hit(owner, victim, waist, 0x3333);
  first_native_to(victim, 277, 1, first_lower_impact);
}

void cage_first_torso_half(Fighter *torso) {
  first_native_to(torso, 109, 2, NULL);
  native_script_end(torso, 0x4e1360, fighter_ready, 14);
}

void cage_first_torso_123(Fighter *torso) {
  torso->sample_override = 123;
  battle_sound(SOUND_B, 0x17);
  battle_camera_shake(9, 3, 0x2e14);
  retail_vm_enter_owned(torso, 0x4e1370, fighter_ready, 14);
}

void cage_first_torso_finish(Fighter *torso) {
  battle_sound(SOUND_B, 0x17);
  battle_camera_shake(9, 3, 0x2e14);
  retail_vm_enter_owned(torso, 0x4e1380, fighter_ready, 14);
}

void cage_first_task_end(Fighter *actor) {
  Fighter *owner = battle_is_scene_actor(actor) ? first_body_owner(actor) : (Fighter *)fighter_opponent(actor);
  bool torso = battle_is_scene_actor(actor);
  fighter_stop_mmove(actor);
  /* Retiring the retail task leaves its final rendered body in the scene. */
  owner->native_variant |= torso ? FIRST_TORSO_DONE : FIRST_VICTIM_DONE;
  first_maybe_complete(owner);
}

void cage_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  fighter->state = S_PUNCH;
  fighter->action = 0x700;
  mk4_script_scene_pose(victim, 13, 0);
  native_hold(victim, 1, NULL);
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(fighter, female_victim(victim) ? owner_female : owner_male, 1, first_intro);
}

#include "retail_vm.h"
#include "../character.h"

/* First fatality: 467ce0 -> 43cbb0 -> roster table 4e0498[11] = 44ad70.
 * Owner clip 440 with spin clip 445 and victory loop 443; victim task
 * 44b0e0 blends into clip 441 with turn clip 444; 44b2c0 severs the
 * waist and 44b4a0 starts the independent torso on clip 442. */
#define FIRST_OWNER_READY 1
#define FIRST_VICTIM_DONE 2
#define FIRST_TORSO_DONE 4
static const MoveDef first_torso = {.clip = 442, .last = 335, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 44ad70 holds the owner's current standing pose for sixteen visits. */
static const MoveDef first_wait = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip owner_blend[] = {
  {{.clip = 440, .last = 439, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_blends[] = {
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

static bool female_victim(const Fighter *fighter) {
  /* 474e20 selects the second roster pose for assets 2 and 9. */
  int asset = roster_def(fighter->roster)->asset;
  return asset == 2 || asset == 9;
}

static Fighter *first_scene_owner(Fighter *actor) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    if (&slot->actor == actor) {
      return &g_battle.fighters[slot->owner];
    }
  }
  require(false, "Kai torso has no scene owner");
  return NULL;
}

static void first_maybe_complete(Fighter *owner) {
  if (owner->native_variant == (FIRST_OWNER_READY | FIRST_VICTIM_DONE | FIRST_TORSO_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 428d70: advance one stored sample per visit to the target, then hold
 * one visit before the continuation. */
static void first_play_to(Fighter *fighter, int target, FighterFn end) {
  int first = fighter_sample(fighter);
  require(target > first && target <= fighter_move(fighter)->last, "Invalid Kai first-fatality playback range");
  native_begin(fighter, 1, target - first + 1, end);
  for (int frame = 1; frame <= target - first; ++frame) {
    native_frame(fighter, frame, first + frame, NULL);
  }
  native_frame(fighter, target - first + 1, target, NULL);
  native_play(fighter, false);
}

static void first_impact(int shake_row) {
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

static void first_owner_victory(Fighter *fighter) {
  /* 44af5a: second impact, then 4e1468 finishes 440 and loops 443. */
  first_impact(7);
  fighter->state = S_VICTORY;
  fighter->native_variant |= FIRST_OWNER_READY;
  retail_vm_enter_owned(fighter, 0x4e1468, fighter_ready, 11);
  first_maybe_complete(fighter);
}

static void first_owner_pull(Fighter *fighter) {
  /* 44af9c: impact, a female victim rises .1 (474e20), then 365..393. */
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  first_impact(5);
  if (female_victim(victim)) {
    victim->body.pos[AXIS_Y] -= 0x1999;
  }
  fighter->sample_override = 365;
  first_play_to(fighter, 393, first_owner_victory);
}

void kai_first_owner_tear(Fighter *fighter) {
  /* 44b043: rebind clip 440 at 124 and play to 364. */
  mk4_mmove_bind(fighter, 440, 440, 124);
  first_play_to(fighter, 364, first_owner_pull);
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

static void first_torso_start(Fighter *torso) {
  /* 44b4a0: torso B[0] (its voice list 12 has no portable bank), then 4e14c8. */
  battle_sound(SOUND_B, 0);
  retail_vm_enter_owned(torso, 0x4e14c8, fighter_ready, 11);
}

static void first_sever(Fighter *victim) {
  /* 476eb0(3) hides the victim waist subtree; 476120 copies the body,
   * 476d80 hides legs 11/12 on the copy; 44b680's blood stream is the
   * shared waist blood hit. The victim then finishes 441 through 4e14b8. */
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->hidden_joints[0] |= UINT64_C(1) << 3;
  Fighter *torso = battle_scene_actor_start(owner, victim, &first_torso, first_torso_start);
  require(torso != NULL, "Kai torso needs its bounded independent scene slot");
  torso->hidden_joints[0] = (UINT64_C(1) << 11) | (UINT64_C(1) << 12);
  Vec3 waist;
  Mat3 unused;
  fighter_joint_position(victim, 3, &waist, &unused);
  battle_blood_hit(owner, victim, waist, 0x3333);
  retail_vm_enter_owned(victim, 0x4e14b8, fighter_ready, 11);
}

static void first_victim_rip(Fighter *victim) {
  /* 44b346: B[2b], 145..161. */
  battle_sound(SOUND_B, 0x2b);
  first_play_to(victim, 161, first_sever);
}

void kai_first_victim_sever(Fighter *victim) {
  /* 44b3dd: B[2a], rebind clip 441 at 124 and play to 145. */
  battle_sound(SOUND_B, 0x2a);
  mk4_mmove_bind(victim, 441, 402, 124);
  first_play_to(victim, 145, first_victim_rip);
}

void kai_first_torso_203(Fighter *torso) {
  /* 44b600: a female torso rises .1, cursor 203, then 4e14d8. */
  if (female_victim(torso)) {
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
  Fighter *owner = torso ? first_scene_owner(actor) : (Fighter *)fighter_opponent(actor);
  fighter_stop_mmove(actor);
  owner->native_variant |= torso ? FIRST_TORSO_DONE : FIRST_VICTIM_DONE;
  first_maybe_complete(owner);
}

static void first_victim_start(Fighter *victim) {
  /* 4756f0 copies the owner's X/Z to the victim before 4e1480: clips 440
   * and 441 share one authored origin, so the pair is not separated. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  victim->flags |= FF_PAIRED;
  retail_vm_enter_owned(victim, 0x4e1480, fighter_ready, 11);
}

static void first_victim_blend(Fighter *victim) {
  /* 44b0e0 after eleven visits: 475190 common blend, then 475340 into 441. */
  native_scene_blends(victim, victim_blends, 2, first_victim_start);
}

static void first_owner_blend(Fighter *fighter) {
  /* 44adae after sixteen visits: 475250 blends into 440, then 4e1430. */
  native_scene_blends(fighter, owner_blend, 1, NULL);
  native_script_end(fighter, 0x4e1430, fighter_ready, 11);
}

void kai_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, KAI_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  int pose = fighter_sample(fighter);
  fighter_start_move(fighter, S_PUNCH, &first_wait);
  fighter->sample_override = pose;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_variant = 0;
  mk4_script_scene_pose(victim, 13, 0);
  /* 44ad70 -> 475530 places the victim 1.15 units along the pair axis.
   * Camera tasks 44bad0/44bbf0 remain portable fight framing. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x12666);
  }
  native_hold(victim, 11, first_victim_blend);
  native_hold(fighter, 16, first_owner_blend);
}

#include "retail_vm.h"
#include "../character.h"

/* F,D,F+BLK 467ce0 -> first-fatality table 4e0498[7] = 452730: Reiko and
 * the dizzy victim share one origin. Owner clip 425 tears the victim's
 * clip 428 apart at pose 37 (452ac0). Torso (joint 3), head (2) and both
 * arms (5, 6) then continue as detached rig subtrees with their own local
 * clips 429/430/427/426 (407330), while the victim keeps its pelvis and
 * legs. The owner ends in the 443 victory loop. */
#define FIRST_OWNER_DONE 1
#define FIRST_VICTIM_DONE 2
#define FIRST_PIECES_DONE (4 | 8 | 16 | 32)

static const MoveDef first_owner = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip owner_intro[] = {
  {{.clip = 425, .last = 93, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip owner_victory[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
static const SceneClip victim_reach_clip[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_body[] = {
  {{.clip = 428, .last = 193, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
/* 4e3638 camera shake rows used by this scene: pulses, half cycle, Y. */
static const int shake_rows[][3] = {
  [2] = {2, 2, 0x1999},
  [6] = {3, 3, 0x3333},
  [7] = {2, 3, 0x0f5c},
};
// clang-format on
static const MoveDef torso_piece = {.clip = 429, .last = 50, .action = 0x700};
static const MoveDef head_piece = {.clip = 430, .last = 124, .action = 0x700};
static const MoveDef left_arm_piece = {.clip = 427, .last = 69, .action = 0x700};
static const MoveDef right_arm_piece = {.clip = 426, .last = 88, .action = 0x700};

static void shake(int row) {
  battle_camera_shake(shake_rows[row][0], shake_rows[row][1], shake_rows[row][2]);
}

/* 45e360: the 45e570 close-finish gate and held switches 4e4118: BLK, HK,
 * LP and LK down; Up, HP and RUN released. */
bool reiko_can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_RUN);
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f && (fighter->held & mask) == chord;
}

static Fighter *first_owner_of(Fighter *actor) {
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

static void first_done(Fighter *owner, int flag) {
  owner->native_variant |= flag;
  if (owner->native_variant == (FIRST_OWNER_DONE | FIRST_VICTIM_DONE | FIRST_PIECES_DONE) && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

static void random_hit_sound(void) {
  /* 472800: B[1 + random(2)]. */
  battle_sound(SOUND_B, 1 + (int)battle_random(2));
}

static void random_piece_sound(void) {
  /* 453170: B[0x3b + random(4)]. */
  battle_sound(SOUND_B, 0x3b + (int)battle_random(4));
}

static void owner_victory_loop(Fighter *fighter) {
  fighter->state = S_VICTORY;
  retail_vm_enter_owned(fighter, 0x4e2d18, fighter_ready, 7);
  first_done(fighter, FIRST_OWNER_DONE);
}

/* 4528a0: 475430 blends into victory clip 443 at step 0xccc, then 4e2d18. */
void reiko_first_victory(Fighter *fighter) {
  native_scene_blends(fighter, owner_victory, 1, owner_victory_loop);
}

static void owner_tear(Fighter *fighter) {
  /* 452730 state 3 enters 4e2cf0 after 43d7d0 and twenty visits. */
  native_hold(fighter, 20, reiko_entry_retail_script_004e2cf0);
}

static void owner_blend(Fighter *fighter) {
  /* 452730 state 1: 475250 blends into 425. 43d720/43d7d0 swap stage
   * objects 0x45/0x7a; the portable arena keeps its objects. */
  native_scene_blends(fighter, owner_intro, 1, owner_tear);
}

static void victim_place(Fighter *victim) {
  /* 452950 state 3: 4756f0 puts the victim at the owner's X/Z. */
  const Fighter *owner = fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  retail_vm_enter_owned(victim, 0x4e2d28, fighter_ready, 7);
}

static void victim_body_blend(Fighter *victim) {
  /* 475190 -> 475690 copies the owner's facing and angles; 475340 blends
   * into 428 while 475510 keeps the body on the floor. */
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->heading, owner->heading, sizeof victim->heading);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  native_scene_blends(victim, victim_body, 1, victim_place);
}

static void victim_reach(Fighter *victim) {
  native_scene_blends(victim, victim_reach_clip, 1, victim_body_blend);
}

static void piece_place(Fighter *piece, int joint) {
  /* 453740 copies the victim's X/Z, angles and facing; Y is -0x12c49. */
  piece->subtree_joint = joint + 1;
  piece->body.pos[AXIS_Y] = -0x12c49;
  piece->sample_override = 0;
}

static void torso_start(Fighter *piece) {
  /* 4534e0: joint 3 without the separately detached head and arms. */
  piece_place(piece, 3);
  piece->hidden_joints[0] |= (UINT64_C(1) << 2) | (UINT64_C(1) << 5) | (UINT64_C(1) << 6);
  retail_vm_enter_owned(piece, 0x4e2d68, fighter_ready, 7);
}

static void head_start(Fighter *piece) {
  piece_place(piece, 2);
  retail_vm_enter_owned(piece, 0x4e2d88, fighter_ready, 7);
}

static void left_arm_start(Fighter *piece) {
  piece_place(piece, 5);
  retail_vm_enter_owned(piece, 0x4e2de0, fighter_ready, 7);
}

static void right_arm_start(Fighter *piece) {
  piece_place(piece, 6);
  retail_vm_enter_owned(piece, 0x4e2e18, fighter_ready, 7);
}

/* The independent 452c90 sound task plays only while this scene runs. */
static bool tear_sounds(BattleEvent *event, Fighter *owner, Fighter *victim) {
  static const int sounds[] = {0x38, 0x34, 0x39, 0x34, 0x34};
  battle_sound(SOUND_B, sounds[event->frame_value]);
  return g_battle.phase == PHASE_FATALITY && fighter_opponent(owner) == victim;
}

static bool tear_sequence_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* The program has played its last row; 452de5 retires the task. */
  return event->frame_count > 0 && fighter_opponent(owner) == victim;
}

/* 452ac0 at victim pose 37: four detached subtree tasks, the 452c90 sound
 * sequence (B 0x38, 0x34, 0x39, 0x34, 0x34 at 0/4/15/19/34), B[0x18] and
 * B[0x3a], shake row 7, and 476eb0 hides joint 3's subtree on the body. */
void reiko_first_tear(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  static const MoveDef *const moves[] = {&torso_piece, &head_piece, &left_arm_piece, &right_arm_piece};
  static const FighterFn starts[] = {torso_start, head_start, left_arm_start, right_arm_start};
  for (int i = 0; i < 4; ++i) {
    Fighter *piece = battle_scene_actor_start(owner, victim, moves[i], starts[i]);
    require(piece != NULL, "Reiko's first fatality needs its four bounded scene pieces");
  }
  BattleEvent *sounds = battle_event_start(owner, tear_sequence_done, false, false);
  if (sounds != NULL) {
    static const int at[] = {0, 4, 15, 19, 34};
    battle_event_program(sounds, 35);
    for (int i = 0; i < 5; ++i) {
      battle_event_frame(sounds, at[i], i, tear_sounds);
    }
  }
  battle_sound(SOUND_B, 0x18);
  battle_sound(SOUND_B, 0x3a);
  shake(7);
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
  shake(6);
  victim->sample_override = 118;
  retail_vm_enter_owned(victim, 0x4e2d48, fighter_ready, 7);
}

void reiko_first_victim_165(Fighter *victim) {
  /* 452c10: random hit sound, shake row 2, then 4e2d58 finishes 428. */
  random_hit_sound();
  shake(2);
  retail_vm_enter_owned(victim, 0x4e2d58, fighter_ready, 7);
}

/* 452c50 / 452e90 / 452f90 / 453040 / 4530f0: a floor pool (475730 /
 * 475930) and task_finish; the final pose stays in the scene. */
void reiko_first_piece_end(Fighter *actor) {
  Fighter *owner = first_owner_of(actor);
  fighter_stop_mmove(actor);
  int flag = FIRST_VICTIM_DONE;
  if (battle_is_scene_actor(actor)) {
    int joint = actor->subtree_joint - 1;
    flag = joint == 3 ? 4 : joint == 2 ? 8 : joint == 5 ? 16 : 32;
  }
  first_done(owner, flag);
}

void reiko_first_torso_29(Fighter *piece) {
  /* 452e50: random hit sound, shake row 7, then 4e2d78 finishes 429. */
  random_hit_sound();
  shake(7);
  retail_vm_enter_owned(piece, 0x4e2d78, fighter_ready, 7);
}

void reiko_first_torso_end(Fighter *piece) {
  /* 452e90: random hit sound and shake row 7 before the shared end. */
  random_hit_sound();
  shake(7);
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
  random_piece_sound();
  piece->sample_override = 83;
  random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2dd0, fighter_ready, 7);
}

void reiko_first_left_arm_56(Fighter *piece) {
  /* 452fe0: random piece sound, pose 56, then 4e2df8 to 66. */
  random_piece_sound();
  piece->sample_override = 56;
  retail_vm_enter_owned(piece, 0x4e2df8, fighter_ready, 7);
}

void reiko_first_left_arm_land(Fighter *piece) {
  /* 453020: random piece sound, then 4e2e08 finishes 427. */
  random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2e08, fighter_ready, 7);
}

void reiko_first_right_arm_44(Fighter *piece) {
  /* 453090: random piece sound, pose 44, then 4e2e40 to 68. */
  random_piece_sound();
  piece->sample_override = 44;
  retail_vm_enter_owned(piece, 0x4e2e40, fighter_ready, 7);
}

void reiko_first_right_arm_land(Fighter *piece) {
  /* 4530d0: random piece sound, then 4e2e50 finishes 426. */
  random_piece_sound();
  retail_vm_enter_owned(piece, 0x4e2e50, fighter_ready, 7);
}

void reiko_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, REIKO_FINISH_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 467ce0 stops the owner in its stance clip (4611c0 -> 284); the peer
   * keeps the dizzy finish pose. 475530 aligns the victim 1.3 units ahead. */
  fighter_start_move(fighter, S_PUNCH, &first_owner);
  fighter->sample_override = 0;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  mk4_script_scene_pose(victim, 13, 0);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14ccc);
  }
  /* 452730 state 0 sleeps sixteen visits; 452950 sleeps sixty-three. */
  native_hold(fighter, 16, owner_blend);
  native_hold(victim, 63, victim_reach);
}

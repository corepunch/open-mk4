#include "native_services.h"

enum {
  TANYA_EVENT_FATALITY_1 = 35,
  TANYA_EVENT_FATALITY_2
};

#include "retail_native.h"
#include <math.h>
#include <string.h>

/* BLK D,D,U,D record 4e4520 -> 467ce0 -> 43cbb0 -> 4e0498[9] = 44d340. */
static const MoveDef owner_pose = {.clip = 439, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 475250/475190/475340 blend at step 0x32f1 from weight 0x3333. */
static const SceneClip owner_kiss[] = {
  {{.clip = 395, .last = 140, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_turn[] = {
  {{.clip = 389, .last = 0, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip victim_swell[] = {
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
static const VictimCue victim_cues[] = {
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

static bool cue_next(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  int index = event->data[1]++;
  if (index >= (int)(sizeof victim_cues / sizeof victim_cues[0])) {
    return false;
  }
  const VictimCue *cue = &victim_cues[index];
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
static bool bob_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  event->data[1] += 0xccc;
  float phase = q16_float(event->data[1]);
  owner->body.pos[AXIS_Y] = event->data[0] + (q16)(-0x11eb * sinf(phase));
  return true;
}

static bool bob_begin(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  event->data[0] = owner->body.pos[AXIS_Y];
  event->data[1] = 0;
  event->step = bob_step;
  return true;
}

static void first_end(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

/* 44dd90's six tasks: shake row 8 (44e090), B 0x15/0x1c (44e160) and the
 * 4e1cd0 joint list {2, .1}, {9, .08}, {10, .08} (44eb00/407030), which
 * leaves the swollen body. Their particle and mesh tasks (472fb0, 474580,
 * 4b8140) use the portable blood spray. */
static bool burst_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
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
  first_end(owner);
  return false;
}

/* 44d7a0: 44dd90 waits 88 visits before the burst; 4e16d0 keeps shaking. */
void tanya_first_victim_burst(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, burst_step, false, false);
  battle_event_program(event, 88);
  retail_vm_enter_owned(fighter, 0x4e16d0, fighter_ready, 9);
}

/* 44d750: one more 4e15f0 pass, then cue task 44d8d0 and clip 409. */
void tanya_first_victim_repeat(Fighter *fighter) {
  if (fighter->native_variant-- > 0) {
    retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
    return;
  }
  BattleEvent *event = battle_event_start(fighter, cue_next, false, false);
  event->data[1] = 0;
  cue_next(event, fighter, (Fighter *)fighter_opponent(fighter));
  retail_vm_enter_owned(fighter, 0x4e1600, fighter_ready, 9);
}

/* 44d6e0: the 44d7c0 bob task, sound B 0x2b and a two-pass counter. */
void tanya_first_victim_swell(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, bob_begin, false, false);
  battle_event_program(event, 25);
  battle_sound(SOUND_B, 0x2b);
  fighter->native_variant = 1;
  retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
}

/* 44d560 state 3: 4756f0 places the victim on the owner's X/Z. */
static void victim_scene(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  owner->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED;
  retail_vm_enter_owned(victim, 0x4e15e0, fighter_ready, 9);
}

/* 475190 state 1: 475690 copies the owner's facing and angles. */
static void victim_turned(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  victim->facing = owner->facing;
  memcpy(victim->body.angle, owner->body.angle, sizeof victim->body.angle);
  native_scene_blends(victim, victim_swell, 1, victim_scene);
}

static void victim_start(Fighter *victim) {
  native_scene_blends(victim, victim_turn, 1, victim_turned);
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

static void owner_kiss_start(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1598, fighter_ready, 9);
}

/* 44d340 state 1 after sixteen visits: sound B 0x30, then 475250 blends
 * into clip 395. 44dca0/43d7d0 camera and object cleanup are presentation. */
static void owner_intro(Fighter *fighter) {
  battle_sound(SOUND_B, 0x30);
  native_scene_blends(fighter, owner_kiss, 1, owner_kiss_start);
}

void tanya_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, TANYA_EVENT_FATALITY_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  mk4_script_scene_pose(victim, 13, 0);
  /* 44d340 state 0 -> 475530 aligns the victim 1.15 ahead. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x12666);
  }
  /* Task 44d560 waits eleven visits; the owner waits sixteen. */
  native_hold(victim, 11, victim_start);
  native_hold(fighter, 16, owner_intro);
}

#include "../character.h"

/* Fatality I: 467ce0 -> 43cbb0 -> 4e0498[6] = 455000. Shinnok vanishes
 * through clip 12 while a giant hand (rig 5083c4, nf_fat_g.geo) rises in
 * front of him, squeezes the victim and drops it; he then leaps back in. */
static const MoveDef grab_hand_pose = {.clip = 434, .last = 379, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
/* 4558f0 states 2/3: 475190 blends descriptor 4fbfb8 (clip 389), then
 * 475340 blends 4fc1e0 (clip 435), both with 475250's .2 start and step. */
static const SceneClip victim_blends[] = {
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
static const SoundCue victim_cues[] = {
  {0x29, 10}, {0x2a, 20}, {0x1b, 10}, {0x33, 10}, {0x1c, 20}, {0x2b, 3}, {0x2a, 4}, {0x2b, 3}, {0x29, 4}, {0x2a, 3}, {0x29, 4},
  {0x2b, 4}, {0x29, 4}, {0x29, 4}, {0x2a, 3}, {0x29, 4}, {0x2b, 3}, {0x2a, 4}, {0x2b, 3}, {0x2a, 4}, {0x1b, 6}, {0x33, 13},
  {0x46, 5}, {0x2b, 3}, {0x2a, 4}, {0x2b, 0},
};
/* 4564b0 -> 456630: the hand's B 0x49, 0x4a, 0x4a and 0x4b cues. */
static const SoundCue hand_cues[] = {
  {0x49, 125}, {0x4a, 189}, {0x4a, 189}, {0x4b, 0},
};
// clang-format on

static bool play_cues(BattleEvent *event, const SoundCue *cues) {
  const SoundCue *cue = &cues[event->phase++];
  battle_sound(SOUND_B, cue->selector);
  if (cue->wait == 0) {
    return false;
  }
  battle_event_delay(event, cue->wait);
  return true;
}

static bool victim_cue(BattleEvent *event, Fighter *victim, Fighter *owner) {
  if (victim_cues[event->phase].selector == 0x46) {
    /* 455bf0 launches a type 0x92 piece from the victim's head joint. */
    Vec3 head;
    Mat3 unused;
    fighter_joint_position(victim, 2, &head, &unused);
    battle_blood_hit(owner, victim, head, 0x4000);
  }
  return play_cues(event, victim_cues);
}

static bool hand_cue(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)owner;
  (void)victim;
  return play_cues(event, hand_cues);
}

static void start_cues(Fighter *owner, BattleEventFn step) {
  BattleEvent *event = battle_event_start(owner, step, false, false);
  require(event != NULL, "Shinnok fatality needs a bounded cue sequence");
  event->phase = 0;
  event->due_tick = g_battle.ticks;
}

static void victim_sounds(Fighter *victim) {
  /* 456990 first queues roster voice list 0xb (4727e0); the portable bank
   * resolves only voice lists 1 and 8, so the sampled B cues follow. */
  start_cues(victim, victim_cue);
}

static Fighter *hand_owner(Fighter *hand) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (&g_battle.scene_actors[i].actor == hand) {
      return &g_battle.fighters[g_battle.scene_actors[i].owner];
    }
  }
  require(false, "Shinnok hand has no scene owner");
  return NULL;
}

static void hand_entry(Fighter *hand) {
  if (hand->assets != NULL) {
    hand->scene_model = &hand->assets->shinnok_hand;
  }
  shinnok_entry_retail_script_004e2f40(hand);
}

/* 4556d0 state 1: rig 5083c4 is placed 1.4 along the owner's heading at
 * his floor height, with his yaw and facing; 4e2f40 animates it. */
static bool hand_spawn(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)victim;
  Fighter *hand = battle_scene_actor_start(owner, owner, &grab_hand_pose, hand_entry);
  require(hand != NULL, "Shinnok hand needs its bounded scene slot");
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    hand->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x16666);
  }
  hand->body.pos[AXIS_Y] = owner->body.ground;
  start_cues(owner, hand_cue);
  return false;
}

static void first_vanish(Fighter *fighter) {
  /* 455000 state 1 after 16 ticks: 4556d0 sleeps 65, then the hand. */
  BattleEvent *event = battle_event_start(fighter, hand_spawn, false, false);
  require(event != NULL, "Shinnok hand needs its delayed spawner");
  battle_event_delay(event, 65);
  shinnok_fatality_vanish_cue(fighter);
  shinnok_entry_retail_script_004e2ef0(fighter);
}

static void victim_lift(Fighter *victim) {
  /* 4558f0 state 4 rechecks the 1.4 placement before 4e2f78. */
  const Fighter *owner = fighter_opponent(victim);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x16666);
  }
  shinnok_entry_retail_script_004e2f78(victim);
}

static void victim_blend(Fighter *victim) {
  native_scene_blends(victim, victim_blends, 2, victim_lift);
}

void shinnok_first_vanished(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  shinnok_entry_retail_script_004e2f00(fighter);
}

void shinnok_first_hidden(Fighter *fighter) {
  native_hold(fighter, 0x235, shinnok_fatality_return);
}

void shinnok_first_hand_squeeze(Fighter *hand) {
  native_repeat(hand, shinnok_retail_script_004e2f58_repeat_phase(), 23, shinnok_entry_retail_script_004e2f68);
  native_play(hand, false);
}

void shinnok_first_squeeze_cycle(Fighter *fighter) {
  /* 4558d0/455b80 decrement the native counter; native_repeat owns it. */
  mk4_script_fan_cycle(fighter);
}

void shinnok_first_task_end(Fighter *actor) {
  /* 49cb60 retires the task; the body keeps its final rendered pose. */
  fighter_stop_mmove(actor);
  bool hand = battle_is_scene_actor(actor);
  Fighter *owner = hand ? hand_owner(actor) : (Fighter *)fighter_opponent(actor);
  owner->native_variant |= hand ? SHINNOK_FATALITY_HAND_DONE : SHINNOK_FATALITY_VICTIM_DONE;
  shinnok_fatality_maybe_complete(owner);
}

void shinnok_first_victim_held(Fighter *victim) {
  /* 455ae0 marks the group 0x1000 and 4773a0 detaches it from its player
   * slot; the portable victim simply remains scene-owned. */
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  shinnok_entry_retail_script_004e2f88(victim);
}

void shinnok_first_victim_squeeze(Fighter *victim) {
  native_repeat(victim, shinnok_retail_script_004e2f98_repeat_phase(), 23, shinnok_entry_retail_script_004e2fa8);
  native_play(victim, false);
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
  shinnok_entry_retail_script_004e2fb8(victim);
}

void shinnok_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, SHINNOK_FINISH_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  shinnok_fatality_begin(fighter, SHINNOK_FATALITY_DONE);
  /* 475530 aligns the pair 1.4 units apart (53c444 = 0x16666). */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x16666);
  }
  /* 4558f0: sleep 71, start 456990, sleep 100, then the two blends. */
  native_begin(victim, 0, 170, victim_blend);
  for (int frame = 0; frame <= 170; ++frame) {
    native_frame(victim, frame, 0, frame == 70 ? victim_sounds : NULL);
  }
  native_play(victim, false);
  native_hold(fighter, 16, first_vanish);
}

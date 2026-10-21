#include "native_services.h"

enum {
  SHINNOK_FINISH_1 = 21,
  SHINNOK_FINISH_2
};

enum {
  SHINNOK_FATALITY_OWNER_READY = 1,
  SHINNOK_FATALITY_VICTIM_DONE = 2,
  SHINNOK_FATALITY_HAND_DONE = 4,
  SHINNOK_FATALITY_DONE = 7
};

/* Fatality II: 467cd0 -> 43bd70 -> 4dfe90[6] = 43c6d0. The victim floats
 * up while Shinnok vanishes; two mirrored giant hands (rig 5083c4) clap
 * and burst it, then he leaps back in. */
static const MoveDef clap_hand_pose = {.clip = 541, .last = 199, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 542, .last = 11, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};

/* Rows of the independent 46a280/46a380 lift task. */
#define LIFT_ROWS 400

void shinnok_second_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector >= 48 && selector <= 50, "Unexpected Shinnok clap camera shot");
  /* 4315b0 queues camera director 4dea40; the finisher keeps the lock. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

static void lift_gravity(Fighter *victim, float gravity) {
  victim->body.gravity = (q16)gravity;
}

/* 46a280: gravity -0x28f, then +0x41 for nine visits; the tenth enters
 * 46a340 (gravity 0x83 until Y speed >= 0). 46a380 then holds 10 ticks,
 * reverses to -0x83 until the speed is upward, holds 10 and repeats.
 * The velocity tests are resolved here from the deterministic motion. */
static void lift_victim(Fighter *victim) {
  native_motion_begin(victim, LIFT_ROWS, NULL);
  q16 gravity = -0x28f, speed = 0;
  int row = 0;
  native_motion_frame(victim, row++, lift_gravity, (float)gravity, NULL);
  speed += gravity;
  native_motion_frame(victim, row++, lift_gravity, (float)gravity, NULL);
  speed += gravity;
  for (int visit = 0; visit < 10; ++visit) {
    gravity = visit == 9 ? 0x83 : gravity + 0x41;
    native_motion_frame(victim, row++, lift_gravity, (float)gravity, NULL);
    speed += gravity;
  }
  bool falling = true;
  while (row < LIFT_ROWS) {
    /* 48f490 / 48f510 yield once before testing the live speed. */
    for (int test = 0; row < LIFT_ROWS && (test == 0 || (falling ? speed < 0 : speed >= 0)); ++test) {
      native_motion_frame(victim, row++, lift_gravity, (float)gravity, NULL);
      speed += gravity;
    }
    for (int hold = 0; hold < 10 && row < LIFT_ROWS; ++hold) {
      native_motion_frame(victim, row++, lift_gravity, (float)gravity, NULL);
      speed += gravity;
    }
    falling = !falling;
    gravity = falling ? 0x83 : -0x83;
  }
  native_motion_play(victim);
}

/* 4ea4e8[179] = 481510: start the lift task and stream 4ea270. */
void shinnok_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  lift_victim(victim);
  retail_vm_enter_owned(victim, 0x4ea270, fighter_ready, 6);
}

void shinnok_second_vanish(Fighter *fighter) {
  /* 43c820 state 0 -> 4552e0. */
  shinnok_fatality_vanish_cue(fighter);
  retail_vm_enter_owned(fighter, 0x4e2fc8, fighter_ready, 6);
}

void shinnok_second_vanished(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  retail_vm_enter_owned(fighter, 0x4e2fd8, fighter_ready, 6);
}

static void hand_model(Fighter *hand) {
  if (hand->assets != NULL) {
    hand->scene_model = &hand->assets->shinnok_hand;
  }
}

static void first_hand_entry(Fighter *hand) {
  hand_model(hand);
  retail_vm_enter_owned(hand, 0x4e00b8, fighter_ready, 6);
}

static void second_hand_entry(Fighter *hand) {
  hand_model(hand);
  /* 490350 toggles the second hand's facing bit. */
  hand->facing = -hand->facing;
  hand->heading[0] = -hand->heading[0];
  hand->heading[1] = -hand->heading[1];
  retail_vm_enter_owned(hand, 0x4e00e0, fighter_ready, 6);
}

/* 43caa0: rig 5083c4 at the victim's X, Z zero, with the owner's yaw and
 * facing; the group Y keeps the object default floor. */
static void place_hand(Fighter *hand, const Fighter *victim) {
  hand->body.pos[AXIS_X] = victim->body.pos[AXIS_X];
  hand->body.pos[AXIS_Y] = 0;
  hand->body.pos[AXIS_Z] = 0;
  hand->body.ground = 0;
}

void shinnok_second_hands(Fighter *fighter) {
  /* 455360 returns to 43c820 state 1: 5322c4, both hand tasks, wait 300. */
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  Fighter *first = battle_scene_actor_start(fighter, fighter, &clap_hand_pose, first_hand_entry);
  Fighter *second = battle_scene_actor_start(fighter, fighter, &clap_hand_pose, second_hand_entry);
  require(first != NULL && second != NULL, "Shinnok clap needs both bounded scene slots");
  place_hand(first, victim);
  place_hand(second, victim);
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
  retail_vm_enter_owned(hand, 0x4e00f8, fighter_ready, 6);
}

/* 41db70/41dba0/41dbd0 -> 41dc00 allocate generic flesh pieces from
 * tables 4d3a18/4d3c80/4d3d88 at the hand. */
void shinnok_second_gore(Fighter *hand) {
  battle_particles(hand, 3, 12, false);
}

void shinnok_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, SHINNOK_FINISH_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  shinnok_fatality_begin(fighter, SHINNOK_FATALITY_OWNER_READY);
  /* 43c2f0/43c330: owner at the arena origin, victim one unit to the side
   * it already occupies (48f030), both on Z zero. */
  q16 side = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? Q16_ONE : -Q16_ONE;
  fighter->body.pos[AXIS_X] = fighter->body.pos[AXIS_Z] = 0;
  victim->body.pos[AXIS_X] = side;
  victim->body.pos[AXIS_Z] = 0;
  retail_vm_enter_owned(fighter, 0x4e0088, fighter_ready, 6);
}

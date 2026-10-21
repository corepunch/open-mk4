#include "retail_vm.h"
#include "../character.h"

/* 47bd10 sets action 210 and enters 4e8210: sound C60, forward .18, Y -.1,
 * gravity 41, then clip 298 advances to sample 9 before native 47bd40. */
static const MoveDef aerial_punch = {.clip = 298, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x210};
/* D[16] 0x4ec0e0: 13% damage, reaction 123 on the ground or in the air, no block reaction. */
static const AttackRecord aerial_contact = {0x107ae, -0x8f1a, 0x107ae, 0xb851, 123, 123, 0, 0, 0x2147};

enum {
  AERIAL_QUERIES = 15,
  AERIAL_CONTACT_STOP = 10,
  FALL_CLIP = 310,
  FALL_COUNT = 28,
  FALL_FIRST = 7,
  FALL_HOLD = 18
};

static void aerial_drag(Fighter *fighter) {
  /* 4e8250 -> 47bf50 -> 490650: scale the X/Z velocity by .9 each world step. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void aerial_cleanup(Fighter *fighter) {
  if (fighter->body.physics == aerial_drag) {
    fighter->body.physics = NULL;
  }
}

static void fall_landed(Fighter *fighter) {
  /* 480cf0 on the floor: cursor 18, 428d10 finishes clip 310, then 46f650. */
  native_begin(fighter, 1, FALL_COUNT - FALL_HOLD, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= FALL_COUNT - FALL_HOLD; ++frame) {
    native_frame(fighter, frame, FALL_HOLD + frame - 1, NULL);
  }
  native_play(fighter, false);
}

static void fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    fall_landed(fighter);
  }
}

/* Shared native 480cf0: gravity and Y velocity, action 1005, face the peer
 * and play fall clip 310 from sample 7 to its sample-18 air hold. Landing
 * at any visit plays samples 18..27 and returns through the controller. */
static void aerial_fall(Fighter *fighter, q16 gravity) {
  fighter->body.physics = NULL;
  fighter->body.gravity = gravity;
  fighter->action = 0x1005;
  retail_face_peer(fighter);
  mk4_mmove_bind(fighter, FALL_CLIP, FALL_COUNT, FALL_FIRST);
  native_begin(fighter, 1, FALL_HOLD - FALL_FIRST, fall_landed);
  for (int frame = 1; frame <= FALL_HOLD - FALL_FIRST; ++frame) {
    native_frame(fighter, frame, FALL_FIRST + frame, frame < FALL_HOLD - FALL_FIRST ? fall_check : mk4_mmove_wait_grounded);
  }
  native_play(fighter, false);
}

static void contact_fall(Fighter *fighter) {
  /* 47be50 resumes after its stop with 480cf0, gravity 28f and the stopped Y velocity. */
  aerial_fall(fighter, 0x28f);
}

static void aerial_hit(Fighter *fighter) {
  /* 470e80 sets action 1006; 47be50 stops all motion and sleeps ten visits. */
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_stop(fighter);
  native_hold(fighter, AERIAL_CONTACT_STOP, contact_fall);
}

static void aerial_land(Fighter *fighter) {
  /* 470e80 sets action 1006; 47bed0 runs 46fd90 landing, then 46f650. */
  fighter->action = ACTION_RETAIL_1006;
  fighter_finish_mmove(fighter, mk4_land);
}

static void timeout_fall(Fighter *fighter) {
  /* 4914a0 removes the drag callback before 480cf0 continues the current Y velocity. */
  aerial_fall(fighter, 0x28f);
}

static void aerial_timeout(Fighter *fighter) {
  /* 47bd60: install the drag callback and gravity 28f, then 428d10 plays
   * the remaining clip-298 samples without a floor check. */
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  fighter->body.physics = aerial_drag;
  fighter->body.gravity = 0x28f;
  if (first >= last) {
    timeout_fall(fighter);
    return;
  }
  native_begin(fighter, 1, last - first, timeout_fall);
  for (int frame = 1; frame <= last - first; ++frame) {
    native_frame(fighter, frame, first + frame, NULL);
  }
  native_play(fighter, false);
}

static void aerial_first_query(Fighter *fighter) {
  if (battle_contact(fighter, &aerial_contact) != CONTACT_MISS) {
    aerial_hit(fighter);
  }
}

static void aerial_query(Fighter *fighter) {
  /* 4704d0 checks the floor (48ef30) before every later D[16] query. */
  if (!fighter_airborne(fighter)) {
    aerial_land(fighter);
    return;
  }
  aerial_first_query(fighter);
}

static void aerial_expired(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    aerial_land(fighter);
  } else {
    aerial_timeout(fighter);
  }
}

void kai_aerial_punch_contact(Fighter *fighter) {
  /* 4e8238 = {15 visits, D[16], contact 47be50, floor 47bed0}, then timeout 47bd60. */
  require(fighter_move(fighter)->clip == 298 && fighter_sample(fighter) == 9, "Kai aerial punch needs its generated hop startup");
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, AERIAL_QUERIES, aerial_expired);
  for (int frame = 1; frame <= AERIAL_QUERIES; ++frame) {
    native_frame(fighter, frame, pose, frame == 1 ? aerial_first_query : aerial_query);
  }
  native_play(fighter, false);
}

void kai_input_47bd10(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &aerial_punch);
  fighter->mmove_cleanup = aerial_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8210, fighter_ready, 11);
}

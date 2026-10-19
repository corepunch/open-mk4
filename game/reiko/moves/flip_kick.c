#include "../character.h"

/* B,D,F+HK 4791a0 (also the far CPU row): native-only state machine. It
 * stamps slot 16, plays C[0x62], stores action 0x21e and moves 0.2 along
 * the pair axis while 47bf50 decays horizontal speed by 0.9 every world
 * pump. Clip 110 (53cf3c) advances to pose 7, holds it one visit, then
 * shows 8, 9 and 10 while querying D[29]. */
static const MoveDef s_flip_kick = {.clip = 110, .last = 26, .flags = SF_LOCK | SF_RECORD, .action = 0x21e};
static const AttackRecord flip_record = {0x1353f, -0xab02, 0x12395, 0x1b2b0, 171, 171, 9, 0, 0x1eb8};

/* 45e7a0: slot 16 at least 90 ticks old and a peer combo of at most four. */
bool reiko_can_flip_kick(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && mk4_special_elapsed(fighter, 0x10, 90) && fighter_opponent(fighter)->hits <= 4;
}

static void flip_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void blocked_hold(Fighter *fighter) {
  /* 4791a0 state 6 sleeps sixteen visits before the controller. */
  native_hold(fighter, 16, mk4_return_to_stance);
}

/* 428cf0 finishes clip 110 from the next pose; a block first sets
 * no-guard (490140) and adds the sixteen-visit wait. */
static void flip_finish(Fighter *fighter, bool blocked) {
  int pose = fighter_sample(fighter);
  if (blocked) {
    fighter->flags |= FF_NO_GUARD;
  }
  native_begin(fighter, 1, s_flip_kick.last - pose, blocked ? blocked_hold : mk4_return_to_stance);
  for (int frame = 1; frame <= s_flip_kick.last - pose; ++frame) {
    native_frame(fighter, frame, pose + frame, NULL);
  }
  native_play(fighter, false);
}

static void flip_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &flip_record);
  if (result != CONTACT_MISS) {
    flip_finish(fighter, result == CONTACT_BLOCK);
  }
}

static void flip_miss(Fighter *fighter) {
  flip_finish(fighter, false);
}

void reiko_flip_kick(Fighter *fighter) {
  mk4_special_stamp(fighter, 0x10);
  battle_sound(SOUND_C, 0x62);
  fighter_start_move(fighter, S_KICK, &s_flip_kick);
  fighter_set_speed(fighter, 0x3333);
  fighter->body.physics = flip_decay;
  /* 428750/428d30: pose 0 is replaced by 1 on the entry visit; 428d70
   * resubmits 7, then states 1..4 show 8 and query at 8, 9 and 10. */
  static const int poses[] = {1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 9, 10};
  native_begin(fighter, 1, 12, flip_miss);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, poses[frame - 1], frame >= 10 ? flip_query : NULL);
  }
  native_play(fighter, true);
}

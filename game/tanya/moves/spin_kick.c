#include "retail_vm.h"
#include "../character.h"

/* LK F,F record 4e4550 -> T4[30] 479520: action 21b, clip 109 through
 * 428750/53cf40, speed .15 and the 4797c0 cursor callback, which loops
 * samples 14..28 after the first pass. */
static const MoveDef spin = {.clip = 109, .last = 28, .flags = SF_LOCK | SF_RECORD, .action = 0x21b};
/* D[28] 4ebfc0: 12%, hit/air 158, block 3. */
static const AttackRecord spin_record = {0x12dd2, 0xf5c, 0x16872, 0x6dd2, 158, 158, 3, 0, 0x1eb8};

/* 45e8d0: at least 60 ticks since stamp 0xd and an opponent combo count
 * of at most four (45ebb0/45f5b0). */
bool tanya_can_spin_kick(const Fighter *fighter) {
  return g_battle.ticks >= fighter->spin_kick_ready && fighter_opponent(fighter)->hits <= 4;
}

static int spin_next(int pose) {
  return pose + 1 >= 29 ? 14 : pose + 1;
}

static void spin_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  bool airborne = fighter_airborne(fighter);
  mk4_script_stop_body(fighter);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  /* An interrupted recoil retains a fall; the replacing move owns landing. */
  if (airborne) {
    fighter->body.gravity = 0x28f;
  }
}

static void spin_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 479705: clear the cursor callback and motion, set no-guard, bind
 * sample 10 and let 429600 rewind one sample per visit to 0. */
static void spin_recover(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->sample_override = 10;
  native_begin(fighter, 0, 9, spin_ready);
  for (int frame = 0; frame <= 9; ++frame) {
    native_frame(fighter, frame, 9 - frame, NULL);
  }
  native_play(fighter, false);
}

static void recoil_wait(Fighter *fighter) {
  if (fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  /* 47daa0: 490720/490de0 clear spin and angles, 490bd0 faces the peer. */
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 9);
}

/* Shared blocked recoil 47d940: action 1010, 4e84a8 speed -.05, Y -0x23d7
 * and gravity 0x168; 47dbc0 scales X velocity by .6 for slots 7/9/12/14.
 * 47db60 holds the last sample of clip 301 with X spin -0x4ccc, lifts the
 * group .2 and installs the 4707e0 whoosh; 48f490/48f410 wait for descent
 * below -2.5. 48fed0's 4ec2c4 peer adjustment is not modelled, as in Cage's
 * port of the same recoil. */
static void spin_blocked(Fighter *fighter) {
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0x9999);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  fighter->body.on_land = NULL;
  fighter->body.physics = mk4_script_landing_motion;
  fighter->body.spin[AXIS_X] = -0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  mk4_mmove_bind(fighter, 301, 9, 8);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, recoil_wait);
  native_play(fighter, false);
}

void tanya_blocked_recoil_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void spin_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &spin_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    spin_blocked(fighter);
    return;
  }
  /* An unblocked contact keeps spinning and moving for twelve visits. */
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, spin_recover);
  for (int frame = 1; frame <= 12; ++frame) {
    pose = spin_next(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}

static void spin_sounds(Fighter *fighter) {
  mk4_script_indexed_sound(fighter, 0x35);
  battle_sound(SOUND_C, 0x57);
}

/* Eight visits of startup, sounds, then fifteen D[28] queries. */
void tanya_spin_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &spin);
  fighter->mmove_cleanup = spin_cleanup;
  fighter->spin_kick_ready = g_battle.ticks + 60;
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x2666);
  native_begin(fighter, 1, 23, spin_recover);
  int pose = 0;
  for (int frame = 1; frame <= 23; ++frame) {
    pose = spin_next(pose);
    native_frame(fighter, frame, pose, frame == 8 ? spin_sounds : frame > 8 ? spin_query : NULL);
  }
  native_play(fighter, false);
}

#include "retail_vm.h"
#include "../character.h"

static void leap_rise(Fighter *fighter);
static void leap_fall(Fighter *fighter);
static void leap_cleanup(Fighter *fighter);
static const MoveDef s_leap = {.clip = 601, .last = 51, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x409};
/* D[34]: the zero-damage stomp box under the falling body; every
 * outcome selects reaction 191. 43b370 applies 30% separately. */
static const AttackRecord stomp_record = {0x68f5, 0x44dd, 0xe666, 0xc76c, 191, 191, 191, 0, 0};

enum {
  LEAP_APEX_SAMPLE = 41, /* 23 + ceil(0x3cccc / 0x3851) one-pose rising visits */
  LEAP_RISE = -0x3851,
  LEAP_APEX = -0x3cccc,
  LEAP_DROP_HEIGHT = -0x40000,
  LEAP_DROP_SPEED = 0x1999,
  LEAP_DROP_GRAVITY = 0xa3d,
  LEAP_DAMAGE = 0x4ccc
};

/* 480f40 sets 532254 every physics visit; 489791 then skips the close-range
 * body separation while Goro stands on his victim. */
void goro_stomp_hold_separation(Fighter *fighter) {
  fighter->flags |= FF_HOLD_SEPARATION;
}

static void leap_cleanup(Fighter *fighter) {
  if (fighter->body.physics == goro_stomp_hold_separation) {
    fighter->body.physics = NULL;
  }
}

/* 43b320 (HK F,F,B and the CPU far row): voice 0x64f, action 0409, then
 * 4dfbb8 binds clip 601 and double-steps to 23 before 43b370. */
void goro_leap_stomp(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &s_leap, 0x64f);
  fighter->mmove_cleanup = leap_cleanup;
  retail_vm_enter_owned(fighter, 0x4dfbb8, fighter_ready, 15);
}

static void leap_descend(Fighter *fighter) {
  /* 43b42d: relocate above the X/Z captured at launch and drop from 4.0. */
  fighter->body.pos[AXIS_X] = fighter->saved_motion[0];
  fighter->body.pos[AXIS_Z] = fighter->saved_motion[1];
  fighter->body.pos[AXIS_Y] = LEAP_DROP_HEIGHT;
  fighter->body.vel[AXIS_Y] = LEAP_DROP_SPEED;
  fighter->body.gravity = LEAP_DROP_GRAVITY;
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, LEAP_APEX_SAMPLE, leap_fall);
  native_play(fighter, false);
}

static void leap_rise(Fighter *fighter) {
  if (fighter->body.pos[AXIS_Y] <= LEAP_APEX) {
    leap_descend(fighter);
  }
}

/* 43b3a4: until the body lands, query D[34] without advancing the pose.
 * A contact disturbs resting Goro objects, deals 30% and enters 4dfbd0;
 * landing first enters the miss recovery 43b620 -> 4dfc38. */
static void leap_fall(Fighter *fighter) {
  fighter->mmove_hold = true;
  if (!fighter_airborne(fighter)) {
    fighter->mmove_hold = false;
    fighter->sample_override = LEAP_APEX_SAMPLE;
    goro_quake_resting_weapons();
    retail_vm_enter_owned(fighter, 0x4dfc38, fighter_ready, 15);
    return;
  }
  if (battle_contact(fighter, &stomp_record) == CONTACT_MISS) {
    return;
  }
  fighter->mmove_hold = false;
  goro_quake_resting_weapons();
  battle_damage((Fighter *)fighter_opponent(fighter), LEAP_DAMAGE);
  fighter->sample_override = LEAP_APEX_SAMPLE;
  retail_vm_enter_owned(fighter, 0x4dfbd0, fighter_ready, 15);
}

/* 43b505 records the victim X/Z and rises at -0x3851 per visit; 43b42d
 * advances one pose per visit (4285d0) until the body passes -3.8. */
void goro_script_leap_continue(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  fighter->saved_motion[0] = victim->body.pos[AXIS_X];
  fighter->saved_motion[1] = victim->body.pos[AXIS_Z];
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = LEAP_RISE;
  native_begin(fighter, first + 1, last, leap_descend);
  for (int pose = first + 1; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, leap_rise);
  }
  native_play(fighter, false);
}

/* 43b590 -> 43baf0: face the victim, stop and return the controller. The
 * retail 80-visit hold polls the CPU dispatcher 4339b0; portable CPU
 * policy owns that decision after the controller returns. */
void goro_script_stomp_return(Fighter *fighter) {
  leap_cleanup(fighter);
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

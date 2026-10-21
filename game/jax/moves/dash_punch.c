#include "native_services.h"

/* LP D,B record 4e4394 (flags 0x4, gate 45e910) and the near/far CPU rows
 * select 47ab10: stamp 0xc, action 0x215 and script 4e8150 (indexed 37,
 * C[78], speed 0x2666, clip 299 two poses per visit to 18, tail 47ab50). */
static const MoveDef s_dash = {.clip = 299, .last = 34, .flags = SF_LOCK | SF_RECORD, .action = 0x215};
/* D[22] record 4ec050: 16%, reactions 157/10/165. */
static const AttackRecord dash_record = {0x16b43, -0x4c8b, 0xcf1a, 0x2b85, 157, 10, 165, 0, 0x28f5};
static const MoveDef s_dash_stagger = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_dash_fall = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_dash_air_fall = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};

enum {
  DASH_QUERIES = 15,
  DASH_LOOP_POSE = 23,
  DASH_STOP_TICKS = 5,
  DASH_BLOCK_TICKS = 40,
  DASH_FINAL_TICKS = 20
};

static void dash_query(Fighter *fighter);
static void dash_step_query(Fighter *fighter);

bool jax_can_dash_punch(const Fighter *fighter) {
  return g_battle.ticks >= fighter->dash_punch_ready;
}

void jax_dash_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_dash);
  fighter->dash_punch_ready = g_battle.ticks + 60;
  retail_vm_enter_owned(fighter, 0x4e8150, fighter_ready, 13);
}

/* 47af90, installed through descriptor 4e8174 as the group +44 callback:
 * X/Z velocity *= 0xe666, then 4297a0 advances one pose and loops the
 * final pose back to 23. The pose half is resolved in the frame rows. */
static void dash_friction(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static int dash_advance(int pose) {
  return pose < s_dash.last ? pose + 1 : DASH_LOOP_POSE;
}

static void dash_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->body.physics == dash_friction) {
    fighter->body.physics = NULL;
  }
}

static void dash_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* While the task waits, the +44 callback keeps advancing the looping pose.
 * Rows [first, first + ticks) start with action and continue that pose. */
static int dash_rows(Fighter *fighter, int first, int ticks, int pose, FighterFn action) {
  native_frame(fighter, first, pose, action);
  for (int frame = first + 1; frame < first + ticks; ++frame) {
    pose = dash_advance(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  return dash_advance(pose);
}

/* 47af10: blocked contact stops motion, removes guard and stores 0x100e. */
static void dash_block_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->action = 0x100e;
}

/* 47ae06: camera shake 6, then 48f6e0 saves and stops the owner's motion. */
static void dash_stop(Fighter *fighter) {
  mk4_script_shake(fighter, 6);
  mk4_script_save_motion(fighter);
}

/* 47acc0 phase 1: 48f750 restores the saved velocity, then one decay. */
static void dash_resume(Fighter *fighter) {
  mk4_script_restore_motion(fighter);
  dash_friction(fighter);
}

/* Phase 2 with the counter still set: reaction 157, then another stop. */
static void dash_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  dash_stop(fighter);
}

/* 47ae90: reaction 168 ends the sequence; stop and wait 20 ticks. */
static void dash_final(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 168);
  mk4_script_stop_body(fighter);
}

/* The last phase-2 visit dispatches 157 and, in the same visit, 47ae90. */
static void dash_last_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  dash_final(fighter);
}

/* 47acc0 phase 0 on D[22] contact: blocked -> 47af10 (40 ticks); an
 * airborne victim (48ee60) -> 47ae90; otherwise the cursor is raised to 24
 * and the counter of two runs stop 5, resume 5, reaction 157 twice. */
static void dash_contact(Fighter *fighter, ContactResult result) {
  int pose = fighter_sample(fighter);
  if (result == CONTACT_BLOCK) {
    native_begin(fighter, 0, DASH_BLOCK_TICKS - 1, dash_ready);
    dash_rows(fighter, 0, DASH_BLOCK_TICKS, pose, dash_block_stop);
  } else if (fighter_airborne(fighter_opponent(fighter))) {
    native_begin(fighter, 0, DASH_FINAL_TICKS - 1, dash_ready);
    dash_rows(fighter, 0, DASH_FINAL_TICKS, pose, dash_final);
  } else {
    int frame = 0;
    native_begin(fighter, 0, 4 * DASH_STOP_TICKS + DASH_FINAL_TICKS - 1, dash_ready);
    pose = dash_rows(fighter, frame, DASH_STOP_TICKS, maxi(pose, 24), dash_stop);
    pose = dash_rows(fighter, frame += DASH_STOP_TICKS, DASH_STOP_TICKS, pose, dash_resume);
    pose = dash_rows(fighter, frame += DASH_STOP_TICKS, DASH_STOP_TICKS, pose, dash_pummel);
    pose = dash_rows(fighter, frame += DASH_STOP_TICKS, DASH_STOP_TICKS, pose, dash_resume);
    dash_rows(fighter, frame + DASH_STOP_TICKS, DASH_FINAL_TICKS, pose, dash_last_pummel);
  }
  native_play(fighter, false);
}

static void dash_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &dash_record);
  if (result != CONTACT_MISS) {
    dash_contact(fighter, result);
  }
}

/* 47ab50 phase 1 calls 47af90 itself before each further query. */
static void dash_step_query(Fighter *fighter) {
  dash_friction(fighter);
  dash_query(fighter);
}

/* Counter zero: 47af90 once more, no guard, 4914a0 removes the callback
 * and 429460 rewinds from the final stored pose to zero before return. */
static void dash_miss(Fighter *fighter) {
  dash_friction(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->body.physics = NULL;
}

/* 47ab50 phase 0: install 47af90, set the counter to 15 and query D[22]
 * once per visit. Both the callback and the phase-1 call advance a pose. */
void jax_dash_punch_contact(Fighter *fighter) {
  fighter->body.physics = dash_friction;
  fighter->mmove_cleanup = dash_cleanup;
  int pose = fighter_sample(fighter);
  int last = DASH_QUERIES + 1 + s_dash.last;
  native_begin(fighter, 0, last, dash_ready);
  native_frame(fighter, 0, pose, dash_query);
  for (int frame = 1; frame < DASH_QUERIES; ++frame) {
    pose = dash_advance(dash_advance(pose));
    native_frame(fighter, frame, pose, dash_step_query);
  }
  native_frame(fighter, DASH_QUERIES, dash_advance(dash_advance(pose)), dash_miss);
  for (int frame = DASH_QUERIES + 1; frame <= last; ++frame) {
    native_frame(fighter, frame, last - frame, NULL);
  }
  native_play(fighter, false);
}

/* 4ea4e8[157] -> 4883f0 -> 4ea028: effect 9, context sound 1, clip 311. */
void jax_dash_victim_hit(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &s_dash_stagger);
  retail_vm_enter_owned(victim, 0x4ea028, fighter_ready, 13);
}

/* 4ea4e8[168] -> 4867b0: disarm; an airborne victim takes 481ce0 (shake 0,
 * 4e89d8 fall and 488c00 roll getup), a grounded one the 4913a0 slide and
 * clip 130 of 4e9e38. */
void jax_dash_victim_throw(Fighter *victim) {
  mk4_script_disarm_self(victim);
  if (fighter_airborne(victim)) {
    mk4_script_shake(victim, 0);
    fighter_start_move(victim, S_KNOCKDOWN, &s_dash_air_fall);
    retail_vm_enter_owned(victim, 0x4e89d8, fighter_ready, 13);
    return;
  }
  fighter_start_move(victim, S_KNOCKDOWN, &s_dash_fall);
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e9e38, fighter_ready, 13);
}

/* 488c00 -> 4914c0(4ea220): roll getup 191/16 at -0x147a, crouch 190/16. */

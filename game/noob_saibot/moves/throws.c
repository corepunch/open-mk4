#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void noob_saibot_lk_throw(Fighter *fighter);
void noob_saibot_air_throw(Fighter *fighter);
static const MoveDef s_noob_air_throw = {.clip = 74, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_noob_air_thrown = {.clip = 150, .last = 27, .flags = SF_LOCK, .action = 0x4003};
/* 484760 changes neither the action nor the clip before 4e9880 binds
 * clip 175, so the captured victim keeps its thrown action. */
static const MoveDef s_slam_victim = {.clip = 175, .last = 21, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};

/* 46ac10: both paired bodies fall with gravity 0x147a after an upward
 * 0x1999 kick, then 4e5ea0 advances the pair to sample 27. */
void noob_saibot_script_air_throw_continue(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  noob_saibot_entry_retail_script_004e5ea0(fighter);
}

static void slam_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &s_slam_victim);
  victim->flags |= FF_REACTING;
  noob_saibot_entry_retail_script_004e9880(victim);
}

/* 480f50[105] -> 484760: disarm (494520), snap to the floor and stop
 * (4906c0), clear group flag 0x80 (490290), then the generated 4e9880
 * push-back, impact and clip-175 recovery on the victim's next update. */
void noob_saibot_slam_reaction(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  mk4_script_stop_body(victim);
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.angle[axis] = 0;
    victim->body.spin[axis] = 0;
  }
  victim->body.root_y = 0;
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->reaction = 105;
  victim->flags |= FF_REACTING;
  fighter_interrupt(victim, slam_victim);
}

static void slam_flip(Fighter *fighter) {
  mk4_throw_flip_tail(fighter);
}

/* 46acc0 state 0, on landing: the peer takes reaction 105, the owner's
 * action is cleared and 490890 applies 4e5ec0 (0x8f5 along the pair axis,
 * Y -0x30a3, gravity 0x28f) while 428d10 finishes the current clip one
 * sample per visit. State 1 re-enters the shared 470080 flip tail. */
void noob_saibot_slam_tail(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  noob_saibot_slam_reaction(victim);
  fighter->action = 0;
  fighter_set_speed(fighter, 0x8f5);
  fighter->body.vel[AXIS_Y] = -0x30a3;
  fighter->body.gravity = 0x28f;
  int sample = fighter_sample(fighter), last = fighter_move(fighter)->last;
  if (sample >= last) {
    fighter_finish_mmove(fighter, slam_flip);
    return;
  }
  native_begin(fighter, 1, last - sample, slam_flip);
  for (int frame = 1; frame <= last - sample; ++frame) {
    native_frame(fighter, frame, sample + frame, NULL);
  }
  native_play(fighter, false);
}

/* 46aca0 after the 4e5eb0 landing wait: 48de80 subtracts 0x30a3 from the
 * peer, then the shared slam tail 46acc0 releases it. */
void noob_saibot_script_air_throw_end(Fighter *fighter) {
  battle_damage((Fighter *)fighter_opponent(fighter), 0x30a3);
  noob_saibot_slam_tail(fighter);
}

static void released_fall_gate(Fighter *victim) {
  victim->mmove_hold = fighter_airborne(victim);
}

/* Portable interruption policy: retail parks the captured peer inside the
 * owner's task (481010) and has no release on this path. If the owner is
 * interrupted before landing, the shared hold drops the victim; it falls
 * without damage and returns to control through the KO-aware finish. */
static void released_fall(Fighter *victim) {
  native_begin(victim, 0, 0, mk4_mmove_thrown_finish);
  native_frame(victim, 0, fighter_sample(victim), released_fall_gate);
  native_play(victim, false);
}

void noob_saibot_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de798);
}

void noob_saibot_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de2f8);
}

/* 46aba0: action 0x502, face the peer (490bd0), then the shared 480fc0(8)
 * and 48f9f0 capture before 4e5e78. 532254's 30-tick overlap suppression
 * is the portable paired-body exemption. */
void noob_saibot_air_throw(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  }
  mk4_air_throw_frames_with_moves(fighter, &s_noob_air_throw, &s_noob_air_thrown, noob_saibot_entry_retail_script_004e5e78, released_fall);
}

#include "../character.h"

/* LK [F,D] record 4e4838 and the medium/far CPU rows select native 47cf30.
 * Clips: 87 sink (53cf54), 88 stomp opening (4e8418), 89 repeated stomp
 * (53cf58), 90 release (4e8420), 91 missed landing (4e8438), 301 exit.
 * Later clips are bound within the same action, like 428750/4287a0. */
static const MoveDef s_sink = {.clip = 87, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x400};
/* 47cf89 queries D[10]/4ec158 every falling visit. */
static const AttackRecord stomp_record = {0x68f5, 0x6e14, 0xf0a3, 0xa3d7, 106, 106, 0, 0, 0x3851};
static const MoveDef s_stomped = {.clip = 178, .last = 27, .flags = SF_LOCK | SF_REACTION};

enum {
  /* Stored sample counts of the clips bound by the native continuations. */
  STOMP_OPEN_COUNT = 10,
  STOMP_COUNT = 15,
  STOMP_RELEASE_COUNT = 13,
  MISSED_COUNT = 9,
  FLIP_COUNT = 9,
  STOMP_REPEATS = 3, /* 47d28e loads counter 53c43c with three */
  VICTIM_CYCLES = 4, /* 484420's jump table has four 484650 states */
};

static void teleport_drop(Fighter *fighter);
static void stomp_cycle(Fighter *fighter);

static void teleport_sink(Fighter *fighter);

/* 4914a0 clears the sink callback once 47d550 relocates the owner. Keep an
 * interrupted owner from staying below the floor. */
static void teleport_cleanup(Fighter *fighter) {
  if (fighter->body.physics == teleport_sink) {
    fighter->body.physics = NULL;
  }
  if (fighter->body.gravity == 0 && fighter->body.pos[AXIS_Y] > fighter->body.ground) {
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    fighter->body.vel[AXIS_Y] = 0;
  }
}

/* 47d550: once group Y reaches 0x9999, move to the opponent X/Z saved by
 * state 1, four units above the origin, falling with gravity 0xa3d. */
static void teleport_sink(Fighter *fighter) {
  if (fighter->body.pos[AXIS_Y] < 0x9999) {
    return;
  }
  fighter->body.pos[AXIS_X] = fighter->saved_motion[0];
  fighter->body.pos[AXIS_Z] = fighter->saved_motion[1];
  fighter->body.pos[AXIS_Y] = -0x40000;
  fighter->body.gravity = 0xa3d;
  fighter->body.physics = NULL;
}

static void teleport_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* Play the bound clip forwards from its current sample to its last one. */
static void play_out(Fighter *fighter, FighterFn end) {
  int pose = fighter_sample(fighter), last = fighter_move(fighter)->last;
  if (pose >= last) {
    end(fighter);
    return;
  }
  native_begin(fighter, pose + 1, last, end);
  for (int frame = pose + 1; frame <= last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 47d500: camera shake 0, sound A[19], clip 91 to its end, then 46f650. */
static void teleport_missed(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  battle_sound(SOUND_A, 19);
  mk4_mmove_bind(fighter, 91, MISSED_COUNT, 0);
  play_out(fighter, teleport_ready);
}

/* 47d3fa/47d350: action 0, stand at 0.98 of the opponent origin, then flip
 * off with speed -0x1999, Y velocity -0x3333 and gravity 0x28f; clip 301 at
 * its tuck sample spins at 0x4ccc into the shared 470080 return. */
static void stomp_exit_launch(Fighter *fighter) {
  mk4_mmove_bind(fighter, 301, FLIP_COUNT, FLIP_COUNT - 1);
  fighter_set_speed(fighter, -0x1999);
  fighter->body.vel[AXIS_Y] = -0x3333;
  fighter->body.gravity = 0x28f;
  fighter->body.spin[AXIS_X] = 0x4ccc;
  mk4_flip_apex_return(fighter);
}

static void stomp_exit(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->action = 0;
  fighter->body.pos[AXIS_X] = q16_mul(victim->body.pos[AXIS_X], 0xfae1);
  fighter->body.pos[AXIS_Z] = q16_mul(victim->body.pos[AXIS_Z], 0xfae1);
  native_hold(fighter, 1, stomp_exit_launch);
}

static void stomp_sound(Fighter *fighter) {
  mk4_script_context_sound(fighter, 12); /* 47d4b0 -> 48a100(12) */
}

/* 47d1ca: sound again, then repeat clip 89 or play clip 90 into 47d320. */
static void stomp_cycle_end(Fighter *fighter) {
  stomp_sound(fighter);
  if (--fighter->script_phase > 0) {
    stomp_cycle(fighter);
    return;
  }
  mk4_mmove_bind(fighter, 90, STOMP_RELEASE_COUNT, 0);
  play_out(fighter, stomp_exit);
}

/* 47d298/47d201: bind clip 89, advance seven samples, sound, step one more
 * sample and play the clip out. */
static void stomp_cycle(Fighter *fighter) {
  mk4_mmove_bind(fighter, 89, STOMP_COUNT, 0);
  native_begin(fighter, 1, STOMP_COUNT - 1, stomp_cycle_end);
  for (int frame = 1; frame < STOMP_COUNT; ++frame) {
    native_frame(fighter, frame, frame, frame == 8 ? stomp_sound : NULL);
  }
  native_play(fighter, false);
}

static void stomp_open_end(Fighter *fighter) {
  fighter->script_phase = STOMP_REPEATS;
  stomp_cycle(fighter);
}

/* 47cfbd: 490ae0(4e8414) installs 480f40 (camera mode) and 4287a0(4e8418)
 * plays clip 88 before 47d1a0. Gravity continues, so the owner lands. */
static void teleport_hit(Fighter *fighter) {
  mk4_mmove_bind(fighter, 88, STOMP_OPEN_COUNT, 0);
  play_out(fighter, stomp_open_end);
}

/* 48f410: hold until the recoil falls back below Y -2.5; 47ff80 rights the
 * body before generated 4e8818 retracts clip 301 and lands. */
static void blocked_low(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.pos[AXIS_Y] < -0x28000;
}

static void blocked_retract(Fighter *fighter) {
  fighter_upright(fighter);
  quan_chi_entry_retail_script_004e8818(fighter);
}

static void blocked_fall(Fighter *fighter) {
  native_begin(fighter, 0, 0, blocked_retract);
  native_frame(fighter, 0, FLIP_COUNT - 1, blocked_low);
  native_play(fighter, false);
}

/* 48f490: hold until the recoil reaches its apex. */
static void blocked_apex(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.vel[AXIS_Y] < 0;
}

/* 47fe60: action 1011; 48fed0 places the blocker 0.5 ahead with friction;
 * motion 4e87f8 (-0x28f, -0x1999, 0x1ca); 47db60 binds clip 301 at its
 * tuck, spins -0x4ccc, raises the root 0x3333 and installs 4707e0. */
static void teleport_blocked(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x8000);
  }
  victim->body.physics = fighter_friction;
  fighter->action = 0x1011;
  mk4_mmove_bind(fighter, 301, FLIP_COUNT, FLIP_COUNT - 1);
  fighter_set_speed(fighter, -0x28f);
  fighter->body.vel[AXIS_Y] = -0x1999;
  fighter->body.gravity = 0x1ca;
  fighter->body.spin[AXIS_X] = -0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.physics = mk4_script_landing_motion;
  native_begin(fighter, 0, 0, blocked_fall);
  native_frame(fighter, 0, FLIP_COUNT - 1, blocked_apex);
  native_play(fighter, false);
}

/* 47cf6b: gravity zero means the owner landed without contact (47d500). */
static void teleport_query(Fighter *fighter) {
  if (fighter->body.gravity == 0) {
    teleport_missed(fighter);
    return;
  }
  ContactResult result = battle_contact(fighter, &stomp_record);
  if (result == CONTACT_BLOCK) {
    teleport_blocked(fighter);
  } else if (result == CONTACT_HIT) {
    teleport_hit(fighter);
  } else {
    fighter->mmove_hold = true;
  }
}

static void teleport_drop(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, fighter_sample(fighter), teleport_query);
  native_play(fighter, false);
}

/* State 2: wait while the sink callback is still installed. */
static void teleport_sinking(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.physics == teleport_sink;
}

/* State 1 (47d022): save the opponent X/Z in the task for 47d550. */
static void teleport_save(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->saved_motion[0] = victim->body.pos[AXIS_X];
  fighter->saved_motion[1] = victim->body.pos[AXIS_Z];
  native_begin(fighter, 0, 1, teleport_drop);
  native_frame(fighter, 0, fighter_sample(fighter), NULL);
  native_frame(fighter, 1, fighter_sample(fighter), teleport_sinking);
  native_play(fighter, false);
}

/* State 0 (47d08c): action 400, sink callback 4e8410/47d550, sound C[36],
 * indexed 22, Y velocity 0x1999 and clip 87 played to sample 15 (428d30). */
void quan_chi_teleport_stomp(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_sink);
  fighter->mmove_cleanup = teleport_cleanup;
  fighter->sample_override = 0;
  fighter->saved_motion[0] = fighter->body.pos[AXIS_X];
  fighter->saved_motion[1] = fighter->body.pos[AXIS_Z];
  battle_sound(SOUND_C, 36);
  battle_sound(SOUND_INDEXED, 22);
  fighter->body.vel[AXIS_Y] = 0x1999;
  fighter->body.gravity = 0;
  fighter->body.physics = teleport_sink;
  native_begin(fighter, 1, 15, teleport_save);
  for (int frame = 1; frame <= 15; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 47ffc0: 46fd90 landing, then the controller return. */
void quan_chi_script_teleport_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void victim_cycle(Fighter *victim);

/* 4845f0: wait 20 ticks, then 488ba0 reversed getup 328 (crouch 184). */
static void victim_getup(Fighter *victim) {
  static const NativeGetupDef profile = {.clip = 328, .count = 15, .crouch_clip = 184, .crouch_count = 16, .speed = 0, .crouch_speed = -0xa3d};
  mk4_mmove_getup(victim, &profile);
}

static void victim_release(Fighter *victim) {
  native_hold(victim, 20, victim_getup);
}

/* 484650 state 1: continue while the stomper keeps action 400 or 409. */
static void victim_cycle_end(Fighter *victim) {
  int action = fighter_opponent(victim)->action;
  if ((action == 0x400 || action == 0x409) && ++victim->script_phase < VICTIM_CYCLES) {
    victim_cycle(victim);
  } else {
    victim_release(victim);
  }
}

/* 484650 state 0: shake 0, effect 13, cursor 15 and play clip 178 out. */
static void victim_cycle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
  victim->sample_override = 15;
  play_out(victim, victim_cycle_end);
}

/* 484420 after generated 4e9868 played clip 178. */
void quan_chi_script_stomp_victim(Fighter *victim) {
  victim->script_phase = 0;
  victim_cycle(victim);
}

/* Reaction 106 (4843b0): 494520 disarm; an airborne victim (48ef30) drops
 * with Y velocity and gravity 0x1999; then VM 4e9868. */
void quan_chi_stomp_reaction(Fighter *victim) {
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_stomped);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim)) {
    victim->body.vel[AXIS_Y] = 0x1999;
    victim->body.gravity = 0x1999;
  }
  quan_chi_entry_retail_script_004e9868(victim);
}

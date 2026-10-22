#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: 47bf90, 47bfd0. */
static const MoveDef n47bf90_dive = {.clip = 58, .last = 9, .flags = SF_LOCK | SF_RECORD, .action = 0x20f};
static const AttackRecord n47bf90_dive_contact = {0x12666, -0xa3d, 0x107ae, 0x851e, 10, 10, 0, 0, 0x147a};

static void n47bf90_dive_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.on_land = NULL;
}

bool fujin_can_dive(const Fighter *fighter) {
  /* 45eeb0 requires root Y <= -1.8 and the opponent combo-hit count <=4 gate. */
  return fighter->body.pos[AXIS_Y] + fighter->body.root_y <= -0x1cccc && fighter_opponent(fighter)->hits <= 4;
}

void fujin_dive_land(Fighter *fighter) {
  /* 47bfd0 runs the shared landing entry46fd90, then controller return. */
  fighter_finish_mmove(fighter, mk4_land);
}

static void n47bf90_dive_recoil(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  retail_vm_enter_owned(fighter, 0x4e8298, fighter_ready, 5);
}

static void n47bf90_dive_query(Fighter *fighter) {
  /* 4704d0 checks the floor before each subsequent D[15] query. */
  if (!fighter_airborne(fighter)) {
    fujin_dive_land(fighter);
    return;
  }
  if (battle_contact(fighter, &n47bf90_dive_contact) != CONTACT_MISS) {
    n47bf90_dive_recoil(fighter);
  }
}

void fujin_dive_contact(Fighter *fighter) {
  /* 47bf90 selects4e8280:60 visits,D[15],contact/timeout47bfb0,
   * floor47bfd0. The clip holds its final pose while motion continues. */
  native_begin(fighter, 1, 60, n47bf90_dive_recoil);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, 9, n47bf90_dive_query);
  }
  native_play(fighter, false);
}

void fujin_dive(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n47bf90_dive);
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->mmove_cleanup = n47bf90_dive_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8258, fighter_ready, 5);
}

/* Native service cluster: 47dae0. */
/* Jarek's cannonball 47b310 (B,F+LK and every CPU range row) and upward
 * ball 4797f0 (F,D,F+HP). Both bind tuck clip 301 through 53cf4c/53cf44,
 * play it two poses per visit (429220), then query D[19] every visit. */
static const AttackRecord n47dae0_ball_record = {0x5958, -0x49ba, 0xccc6, 0xadd2, 42, 42, 0, 0, 0x23d7};
static const MoveDef n47dae0_roll_move = {.clip = 301, .last = 8, .flags = SF_LOCK | SF_RECORD, .action = 0x214};
static const MoveDef n47dae0_rise_move = {.clip = 301, .last = 8, .flags = SF_LOCK | SF_RECORD, .action = 0x21a};
static const MoveDef n47dae0_victim_move = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};

static void n47dae0_ball_face(Fighter *fighter) {
  /* 490bd0 corrects the facing relative to the peer. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

static void n47dae0_ball_tuck(Fighter *fighter, q16 spin) {
  /* 4702e0 lowers the root by .2 while raising the group by the same amount. */
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = spin;
}

static void n47dae0_rise_decay(Fighter *fighter);

static void n47dae0_ball_cleanup(Fighter *fighter) {
  if (fighter->body.physics == mk4_script_landing_motion) {
    native_motion_stop(fighter);
  }
  if (fighter->body.physics == n47dae0_rise_decay) {
    fighter->body.physics = NULL;
  }
  fighter_upright(fighter);
}

static void n47dae0_ball_startup(Fighter *fighter, const MoveDef *move, FighterFn setup, FighterFn next) {
  fighter_start_move(fighter, S_KICK, move);
  fighter->mmove_cleanup = n47dae0_ball_cleanup;
  /* 48a0d0 indexed 0x22 and 489f90 bank C 0x45 precede the clip bind. */
  battle_sound(SOUND_INDEXED, 0x22);
  battle_sound(SOUND_C, 0x45);
  fighter->sample_override = 0;
  /* Five visits submit poses 0, 2, 4, 6 and 8; the popped native continuation
   * runs on the next visit and yields once before its first contact query. */
  native_begin(fighter, 0, 5, next);
  for (int frame = 0; frame <= 4; ++frame) {
    native_frame(fighter, frame, 2 * frame, NULL);
  }
  native_frame(fighter, 5, 8, setup);
  native_play(fighter, true);
}

static void n47dae0_recoil_wait(Fighter *fighter) {
  /* 48f490 holds while rising; 47da70 then holds above absolute Y -2.5. */
  if (fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  /* 47daa0: 490720/490de0 clear spin and orientation, 490bd0 faces the
   * peer, then generated 4e84b8 rewinds 301 and lands through 47dae0. */
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  n47dae0_ball_face(fighter);
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 12);
}

/* 47d940: contact recoil shared by both balls. */
static void n47dae0_ball_recoil(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->action = 0x1010;
  fighter->body.physics = NULL;
  /* 48fed0 -> 48fcd0(4ec2c4): 48fee0 runs in the victim's context and
   * places this actor .5 units along the victim's heading, without Y. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = victim->body.pos[axis] + q16_mul(fighter_axis(victim, axis), 0x8000);
  }
  /* 490890(4e84a8): back .05, up .14, gravity .0055. */
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  fighter->body.on_land = NULL;
  /* Roster 12 selects 47dbc0, which scales only the X velocity by .6. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0x9999);
  /* 47db60 binds 301 at its last pose, adds another tuck and installs the
   * 4e84d0 -> 4707e0 twelve-visit whoosh as the physics callback. */
  mk4_mmove_bind(fighter, 301, 9, 8);
  n47dae0_ball_tuck(fighter, -0x4ccc);
  mk4_script_landing_motion(fighter);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, n47dae0_recoil_wait);
  native_play(fighter, false);
}

static bool n47dae0_ball_contact(Fighter *fighter) {
  if (battle_contact(fighter, &n47dae0_ball_record) == CONTACT_MISS) {
    return false;
  }
  n47dae0_ball_recoil(fighter);
  return true;
}

static void n47dae0_roll_query(Fighter *fighter) {
  n47dae0_ball_contact(fighter);
}

static void n47dae0_roll_ready(Fighter *fighter) {
  n47dae0_ball_face(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

static void n47dae0_roll_end(Fighter *fighter) {
  /* 4702b0, 48e080, cleared spin/orientation, 4906e0 and 4906c0. */
  fighter_upright(fighter);
  fighter_neutral(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  /* 429380 submits the current pose, then reverses one pose per visit. */
  native_begin(fighter, 0, 8, n47dae0_roll_ready);
  for (int frame = 0; frame <= 7; ++frame) {
    native_frame(fighter, frame, 7 - frame, NULL);
  }
  native_frame(fighter, 8, 0, NULL);
  native_play(fighter, false);
}

static void n47dae0_roll_setup(Fighter *fighter) {
  n47dae0_ball_tuck(fighter, 0x4ccc);
  fighter_set_speed(fighter, 0x3333);
}

static void n47dae0_roll_contacts(Fighter *fighter) {
  /* 47b310 stage 2 queries D[19] for nineteen visits. */
  native_begin(fighter, 0, 18, n47dae0_roll_end);
  for (int frame = 0; frame <= 18; ++frame) {
    native_frame(fighter, frame, 8, n47dae0_roll_query);
  }
  native_play(fighter, false);
}

void jarek_cannonball(Fighter *fighter) {
  n47dae0_ball_startup(fighter, &n47dae0_roll_move, n47dae0_roll_setup, n47dae0_roll_contacts);
}

static void n47dae0_rise_decay(Fighter *fighter) {
  /* 4e80a0 -> 47bf50 -> 490650 scales horizontal velocity by .9. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void n47dae0_rise_land(Fighter *fighter) {
  /* 48f390 waits for gravity to clear; 479a10 then clears orientation,
   * restores the root and returns through the shared 46fd90 landing. */
  if (fighter->body.gravity != 0) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  fighter_upright(fighter);
  fighter_neutral(fighter);
  retail_recoil_land(fighter);
}

static void n47dae0_rise_query(Fighter *fighter) {
  if (n47dae0_ball_contact(fighter)) {
    return;
  }
  /* 479960 stops querying once the rise slows to -.01 per visit. */
  if (fighter->body.vel[AXIS_Y] < -0x28f) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, n47dae0_rise_land);
  native_play(fighter, false);
}

static void n47dae0_rise_contacts(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, n47dae0_rise_query);
  native_play(fighter, false);
}

static void n47dae0_rise_setup(Fighter *fighter) {
  n47dae0_ball_tuck(fighter, 0x6666);
  fighter->body.vel[AXIS_Y] = -0x3333;
  fighter->body.gravity = 0x24d;
  fighter->body.physics = n47dae0_rise_decay;
}

void jarek_upward_ball(Fighter *fighter) {
  n47dae0_ball_startup(fighter, &n47dae0_rise_move, n47dae0_rise_setup, n47dae0_rise_contacts);
  /* 4797f0 sets its forward speed before binding the tuck clip. */
  fighter_set_speed(fighter, 0x28f5);
}

/* 4ea4e8[42] -> 4835a0: drop the victim's weapon, then generated 4e9658. */
void jarek_ball_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &n47dae0_victim_move);
  fighter->flags |= FF_REACTING;
  retail_vm_enter_owned(fighter, 0x4e9658, fighter_ready, 12);
}

/* Native service cluster: 478e20. */
/* 478df0 sets action 20b and enters 4e8870: indexed cue 59, clip 551 with
 * its fast-start branch, sound C31 and forward .15 at sample 26. */
static const MoveDef n478e20_lunge_kick = {.clip = 551, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x20b};
/* B[9] 0x4eba20: 14% damage, reactions 127 hit/air and 5 block. */
static const AttackRecord n478e20_lunge_contact = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};

static void n478e20_lunge_query(Fighter *fighter) {
  /* 470b30 skips 470dc0 after the first contact in this list. */
  if (!fighter->connected) {
    battle_contact(fighter, &n478e20_lunge_contact);
  }
}

static void n478e20_lunge_recover(Fighter *fighter) {
  /* 478e40: stop motion, 428cf0 finishes clip 551 from the next cursor,
   * then 46f650 returns through the controller. */
  int first = fighter_sample(fighter) + 1, last = fighter_move(fighter)->last;
  mk4_script_stop(fighter);
  require(first <= last, "Kai lunge kick recovery needs its remaining clip");
  native_begin(fighter, 1, last - first + 1, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= last - first + 1; ++frame) {
    native_frame(fighter, frame, first + frame - 1, NULL);
  }
  native_play(fighter, false);
}

void kai_lunge_kick_contact(Fighter *fighter) {
  /* 478e20 -> 470b30 list 4e88a0: B[9] at samples 30..34, then 478e40. */
  require(fighter_move(fighter)->clip == 551 && fighter_sample(fighter) == 29, "Kai lunge kick needs its generated startup");
  native_begin(fighter, 1, 5, n478e20_lunge_recover);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, 29 + frame, n478e20_lunge_query);
  }
  native_play(fighter, false);
}

void kai_input_478df0(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n478e20_lunge_kick);
  retail_vm_enter_owned(fighter, 0x4e8870, fighter_ready, 11);
}

/* Native service cluster: 47ee00. */
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static void n47ee00_flying_query(Fighter *fighter);
static void n47ee00_flying_branch(Fighter *fighter);
static void n47ee00_flying_hold(Fighter *fighter);
static const MoveDef n47ee00_s_flying_kick = {.clip = 92, .last = 5, .flags = RETAIL, .action = ACTION_RETAIL_0207};
static const AttackRecord n47ee00_flying_record = {0x14f5c, -0xf5c, 0xf5c2, 0x51eb, 31, 31, 0, 0, 0x23d7};

static void n47ee00_flying_query(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter) && battle_contact(fighter, &n47ee00_flying_record) == CONTACT_MISS;
}

static void n47ee00_flying_branch(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    retail_vm_enter_owned(fighter, 0x4e86d0, fighter_ready, 3);
  } else if (fighter->contact == CONTACT_BLOCK) {
    retail_vm_enter_owned(fighter, 0x4e8718, fighter_ready, 3);
  } else {
    retail_vm_enter_owned(fighter, 0x4e86e8, fighter_ready, 3);
  }
}

static void n47ee00_flying_hold(Fighter *fighter) {
  if (fighter_airborne(fighter) && battle_contact(fighter, &n47ee00_flying_record) == CONTACT_MISS) {
    native_begin(fighter, 0, 0, n47ee00_flying_branch);
    fighter->native.frames[0].thinkfunc = n47ee00_flying_query;
    native_play(fighter, false);
  } else {
    n47ee00_flying_branch(fighter);
  }
}

void liu_kang_flying_kick_script(Fighter *fighter) {
  native_finish_clip(fighter, n47ee00_flying_hold);
}

void liu_kang_flying_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47ee00_s_flying_kick);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x23d7);
  fighter->body.vel[AXIS_Y] = -0x170a;
  fighter->body.gravity = 0x106;
  battle_sound(SOUND_INDEXED, 0);
  retail_vm_enter_owned(fighter, 0x4e86a8, fighter_ready, 3);
}

#undef RETAIL

/* Native service cluster: 47a3b0. */
/* HK [F,F] record 4e4858 and the near CPU row select native 47a370: action
 * 217, 490ae0(4e80f8) installs group callback 47bf50, then VM 4e8100 plays
 * clip 105 to sample 10, sets speed 0x547a and reaches sample 15. */
static const MoveDef n47a3b0_s_flip_kick = {.clip = 105, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x217};
/* 47a500 queries D[24]/4ec020. */
static const AttackRecord n47a3b0_flip_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 160, 160, 8, 0, 0x2666};
static const MoveDef n47a3b0_s_flip_victim = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION};

/* 47bf50 -> 490650: scale horizontal velocity by 0xe666 every world pump. */
static void n47a3b0_flip_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void n47a3b0_flip_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->body.physics == n47a3b0_flip_decay) {
    fighter->body.physics = NULL;
  }
}

static void n47a3b0_flip_ready(Fighter *fighter) {
  n47a3b0_flip_cleanup(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 47a610 -> 428d10 plays the remaining stored samples, then 46f650. */
static void n47a3b0_flip_finish(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  if (pose >= n47a3b0_s_flip_kick.last) {
    n47a3b0_flip_ready(fighter);
    return;
  }
  native_begin(fighter, pose + 1, n47a3b0_s_flip_kick.last, n47a3b0_flip_ready);
  for (int frame = pose + 1; frame <= n47a3b0_s_flip_kick.last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 47a500: contact submits pose 19 (429970), stops motion (4906e0) and sets
 * no-guard (490140). A block waits five extra ticks before 47a5c0's ten. */
static void n47a3b0_flip_contact(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &n47a3b0_flip_record);
  if (result == CONTACT_MISS) {
    return;
  }
  fighter->sample_override = 19;
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  native_hold(fighter, result == CONTACT_BLOCK ? 15 : 10, n47a3b0_flip_finish);
}

/* 47a3b0: four visits each query D[24] and advance one sample (429830);
 * the first three wait one tick, the fourth waits eight before 47a610. */
void quan_chi_script_flip_kick_tail(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  require(fighter_move(fighter)->clip == 105 && pose == 15, "Flip kick contact needs its selected 4e8100 startup");
  native_begin(fighter, 0, 11, n47a3b0_flip_finish);
  for (int frame = 0; frame <= 11; ++frame) {
    native_frame(fighter, frame, pose + (frame < 4 ? frame : 4), frame < 4 ? n47a3b0_flip_contact : NULL);
  }
  native_play(fighter, false);
}

void quan_chi_flip_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n47a3b0_s_flip_kick);
  fighter->mmove_cleanup = n47a3b0_flip_cleanup;
  fighter->body.physics = n47a3b0_flip_decay;
  retail_vm_enter_owned(fighter, 0x4e8100, fighter_ready, 8);
}

/* 488c00: shared roll getup after the reaction-160 landing. */

/* Reaction 160 (481d90): 488ea0 wall recoil, 494520 disarm, then VM 4e8a48
 * launches clip 189 and lands into 488c00. */
void quan_chi_flip_reaction(Fighter *victim) {
  battle_wall_recoil(victim);
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &n47a3b0_s_flip_victim);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  retail_vm_enter_owned(victim, 0x4e8a48, fighter_ready, 8);
}

/* Native service cluster: 484420. */
/* LK [F,D] record 4e4838 and the medium/far CPU rows select native 47cf30.
 * Clips: 87 sink (53cf54), 88 stomp opening (4e8418), 89 repeated stomp
 * (53cf58), 90 release (4e8420), 91 missed landing (4e8438), 301 exit.
 * Later clips are bound within the same action, like 428750/4287a0. */
static const MoveDef n484420_s_sink = {.clip = 87, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x400};
/* 47cf89 queries D[10]/4ec158 every falling visit. */
static const AttackRecord n484420_stomp_record = {0x68f5, 0x6e14, 0xf0a3, 0xa3d7, 106, 106, 0, 0, 0x3851};
static const MoveDef n484420_s_stomped = {.clip = 178, .last = 27, .flags = SF_LOCK | SF_REACTION};

enum {
  /* Stored sample counts of the clips bound by the native continuations. */
  n484420_STOMP_OPEN_COUNT = 10,
  n484420_STOMP_COUNT = 15,
  n484420_STOMP_RELEASE_COUNT = 13,
  n484420_MISSED_COUNT = 9,
  n484420_FLIP_COUNT = 9,
  n484420_STOMP_REPEATS = 3, /* 47d28e loads counter 53c43c with three */
  n484420_VICTIM_CYCLES = 4, /* 484420's jump table has four 484650 states */
};

static void n484420_teleport_drop(Fighter *fighter);
static void n484420_stomp_cycle(Fighter *fighter);

static void n484420_teleport_sink(Fighter *fighter);

/* 4914a0 clears the sink callback once 47d550 relocates the owner. Keep an
 * interrupted owner from staying below the floor. */
static void n484420_teleport_cleanup(Fighter *fighter) {
  if (fighter->body.physics == n484420_teleport_sink) {
    fighter->body.physics = NULL;
  }
  if (fighter->body.gravity == 0 && fighter->body.pos[AXIS_Y] > fighter->body.ground) {
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    fighter->body.vel[AXIS_Y] = 0;
  }
}

/* 47d550: once group Y reaches 0x9999, move to the opponent X/Z saved by
 * state 1, four units above the origin, falling with gravity 0xa3d. */
static void n484420_teleport_sink(Fighter *fighter) {
  if (fighter->body.pos[AXIS_Y] < 0x9999) {
    return;
  }
  fighter->body.pos[AXIS_X] = fighter->saved_motion[0];
  fighter->body.pos[AXIS_Z] = fighter->saved_motion[1];
  fighter->body.pos[AXIS_Y] = -0x40000;
  fighter->body.gravity = 0xa3d;
  fighter->body.physics = NULL;
}

static void n484420_teleport_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* Play the bound clip forwards from its current sample to its last one. */
static void n484420_play_out(Fighter *fighter, FighterFn end) {
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
static void n484420_teleport_missed(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  battle_sound(SOUND_A, 19);
  mk4_mmove_bind(fighter, 91, n484420_MISSED_COUNT, 0);
  n484420_play_out(fighter, n484420_teleport_ready);
}

/* 47d3fa/47d350: action 0, stand at 0.98 of the opponent origin, then flip
 * off with speed -0x1999, Y velocity -0x3333 and gravity 0x28f; clip 301 at
 * its tuck sample spins at 0x4ccc into the shared 470080 return. */
static void n484420_stomp_exit_launch(Fighter *fighter) {
  mk4_mmove_bind(fighter, 301, n484420_FLIP_COUNT, n484420_FLIP_COUNT - 1);
  fighter_set_speed(fighter, -0x1999);
  fighter->body.vel[AXIS_Y] = -0x3333;
  fighter->body.gravity = 0x28f;
  fighter->body.spin[AXIS_X] = 0x4ccc;
  mk4_flip_apex_return(fighter);
}

static void n484420_stomp_exit(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->action = 0;
  fighter->body.pos[AXIS_X] = q16_mul(victim->body.pos[AXIS_X], 0xfae1);
  fighter->body.pos[AXIS_Z] = q16_mul(victim->body.pos[AXIS_Z], 0xfae1);
  native_hold(fighter, 1, n484420_stomp_exit_launch);
}

static void n484420_stomp_sound(Fighter *fighter) {
  mk4_script_context_sound(fighter, 12); /* 47d4b0 -> 48a100(12) */
}

/* 47d1ca: sound again, then repeat clip 89 or play clip 90 into 47d320. */
static void n484420_stomp_cycle_end(Fighter *fighter) {
  n484420_stomp_sound(fighter);
  if (--fighter->script_phase > 0) {
    n484420_stomp_cycle(fighter);
    return;
  }
  mk4_mmove_bind(fighter, 90, n484420_STOMP_RELEASE_COUNT, 0);
  n484420_play_out(fighter, n484420_stomp_exit);
}

/* 47d298/47d201: bind clip 89, advance seven samples, sound, step one more
 * sample and play the clip out. */
static void n484420_stomp_cycle(Fighter *fighter) {
  mk4_mmove_bind(fighter, 89, n484420_STOMP_COUNT, 0);
  native_begin(fighter, 1, n484420_STOMP_COUNT - 1, n484420_stomp_cycle_end);
  for (int frame = 1; frame < n484420_STOMP_COUNT; ++frame) {
    native_frame(fighter, frame, frame, frame == 8 ? n484420_stomp_sound : NULL);
  }
  native_play(fighter, false);
}

static void n484420_stomp_open_end(Fighter *fighter) {
  fighter->script_phase = n484420_STOMP_REPEATS;
  n484420_stomp_cycle(fighter);
}

/* 47cfbd: 490ae0(4e8414) installs 480f40 (camera mode) and 4287a0(4e8418)
 * plays clip 88 before 47d1a0. Gravity continues, so the owner lands. */
static void n484420_teleport_hit(Fighter *fighter) {
  mk4_mmove_bind(fighter, 88, n484420_STOMP_OPEN_COUNT, 0);
  n484420_play_out(fighter, n484420_stomp_open_end);
}

/* 48f410: hold until the recoil falls back below Y -2.5; 47ff80 rights the
 * body before generated 4e8818 retracts clip 301 and lands. */
static void n484420_blocked_low(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.pos[AXIS_Y] < -0x28000;
}

static void n484420_blocked_retract(Fighter *fighter) {
  fighter_upright(fighter);
  retail_vm_enter_owned(fighter, 0x4e8818, fighter_ready, 8);
}

static void n484420_blocked_fall(Fighter *fighter) {
  native_begin(fighter, 0, 0, n484420_blocked_retract);
  native_frame(fighter, 0, n484420_FLIP_COUNT - 1, n484420_blocked_low);
  native_play(fighter, false);
}

/* 48f490: hold until the recoil reaches its apex. */
static void n484420_blocked_apex(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.vel[AXIS_Y] < 0;
}

/* 47fe60: action 1011; 48fed0 places the blocker 0.5 ahead with friction;
 * motion 4e87f8 (-0x28f, -0x1999, 0x1ca); 47db60 binds clip 301 at its
 * tuck, spins -0x4ccc, raises the root 0x3333 and installs 4707e0. */
static void n484420_teleport_blocked(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x8000);
  }
  victim->body.physics = fighter_friction;
  fighter->action = 0x1011;
  mk4_mmove_bind(fighter, 301, n484420_FLIP_COUNT, n484420_FLIP_COUNT - 1);
  fighter_set_speed(fighter, -0x28f);
  fighter->body.vel[AXIS_Y] = -0x1999;
  fighter->body.gravity = 0x1ca;
  fighter->body.spin[AXIS_X] = -0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.physics = mk4_script_landing_motion;
  native_begin(fighter, 0, 0, n484420_blocked_fall);
  native_frame(fighter, 0, n484420_FLIP_COUNT - 1, n484420_blocked_apex);
  native_play(fighter, false);
}

/* 47cf6b: gravity zero means the owner landed without contact (47d500). */
static void n484420_teleport_query(Fighter *fighter) {
  if (fighter->body.gravity == 0) {
    n484420_teleport_missed(fighter);
    return;
  }
  ContactResult result = battle_contact(fighter, &n484420_stomp_record);
  if (result == CONTACT_BLOCK) {
    n484420_teleport_blocked(fighter);
  } else if (result == CONTACT_HIT) {
    n484420_teleport_hit(fighter);
  } else {
    fighter->mmove_hold = true;
  }
}

static void n484420_teleport_drop(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, fighter_sample(fighter), n484420_teleport_query);
  native_play(fighter, false);
}

/* State 2: wait while the sink callback is still installed. */
static void n484420_teleport_sinking(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.physics == n484420_teleport_sink;
}

/* State 1 (47d022): save the opponent X/Z in the task for 47d550. */
static void n484420_teleport_save(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->saved_motion[0] = victim->body.pos[AXIS_X];
  fighter->saved_motion[1] = victim->body.pos[AXIS_Z];
  native_begin(fighter, 0, 1, n484420_teleport_drop);
  native_frame(fighter, 0, fighter_sample(fighter), NULL);
  native_frame(fighter, 1, fighter_sample(fighter), n484420_teleport_sinking);
  native_play(fighter, false);
}

/* State 0 (47d08c): action 400, sink callback 4e8410/47d550, sound C[36],
 * indexed 22, Y velocity 0x1999 and clip 87 played to sample 15 (428d30). */
void quan_chi_teleport_stomp(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n484420_s_sink);
  fighter->mmove_cleanup = n484420_teleport_cleanup;
  fighter->sample_override = 0;
  fighter->saved_motion[0] = fighter->body.pos[AXIS_X];
  fighter->saved_motion[1] = fighter->body.pos[AXIS_Z];
  battle_sound(SOUND_C, 36);
  battle_sound(SOUND_INDEXED, 22);
  fighter->body.vel[AXIS_Y] = 0x1999;
  fighter->body.gravity = 0;
  fighter->body.physics = n484420_teleport_sink;
  native_begin(fighter, 1, 15, n484420_teleport_save);
  for (int frame = 1; frame <= 15; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 47ffc0: 46fd90 landing, then the controller return. */

static void n484420_victim_cycle(Fighter *victim);

/* 4845f0: wait 20 ticks, then 488ba0 reversed getup 328 (crouch 184). */
static void n484420_victim_getup(Fighter *victim) {
  mk4_mmove_getup(victim, 0x4ea210);
}

static void n484420_victim_release(Fighter *victim) {
  native_hold(victim, 20, n484420_victim_getup);
}

/* 484650 state 1: continue while the stomper keeps action 400 or 409. */
static void n484420_victim_cycle_end(Fighter *victim) {
  int action = fighter_opponent(victim)->action;
  if ((action == 0x400 || action == 0x409) && ++victim->script_phase < n484420_VICTIM_CYCLES) {
    n484420_victim_cycle(victim);
  } else {
    n484420_victim_release(victim);
  }
}

/* 484650 state 0: shake 0, effect 13, cursor 15 and play clip 178 out. */
static void n484420_victim_cycle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
  victim->sample_override = 15;
  n484420_play_out(victim, n484420_victim_cycle_end);
}

/* 484420 after generated 4e9868 played clip 178. */
void quan_chi_script_stomp_victim(Fighter *victim) {
  victim->script_phase = 0;
  n484420_victim_cycle(victim);
}

/* Reaction 106 (4843b0): 494520 disarm; an airborne victim (48ef30) drops
 * with Y velocity and gravity 0x1999; then VM 4e9868. */
void quan_chi_stomp_reaction(Fighter *victim) {
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &n484420_s_stomped);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim)) {
    victim->body.vel[AXIS_Y] = 0x1999;
    victim->body.gravity = 0x1999;
  }
  retail_vm_enter_owned(victim, 0x4e9868, fighter_ready, 8);
}

/* Native service cluster: native_teleport. */
static bool native_teleport_teleport_below_ground(const Fighter *fighter);
static void native_teleport_teleport_done(Fighter *fighter);
static void native_teleport_teleport_land(Fighter *fighter);
static void native_teleport_teleport_drop(Fighter *fighter);
static void native_teleport_teleport_script(Fighter *fighter);
static const MoveDef native_teleport_s_teleport = {.clip = 304, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20c};

static bool native_teleport_teleport_below_ground(const Fighter *fighter) {
  return fighter->body.pos[AXIS_Y] >= fighter->body.ground + Q16_ONE;
}

static void native_teleport_teleport_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void native_teleport_teleport_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_UNTOUCHABLE;
  fighter->teleport_ready = g_battle.ticks + 32;
  native_hold(fighter, 4, native_teleport_teleport_done);
}

static void native_teleport_teleport_drop(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = victim->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x10000);
  }
  fighter->body.pos[AXIS_Y] = fighter->body.ground - 3 * Q16_ONE;
  fighter->body.vel[AXIS_Y] = -0x451e;
  fighter->body.gravity = 0x28f;
  fighter->flags |= FF_UNTOUCHABLE;
  native_await(fighter, native_grounded, native_teleport_teleport_land);
}

static void native_teleport_teleport_script(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = 0x4ccc;
  fighter->body.gravity = 0;
  native_await(fighter, native_teleport_teleport_below_ground, native_teleport_teleport_drop);
}

void raiden_teleport(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &native_teleport_s_teleport);
  battle_sound(SOUND_A, 0x30);
  fighter->mmove_cleanup = raiden_special_cleanup;
  native_teleport_teleport_script(fighter);
}

bool raiden_can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready;
}

/* Native service cluster: native_flip_kick. */
/* B,D,F+HK 4791a0 (also the far CPU row): native-only state machine. It
 * stamps slot 16, plays C[0x62], stores action 0x21e and moves 0.2 along
 * the pair axis while 47bf50 decays horizontal speed by 0.9 every world
 * pump. Clip 110 (53cf3c) advances to pose 7, holds it one visit, then
 * shows 8, 9 and 10 while querying D[29]. */
static const MoveDef native_flip_kick_s_flip_kick = {.clip = 110, .last = 26, .flags = SF_LOCK | SF_RECORD, .action = 0x21e};
static const AttackRecord native_flip_kick_flip_record = {0x1353f, -0xab02, 0x12395, 0x1b2b0, 171, 171, 9, 0, 0x1eb8};

/* 45e7a0: slot 16 at least 90 ticks old and a peer combo of at most four. */
bool reiko_can_flip_kick(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && mk4_special_elapsed(fighter, 0x10, 90) && fighter_opponent(fighter)->hits <= 4;
}

static void native_flip_kick_flip_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void native_flip_kick_blocked_hold(Fighter *fighter) {
  /* 4791a0 state 6 sleeps sixteen visits before the controller. */
  native_hold(fighter, 16, mk4_return_to_stance);
}

/* 428cf0 finishes clip 110 from the next pose; a block first sets
 * no-guard (490140) and adds the sixteen-visit wait. */
static void native_flip_kick_flip_finish(Fighter *fighter, bool blocked) {
  int pose = fighter_sample(fighter);
  if (blocked) {
    fighter->flags |= FF_NO_GUARD;
  }
  native_begin(fighter, 1, native_flip_kick_s_flip_kick.last - pose, blocked ? native_flip_kick_blocked_hold : mk4_return_to_stance);
  for (int frame = 1; frame <= native_flip_kick_s_flip_kick.last - pose; ++frame) {
    native_frame(fighter, frame, pose + frame, NULL);
  }
  native_play(fighter, false);
}

static void native_flip_kick_flip_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &native_flip_kick_flip_record);
  if (result != CONTACT_MISS) {
    native_flip_kick_flip_finish(fighter, result == CONTACT_BLOCK);
  }
}

static void native_flip_kick_flip_miss(Fighter *fighter) {
  native_flip_kick_flip_finish(fighter, false);
}

void reiko_flip_kick(Fighter *fighter) {
  mk4_special_stamp(fighter, 0x10);
  battle_sound(SOUND_C, 0x62);
  fighter_start_move(fighter, S_KICK, &native_flip_kick_s_flip_kick);
  fighter_set_speed(fighter, 0x3333);
  fighter->body.physics = native_flip_kick_flip_decay;
  /* 428750/428d30: pose 0 is replaced by 1 on the entry visit; 428d70
   * resubmits 7, then states 1..4 show 8 and query at 8, 9 and 10. */
  static const int poses[] = {1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 9, 10};
  native_begin(fighter, 1, 12, native_flip_kick_flip_miss);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, poses[frame - 1], frame >= 10 ? native_flip_kick_flip_query : NULL);
  }
  native_play(fighter, true);
}

/* Native service cluster: native_orbit. */
/* B,F+LK 478ee0: a native-only circling step behind the peer, with no
 * contact query. Entry stamps slot 17, stores action 0x2014, plays C[0x63]
 * and binds clip 306 (53cf38) at pose 0. 4245a0 takes the angle from this
 * fighter to the peer and adds pi (4d0320); 487e20 then places the body at
 * peer + radius * (cos, sin) on twenty visits. The angle turns -0x283a per
 * visit while the radius shrinks linearly from the distance towards 1.2. */
static const MoveDef native_orbit_s_orbit = {.clip = 306, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x2014};
static const MoveDef native_orbit_s_orbit_stance = {.clip = 284, .last = 39, .flags = SF_LOCK, .action = 0x2014};

/* 45e710: at least 1.3 units apart, slot 17 at least 80 ticks old, the peer
 * not near the wall unless it is nearer the centre than this fighter
 * (48f100), and the peer not in flip-kick action 0x21e (45f540). */
bool reiko_can_orbit(const Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  int64_t radius2 = ((int64_t)g_battle.arena_radius * g_battle.arena_radius) >> 16;
  int64_t own2 =
      (((int64_t)fighter->body.pos[AXIS_X] * fighter->body.pos[AXIS_X]) >> 16) + (((int64_t)fighter->body.pos[AXIS_Z] * fighter->body.pos[AXIS_Z]) >> 16);
  int64_t peer2 = (((int64_t)peer->body.pos[AXIS_X] * peer->body.pos[AXIS_X]) >> 16) + (((int64_t)peer->body.pos[AXIS_Z] * peer->body.pos[AXIS_Z]) >> 16);
  bool clear = peer2 < own2 || peer2 <= radius2 - 10 * Q16_ONE;
  return g_battle.phase == PHASE_FIGHT && fighter_distance(fighter) >= q16_float(0x14ccc) && mk4_special_elapsed(fighter, 0x11, 80) && clear &&
         peer->action != 0x21e;
}

static void native_orbit_orbit_place(Fighter *fighter) {
  /* saved_motion holds the current angle, radius and per-visit decrement. */
  const Fighter *peer = fighter_opponent(fighter);
  double radians = q16_float(fighter->saved_motion[0]);
  q16 radius = fighter->saved_motion[1];
  fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + (q16)llround(cos(radians) * radius);
  fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + (q16)llround(sin(radians) * radius);
  fighter->saved_motion[0] -= 0x283a;
  fighter->saved_motion[1] = radius - fighter->saved_motion[2];
}

/* 478f49: face the peer (490bd0), show the stance clip (4611c0 ->
 * 490f60/53d408), wait twenty visits, then 470420 stops and returns. */
static void native_orbit_orbit_finish(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter_start_move(fighter, S_KICK, &native_orbit_s_orbit_stance);
  fighter->sample_override = 0;
  native_hold(fighter, 20, mk4_mmove_thrown_finish);
}

void reiko_orbit(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  mk4_special_stamp(fighter, 0x11);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &native_orbit_s_orbit);
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 0x63);
  double dx = (double)peer->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  double dz = (double)peer->body.pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
  q16 distance = g_battle.distance;
  fighter->saved_motion[0] = (q16)llround(atan2(dz, dx) * Q16_ONE) + 0x3243f;
  fighter->saved_motion[1] = distance;
  fighter->saved_motion[2] = q16_mul(distance - 0x13333, 0xccc);
  native_begin(fighter, 0, 19, native_orbit_orbit_finish);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(fighter, frame, 0, native_orbit_orbit_place);
  }
  native_play(fighter, true);
}

/* Native service cluster: 47a6d0, 47b010. */
/* B,F+LK record 4e45dc (gate 45e950) and the near/medium CPU rows enter
 * 47a690: family-11 stamp, action 216, VM 4e8128 plays clip 302, binds
 * 303/0 with speed 0x2e14 and tails into 47a6d0. */
static const MoveDef n47a6d0_dash = {.clip = 302, .last = 7, .flags = SF_LOCK | SF_RECORD, .action = 0x216};
/* 47a6d0 queries D[23] (12%, reactions 158/158/165). */
static const AttackRecord n47a6d0_dash_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 158, 158, 165, 0, 0x1eb8};
/* B,F+LP record 4e4590 and the far CPU row enter 47afe0: action 20d,
 * VM 4e8178 on clip 106, D[21] probes and tail 47b010. */
static const MoveDef n47a6d0_roll = {.clip = 106, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x20d};
static const AttackRecord n47a6d0_roll_record = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 40, 10, 9, 0, 0x170a};
static const MoveDef n47a6d0_roll_victim = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};

bool reptile_can_dash(const Fighter *fighter) {
  /* 45e950: action family 11 must be at least 70 ticks old and the peer
   * combo count (+7c, queried by 45f5b0) must not exceed four. */
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && fighter_opponent(fighter)->hits <= 4;
}

/* 47a7e0: 428750 binds clip 302 (first word of 53cf48), 429460 starts at
 * its last sample and plays it backwards, then 46f650 returns. */
static void n47a6d0_dash_recover(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n47a6d0_dash);
  fighter->sample_override = 7;
  native_begin(fighter, 1, 8, mk4_return_to_stance);
  for (int frame = 1; frame <= 7; ++frame) {
    native_frame(fighter, frame, 7 - frame, NULL);
  }
  native_frame(fighter, 8, 0, NULL);
  native_play(fighter, false);
}

/* 47a9e0/47aa90 add +0xccc then -0xccc to world X, two visits each. */
static void n47a6d0_dash_shake_out(Fighter *fighter) {
  fighter->body.pos[AXIS_X] += 0xccc;
}

static void n47a6d0_dash_shake_back(Fighter *fighter) {
  fighter->body.pos[AXIS_X] -= 0xccc;
}

static void n47a6d0_dash_contact(Fighter *fighter) {
  if (fighter->connected) {
    return;
  }
  ContactResult result = battle_contact(fighter, &n47a6d0_dash_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result != CONTACT_BLOCK) {
    /* 47a890: an unblocked hit waits eight visits before 47a7e0. */
    native_hold(fighter, 8, n47a6d0_dash_recover);
    return;
  }
  /* 47a8f0: action 100e, stop motion, no guard and eight 47a9e0 shakes. */
  fighter->action = 0x100e;
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 32, n47a6d0_dash_recover);
  for (int frame = 1; frame <= 32; ++frame) {
    native_frame(fighter, frame, pose, frame % 4 == 1 ? n47a6d0_dash_shake_out : frame % 4 == 3 ? n47a6d0_dash_shake_back : NULL);
  }
  native_play(fighter, false);
}

/* 47a6d0: one setup visit, then sixteen D[23] queries holding 303/0.
 * Its 490bd0 steering only turns the model toward the opponent, which
 * the pair-axis heading already does. Physics 47aab0 plays a random
 * footstep from token list 4ea8b0 every five ticks; the engine has no
 * raw-token sound bank, so those cues are omitted. */
void reptile_script_dash_contact(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 0, 16, n47a6d0_dash_recover);
  for (int frame = 0; frame <= 16; ++frame) {
    native_frame(fighter, frame, pose, frame > 0 ? n47a6d0_dash_contact : NULL);
  }
  native_play(fighter, false);
}

void reptile_input_47a690(Fighter *fighter) {
  fighter->slide_ready = g_battle.ticks + 70;
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47a6d0_dash);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e8128, fighter_ready, 10);
}

static void n47a6d0_roll_probe(Fighter *fighter) {
  /* 47b260 queries D[21] until its local latch is set. */
  if (!fighter->connected) {
    battle_contact(fighter, &n47a6d0_roll_record);
  }
}

/* 47b010 from 106/17: probes at 18, 19 and 18; six cycles of 428cf0
 * play-out to 21 and 4292f0 rewind to 19; 429380 reverses to zero and
 * 46f650 returns. */
void reptile_script_roll_tail(Fighter *fighter) {
  static const int cycle[] = {20, 21, 21, 20, 19};
  require(fighter_move(fighter)->clip == 106 && fighter_sample(fighter) == 17, "Roll tail needs its generated clip-106 startup");
  native_begin(fighter, 1, 3 + 6 * 5 + 20, mk4_return_to_stance);
  int frame = 1;
  native_frame(fighter, frame++, 18, n47a6d0_roll_probe);
  native_frame(fighter, frame++, 19, n47a6d0_roll_probe);
  native_frame(fighter, frame++, 18, n47a6d0_roll_probe);
  for (int repeat = 0; repeat < 6; ++repeat) {
    for (int i = 0; i < 5; ++i) {
      native_frame(fighter, frame++, cycle[i], NULL);
    }
  }
  for (int sample = 18; sample >= 0; --sample) {
    native_frame(fighter, frame++, sample, NULL);
  }
  native_frame(fighter, frame, 0, NULL);
  native_play(fighter, false);
}

void reptile_input_47afe0(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47a6d0_roll);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e8178, fighter_ready, 10);
}

/* Reaction 40 (483550): disarm and VM 4e9620, clip 183 launch, landing
 * wait and the shared 488b40 side getup. */
void reptile_reaction_40(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &n47a6d0_roll_victim);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e9620, fighter_ready, 10);
}

/* 488b80: 490350 toggles the model mirror bit, which the pair-axis
 * heading supersedes here, then 4914c0 runs the 4ea210 getup pairs. */

/* Native service cluster: 4698b0. */
/* BLOCK record 4e45f0 and HK record 4e45b0 both enter 4698a0 -> VM 4e5bb0:
 * clip 12 (58 samples) advances 24 samples, cues C[75]/C[74], then native
 * 4698b0 toggles Reptile's visibility halfway through the clip. */
static const MoveDef n4698b0_invisibility = {.clip = 12, .last = 57, .flags = SF_LOCK | SF_RECORD};

bool reptile_can_invisibility(const Fighter *fighter) {
  /* 45f260 -> 45f2b0 with record 4e4228: P1 word 0x4d00a4 & 0x71 == 0x60
   * and word 0x4d00a8 & 7 == 0 (P2 uses the same bits shifted by eight).
   * BLOCK and HK are held; UP, HP, LP, LK and RUN are not; directions are
   * outside the mask. The 53c880 override is a non-gameplay switch. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return (fighter->held & mask) == chord;
}

static void n4698b0_toggle(Fighter *fighter) {
  /* 4698b0 state 1: fighter render node +0x34 ^= 4, the culled bit. */
  fighter->invisible = !fighter->invisible;
}

/* 4698b0: 428d70 plays to sample 40 and submits it for one more visit,
 * the toggle runs, 428cf0 resumes at 41 and plays out the clip, then
 * 470420 returns to the controller. The 404cc0/404df0 green light pulse
 * and its 40fa20 effect task are presentation without an engine light. */
void reptile_script_invisibility(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 12 && fighter_sample(fighter) == 24, "Invisibility needs its generated clip-12 startup");
  native_begin(fighter, 1, 35, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= 16; ++frame) {
    native_frame(fighter, frame, 24 + frame, NULL);
  }
  native_frame(fighter, 17, 40, NULL);
  native_frame(fighter, 18, 41, n4698b0_toggle);
  for (int frame = 19; frame <= 34; ++frame) {
    native_frame(fighter, frame, 23 + frame, NULL);
  }
  native_frame(fighter, 35, 57, NULL);
  native_play(fighter, false);
}

void reptile_invisibility(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n4698b0_invisibility);
  retail_vm_enter_owned(fighter, 0x4e5bb0, fighter_ready, 10);
}

/* Native service cluster: 47fcf0, 480040, 484c00. */
static void n47fcf0_teleport_land_gate(Fighter *fighter);
static void n47fcf0_teleport_drop(Fighter *fighter);
static void n47fcf0_teleport_pause(Fighter *fighter);
static void n47fcf0_teleport_block_return(Fighter *fighter);
static void n47fcf0_teleport_query(Fighter *fighter);
static void n47fcf0_teleport_vanish(Fighter *fighter);
static void n47fcf0_teleport_range(Fighter *fighter);
static const AttackRecord n47fcf0_teleport_record = {0x128f5, -0x3ae1, 0xab85, 0x91eb, 90, 90, 0, ATTACK_MARK, 0x170a};
static const MoveDef n47fcf0_s_teleport = {.clip = 312, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0209};
static const MoveDef n47fcf0_s_teleport_block = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1011};

static void n47fcf0_teleport_land_gate(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void n47fcf0_teleport_drop(Fighter *fighter) {
  fighter->body.gravity = 0x1999;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  native_begin(fighter, 0, 0, scorpion_special_ready);
  native_frame(fighter, 0, fighter_sample(fighter), n47fcf0_teleport_land_gate);
  native_play(fighter, false);
}

static void n47fcf0_teleport_pause(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  fighter->script_phase = 2;
  native_hold(fighter, 6, n47fcf0_teleport_drop);
}

static void n47fcf0_teleport_block_return(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8818, fighter_ready, 0);
}

static void n47fcf0_teleport_query(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  ContactResult result = battle_contact(fighter, &n47fcf0_teleport_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    fighter_start_move(fighter, S_JUMP, &n47fcf0_s_teleport_block);
    fighter->sample_override = 8;
    fighter_set_speed(fighter, -0x28f);
    fighter->body.vel[AXIS_Y] = -0x2147;
    fighter->body.gravity = 0x1ca;
    native_hold(fighter, 21, n47fcf0_teleport_block_return);
  } else {
    n47fcf0_teleport_pause(fighter);
  }
}

static void n47fcf0_teleport_vanish(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  int first = fighter->mmove_frame + 1;
  /* 480140 reappears past the opponent along this fighter's pair axis, on
   * the opponent when that point leaves the arena, then travels back along
   * the pre-teleport axis. */
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  int direction = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? 1 : -1;
  q16 offset = 0x26666 + maxi(0, 4 * Q16_ONE - distance);
  q16 x = victim->body.pos[AXIS_X] + q16_mul(fighter_axis(fighter, AXIS_X), offset);
  q16 z = victim->body.pos[AXIS_Z] + q16_mul(fighter_axis(fighter, AXIS_Z), offset);
  double radius = g_battle.arena_radius;
  bool outside = (double)x * x + (double)z * z > radius * radius;
  fighter->body.pos[AXIS_X] = outside ? victim->body.pos[AXIS_X] : x;
  fighter->body.pos[AXIS_Z] = outside ? victim->body.pos[AXIS_Z] : z;
  fighter->facing = -direction;
  fighter_set_speed(fighter, -0x2666);
  fighter->sample_override = 9;
  fighter->script_phase = 1;
  native_begin(fighter, first, maxi(first, 70), n47fcf0_teleport_pause);
  for (int frame = first; frame <= maxi(first, 70); ++frame) {
    native_frame(fighter, frame, 9, n47fcf0_teleport_query);
  }
  native_play(fighter, false);
}

static void n47fcf0_teleport_range(Fighter *fighter) {
  scorpion_script_teleport_motion(fighter);
  if (fighter->native_signal) {
    n47fcf0_teleport_vanish(fighter);
  }
}

void scorpion_teleport_script(Fighter *fighter) {
  int first = fighter->ticks + 1;
  native_begin(fighter, first, maxi(first, 10), n47fcf0_teleport_vanish);
  for (int frame = first; frame <= maxi(first, 10); ++frame) {
    native_frame(fighter, frame, mini(9, frame), n47fcf0_teleport_range);
  }
  native_play(fighter, false);
}

bool scorpion_can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready && fighter_opponent(fighter)->hits < 3;
}

void scorpion_teleport(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n47fcf0_s_teleport);
  fighter->sample_override = 0;
  fighter->teleport_ready = g_battle.ticks + 69;
  fighter->native_signal = false;
  fighter->flags |= FF_TELEPORTING;
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? -1 : 1;
  retail_vm_enter_owned(fighter, 0x4e87b0, fighter_ready, 0);
}

void scorpion_script_teleport_motion(Fighter *fighter) {
  /* 480040 checks the portable arena/range limits before the native vanish
   * continuation. World physics owns integration of generated launch motion. */
  double x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z], wall = g_battle.arena_radius - Q16_ONE;
  if (fighter->script_phase == 0 &&
      (fighter_distance(fighter) >= 5.9f || x * x + z * z >= wall * wall || (fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0))) {
    fighter->native_signal = true;
  }
}

void scorpion_script_teleport_victim_tail(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4e9970, mk4_script_thrown_ready, 0);
}

/* Native service cluster: native_morph. */
/* 469a00..469ad0 store a roster index in 53c42c and enter 469ae0.
 * State 0 writes it to task +0x34, then 4aa7e0 parks the actor for four
 * ticks. Its "LOADING" text (4ce220) and object freeze (53dbc0) cover
 * the retail sound/model reload; decoded assets stay resident here, so
 * only the actor's four-tick hold remains. State 1 queues the target's
 * name token, stamps slot 5 (48e3a0) and returns through 470420. */
static const MoveDef native_morph_morph_hold = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK};

bool shinnok_can_morph(const Fighter *fighter) {
  /* 45e6a0 reads only the round winner; the command interface passes the actor. */
  (void)fighter;
  return g_battle.ko_winner == 0;
}

static void native_morph_morph_complete(Fighter *fighter) {
  /* 469ae0 plays its inline token for roster < 16. For every morph target
   * (0..14) it equals indexed cue 4ead30[38 + roster]. */
  battle_sound(SOUND_INDEXED, 38 + fighter->roster);
  fighter->morph_tick = g_battle.ticks;
  fighter_finish_mmove(fighter, fighter_ready);
}

static void native_morph_morph(Fighter *fighter, int roster) {
  fighter_change_character(fighter, roster);
  /* A locked ground move, like the sidestep, so the controller return
   * re-enters the target's own standing state. */
  fighter_start_move(fighter, S_WALK, &native_morph_morph_hold);
  native_hold(fighter, 4, native_morph_morph_complete);
}

void shinnok_morph_scorpion(Fighter *fighter) {
  native_morph_morph(fighter, 0);
}

void shinnok_morph_raiden(Fighter *fighter) {
  native_morph_morph(fighter, 1);
}

void shinnok_morph_sonya(Fighter *fighter) {
  native_morph_morph(fighter, 2);
}

void shinnok_morph_liu_kang(Fighter *fighter) {
  native_morph_morph(fighter, 3);
}

void shinnok_morph_sub_zero(Fighter *fighter) {
  native_morph_morph(fighter, 4);
}

void shinnok_morph_fujin(Fighter *fighter) {
  native_morph_morph(fighter, 5);
}

void shinnok_morph_reiko(Fighter *fighter) {
  native_morph_morph(fighter, 7);
}

void shinnok_morph_quan_chi(Fighter *fighter) {
  native_morph_morph(fighter, 8);
}

void shinnok_morph_tanya(Fighter *fighter) {
  native_morph_morph(fighter, 9);
}

void shinnok_morph_reptile(Fighter *fighter) {
  native_morph_morph(fighter, 10);
}

void shinnok_morph_kai(Fighter *fighter) {
  native_morph_morph(fighter, 11);
}

void shinnok_morph_jarek(Fighter *fighter) {
  native_morph_morph(fighter, 12);
}

void shinnok_morph_jax(Fighter *fighter) {
  native_morph_morph(fighter, 13);
}

void shinnok_morph_cage(Fighter *fighter) {
  native_morph_morph(fighter, 14);
}

/* Native service cluster: 47e1d0. */
static void n47e1d0_square_query(Fighter *fighter);
static void n47e1d0_square_done(Fighter *fighter);
static void n47e1d0_square_land(Fighter *fighter);
static void n47e1d0_square_fall(Fighter *fighter);
static void n47e1d0_square_victim_drop(Fighter *victim);
static void n47e1d0_square_victim_guard(Fighter *victim);
static const MoveDef n47e1d0_s_square_wave = {.clip = 72, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x201};
static const AttackRecord n47e1d0_square_record = {19005, -9830, 39321, 57671, 87, 87, 0, 0, 13107};

static void n47e1d0_square_query(Fighter *fighter) {
  battle_contact(fighter, &n47e1d0_square_record);
  fighter->mmove_hold = fighter_height(fighter) < 2.4f && !fighter->connected;
}

static void n47e1d0_square_done(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter_finish_mmove(fighter, fighter_ready);
}

static void n47e1d0_square_land(Fighter *fighter) {
  native_await(fighter, native_grounded, n47e1d0_square_done);
}

static void n47e1d0_square_fall(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  fighter_set_speed(fighter, fighter->connected ? -0x1999 : 0);
  if (fighter->connected) {
    native_hold(fighter, 32, n47e1d0_square_land);
  } else {
    n47e1d0_square_land(fighter);
  }
}

void sonya_script_square_wave_tail(Fighter *fighter) {
  if (fighter_height(fighter) < 2.4f && !fighter->connected) {
    native_begin(fighter, 0, 0, n47e1d0_square_fall);
    native_frame(fighter, 0, fighter_sample(fighter), n47e1d0_square_query);
    native_play(fighter, false);
  } else {
    n47e1d0_square_fall(fighter);
  }
}

void sonya_square_wave(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &n47e1d0_s_square_wave);
  fighter->mmove_cleanup = sonya_special_cleanup;
  retail_vm_enter_owned(fighter, 0x4e85c0, fighter_ready, 2);
}

static void n47e1d0_square_victim_drop(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->body.gravity = 0x28f;
  victim->body.vel[AXIS_Y] = 0x666;
  retail_getup_roll(victim);
}

static void n47e1d0_square_victim_guard(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (owner->action != 0x201 || owner->pending != NULL) {
    n47e1d0_square_victim_drop(victim);
  }
}

/* The carried pose cycle repeats until the owner leaves the square wave. */
void retail_square_victim(Fighter *victim) {
  retail_vm_bind_animation(victim, mk4_image_u32(retail_vm_image(), 0x53cf94) * 4, 0);
  victim->flags |= FF_PAIRED;
  const Fighter *owner = fighter_opponent(victim);
  if (owner->action != 0x201 || owner->pending != NULL) {
    n47e1d0_square_victim_drop(victim);
    return;
  }
  native_begin(victim, 0, 19, NULL);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(victim, frame, 4 + (frame + 1) % 20, n47e1d0_square_victim_guard);
  }
  native_play(victim, false);
}

/* Native service cluster: native_clone. */
static void native_clone_clone_object(Projectile *p);
static void native_clone_clone_cleanup(Fighter *fighter);
static void native_clone_clone_done(Fighter *fighter);
static void native_clone_clone_land_frames_frame_0(Fighter *fighter);
static void native_clone_clone_land(Fighter *fighter);
static void native_clone_clone_turn(Fighter *fighter);
static bool native_clone_clone_falling(const Fighter *fighter);
static void native_clone_clone_apex_frames_frame_0(Fighter *fighter);
static void native_clone_clone_apex_wait(Fighter *fighter);
static void native_clone_clone_spin(Fighter *fighter);
static const AttackRecord native_clone_clone_record = {0x4ccc, -0xa3d, 0xbae1, 0x9e76, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef native_clone_s_clone = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0405};
static const MoveDef native_clone_s_clone_near = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0406};
static const OrbitLightDef native_clone_clone_light = {25 * Q16_ONE, -25 * Q16_ONE, 0x18000, 0xccc};
static const mframe_t native_clone_clone_land_frames[1] = {
    [0] = {.thinkfunc = native_clone_clone_land_frames_frame_0},
};
static const mmove_t native_clone_clone_land_phase = {
    .firstframe = 0, .lastframe = 0, .frame = native_clone_clone_land_frames, .endfunc = native_clone_clone_done};
// clang-format off: Keep table entries on one row, including long rows.
static const int native_clone_clone_reverse_frames_samples[] = {
  8,
  7,
  6,
  5,
  4,
  3,
  2,
  1,
};
// clang-format on
static const mframe_t native_clone_clone_reverse_frames[8] = {{0}};
static const mmove_t native_clone_clone_reverse = {.firstframe = 0,
    .lastframe = 7,
    .frame = native_clone_clone_reverse_frames,
    .endfunc = native_clone_clone_land,
    .samples = native_clone_clone_reverse_frames_samples};
static const mframe_t native_clone_clone_apex_frames[1] = {
    [0] = {.thinkfunc = native_clone_clone_apex_frames_frame_0},
};
static const mmove_t native_clone_clone_apex = {.firstframe = 8, .lastframe = 8, .frame = native_clone_clone_apex_frames, .endfunc = native_clone_clone_turn};
static const mframe_t native_clone_clone_extend_frames[5] = {{0}};
static const mmove_t native_clone_clone_extend = {
    .firstframe = 3, .lastframe = 7, .frame = native_clone_clone_extend_frames, .endfunc = native_clone_clone_apex_wait};
static const mframe_t native_clone_clone_start_frames[3] = {{0}};
static const mmove_t native_clone_clone_start = {.firstframe = 0, .lastframe = 2, .frame = native_clone_clone_start_frames, .endfunc = native_clone_clone_spin};

static void native_clone_clone_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (g_battle.contacts_off || battle_contact_from(owner, &native_clone_clone_record, p->pos, p->facing) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void native_clone_clone_cleanup(Fighter *fighter) {
  fighter_upright(fighter);
}

static void native_clone_clone_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, retail_air_land);
}

static void native_clone_clone_land_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void native_clone_clone_land(Fighter *fighter) {
  fighter->sample_override = 0;
  if (fighter_airborne(fighter)) {
    fighter_mmove_phase(fighter, &native_clone_clone_land_phase);
  } else {
    native_clone_clone_done(fighter);
  }
}

static void native_clone_clone_turn(Fighter *fighter) {
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &native_clone_clone_reverse);
}

static bool native_clone_clone_falling(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static void native_clone_clone_apex_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = !native_clone_clone_falling(fighter);
}

static void native_clone_clone_apex_wait(Fighter *fighter) {
  fighter_mmove_phase(fighter, &native_clone_clone_apex);
}

static void native_clone_clone_spin(Fighter *fighter) {
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = -0x6147;
  fighter_mmove_phase(fighter, &native_clone_clone_extend);
}

bool sub_zero_can_clone(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->clone_ready && fighter_opponent(fighter)->action != ACTION_RETAIL_1003 &&
         !sub_zero_owned_object(fighter, PROJECTILE_CLONE);
}

void sub_zero_clone(Fighter *fighter) {
  bool near = fighter_distance(fighter) < 1.f;
  if (!near) {
    Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_CLONE, fighter_opponent(fighter)->human ? 74 : 53, native_clone_clone_object);
    if (p != NULL) {
      p->clip = fighter_move(fighter)->clip;
      p->sample = fighter_pose_sample(fighter);
      battle_object_light(p, &native_clone_clone_light);
    }
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_JUMP, near ? &native_clone_s_clone_near : &native_clone_s_clone);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, -0x170a);
  fighter->body.vel[AXIS_Y] = -0x28f5;
  fighter->body.gravity = 0x28f;
  fighter->clone_ready = g_battle.ticks + 180;
  battle_sound(SOUND_C, 0xa);
  fighter->mmove_cleanup = native_clone_clone_cleanup;
  fighter_mmove_phase(fighter, &native_clone_clone_start);
}

/* Native service cluster: 41b450, 47dc40, 47de00. */
static void n41b450_slide_cleanup(Fighter *fighter);
static void n41b450_slide_contact(Fighter *fighter);
static const AttackRecord n41b450_slide_record = {0xf0a3, 0x947a, 0x111eb, 0x8f5c, 122, 122, 165, 0, 0x147a};
static const MoveDef n41b450_s_slide = {.clip = 95, .last = 10, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0204};
// clang-format off: Keep table entries on one row, including long rows.
static const int n41b450_slide_contact_frames_samples[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t n41b450_slide_contact_frames[25] = {
  [8] = {.thinkfunc = n41b450_slide_contact},
  [9] = {.thinkfunc = n41b450_slide_contact},
  [10] = {.thinkfunc = n41b450_slide_contact},
  [11] = {.thinkfunc = n41b450_slide_contact},
  [12] = {.thinkfunc = n41b450_slide_contact},
  [13] = {.thinkfunc = n41b450_slide_contact},
  [14] = {.thinkfunc = n41b450_slide_contact},
  [15] = {.thinkfunc = n41b450_slide_contact},
  [16] = {.thinkfunc = n41b450_slide_contact},
  [17] = {.thinkfunc = n41b450_slide_contact},
  [18] = {.thinkfunc = n41b450_slide_contact},
  [19] = {.thinkfunc = n41b450_slide_contact},
  [20] = {.thinkfunc = n41b450_slide_contact},
  [21] = {.thinkfunc = n41b450_slide_contact},
  [22] = {.thinkfunc = n41b450_slide_contact},
  [23] = {.thinkfunc = n41b450_slide_contact},
  [24] = {.thinkfunc = n41b450_slide_contact},
};
// clang-format on
static const mmove_t n41b450_slide_contact_phase = {.firstframe = 0,
    .lastframe = 24,
    .frame = n41b450_slide_contact_frames,
    .endfunc = sub_zero_script_slide_return,
    .samples = n41b450_slide_contact_frames_samples};
static const mframe_t n41b450_breath_slide_phase_frames[9] = {{0}};
static const mmove_t n41b450_breath_slide_phase = {
    .firstframe = 0, .lastframe = 8, .frame = n41b450_breath_slide_phase_frames, .endfunc = sub_zero_breath_travel};
// clang-format off: Keep table entries on one row, including long rows.
static const int n41b450_slide_return_frames_samples[] = {
  10,
  9,
  8,
  7,
  6,
  5,
  4,
  3,
  2,
  1,
  0,
};
// clang-format on
static const mframe_t n41b450_slide_return_frames[11] = {{0}};
static const mmove_t n41b450_slide_return_phase = {
    .firstframe = 0, .lastframe = 10, .frame = n41b450_slide_return_frames, .endfunc = fighter_ready, .samples = n41b450_slide_return_frames_samples};

static void n41b450_slide_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  mk4_script_stop_body(fighter);
}

static void n41b450_slide_contact(Fighter *fighter) {
  fighter->sample_override = 9;
  ContactResult contact = battle_contact(fighter, &n41b450_slide_record);
  if (contact == CONTACT_MISS) {
    return;
  }
  fighter->script_phase = 1;
  if (contact == CONTACT_BLOCK) {
    retail_vm_enter_owned(fighter, 0x4e8530, fighter_ready, 4);
  } else {
    fighter->slide_ready = g_battle.ticks + 64;
    retail_vm_enter_owned(fighter, 0x4e84f8, fighter_ready, 4);
  }
}

void sub_zero_slide_script(Fighter *fighter) {
  fighter_mmove_phase(fighter, &n41b450_slide_contact_phase);
}

bool sub_zero_can_slide(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && (fighter->held & mask) == chord;
}

void sub_zero_slide(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n41b450_s_slide);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = n41b450_slide_cleanup;
  retail_vm_enter_owned(fighter, 0x4e84d8, fighter_ready, 4);
}

void sub_zero_script_breath_slide(Fighter *fighter) {
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &n41b450_breath_slide_phase);
}

void sub_zero_script_slide_return(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->script_phase = 2;
  fighter_mmove_phase(fighter, &n41b450_slide_return_phase);
}

/* Native service cluster: 47ba20. */
/* LK F,D,B record 4e455c and all three CPU range rows -> T4[18] 47b9f0:
 * action 211 and VM 4e81f0 (indexed sound 31, speed 0x23d7, clip 103 to
 * sample 12, tail 47ba20). */
static const MoveDef n47ba20_drill = {.clip = 103, .last = 49, .flags = SF_LOCK | SF_RECORD, .action = 0x211};
/* D[17] 4ec0c8: 15%, hit/air/block 94/122/9. */
static const AttackRecord n47ba20_drill_record = {0x12624, -0x68b4, 0x11993, 0x15be7, 94, 122, 9, 0, 0x2666};

static void n47ba20_drill_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
}

static void n47ba20_drill_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 47bbd0 state 1: stop, camera shake 0, then 428cf0 finishes clip 103.
 * The 48a330 random list (tokens 0xbf/0xd7) has no portable sound bank
 * entry for 0xbf and is not queued. */
static void n47ba20_drill_land(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_script_shake(fighter, 0);
}

/* 47bbd0: two cursor submissions on entry, one per visit until sample 20,
 * a held visit, then the clip tail from two samples further on. */
static void n47ba20_drill_recover(Fighter *fighter) {
  int poses[64], count = 0, pose = fighter_sample(fighter) + 2;
  poses[count++] = pose;
  while (pose < 20) {
    poses[count++] = ++pose;
  }
  poses[count++] = pose;
  int land = count - 1;
  for (pose += 2; pose <= 49; ++pose) {
    poses[count++] = pose;
  }
  native_begin(fighter, 0, count - 1, n47ba20_drill_ready);
  for (int frame = 0; frame < count; ++frame) {
    native_frame(fighter, frame, mini(49, poses[frame]), frame == land ? n47ba20_drill_land : NULL);
  }
  native_play(fighter, false);
}

/* 47ba90 state 1: count the extra hit, shake, install reaction 95 on the
 * peer through 480f80, then recover. */
static void n47ba20_drill_second_hit(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->hits++;
  mk4_script_shake(fighter, 0);
  battle_react(victim, 95);
  n47ba20_drill_recover(fighter);
}

static void n47ba20_drill_contact(Fighter *fighter, ContactResult result) {
  mk4_script_shake(fighter, 0);
  if (result == CONTACT_BLOCK || fighter_airborne(fighter_opponent(fighter))) {
    n47ba20_drill_recover(fighter);
    return;
  }
  /* 429490 advances six samples before the grounded follow-up. */
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 6, n47ba20_drill_second_hit);
  for (int frame = 1; frame <= 6; ++frame) {
    native_frame(fighter, frame, pose + frame, NULL);
  }
  native_play(fighter, false);
}

/* 47ba20 submits samples 13 and 14 with a D[17] query after each. */
void tanya_drill_kick_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 103 && fighter_sample(fighter) == 12, "Tanya drill kick contact needs its generated startup");
  for (int pose = 13; pose <= 14; ++pose) {
    fighter->sample_override = pose;
    ContactResult result = battle_contact(fighter, &n47ba20_drill_record);
    if (result != CONTACT_MISS) {
      n47ba20_drill_contact(fighter, result);
      return;
    }
  }
  n47ba20_drill_recover(fighter);
}

void tanya_drill_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47ba20_drill);
  fighter->mmove_cleanup = n47ba20_drill_cleanup;
  retail_vm_enter_owned(fighter, 0x4e81f0, fighter_ready, 9);
}

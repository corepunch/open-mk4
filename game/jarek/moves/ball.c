#include "../character.h"

/* Jarek's cannonball 47b310 (B,F+LK and every CPU range row) and upward
 * ball 4797f0 (F,D,F+HP). Both bind tuck clip 301 through 53cf4c/53cf44,
 * play it two poses per visit (429220), then query D[19] every visit. */
static const AttackRecord ball_record = {0x5958, -0x49ba, 0xccc6, 0xadd2, 42, 42, 0, 0, 0x23d7};
static const MoveDef roll_move = {.clip = 301, .last = 8, .flags = SF_LOCK | SF_RECORD, .action = 0x214};
static const MoveDef rise_move = {.clip = 301, .last = 8, .flags = SF_LOCK | SF_RECORD, .action = 0x21a};
static const MoveDef victim_move = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};

static void ball_face(Fighter *fighter) {
  /* 490bd0 corrects the facing relative to the peer. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

static void ball_tuck(Fighter *fighter, q16 spin) {
  /* 4702e0 lowers the root by .2 while raising the group by the same amount. */
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = spin;
}

static void rise_decay(Fighter *fighter);

static void ball_cleanup(Fighter *fighter) {
  if (fighter->body.physics == mk4_script_landing_motion) {
    native_motion_stop(fighter);
  }
  if (fighter->body.physics == rise_decay) {
    fighter->body.physics = NULL;
  }
  fighter_upright(fighter);
}

static void ball_startup(Fighter *fighter, const MoveDef *move, FighterFn setup, FighterFn next) {
  fighter_start_move(fighter, S_KICK, move);
  fighter->mmove_cleanup = ball_cleanup;
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

void jarek_ball_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void recoil_wait(Fighter *fighter) {
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
  ball_face(fighter);
  jarek_entry_retail_script_004e84b8(fighter);
}

/* 47d940: contact recoil shared by both balls. */
static void ball_recoil(Fighter *fighter) {
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
  ball_tuck(fighter, -0x4ccc);
  mk4_script_landing_motion(fighter);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, recoil_wait);
  native_play(fighter, false);
}

static bool ball_contact(Fighter *fighter) {
  if (battle_contact(fighter, &ball_record) == CONTACT_MISS) {
    return false;
  }
  ball_recoil(fighter);
  return true;
}

static void roll_query(Fighter *fighter) {
  ball_contact(fighter);
}

static void roll_ready(Fighter *fighter) {
  ball_face(fighter);
  fighter_finish_mmove(fighter, jarek_ready);
}

static void roll_end(Fighter *fighter) {
  /* 4702b0, 48e080, cleared spin/orientation, 4906e0 and 4906c0. */
  fighter_upright(fighter);
  fighter_neutral(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  /* 429380 submits the current pose, then reverses one pose per visit. */
  native_begin(fighter, 0, 8, roll_ready);
  for (int frame = 0; frame <= 7; ++frame) {
    native_frame(fighter, frame, 7 - frame, NULL);
  }
  native_frame(fighter, 8, 0, NULL);
  native_play(fighter, false);
}

static void roll_setup(Fighter *fighter) {
  ball_tuck(fighter, 0x4ccc);
  fighter_set_speed(fighter, 0x3333);
}

static void roll_contacts(Fighter *fighter) {
  /* 47b310 stage 2 queries D[19] for nineteen visits. */
  native_begin(fighter, 0, 18, roll_end);
  for (int frame = 0; frame <= 18; ++frame) {
    native_frame(fighter, frame, 8, roll_query);
  }
  native_play(fighter, false);
}

void jarek_cannonball(Fighter *fighter) {
  ball_startup(fighter, &roll_move, roll_setup, roll_contacts);
}

static void rise_decay(Fighter *fighter) {
  /* 4e80a0 -> 47bf50 -> 490650 scales horizontal velocity by .9. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void rise_land(Fighter *fighter) {
  /* 48f390 waits for gravity to clear; 479a10 then clears orientation,
   * restores the root and returns through the shared 46fd90 landing. */
  if (fighter->body.gravity != 0) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  fighter_upright(fighter);
  fighter_neutral(fighter);
  jarek_ball_land(fighter);
}

static void rise_query(Fighter *fighter) {
  if (ball_contact(fighter)) {
    return;
  }
  /* 479960 stops querying once the rise slows to -.01 per visit. */
  if (fighter->body.vel[AXIS_Y] < -0x28f) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, rise_land);
  native_play(fighter, false);
}

static void rise_contacts(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, rise_query);
  native_play(fighter, false);
}

static void rise_setup(Fighter *fighter) {
  ball_tuck(fighter, 0x6666);
  fighter->body.vel[AXIS_Y] = -0x3333;
  fighter->body.gravity = 0x24d;
  fighter->body.physics = rise_decay;
}

void jarek_upward_ball(Fighter *fighter) {
  ball_startup(fighter, &rise_move, rise_setup, rise_contacts);
  /* 4797f0 sets its forward speed before binding the tuck clip. */
  fighter_set_speed(fighter, 0x28f5);
}

/* 4ea4e8[42] -> 4835a0: drop the victim's weapon, then generated 4e9658. */
void jarek_ball_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &victim_move);
  fighter->flags |= FF_REACTING;
  jarek_entry_retail_script_004e9658(fighter);
}

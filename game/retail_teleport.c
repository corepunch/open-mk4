#include "retail_native.h"
#include "retail_vm.h"
#include "native.h"
#include "script.h"
#include "native_services.h"
#include "math_util.h"

/* D,U special 47c070 (also the near/medium CPU row): action 0x20e and script
 * 4e82b8 rise on clip 86 at 0.2 units per visit without gravity. 47c0a0
 * vanishes above the arena, reappears below the floor past the peer and
 * rises out. 4e88f0 then offers air attacks and the BLOCK slam 47c2a0. */
static const MoveDef s_teleport = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20e};
static const MoveDef s_slam = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20e};
static const MoveDef s_slammed = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_recoil = {.clip = 301, .last = 8, .flags = SF_LOCK, .action = ACTION_RETAIL_1011};

/* 47c2d5 queries D[14]: no damage; 480fc0(8) replaces the hit reaction. */
enum {
  TELEPORT_SLAM_ATTACK_RECORD = 0x4ec110
};

static void teleport_landing(Fighter *fighter);

static int64_t square_sum(q16 x, q16 z) {
  return (((int64_t)x * x) >> 16) + (((int64_t)z * z) >> 16);
}

static void landing_gate(Fighter *fighter) {
  fighter->execution.hold = fighter_airborne(fighter);
}

/* 47c230: from the next visit, return to the controller once grounded.
 * 48f670 keeps the human 4e88f0 table installed throughout the fall. */
static void teleport_landing(Fighter *fighter) {
  if (fighter->human) {
    fighter->buttons = &retail_air_buttons[3];
  }
  native_begin(fighter, 0, 0, mk4_return_to_stance);
  native_frame(fighter, 0, fighter_sample(fighter), landing_gate);
  native_play(fighter, false);
}

/* 47c0a0 state 2 completion: the body has risen through the floor. */
static void teleport_emerge(Fighter *fighter) {
  fighter->body.gravity = 0x51e;
  teleport_landing(fighter);
}

static void below_floor_gate(Fighter *fighter) {
  fighter->execution.hold = fighter->body.pos[AXIS_Y] > fighter->body.ground;
}

/* 47c0a0 state 1: Y becomes 0 and 480b20 places the body one unit past the
 * peer's snapshot along this fighter's axis. If the point two units past it
 * is outside radius 5, or the new point is within 20 square units of the
 * arena edge, it lands one unit short instead. The rise keeps -0.2. */
static void teleport_relocate(Fighter *fighter) {
  q16 axis_x = fighter_axis(fighter, AXIS_X), axis_z = fighter_axis(fighter, AXIS_Z);
  q16 x = fighter->saved_motion[AXIS_X] + axis_x, z = fighter->saved_motion[AXIS_Z] + axis_z;
  int64_t radius2 = ((int64_t)g_battle.arena_radius * g_battle.arena_radius) >> 16;
  if (square_sum(x + axis_x, z + axis_z) >= 25 * Q16_ONE || square_sum(x, z) >= radius2 - 20 * Q16_ONE) {
    x -= 2 * axis_x;
    z -= 2 * axis_z;
  }
  fighter->body.pos[AXIS_X] = x;
  fighter->body.pos[AXIS_Y] = 0;
  fighter->body.pos[AXIS_Z] = z;
  memcpy(fighter->body.last_inside, fighter->body.pos, sizeof fighter->body.last_inside);
  retail_face_peer(fighter);
  native_begin(fighter, 0, 0, teleport_emerge);
  native_frame(fighter, 0, fighter_sample(fighter), below_floor_gate);
  native_play(fighter, false);
}

static void vanish_gate(Fighter *fighter) {
  fighter->execution.hold = fighter->body.pos[AXIS_Y] > -0x40000;
}

/* 47c0a0 state 0: 480cc0 snapshots the peer's X/Z, then 480a60 waits from
 * the next visit until the rising body is at least four units up. */
void retail_teleport_vanish(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  fighter->saved_motion[AXIS_X] = peer->body.pos[AXIS_X];
  fighter->saved_motion[AXIS_Z] = peer->body.pos[AXIS_Z];
  native_begin(fighter, 0, 0, teleport_relocate);
  native_frame(fighter, 0, fighter_sample(fighter), vanish_gate);
  native_play(fighter, false);
}

static void recoil_physics(Fighter *fighter);

static void recoil_whoosh(Fighter *fighter) {
  if (fighter->body.physics == recoil_physics) {
    battle_sound(SOUND_C, 0x58);
  }
}

static const mframe_t recoil_sound_frames[12] = {{.thinkfunc = recoil_whoosh}};

/* 4e84d0 -> 4707e0: the spinning recoil's twelve-visit whoosh until 4e8818
 * clears the group callback. */
static void recoil_physics(Fighter *fighter) {
  native_motion_step(fighter);
}

/* 47ff80: clear angles and spin rates, face the peer, then 4e8818 plays
 * clip 301 back to its first pose, waits for the floor and lands. */
static void recoil_retract(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.angle[axis] = 0;
    fighter->body.spin[axis] = 0;
  }
  retail_face_peer(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_TELEPORT_BLOCK_RETURN, fighter_ready);
}

static void recoil_gate(Fighter *fighter) {
  /* 48f490 waits for the apex, then 48f410 for Y >= -2.5. */
  fighter->execution.hold = fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000;
}

/* 47fec0: blocked slam. Action 0x1011; 48fed0 places this body half a unit
 * from the defender along the defender's axis; 4e8808 moves -0x28f, Y
 * -0x2147, gravity 0x1ca. 47db60 holds clip 301's last pose, spins
 * -0x4ccc with a 0.2 root offset and installs the whoosh callback. */
static void teleport_blocked(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  fighter_start_move(fighter, S_JUMP, &s_recoil);
  fighter->sample_override = 8;
  fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + q16_mul(fighter_axis(peer, AXIS_X), 0x8000);
  fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + q16_mul(fighter_axis(peer, AXIS_Z), 0x8000);
  fighter_set_speed(fighter, -0x28f);
  fighter->body.vel[AXIS_Y] = -0x2147;
  fighter->body.gravity = 0x1ca;
  fighter->body.spin[AXIS_X] = -0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  native_motion_bind(fighter, recoil_physics, recoil_sound_frames, 12);
  native_begin(fighter, 0, 0, recoil_retract);
  native_frame(fighter, 0, 8, recoil_gate);
  native_play(fighter, false);
}

/* 47c2a0, entered one visit after the BLOCK press: only a rising body at
 * most 2.2 units up queries D[14]. A miss or other state keeps the 47c230
 * landing wait; a block recoils; a hit faces the peer, then 480fc0(8) and
 * 48f9f0 capture it for 4e82e0, grounded or airborne. */
void retail_teleport_slam(Fighter *fighter) {
  if (fighter->body.vel[AXIS_Y] >= 0 || fighter->body.pos[AXIS_Y] < -0x23333) {
    teleport_landing(fighter);
    return;
  }
  AttackRecord record = retail_vm_attack_record(battle_data(), TELEPORT_SLAM_ATTACK_RECORD);
  ContactResult result = battle_contact(fighter, &record);
  if (result == CONTACT_MISS) {
    teleport_landing(fighter);
  } else if (result == CONTACT_BLOCK) {
    teleport_blocked(fighter);
  } else {
    retail_face_peer(fighter);
    mk4_air_pair_frames(fighter, &s_slam, &s_slammed, MK4_RETAIL_SCRIPT_TELEPORT_SLAM, retail_slam_released_fall);
  }
}

static void slam_fall_physics(Fighter *fighter) {
  /* 47c4d0: both captured bodies accelerate down through the floor. */
  fighter->body.vel[AXIS_Y] += 0x51e;
  ((Fighter *)fighter_opponent(fighter))->body.vel[AXIS_Y] = fighter->body.vel[AXIS_Y];
}

/* 47c360: install 47c4d0, then 4e8310 advances the pair to sample 27. */
void retail_teleport_slam_fall(Fighter *fighter) {
  fighter->body.physics = slam_fall_physics;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_TELEPORT_SLAM_FALL, fighter_ready);
}

static void slam_land(Fighter *fighter) {
  /* 47c4b0: 48de80 subtracts 0x1c28 from the peer, then 46acc0. */
  battle_damage((Fighter *)fighter_opponent(fighter), 0x1c28);
  retail_slam_tail(fighter);
}

static void gravity_gate(Fighter *fighter) {
  fighter->execution.hold = fighter->body.gravity != 0;
}

/* 47c390 state 1: both bodies reappear four units up with gravity 0x51e,
 * keeping their fall speed, and 48f390 waits for the owner's landing. */
static void slam_reenter(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.physics = NULL;
  fighter->body.pos[AXIS_Y] = victim->body.pos[AXIS_Y] = -0x40000;
  fighter->body.gravity = victim->body.gravity = 0x51e;
  native_begin(fighter, 0, 0, slam_land);
  native_frame(fighter, 0, fighter_sample(fighter), gravity_gate);
  native_play(fighter, false);
}

static void slam_depth_gate(Fighter *fighter) {
  fighter->execution.hold = fighter->body.pos[AXIS_Y] < 0x30000;
}

/* 47c390 state 0: 480ac0 waits from the next visit for Y >= 3. */
void retail_teleport_slam_drop(Fighter *fighter) {
  native_begin(fighter, 0, 0, slam_reenter);
  native_frame(fighter, 0, fighter_sample(fighter), slam_depth_gate);
  native_play(fighter, false);
}

/* 47ffc0 -> 46fd90 landing, then the controller. */

void retail_teleport(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_teleport);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_TELEPORT, fighter_ready);
}

#include "retail_native.h"
#include "retail_vm.h"
#include "script.h"
#include "native.h"
#include "native_services.h"
#include "math_util.h"

void retail_air_victim_default(Fighter *victim) {
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_AIR_VICTIM_DEFAULT, mk4_mmove_thrown_finish);
}

void retail_air_victim_flip(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_AIR_VICTIM_FLIP, mk4_mmove_thrown_finish);
}

/* Original native entry operands; the scripts bind both participants. */
void retail_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, MK4_RETAIL_DATA_AIR_THROW, retail_air_victim_default);
}

void retail_air_throw_lift(Fighter *fighter) {
  mk4_air_throw_frames(fighter, MK4_RETAIL_DATA_AIR_THROW_LIFT, retail_air_victim_lift);
}

void retail_air_throw_slam(Fighter *fighter) {
  retail_face_peer(fighter);
  mk4_air_throw_frames(fighter, MK4_RETAIL_DATA_AIR_THROW_SLAM, retail_slam_released_fall);
}

void retail_air_throw_flip(Fighter *fighter) {
  retail_face_peer(fighter);
  mk4_air_throw_frames(fighter, MK4_RETAIL_DATA_AIR_THROW_FLIP, retail_air_victim_flip);
}

void retail_air_throw_release(Fighter *fighter) {
  mk4_mmove_air_throw_release(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_AIR_THROW_RELEASE, fighter_ready);
}

void retail_air_throw_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

/* 484760 changes neither the action nor the clip before 4e9880 binds
 * clip 175, so the captured victim keeps its thrown action. */
static const MoveDef s_slam_victim = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};

static void slam_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &s_slam_victim);
  victim->flags |= FF_REACTING;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_SLAM_VICTIM, mk4_mmove_thrown_finish);
}

/* 480f50[105] -> 484760: disarm (494520), snap to the floor and stop
 * (4906c0), clear group flag 0x80 (490290), then the original 4e9880
 * push-back, impact and clip-175 recovery on the victim's next update. */
void retail_slam_reaction(Fighter *victim) {
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
void retail_slam_tail(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  retail_slam_reaction(victim);
  fighter->action = 0;
  const Mk4Image *image = retail_vm_image();
  fighter_set_speed(fighter, (q16)mk4_image_u32(image, MK4_RETAIL_DATA_AIR_THROW_SLAM_RELEASE_SPEED));
  fighter->body.vel[AXIS_Y] = (q16)mk4_image_u32(image, MK4_RETAIL_DATA_AIR_THROW_SLAM_RELEASE_Y_VELOCITY);
  fighter->body.gravity = (q16)mk4_image_u32(image, MK4_RETAIL_DATA_AIR_THROW_SLAM_RELEASE_GRAVITY);
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

static void released_fall_gate(Fighter *victim) {
  victim->mmove_hold = fighter_airborne(victim);
}

/* Portable interruption policy: retail parks the captured peer inside the
 * owner's task (481010) and has no release on this path. If the owner is
 * interrupted before landing, the shared hold drops the victim; it falls
 * without damage and returns to control through the KO-aware finish. */
void retail_slam_released_fall(Fighter *victim) {
  native_begin(victim, 0, 0, mk4_mmove_thrown_finish);
  native_frame(victim, 0, fighter_sample(victim), released_fall_gate);
  native_play(victim, false);
}

/* Jax's BLOCK air record 4e43d4 (flags 0x11, gate 45ed70) enters 46a990,
 * not the shared 46aa80 runner. It sets action 0x502, prepares the victim
 * through 480fc0[8], cancels both jump rotations through 48f9f0 and enters
 * 4e5dd8, which opens owner clip 84 and victim clip 144. */

static void air_grab_getup_frame(Fighter *victim);
static void air_grab_getup_end(Fighter *victim);

/* 480f50[166] -> 484a10 clears the paired bit and enters 4e98e8. */
void retail_air_victim_lift(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->flags |= FF_REACTING;
  victim->reaction = 166;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_AIR_VICTIM_LIFT, mk4_mmove_thrown_finish);
}

/* 46a9f0 stores gravity 0x3d7 on both paired actors and enters 4e5df8. */
void retail_air_lift_fall(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x3d7;
  victim->body.gravity = 0x3d7;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_AIR_LIFT_FALL, fighter_ready);
}

/* 46aa40 dispatches reaction 166 to the victim, subtracts 0x2147 and enters
 * the owner release 4e5e18. The released victim owns its fall and getup. */
void retail_air_lift_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->flags |= FF_PAIR_RELEASE;
  retail_air_victim_lift(victim);
  battle_damage(victim, 0x2147);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_AIR_LIFT_RELEASE, fighter_ready);
}

/* 484a30 first reschedules once. Each later visit lets a living victim's
 * held Up dispatch the straight jump through 4711f0(2) unless the round is
 * already decided (532354), then 4285d0 advances one stored pose. The final
 * pose of clip 140 returns through the controller; a KO victim stays down. */
static void air_grab_getup_frame(Fighter *victim) {
  if (g_battle.phase == PHASE_FIGHT && victim->health > 0 && (victim->held & BUTTON_BIT(BUTTON_UP))) {
    mk4_script_stop_body(victim);
    fighter_finish_mmove(victim, mk4_jump);
  }
}

static void air_grab_getup_end(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void retail_air_lift_getup(Fighter *victim) {
  int first = fighter_sample(victim), last = fighter_move(victim)->last;
  require(last != MOVE_END && first <= last, "Jax air-grab getup needs the bounded clip-140 pose");
  native_begin(victim, first, last, air_grab_getup_end);
  native_frame(victim, first, first, NULL);
  for (int frame = first + 1; frame <= last; ++frame) {
    native_frame(victim, frame, frame, air_grab_getup_frame);
  }
  native_play(victim, false);
}

static void air_release_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void air_release_fall(Fighter *fighter) {
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = -0x51e;
  native_await(fighter, native_grounded, air_release_land);
}

void retail_air_flip_release(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 85;
  battle_damage(victim, 0x2666);
  fighter_cancel_mmove(victim);
  retail_air_victim_flip(victim);
  native_finish_clip(fighter, air_release_fall);
}

static void air_getup_rise(Fighter *victim) {
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  retail_vm_bind_animation(victim, mk4_image_u32(retail_vm_image(), MK4_RETAIL_ANIMATION_DESCRIPTOR_AIR_GETUP_RISE) * 4, 0);
  native_finish_clip(victim, mk4_mmove_thrown_finish);
}

void retail_air_flip_getup(Fighter *victim) {
  native_finish_clip(victim, air_getup_rise);
}

void retail_air_slam_fall(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_AIR_SLAM_FALL, fighter_ready);
}

void retail_air_slam_end(Fighter *fighter) {
  battle_damage((Fighter *)fighter_opponent(fighter), 0x30a3);
  retail_slam_tail(fighter);
}

/* 488ba0 and 488c00 pass the original records to shared native 4914c0. */
void retail_getup_back(Fighter *fighter) {
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_FLIP_GETUP);
}

void retail_getup_roll(Fighter *fighter) {
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_ROLL_GETUP);
}

/* 488b80 calls 490350 before the ordinary backward getup. Simulation
 * advances outside the renderer's pause gate, so no global frame flag is used. */
void retail_getup_flip(Fighter *fighter) {
  retail_flip_facing(fighter);
  retail_getup_back(fighter);
}

/* 485b10 flips once before488c00;485b50 flips twice via488bd0. */
void retail_getup_roll_flip(Fighter *fighter) {
  retail_flip_facing(fighter);
  retail_getup_roll(fighter);
}

/*488c10 selects4ea230; round/KO result orchestration belongs to battle. */
void retail_getup_front(Fighter *fighter) {
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_FRONT_GETUP);
}

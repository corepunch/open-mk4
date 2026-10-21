#include "retail_vm.h"
#include "../character.h"

/* Native479f00 D[26], followed by two native47a280 paired cycles.
 * 4e80f4 is a physics callback descriptor, not the adjacent105 kick. */
static const AttackRecord power_record = {0xc7ae, -0x16624, 0xe872, 0x13020, 163, 163, 3, 0, 0x1eb8};
static void power_cycle(Fighter *fighter);
static void power_recover(Fighter *fighter);
static void power_victim_fall(Fighter *fighter);

static void power_release(Fighter *fighter) {
  if (fighter->native_followup) {
    Fighter *victim = (Fighter *)fighter_opponent(fighter);
    fighter->native_followup = false;
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    if ((victim->reaction == 163 || victim->reaction == 94) && fighter_airborne(victim)) {
      fighter_interrupt(victim, power_victim_fall);
    }
  }
}

static void power_victim_ground(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void power_victim_fall(Fighter *fighter) {
  static const MoveDef first = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  static const MoveDef middle = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  bool second = fighter->reaction == 94;
  int pose = fighter_sample(fighter);
  bool inherited = fighter_move(fighter)->clip == (second ? 311 : 127);
  fighter_start_move(fighter, S_HIT, second ? &middle : &first);
  fighter->flags |= FF_REACTING;
  fighter->body.gravity = 0x51e;
  fighter->body.on_land = NULL;
  /* Replace any queued capture entry as well as the current held phase.
   * Otherwise its initial stop-motion would undo interruption's gravity. */
  native_begin(fighter, 0, 0, mk4_mmove_thrown_finish);
  native_frame(fighter, 0, inherited ? pose : 0, power_victim_ground);
  native_play(fighter, false);
}

static void power_cleanup(Fighter *fighter) {
  power_release(fighter);
  mk4_script_stop_body(fighter);
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
    fighter->body.on_land = mk4_land;
  }
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
}

static void power_ready(Fighter *fighter) {
  bool airborne = fighter_airborne(fighter);
  q16 velocity[3], gravity = fighter->body.gravity;
  memcpy(velocity, fighter->body.vel, sizeof velocity);
  fighter_finish_mmove(fighter, fighter_ready);
  /* Native46f650 restores air control without cancelling the fall. The
   * interrupted-move cleanup still owns stopping a replaced attack. */
  if (airborne) {
    memcpy(fighter->body.vel, velocity, sizeof velocity);
    fighter->body.gravity = gravity;
    fighter->body.on_land = mk4_land;
  }
}

static void power_finish(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, first, last, power_ready);
  for (int pose = first; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void power_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void power_recover(Fighter *fighter) {
  power_release(fighter);
  fighter->body.physics = power_decay;
  int first = fighter_sample(fighter);
  native_begin(fighter, first, 29, power_finish);
  for (int pose = first; pose <= 29; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void power_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (fighter->native_variant++ == 0) {
    battle_react(victim, 94);
    power_cycle(fighter);
  } else {
    battle_react(victim, 95);
    fighter->body.gravity = 0x51e;
    power_recover(fighter);
  }
}

static void power_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, power_cycle_done);
}

static void power_cycle(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y] - 0x9999;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x4ccc);
  }
  memcpy(fighter->saved_motion, fighter->body.vel, sizeof fighter->saved_motion);
  mk4_script_stop_body(fighter);
  native_hold(fighter, 4, power_restore);
}

void cage_power_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void power_rewind(Fighter *fighter) {
  if (fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000) {
    fighter->mmove_hold = true;
    return;
  }
  fighter->mmove_hold = false;
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 14);
}

static void power_blocked(Fighter *fighter) {
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0x9999);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  fighter->body.physics = NULL;
  fighter->body.spin[AXIS_X] = -0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  /* 47db60 selects the nine-sample clip0 before its threshold wait. */
  mk4_mmove_bind(fighter, 0, 9, 0);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 0, power_rewind);
  native_play(fighter, false);
}

static void power_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &power_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    power_blocked(fighter);
  } else {
    fighter->native_followup = true;
    fighter->native_variant = 0;
    fighter->flags |= FF_PAIRED;
    power_cycle(fighter);
  }
}

static void power_miss(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  power_recover(fighter);
}

void cage_script_powered_punch_tail(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = -0x1eb8;
  fighter->body.gravity = q16_mul(0x1999, -0x28f);
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 5, power_miss);
  for (int frame = 0; frame < 6; ++frame) {
    native_frame(fighter, frame, 24, power_query);
  }
  native_play(fighter, false);
}

void cage_power_victim(Fighter *fighter) {
  require(fighter->reaction == 163, "Invalid Cage powered-punch victim entry");
  static const MoveDef victim = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_HIT, &victim);
  fighter->flags |= FF_REACTING;
  mk4_script_effect(fighter, 15);
  retail_vm_enter_owned(fighter, 0x4ea040, fighter_ready, 14);
}

void cage_powered_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &cage_s_powered_punch_script);
  fighter->native_followup = false;
  fighter->mmove_cleanup = power_cleanup;
  retail_vm_enter_owned(fighter, 0x4e80d0, fighter_ready, 14);
}

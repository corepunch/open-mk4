#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Native service cluster: native_groin_punch. */
static void native_groin_punch_groin_choose(Fighter *fighter);
static void native_groin_punch_groin_query(Fighter *fighter);
static void native_groin_punch_groin_recover(Fighter *fighter);
static void native_groin_punch_groin_victim(Fighter *victim);
static const MoveDef native_groin_punch_groin_move = {.clip = 438, .last = 14, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x110};
static const MoveDef native_groin_punch_groin_stun = {.clip = 126, .last = 14, .flags = SF_LOCK | SF_REACTION, .action = 0x1014};
static const AttackRecord native_groin_punch_groin_record = {0xd6c8, 0x28f5, 0xba5e, 0xd3f7, 164, 164, 165, 0, 0x28f5};

bool cage_can_groin_punch(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_BLOCK);
  unsigned excluded = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) |
                      BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && !fighter->armed && !fighter_airborne(fighter) && (fighter->held & chord) == chord && !(fighter->held & excluded) &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_PUNCH] <= 4 && g_battle.ticks - fighter->latch[BUTTON_BLOCK] <= 4;
}

static void native_groin_punch_groin_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

void cage_groin_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &native_groin_punch_groin_move);
  fighter->mmove_cleanup = native_groin_punch_groin_cleanup;
  battle_sound(SOUND_C, 9);
  native_begin(fighter, 1, 5, native_groin_punch_groin_choose);
  for (int sample = 1; sample <= 5; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

static void native_groin_punch_groin_victim(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &native_groin_punch_groin_stun);
  victim->flags |= FF_REACTING;
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e9f78, mk4_script_thrown_ready, 14);
}

static void native_groin_punch_groin_contact(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &native_groin_punch_groin_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result != CONTACT_BLOCK) {
    fighter_interrupt((Fighter *)fighter_opponent(fighter), native_groin_punch_groin_victim);
  }
  native_groin_punch_groin_recover(fighter);
}

static void native_groin_punch_groin_query(Fighter *fighter) {
  native_begin(fighter, 1, 3, native_groin_punch_groin_recover);
  native_frame(fighter, 1, 12, native_groin_punch_groin_contact);
  native_frame(fighter, 2, 13, native_groin_punch_groin_contact);
  native_frame(fighter, 3, 14, NULL);
  native_play(fighter, false);
  /* Native468b8a queries D[27] at11 before advancing the next two poses. */
  native_groin_punch_groin_contact(fighter);
}

static void native_groin_punch_groin_recover(Fighter *fighter) {
  int repetitions = fighter->contact == CONTACT_BLOCK ? 12 : 5;
  int pose = fighter_sample(fighter), frame = 0;
  fighter->action = 0x100e;
  fighter->flags |= FF_NO_GUARD;
  /* Native4292f0 always steps once before testing its target. A contact at
   * pose11 therefore retreats to10 before the first extension to14. */
  int first_rewind = pose > 12 ? pose - 12 : 1;
  int first_low = pose - first_rewind;
  int frames = first_rewind + 14 - first_low + (repetitions - 1) * 4;
  native_begin(fighter, 1, frames, NULL);
  native_script_end(fighter, 0x4e5ba8, fighter_ready, 14);
  for (int repeat = 0; repeat < repetitions; ++repeat) {
    do {
      native_frame(fighter, ++frame, --pose, NULL);
    } while (pose > 12);
    while (pose < 14) {
      native_frame(fighter, ++frame, ++pose, NULL);
    }
  }
  native_play(fighter, false);
}

static void native_groin_punch_groin_avoid_reverse(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, pose, fighter_ready);
  for (int frame = 1; frame <= pose; ++frame) {
    native_frame(fighter, frame, pose - frame, NULL);
  }
  native_play(fighter, false);
}

static void native_groin_punch_groin_choose(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  if (roster == 2 || roster == 9 || victim->action == 0x1014) {
    native_hold(fighter, 20, native_groin_punch_groin_avoid_reverse);
    return;
  }
  native_begin(fighter, 6, 11, native_groin_punch_groin_query);
  for (int sample = 6; sample <= 11; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 479b70. */
static const MoveDef n479b70_cage_s_cpu_kick_script = {.clip = 308, .last = 24, .flags = SF_LOCK | SF_RECORD};

void cage_cpu_kick(Fighter *fighter);

/* Native479b70 queries D[25] at poses22/23, then holds24 for twelve
 * contact visits while descriptor4e80c8 installs47bf50 velocity decay.
 * The descriptor is data, never the adjacent powered-punch script. */
static const AttackRecord n479b70_shadow_record = {0x17fbe, -0x51a9, 0x13020, 0x2b43, 162, 162, 165, 0, 0x23d7};

static void n479b70_shadow_ready(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

static void n479b70_shadow_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_hold(fighter, 10, n479b70_shadow_ready);
}

static void n479b70_shadow_contact(Fighter *fighter) {
  if (fighter_sample(fighter) == 24) {
    fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
    fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
  }
  if (battle_contact(fighter, &n479b70_shadow_record) != CONTACT_MISS) {
    mk4_script_stop_body(fighter);
    fighter->sample_override = 24;
    native_hold(fighter, 10, n479b70_shadow_ready);
  }
}

void cage_script_cpu_kick_tail(Fighter *fighter) {
  native_begin(fighter, 22, 35, n479b70_shadow_recover);
  for (int frame = 22; frame <= 35; ++frame) {
    native_frame(fighter, frame, frame < 24 ? frame : 24, n479b70_shadow_contact);
  }
  native_play(fighter, false);
}

void cage_shadow_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_vm_enter_owned(fighter, 0x4e9718, fighter_ready, 14);
}

void cage_cpu_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &n479b70_cage_s_cpu_kick_script);
  retail_vm_enter_owned(fighter, 0x4e80a8, fighter_ready, 14);
}

/* Native service cluster: 479f00, 47dae0. */
static const MoveDef n479f00_cage_s_powered_punch_script = {.clip = 85, .last = 54, .flags = SF_LOCK | SF_RECORD};

/* Native479f00 D[26], followed by two native47a280 paired cycles.
 * 4e80f4 is a physics callback descriptor, not the adjacent105 kick. */
static const AttackRecord n479f00_power_record = {0xc7ae, -0x16624, 0xe872, 0x13020, 163, 163, 3, 0, 0x1eb8};
static void n479f00_power_cycle(Fighter *fighter);
static void n479f00_power_recover(Fighter *fighter);
static void n479f00_power_victim_fall(Fighter *fighter);

static void n479f00_power_release(Fighter *fighter) {
  if (fighter->native_followup) {
    Fighter *victim = (Fighter *)fighter_opponent(fighter);
    fighter->native_followup = false;
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    if ((retail_reaction_native(victim) == 0x488400 || retail_reaction_native(victim) == 0x488450) && fighter_airborne(victim)) {
      fighter_interrupt(victim, n479f00_power_victim_fall);
    }
  }
}

static void n479f00_power_victim_ground(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void n479f00_power_victim_fall(Fighter *fighter) {
  static const MoveDef first = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  static const MoveDef middle = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  bool second = retail_reaction_native(fighter) == 0x488450;
  int pose = fighter_sample(fighter);
  bool inherited = fighter_move(fighter)->clip == (second ? 311 : 127);
  fighter_start_move(fighter, S_HIT, second ? &middle : &first);
  fighter->flags |= FF_REACTING;
  fighter->body.gravity = 0x51e;
  fighter->body.on_land = NULL;
  /* Replace any queued capture entry as well as the current held phase.
   * Otherwise its initial stop-motion would undo interruption's gravity. */
  native_begin(fighter, 0, 0, mk4_mmove_thrown_finish);
  native_frame(fighter, 0, inherited ? pose : 0, n479f00_power_victim_ground);
  native_play(fighter, false);
}

static void n479f00_power_cleanup(Fighter *fighter) {
  n479f00_power_release(fighter);
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

static void n479f00_power_ready(Fighter *fighter) {
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

static void n479f00_power_finish(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, first, last, n479f00_power_ready);
  for (int pose = first; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void n479f00_power_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void n479f00_power_recover(Fighter *fighter) {
  n479f00_power_release(fighter);
  fighter->body.physics = n479f00_power_decay;
  int first = fighter_sample(fighter);
  native_begin(fighter, first, 29, n479f00_power_finish);
  for (int pose = first; pose <= 29; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void n479f00_power_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (fighter->native_variant++ == 0) {
    battle_react(victim, 94);
    n479f00_power_cycle(fighter);
  } else {
    battle_react(victim, 95);
    fighter->body.gravity = 0x51e;
    n479f00_power_recover(fighter);
  }
}

static void n479f00_power_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, n479f00_power_cycle_done);
}

static void n479f00_power_cycle(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y] - 0x9999;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x4ccc);
  }
  memcpy(fighter->saved_motion, fighter->body.vel, sizeof fighter->saved_motion);
  mk4_script_stop_body(fighter);
  native_hold(fighter, 4, n479f00_power_restore);
}

static void n479f00_power_rewind(Fighter *fighter) {
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

static void n479f00_power_blocked(Fighter *fighter) {
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
  native_frame(fighter, 0, 0, n479f00_power_rewind);
  native_play(fighter, false);
}

static void n479f00_power_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &n479f00_power_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    n479f00_power_blocked(fighter);
  } else {
    fighter->native_followup = true;
    fighter->native_variant = 0;
    fighter->flags |= FF_PAIRED;
    n479f00_power_cycle(fighter);
  }
}

static void n479f00_power_miss(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  n479f00_power_recover(fighter);
}

void cage_script_powered_punch_tail(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = -0x1eb8;
  fighter->body.gravity = q16_mul(0x1999, -0x28f);
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 5, n479f00_power_miss);
  for (int frame = 0; frame < 6; ++frame) {
    native_frame(fighter, frame, 24, n479f00_power_query);
  }
  native_play(fighter, false);
}

void cage_power_victim(Fighter *fighter) {
  require(retail_reaction_native(fighter) == 0x488400, "Invalid Cage powered-punch victim entry");
  static const MoveDef victim = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_HIT, &victim);
  fighter->flags |= FF_REACTING;
  mk4_script_effect(fighter, 15);
  retail_vm_enter_owned(fighter, 0x4ea040, fighter_ready, 14);
}

void cage_powered_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n479f00_cage_s_powered_punch_script);
  fighter->native_followup = false;
  fighter->mmove_cleanup = n479f00_power_cleanup;
  retail_vm_enter_owned(fighter, 0x4e80d0, fighter_ready, 14);
}

/* Native service cluster: native_punches. */
void cage_powered_punch(Fighter *fighter);

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

/* Native service cluster: 47f060, 47f070. */
static const MoveDef n47f060_triple = {.clip = 100, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x208};
static const MoveDef n47f060_middle_victim = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = 0x4001};
static const AttackRecord n47f060_triple_record = {0x970a, -0x1170a, 0xb851, 0xffbe, 93, 93, 8, 0, 0x30a3};
static void n47f060_triple_recover(Fighter *fighter);
static void n47f060_triple_cycle(Fighter *fighter);

static void n47f060_triple_release(Fighter *fighter) {
  if (!fighter->native_followup) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_followup = false;
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  /* An interrupted owner cannot leave a captured intermediate victim
   * hovering at the paired -.3 offset with its old zero gravity. */
  if ((retail_reaction_native(victim) == 0x488420 || retail_reaction_native(victim) == 0x488450) && fighter_airborne(victim)) {
    victim->body.gravity = 0x51e;
  }
}

static void n47f060_triple_cleanup(Fighter *fighter) {
  n47f060_triple_release(fighter);
  mk4_script_stop_body(fighter);
}

static void n47f060_triple_to(Fighter *fighter, int target, FighterFn end) {
  int first = fighter_sample(fighter);
  require(first <= target && target <= fighter_move(fighter)->last, "Invalid Fujin triple-kick native playback range");
  native_begin(fighter, first, target, end);
  for (int pose = first; pose <= target; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void n47f060_triple_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  n47f060_triple_to(fighter, 44, mk4_mmove_thrown_finish);
}

static void n47f060_triple_recover(Fighter *fighter) {
  n47f060_triple_release(fighter);
  /* 47f280 reaches21 with the existing motion, then stops and finishes. */
  n47f060_triple_to(fighter, 21, n47f060_triple_finish);
}

static void n47f060_triple_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  if (++fighter->native_variant < 3) {
    battle_react((Fighter *)fighter_opponent(fighter), fighter->native_variant == 1 ? 94 : 95);
    n47f060_triple_cycle(fighter);
  } else {
    n47f060_triple_recover(fighter);
  }
}

static void n47f060_triple_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, n47f060_triple_cycle_done);
}

static void n47f060_triple_cycle(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  /* 47f390 ->48fee0 aligns the captured peer .3 ahead and .3 above,
   * freezes the owner's saved momentum four visits, then restores90%
   * horizontal momentum for four visits before submitting pose17. */
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y] - 0x4ccc;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x4ccc);
  }
  memcpy(fighter->saved_motion, fighter->body.vel, sizeof fighter->saved_motion);
  mk4_script_stop_body(fighter);
  native_hold(fighter, 4, n47f060_triple_restore);
}

static void n47f060_triple_floor_hold(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void n47f060_triple_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void n47f060_triple_blocked(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  /* 47f480 ->490890 selects the first4e8790 recoil vector. */
  fighter_set_speed(fighter, -0x51e);
  fighter->body.vel[AXIS_Y] = -0x1999;
  fighter->body.gravity = 0x147;
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 0, n47f060_triple_land);
  native_frame(fighter, 0, fighter_sample(fighter), n47f060_triple_floor_hold);
  native_play(fighter, false);
}

static void n47f060_triple_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &n47f060_triple_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    n47f060_triple_blocked(fighter);
  } else {
    fighter->native_variant = 0;
    fighter->native_followup = true;
    fighter->flags |= FF_PAIRED;
    n47f060_triple_cycle(fighter);
  }
}

void fujin_triple_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 100 && fighter_sample(fighter) == 13, "Fujin triple kick needs its generated13-pose startup");
  /* 47f070 advances before querying D[9], four times at14..17. */
  native_begin(fighter, 1, 4, n47f060_triple_recover);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, 13 + frame, n47f060_triple_query);
  }
  native_play(fighter, false);
}

void fujin_triple_fast(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8770, fighter_ready, 5);
}

void fujin_triple_victim(Fighter *fighter) {
  if (retail_reaction_native(fighter) == 0x488450) {
    fighter_start_move(fighter, S_HIT, &n47f060_middle_victim);
    fighter->flags |= FF_REACTING;
    retail_vm_enter_owned(fighter, 0x4ea050, mk4_mmove_thrown_finish, 5);
  } else {
    require(retail_reaction_native(fighter) == 0x488420, "Invalid Fujin triple-kick victim stage");
    /* Native488420/488440 and488830/488850 are also the exact
     * first/second axe victim entries; reuse their shared adaptations. */
    weapon_axe_reaction(fighter, false, (RetailScriptContinuation){0x4ea040, mk4_mmove_thrown_finish, 5});
  }
}

void fujin_triple_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47f060_triple);
  fighter->native_followup = false;
  fighter->mmove_cleanup = n47f060_triple_cleanup;
  battle_sound(SOUND_C, 0x42);
  retail_vm_enter_owned(fighter, 0x4e8740, fighter_ready, 5);
}

/* Native service cluster: 4803a0. */
static const MoveDef n4803a0_wind_grab = {.clip = 10, .last = 2, .flags = SF_LOCK | SF_RECORD, .action = 0x20a};
static const MoveDef n4803a0_orbit_victim = {.clip = 8, .last = 28, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x100d};
static const MeshAnimation n4803a0_grab_meshes[] = {
    {.first = 356, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
    {.first = 350, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
};
static void n4803a0_grab_loop(Fighter *fighter);

bool fujin_can_wind_grab(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->wind_grab_ready && victim->hits <= 2 && victim->action != 0x1001 &&
         victim->action != 0x100d;
}

static void n4803a0_grab_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

static void n4803a0_orbit_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
}

static void n4803a0_grab_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_mmove_thrown_finish);
}

static void n4803a0_grab_stamp_and_return(Fighter *fighter) {
  fighter->wind_grab_ready = g_battle.ticks + 90;
  n4803a0_grab_return(fighter);
}

static void n4803a0_grab_effect_step(Projectile *effect) {
  const Fighter *owner = &g_battle.fighters[effect->owner];
  effect->pos[AXIS_X] = owner->body.pos[AXIS_X];
  effect->pos[AXIS_Y] = 0;
  effect->pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  if (!effect->phase && owner->action != 0x20a && owner->action != 0x1000) {
    /* 414810 retains the two authored wind surfaces for30 exit visits. */
    effect->phase = 1;
    effect->lifetime = effect->ticks + 30;
  }
}

static void n4803a0_grab_effect_start(Fighter *fighter) {
  /* 4146d0 creates two wind groups at chain offsets0/6 from4d1a18.
   * Keep their original shared surfaces in the existing bounded object pool. */
  for (int i = 0; i < 2; ++i) {
    Projectile *effect = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 6000, n4803a0_grab_effect_step);
    if (effect != NULL) {
      effect->mesh_animation = &n4803a0_grab_meshes[i];
      effect->initial_mesh = n4803a0_grab_meshes[i].first;
      memset(effect->angle, 0, sizeof effect->angle);
    }
  }
}

static void n4803a0_grab_damage(Fighter *fighter) {
  fighter->action = 0;
  mk4_script_damage(fighter, 0x1eb8);
  fighter->wind_grab_ready = g_battle.ticks + 90;
  native_hold(fighter, 14, n4803a0_grab_return);
}

static void n4803a0_grab_close(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(fighter);
  if (battle_blocks_attack(victim, 0)) {
    fighter->action = 0x1015;
    battle_react(victim, 7);
    fighter->flags |= FF_NO_GUARD;
    mk4_mmove_bind(fighter, 13, 56, 0);
    native_hold(fighter, 60, n4803a0_grab_stamp_and_return);
  } else {
    mk4_script_context(fighter, 5);
    fighter->action = 0x1000;
    battle_react(victim, 98);
    battle_sound(SOUND_C, 0x3e);
    native_hold(fighter, 51, n4803a0_grab_damage);
  }
}

static void n4803a0_grab_control(Fighter *fighter) {
  if (fighter_distance(fighter) < 1.f) {
    n4803a0_grab_close(fighter);
    return;
  }
  /* Native496d20/491050 resolves screen-space steering, rather than
   * multiplying a fixed facing sign into world X. LP is word-B bit1/16. */
  q16 direction = fighter->held & BUTTON_BIT(BUTTON_LEFT) ? -0x1999 : 0;
  if (fighter->held & BUTTON_BIT(BUTTON_RIGHT)) {
    direction = 0x1999;
  }
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0x9999);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0x9999);
  fighter_set_screen_speed(fighter, direction);
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    mk4_script_stop_body(fighter);
    fighter->flags |= FF_NO_GUARD;
    native_hold(fighter, 12, n4803a0_grab_stamp_and_return);
  }
}

static void n4803a0_grab_sound_and_control(Fighter *fighter) {
  battle_sound(SOUND_C, 0x3e);
  n4803a0_grab_control(fighter);
}

static void n4803a0_grab_loop(Fighter *fighter) {
  /* Native480730 first sounds after20 visits, then every70. The loop
   * clock is encoded in authored rows; holding LP supplies its exit gate. */
  native_begin(fighter, 0, 69, n4803a0_grab_loop);
  for (int frame = 0; frame < 70; ++frame) {
    native_frame(fighter, frame, 0, frame == 19 ? n4803a0_grab_sound_and_control : n4803a0_grab_control);
  }
  native_play(fighter, false);
}

void fujin_grab_control_start(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 11 && fighter_sample(fighter) == 0, "Wind-grab control needs its generated11-pose entry");
  n4803a0_grab_loop(fighter);
}

static q16 n4803a0_angle_wrap(q16 angle) {
  angle %= 0x6487e;
  return angle < 0 ? angle + 0x6487e : angle;
}

static q16 n4803a0_direction_angle(q16 x, q16 z) {
  return n4803a0_angle_wrap((q16)llround(atan2((double)z, (double)x) * Q16_ONE));
}

static void n4803a0_orbit_place(Fighter *victim, const Fighter *owner, q16 angle, q16 radius) {
  double radians = q16_float(angle);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + (q16)llround(cos(radians) * radius);
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z] + (q16)llround(sin(radians) * radius);
}

static void n4803a0_orbit_release(Fighter *victim, const Fighter *owner, q16 angle) {
  n4803a0_orbit_place(victim, owner, angle, victim->saved_motion[1]);
  victim->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
  q16 x = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
  q16 z = owner->body.pos[AXIS_Z] - victim->body.pos[AXIS_Z];
  double length = hypot((double)x, (double)z);
  victim->facing = x >= 0 ? 1 : -1;
  if (length > 0) {
    victim->heading[0] = (q16)llround(x * Q16_ONE / length);
    victim->heading[1] = (q16)llround(z * Q16_ONE / length);
  }
  retail_vm_enter_owned(victim, 0x4e9fb0, fighter_ready, 5);
}

static void n4803a0_orbit_step(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  q16 angle = victim->saved_motion[0], radius = victim->saved_motion[1];
  n4803a0_orbit_place(victim, owner, angle, radius);
  victim->saved_motion[0] = n4803a0_angle_wrap(angle + 0x4ccc);
  radius += 0x28f;
  victim->saved_motion[1] = radius > 3 * Q16_ONE ? 5 * Q16_ONE : radius;
  victim->mmove_hold = true;
  if (victim->native_variant == 0) {
    q16 y = victim->body.pos[AXIS_Y] - 0x28f;
    victim->body.pos[AXIS_Y] = y < -0x2cccc ? -0x2cccc : y;
    if (owner->action != 0x1000 || owner->pending != NULL) {
      victim->native_variant = 1;
    }
  } else {
    /* Native487a69 releases toward the arena origin once the orbit comes
     * within.31 radians of that heading, then9fb0 supplies the flight. */
    q16 target = n4803a0_direction_angle(-owner->body.pos[AXIS_X], -owner->body.pos[AXIS_Z]);
    q16 difference = n4803a0_angle_wrap(angle - target);
    if (difference > 0x3243f) {
      difference = 0x6487e - difference;
    }
    if (difference <= 0x4f5c) {
      n4803a0_orbit_release(victim, owner, target);
    }
  }
}

void fujin_grab_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &n4803a0_orbit_victim);
  fighter->flags |= FF_REACTING | FF_PAIRED | FF_UNTOUCHABLE;
  fighter->mmove_cleanup = n4803a0_orbit_cleanup;
  fighter->native_variant = 0;
  const Fighter *owner = fighter_opponent(fighter);
  fighter->saved_motion[0] = n4803a0_direction_angle(fighter->body.pos[AXIS_X] - owner->body.pos[AXIS_X], fighter->body.pos[AXIS_Z] - owner->body.pos[AXIS_Z]);
  fighter->saved_motion[1] = 0x8000;
  mk4_script_indexed_sound(fighter, 5);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 0, n4803a0_orbit_step);
  native_play(fighter, false);
}

void fujin_wind_grab(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &n4803a0_wind_grab);
  fighter->mmove_cleanup = n4803a0_grab_cleanup;
  n4803a0_grab_effect_start(fighter);
  retail_vm_enter_owned(fighter, 0x4e8830, fighter_ready, 5);
}

/* Native service cluster: 43afc0, 43b1a0. */
static void n43afc0_special_query(Fighter *fighter);
static void n43afc0_special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries);
static const MoveDef n43afc0_s_special_punch = {.clip = 599, .last = 56, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef n43afc0_s_special_kick = {.clip = 598, .last = 30, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef n43afc0_s_special_uppercut = {.clip = 600, .last = 36, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const AttackRecord n43afc0_punch_record = {0x13810, -0xe106, 0xfd2f, 0x11be7, 189, 189, 7, 0, 0x2666};
static const AttackRecord n43afc0_kick_record = {0x1b2b0, -0x6b43, 0x16872, 0xa624, 190, 190, 7, 0, 0x2666};
static const AttackRecord n43afc0_uppercut_record = {0x16b43, -0xcf5c, 0x109fb, 0x1424d, 192, 192, 6, 0, 0x23d7};

/* Goro's command handlers open with 489ef0's sampled voice token and
 * 45f870, which stops the grounded body and refaces the pair before the
 * selected clip starts. */
void goro_begin_special(Fighter *fighter, StateId state, const MoveDef *move, int token) {
  battle_sound(SOUND_TOKEN, token);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
}

static void n43afc0_special_query(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

/* 43b940 advances one pose and queries D[n] per visit for its row count;
 * a contact (43ba30) keeps advancing without further queries. 428cf0 then
 * finishes the bound clip, 429670 holds its last pose one visit and 43aa50
 * returns a human controller to its stance. */
static void n43afc0_special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries) {
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && first + queries - 1 <= last, "Goro special contact rows exceed the bound clip");
  fighter->native_record = record;
  native_begin(fighter, 1, last - first + 2, fighter_ready);
  for (int frame = 1; frame <= last - first + 2; ++frame) {
    native_frame(fighter, frame, mini(last, first + frame - 1), frame <= queries ? n43afc0_special_query : NULL);
  }
  native_play(fighter, false);
}

/* 43af90 -> 4dfb60 clip 599 to 14, then 43afc0: three D[32] rows. */
void goro_special_punch(Fighter *fighter) {
  goro_begin_special(fighter, S_PUNCH, &n43afc0_s_special_punch, 0x5fa);
  retail_vm_enter_owned(fighter, 0x4dfb60, fighter_ready, 15);
}

void goro_script_punch_tail(Fighter *fighter) {
  n43afc0_special_tail(fighter, &n43afc0_punch_record, fighter_sample(fighter) + 1, 3);
}

/* 43b170 -> 4dfb70 clip 598 to 8, then 43b1a0: eight D[33] rows. */
void goro_special_kick(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &n43afc0_s_special_kick, 0x5fa);
  retail_vm_enter_owned(fighter, 0x4dfb70, fighter_ready, 15);
}

void goro_script_kick_tail(Fighter *fighter) {
  n43afc0_special_tail(fighter, &n43afc0_kick_record, fighter_sample(fighter) + 1, 8);
}

/* 43b640 binds slot 53c6bc (fixup -> descriptor 4fc99c, clip 600) at
 * pose 0 through 428750, advances nine poses with 429490, then queries
 * D[35] for five rows through 43b940 before finishing the clip. */
void goro_special_uppercut(Fighter *fighter) {
  goro_begin_special(fighter, S_UPPERCUT, &n43afc0_s_special_uppercut, 0x5fa);
  mk4_mmove_bind(fighter, 600, 37, 0);
  fighter->native_record = &n43afc0_uppercut_record;
  native_begin(fighter, 0, 37, fighter_ready);
  for (int frame = 0; frame <= 37; ++frame) {
    native_frame(fighter, frame, mini(36, frame), frame >= 9 && frame < 14 ? n43afc0_special_query : NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 43b370, 43b590, 480f40. */
static void n43b370_leap_rise(Fighter *fighter);
static void n43b370_leap_fall(Fighter *fighter);
static void n43b370_leap_cleanup(Fighter *fighter);
static const MoveDef n43b370_s_leap = {.clip = 601, .last = 51, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x409};
/* D[34]: the zero-damage stomp box under the falling body; every
 * outcome selects reaction 191. 43b370 applies 30% separately. */
static const AttackRecord n43b370_stomp_record = {0x68f5, 0x44dd, 0xe666, 0xc76c, 191, 191, 191, 0, 0};

enum {
  n43b370_LEAP_APEX_SAMPLE = 41, /* 23 + ceil(0x3cccc / 0x3851) one-pose rising visits */
  n43b370_LEAP_RISE = -0x3851,
  n43b370_LEAP_APEX = -0x3cccc,
  n43b370_LEAP_DROP_HEIGHT = -0x40000,
  n43b370_LEAP_DROP_SPEED = 0x1999,
  n43b370_LEAP_DROP_GRAVITY = 0xa3d,
  n43b370_LEAP_DAMAGE = 0x4ccc
};

/* 480f40 sets 532254 every physics visit; 489791 then skips the close-range
 * body separation while Goro stands on his victim. */
void goro_stomp_hold_separation(Fighter *fighter) {
  fighter->flags |= FF_HOLD_SEPARATION;
}

static void n43b370_leap_cleanup(Fighter *fighter) {
  if (fighter->body.physics == goro_stomp_hold_separation) {
    fighter->body.physics = NULL;
  }
}

/* 43b320 (HK F,F,B and the CPU far row): voice 0x64f, action 0409, then
 * 4dfbb8 binds clip 601 and double-steps to 23 before 43b370. */
void goro_leap_stomp(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &n43b370_s_leap, 0x64f);
  fighter->mmove_cleanup = n43b370_leap_cleanup;
  retail_vm_enter_owned(fighter, 0x4dfbb8, fighter_ready, 15);
}

static void n43b370_leap_descend(Fighter *fighter) {
  /* 43b42d: relocate above the X/Z captured at launch and drop from 4.0. */
  fighter->body.pos[AXIS_X] = fighter->saved_motion[0];
  fighter->body.pos[AXIS_Z] = fighter->saved_motion[1];
  fighter->body.pos[AXIS_Y] = n43b370_LEAP_DROP_HEIGHT;
  fighter->body.vel[AXIS_Y] = n43b370_LEAP_DROP_SPEED;
  fighter->body.gravity = n43b370_LEAP_DROP_GRAVITY;
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, n43b370_LEAP_APEX_SAMPLE, n43b370_leap_fall);
  native_play(fighter, false);
}

static void n43b370_leap_rise(Fighter *fighter) {
  if (fighter->body.pos[AXIS_Y] <= n43b370_LEAP_APEX) {
    n43b370_leap_descend(fighter);
  }
}

/* 43b3a4: until the body lands, query D[34] without advancing the pose.
 * A contact disturbs resting Goro objects, deals 30% and enters 4dfbd0;
 * landing first enters the miss recovery 43b620 -> 4dfc38. */
static void n43b370_leap_fall(Fighter *fighter) {
  fighter->mmove_hold = true;
  if (!fighter_airborne(fighter)) {
    fighter->mmove_hold = false;
    fighter->sample_override = n43b370_LEAP_APEX_SAMPLE;
    retail_disturb_resting_weapons();
    retail_vm_enter_owned(fighter, 0x4dfc38, fighter_ready, 15);
    return;
  }
  if (battle_contact(fighter, &n43b370_stomp_record) == CONTACT_MISS) {
    return;
  }
  fighter->mmove_hold = false;
  retail_disturb_resting_weapons();
  battle_damage((Fighter *)fighter_opponent(fighter), n43b370_LEAP_DAMAGE);
  fighter->sample_override = n43b370_LEAP_APEX_SAMPLE;
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
  fighter->body.vel[AXIS_Y] = n43b370_LEAP_RISE;
  native_begin(fighter, first + 1, last, n43b370_leap_descend);
  for (int pose = first + 1; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, n43b370_leap_rise);
  }
  native_play(fighter, false);
}

/* 43b590 -> 43baf0: face the victim, stop and return the controller. The
 * retail 80-visit hold polls the CPU dispatcher 4339b0; portable CPU
 * policy owns that decision after the controller returns. */
void goro_script_stomp_return(Fighter *fighter) {
  n43b370_leap_cleanup(fighter);
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

/* Native service cluster: 46a950, 46a970. */
/* Native service cluster: 47ab50. */
/* LP D,B record 4e4394 (flags 0x4, gate 45e910) and the near/far CPU rows
 * select 47ab10: stamp 0xc, action 0x215 and script 4e8150 (indexed 37,
 * C[78], speed 0x2666, clip 299 two poses per visit to 18, tail 47ab50). */
static const MoveDef n47ab50_s_dash = {.clip = 299, .last = 34, .flags = SF_LOCK | SF_RECORD, .action = 0x215};
/* D[22] record 4ec050: 16%, reactions 157/10/165. */
static const AttackRecord n47ab50_dash_record = {0x16b43, -0x4c8b, 0xcf1a, 0x2b85, 157, 10, 165, 0, 0x28f5};
static const MoveDef n47ab50_s_dash_stagger = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION};
static const MoveDef n47ab50_s_dash_fall = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_REACTION};
static const MoveDef n47ab50_s_dash_air_fall = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};

enum {
  n47ab50_DASH_QUERIES = 15,
  n47ab50_DASH_LOOP_POSE = 23,
  n47ab50_DASH_STOP_TICKS = 5,
  n47ab50_DASH_BLOCK_TICKS = 40,
  n47ab50_DASH_FINAL_TICKS = 20
};

static void n47ab50_dash_query(Fighter *fighter);
static void n47ab50_dash_step_query(Fighter *fighter);

bool jax_can_dash_punch(const Fighter *fighter) {
  return g_battle.ticks >= fighter->dash_punch_ready;
}

void jax_dash_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n47ab50_s_dash);
  fighter->dash_punch_ready = g_battle.ticks + 60;
  retail_vm_enter_owned(fighter, 0x4e8150, fighter_ready, 13);
}

/* 47af90, installed through descriptor 4e8174 as the group +44 callback:
 * X/Z velocity *= 0xe666, then 4297a0 advances one pose and loops the
 * final pose back to 23. The pose half is resolved in the frame rows. */
static void n47ab50_dash_friction(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static int n47ab50_dash_advance(int pose) {
  return pose < n47ab50_s_dash.last ? pose + 1 : n47ab50_DASH_LOOP_POSE;
}

static void n47ab50_dash_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->body.physics == n47ab50_dash_friction) {
    fighter->body.physics = NULL;
  }
}

static void n47ab50_dash_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* While the task waits, the +44 callback keeps advancing the looping pose.
 * Rows [first, first + ticks) start with action and continue that pose. */
static int n47ab50_dash_rows(Fighter *fighter, int first, int ticks, int pose, FighterFn action) {
  native_frame(fighter, first, pose, action);
  for (int frame = first + 1; frame < first + ticks; ++frame) {
    pose = n47ab50_dash_advance(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  return n47ab50_dash_advance(pose);
}

/* 47af10: blocked contact stops motion, removes guard and stores 0x100e. */
static void n47ab50_dash_block_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->action = 0x100e;
}

/* 47ae06: camera shake 6, then 48f6e0 saves and stops the owner's motion. */
static void n47ab50_dash_stop(Fighter *fighter) {
  mk4_script_shake(fighter, 6);
  mk4_script_save_motion(fighter);
}

/* 47acc0 phase 1: 48f750 restores the saved velocity, then one decay. */
static void n47ab50_dash_resume(Fighter *fighter) {
  mk4_script_restore_motion(fighter);
  n47ab50_dash_friction(fighter);
}

/* Phase 2 with the counter still set: reaction 157, then another stop. */
static void n47ab50_dash_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  n47ab50_dash_stop(fighter);
}

/* 47ae90: reaction 168 ends the sequence; stop and wait 20 ticks. */
static void n47ab50_dash_final(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 168);
  mk4_script_stop_body(fighter);
}

/* The last phase-2 visit dispatches 157 and, in the same visit, 47ae90. */
static void n47ab50_dash_last_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  n47ab50_dash_final(fighter);
}

/* 47acc0 phase 0 on D[22] contact: blocked -> 47af10 (40 ticks); an
 * airborne victim (48ee60) -> 47ae90; otherwise the cursor is raised to 24
 * and the counter of two runs stop 5, resume 5, reaction 157 twice. */
static void n47ab50_dash_contact(Fighter *fighter, ContactResult result) {
  int pose = fighter_sample(fighter);
  if (result == CONTACT_BLOCK) {
    native_begin(fighter, 0, n47ab50_DASH_BLOCK_TICKS - 1, n47ab50_dash_ready);
    n47ab50_dash_rows(fighter, 0, n47ab50_DASH_BLOCK_TICKS, pose, n47ab50_dash_block_stop);
  } else if (fighter_airborne(fighter_opponent(fighter))) {
    native_begin(fighter, 0, n47ab50_DASH_FINAL_TICKS - 1, n47ab50_dash_ready);
    n47ab50_dash_rows(fighter, 0, n47ab50_DASH_FINAL_TICKS, pose, n47ab50_dash_final);
  } else {
    int frame = 0;
    native_begin(fighter, 0, 4 * n47ab50_DASH_STOP_TICKS + n47ab50_DASH_FINAL_TICKS - 1, n47ab50_dash_ready);
    pose = n47ab50_dash_rows(fighter, frame, n47ab50_DASH_STOP_TICKS, maxi(pose, 24), n47ab50_dash_stop);
    pose = n47ab50_dash_rows(fighter, frame += n47ab50_DASH_STOP_TICKS, n47ab50_DASH_STOP_TICKS, pose, n47ab50_dash_resume);
    pose = n47ab50_dash_rows(fighter, frame += n47ab50_DASH_STOP_TICKS, n47ab50_DASH_STOP_TICKS, pose, n47ab50_dash_pummel);
    pose = n47ab50_dash_rows(fighter, frame += n47ab50_DASH_STOP_TICKS, n47ab50_DASH_STOP_TICKS, pose, n47ab50_dash_resume);
    n47ab50_dash_rows(fighter, frame + n47ab50_DASH_STOP_TICKS, n47ab50_DASH_FINAL_TICKS, pose, n47ab50_dash_last_pummel);
  }
  native_play(fighter, false);
}

static void n47ab50_dash_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &n47ab50_dash_record);
  if (result != CONTACT_MISS) {
    n47ab50_dash_contact(fighter, result);
  }
}

/* 47ab50 phase 1 calls 47af90 itself before each further query. */
static void n47ab50_dash_step_query(Fighter *fighter) {
  n47ab50_dash_friction(fighter);
  n47ab50_dash_query(fighter);
}

/* Counter zero: 47af90 once more, no guard, 4914a0 removes the callback
 * and 429460 rewinds from the final stored pose to zero before return. */
static void n47ab50_dash_miss(Fighter *fighter) {
  n47ab50_dash_friction(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->body.physics = NULL;
}

/* 47ab50 phase 0: install 47af90, set the counter to 15 and query D[22]
 * once per visit. Both the callback and the phase-1 call advance a pose. */
void jax_dash_punch_contact(Fighter *fighter) {
  fighter->body.physics = n47ab50_dash_friction;
  fighter->mmove_cleanup = n47ab50_dash_cleanup;
  int pose = fighter_sample(fighter);
  int last = n47ab50_DASH_QUERIES + 1 + n47ab50_s_dash.last;
  native_begin(fighter, 0, last, n47ab50_dash_ready);
  native_frame(fighter, 0, pose, n47ab50_dash_query);
  for (int frame = 1; frame < n47ab50_DASH_QUERIES; ++frame) {
    pose = n47ab50_dash_advance(n47ab50_dash_advance(pose));
    native_frame(fighter, frame, pose, n47ab50_dash_step_query);
  }
  native_frame(fighter, n47ab50_DASH_QUERIES, n47ab50_dash_advance(n47ab50_dash_advance(pose)), n47ab50_dash_miss);
  for (int frame = n47ab50_DASH_QUERIES + 1; frame <= last; ++frame) {
    native_frame(fighter, frame, last - frame, NULL);
  }
  native_play(fighter, false);
}

/* 4ea4e8[157] -> 4883f0 -> 4ea028: effect 9, context sound 1, clip 311. */
void jax_dash_victim_hit(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &n47ab50_s_dash_stagger);
  retail_vm_enter_owned(victim, 0x4ea028, fighter_ready, 13);
}

/* 4ea4e8[168] -> 4867b0: disarm; an airborne victim takes 481ce0 (shake 0,
 * 4e89d8 fall and 488c00 roll getup), a grounded one the 4913a0 slide and
 * clip 130 of 4e9e38. */
void jax_dash_victim_throw(Fighter *victim) {
  mk4_script_disarm_self(victim);
  if (fighter_airborne(victim)) {
    mk4_script_shake(victim, 0);
    fighter_start_move(victim, S_KNOCKDOWN, &n47ab50_s_dash_air_fall);
    retail_vm_enter_owned(victim, 0x4e89d8, fighter_ready, 13);
    return;
  }
  fighter_start_move(victim, S_KNOCKDOWN, &n47ab50_s_dash_fall);
  victim->body.physics = fighter_friction;
  retail_vm_enter_owned(victim, 0x4e9e38, fighter_ready, 13);
}

/* 488c00 -> 4914c0(4ea220): roll getup 191/16 at -0x147a, crouch 190/16. */

/* Native service cluster: 47bd40. */
/* 47bd10 sets action 210 and enters 4e8210: sound C60, forward .18, Y -.1,
 * gravity 41, then clip 298 advances to sample 9 before native 47bd40. */
static const MoveDef n47bd40_aerial_punch = {.clip = 298, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x210};
/* D[16] 0x4ec0e0: 13% damage, reaction 123 on the ground or in the air, no block reaction. */
static const AttackRecord n47bd40_aerial_contact = {0x107ae, -0x8f1a, 0x107ae, 0xb851, 123, 123, 0, 0, 0x2147};

enum {
  n47bd40_AERIAL_QUERIES = 15,
  n47bd40_AERIAL_CONTACT_STOP = 10,
  n47bd40_FALL_CLIP = 310,
  n47bd40_FALL_COUNT = 28,
  n47bd40_FALL_FIRST = 7,
  n47bd40_FALL_HOLD = 18
};

static void n47bd40_aerial_drag(Fighter *fighter) {
  /* 4e8250 -> 47bf50 -> 490650: scale the X/Z velocity by .9 each world step. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void n47bd40_aerial_cleanup(Fighter *fighter) {
  if (fighter->body.physics == n47bd40_aerial_drag) {
    fighter->body.physics = NULL;
  }
}

static void n47bd40_fall_landed(Fighter *fighter) {
  /* 480cf0 on the floor: cursor 18, 428d10 finishes clip 310, then 46f650. */
  native_begin(fighter, 1, n47bd40_FALL_COUNT - n47bd40_FALL_HOLD, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= n47bd40_FALL_COUNT - n47bd40_FALL_HOLD; ++frame) {
    native_frame(fighter, frame, n47bd40_FALL_HOLD + frame - 1, NULL);
  }
  native_play(fighter, false);
}

static void n47bd40_fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    n47bd40_fall_landed(fighter);
  }
}

/* Shared native 480cf0: gravity and Y velocity, action 1005, face the peer
 * and play fall clip 310 from sample 7 to its sample-18 air hold. Landing
 * at any visit plays samples 18..27 and returns through the controller. */
static void n47bd40_aerial_fall(Fighter *fighter, q16 gravity) {
  fighter->body.physics = NULL;
  fighter->body.gravity = gravity;
  fighter->action = 0x1005;
  retail_face_peer(fighter);
  mk4_mmove_bind(fighter, n47bd40_FALL_CLIP, n47bd40_FALL_COUNT, n47bd40_FALL_FIRST);
  native_begin(fighter, 1, n47bd40_FALL_HOLD - n47bd40_FALL_FIRST, n47bd40_fall_landed);
  for (int frame = 1; frame <= n47bd40_FALL_HOLD - n47bd40_FALL_FIRST; ++frame) {
    native_frame(fighter, frame, n47bd40_FALL_FIRST + frame, frame < n47bd40_FALL_HOLD - n47bd40_FALL_FIRST ? n47bd40_fall_check : mk4_mmove_wait_grounded);
  }
  native_play(fighter, false);
}

static void n47bd40_contact_fall(Fighter *fighter) {
  /* 47be50 resumes after its stop with 480cf0, gravity 28f and the stopped Y velocity. */
  n47bd40_aerial_fall(fighter, 0x28f);
}

static void n47bd40_aerial_hit(Fighter *fighter) {
  /* 470e80 sets action 1006; 47be50 stops all motion and sleeps ten visits. */
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_stop(fighter);
  native_hold(fighter, n47bd40_AERIAL_CONTACT_STOP, n47bd40_contact_fall);
}

static void n47bd40_aerial_land(Fighter *fighter) {
  /* 470e80 sets action 1006; 47bed0 runs 46fd90 landing, then 46f650. */
  fighter->action = ACTION_RETAIL_1006;
  fighter_finish_mmove(fighter, mk4_land);
}

static void n47bd40_timeout_fall(Fighter *fighter) {
  /* 4914a0 removes the drag callback before 480cf0 continues the current Y velocity. */
  n47bd40_aerial_fall(fighter, 0x28f);
}

static void n47bd40_aerial_timeout(Fighter *fighter) {
  /* 47bd60: install the drag callback and gravity 28f, then 428d10 plays
   * the remaining clip-298 samples without a floor check. */
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  fighter->body.physics = n47bd40_aerial_drag;
  fighter->body.gravity = 0x28f;
  if (first >= last) {
    n47bd40_timeout_fall(fighter);
    return;
  }
  native_begin(fighter, 1, last - first, n47bd40_timeout_fall);
  for (int frame = 1; frame <= last - first; ++frame) {
    native_frame(fighter, frame, first + frame, NULL);
  }
  native_play(fighter, false);
}

static void n47bd40_aerial_first_query(Fighter *fighter) {
  if (battle_contact(fighter, &n47bd40_aerial_contact) != CONTACT_MISS) {
    n47bd40_aerial_hit(fighter);
  }
}

static void n47bd40_aerial_query(Fighter *fighter) {
  /* 4704d0 checks the floor (48ef30) before every later D[16] query. */
  if (!fighter_airborne(fighter)) {
    n47bd40_aerial_land(fighter);
    return;
  }
  n47bd40_aerial_first_query(fighter);
}

static void n47bd40_aerial_expired(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    n47bd40_aerial_land(fighter);
  } else {
    n47bd40_aerial_timeout(fighter);
  }
}

void kai_aerial_punch_contact(Fighter *fighter) {
  /* 4e8238 = {15 visits, D[16], contact 47be50, floor 47bed0}, then timeout 47bd60. */
  require(fighter_move(fighter)->clip == 298 && fighter_sample(fighter) == 9, "Kai aerial punch needs its generated hop startup");
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, n47bd40_AERIAL_QUERIES, n47bd40_aerial_expired);
  for (int frame = 1; frame <= n47bd40_AERIAL_QUERIES; ++frame) {
    native_frame(fighter, frame, pose, frame == 1 ? n47bd40_aerial_first_query : n47bd40_aerial_query);
  }
  native_play(fighter, false);
}

void kai_input_47bd10(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n47bd40_aerial_punch);
  fighter->mmove_cleanup = n47bd40_aerial_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8210, fighter_ready, 11);
}

/* Native service cluster: 47eb30, 480e90. */
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static void n47eb30_bicycle_victim_guard(Fighter *victim);
static void n47eb30_bicycle_victim_cycle(Fighter *victim);
static void n47eb30_bicycle_victim_script(Fighter *victim);
static void n47eb30_bicycle_cleanup(Fighter *fighter);
static void n47eb30_bicycle_query(Fighter *fighter);
static void n47eb30_bicycle_pair(Fighter *fighter);
static bool n47eb30_bicycle_grounded(const Fighter *fighter);
static void n47eb30_bicycle_land(Fighter *fighter);
static void n47eb30_bicycle_descend(Fighter *fighter);
static void n47eb30_bicycle_carry_end(Fighter *fighter);
static void n47eb30_bicycle_contact_end(Fighter *fighter);
static void n47eb30_bicycle_motion_pose(Fighter *fighter, float pose);
static const MoveDef n47eb30_s_bicycle = {.clip = 68, .last = 22, .flags = RETAIL, .action = ACTION_RETAIL_0206};
static const MoveDef n47eb30_s_bicycle_victim = {.clip = 136, .last = 23, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord n47eb30_bicycle_record = {0xf851, -0x547a, 0xf5c2, 0x70a3, 84, 84, 0, 0, 0x2666};

static void n47eb30_bicycle_victim_guard(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (victim->script_phase != 0 || owner->action != ACTION_RETAIL_0206 || owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
    fighter_finish_mmove(victim, retail_bicycle_release);
  }
}

static void n47eb30_bicycle_victim_cycle(Fighter *victim) {
  native_begin(victim, 0, 19, NULL);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(victim, frame, 4 + (frame + 1) % 20, n47eb30_bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void n47eb30_bicycle_victim_script(Fighter *victim) {
  native_begin(victim, 1, 24, n47eb30_bicycle_victim_cycle);
  for (int frame = 1; frame <= 24; ++frame) {
    native_frame(victim, frame, frame < 24 ? frame : 4, n47eb30_bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void n47eb30_bicycle_cleanup(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move == &n47eb30_s_bicycle_victim && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
}

static void n47eb30_bicycle_query(Fighter *fighter) {
  if (!fighter_airborne(fighter) || battle_contact(fighter, &n47eb30_bicycle_record) != CONTACT_MISS) {
    n47eb30_bicycle_contact_end(fighter);
  }
}

static void n47eb30_bicycle_pair(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move != &n47eb30_s_bicycle_victim || victim->pending != NULL || victim->current_mmove == NULL || !(fighter->flags & FF_PAIRED) ||
      !(victim->flags & FF_PAIRED)) {
    n47eb30_bicycle_carry_end(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xe666);
  }
}

static bool n47eb30_bicycle_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void n47eb30_bicycle_land(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8678, fighter_ready, 3);
}

static void n47eb30_bicycle_descend(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0xc4;
    native_await(fighter, n47eb30_bicycle_grounded, n47eb30_bicycle_land);
  } else {
    n47eb30_bicycle_land(fighter);
  }
}

static void n47eb30_bicycle_carry_end(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move == &n47eb30_s_bicycle_victim && victim->pending == NULL && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  mk4_script_stop_body(fighter);
  n47eb30_bicycle_descend(fighter);
}

/* The query rows end on landing, contact or their last visit. */
static void n47eb30_bicycle_contact_end(Fighter *fighter) {
  if (fighter->contact == CONTACT_BLOCK) {
    retail_vm_enter_owned(fighter, 0x4e8688, fighter_ready, 3);
    return;
  }
  if (fighter->contact != CONTACT_HIT) {
    n47eb30_bicycle_descend(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  liu_kang_throw_disarm(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &n47eb30_s_bicycle_victim);
  victim->sample_override = 0;
  victim->reaction = 84;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->flags |= FF_PAIRED;
  fighter->body.pos[AXIS_Y] = mini(fighter->body.pos[AXIS_Y], -0x15999);
  fighter_set_speed(fighter, 0x10a3);
  fighter->body.vel[AXIS_Y] = fighter->body.gravity = 0;
  n47eb30_bicycle_victim_script(victim);
  native_begin(fighter, 0, 57, n47eb30_bicycle_carry_end);
  for (int frame = 0; frame < 58; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, n47eb30_bicycle_pair);
  }
  native_play(fighter, false);
}

void liu_kang_bicycle_script(Fighter *fighter) {
  native_begin(fighter, 0, 59, n47eb30_bicycle_contact_end);
  for (int frame = 0; frame < 60; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, n47eb30_bicycle_query);
  }
  native_play(fighter, false);
}

bool liu_kang_can_bicycle(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action != ACTION_RETAIL_100B && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 120;
}

void liu_kang_bicycle_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_opponent(fighter)->action == ACTION_RETAIL_100B) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47eb30_s_bicycle);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = n47eb30_bicycle_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8610, fighter_ready, 3);
}

static void n47eb30_bicycle_motion_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

void liu_kang_liu_script_bicycle_motion(Fighter *fighter) {
  if (fighter->native_motion.binding != liu_kang_liu_script_bicycle_motion) {
    native_motion_begin(fighter, 16, NULL);
    for (int frame = 0; frame < 16; ++frame) {
      native_motion_frame(fighter, frame, n47eb30_bicycle_motion_pose, (float)(7 + (frame + 1) % 16), NULL);
    }
    fighter->native_motion.binding = liu_kang_liu_script_bicycle_motion;
    fighter->native_motion.loop = true;
    native_motion_play(fighter);
    fighter->body.physics = liu_kang_liu_script_bicycle_motion;
  }
  native_motion_step(fighter);
}

#undef RETAIL

/* Native service cluster: native_steal. */
/* HP [F,B] record 4e47f8 selects native 469c80: action 401, sound C[37],
 * clip 7 (53ce1c) played to sample 10 by 428d30. */
static const MoveDef native_steal_s_steal = {.clip = 7, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x401};
/* 469cea queries D[11]/4ec140 once, only against an armed victim. */
static const AttackRecord native_steal_steal_record = {0x4a3d7, -0x17851, 0x4a3d7, 0x1e147, 108, 108, 0, 0, 0x7ae};
static const MoveDef native_steal_s_stolen_ground = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION};
static const MoveDef native_steal_s_stolen_air = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION};

enum {
  native_steal_STEAL_WAIT_TICKS = 30, /* 469d1d loads 53c440 with 0x1e */
};

/* 484100: while the thief keeps action 401 the weapon homes at 0.15 along
 * the X/Z line to the thief; afterwards 494590 drops it normally. */
static void native_steal_stolen_weapon_step(Projectile *weapon) {
  const Fighter *thief = fighter_opponent(&g_battle.fighters[weapon->owner]);
  if (thief->action != 0x401) {
    weapon_object_settle(weapon);
    return;
  }
  double dx = (double)thief->body.pos[AXIS_X] - weapon->pos[AXIS_X];
  double dz = (double)thief->body.pos[AXIS_Z] - weapon->pos[AXIS_Z];
  double length = sqrt(dx * dx + dz * dz);
  if (length > 0) {
    weapon->vel[AXIS_X] = (q16)(dx / length * 0x2666);
    weapon->vel[AXIS_Z] = (q16)(dz / length * 0x2666);
  }
  weapon->pos[AXIS_X] += weapon->vel[AXIS_X];
  weapon->pos[AXIS_Z] += weapon->vel[AXIS_Z];
}

static Projectile *native_steal_stolen_weapon(const Fighter *thief) {
  int victim = (int)(fighter_opponent(thief) - g_battle.fighters);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SWORD && p->step == native_steal_stolen_weapon_step && p->owner == victim) {
      return p;
    }
  }
  return NULL;
}

static void native_steal_steal_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void native_steal_steal_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  native_hold(fighter, 16, native_steal_steal_ready);
}

static void native_steal_steal_wait(Fighter *fighter) {
  native_hold(fighter, 32, native_steal_steal_stop);
}

/* 469d7a: stop, play clip 7 out (428d10), wait 32, stop, wait 16, 46f650. */
static void native_steal_steal_fail(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  int pose = fighter_sample(fighter);
  if (pose >= native_steal_s_steal.last) {
    native_steal_steal_wait(fighter);
    return;
  }
  native_begin(fighter, pose + 1, native_steal_s_steal.last, native_steal_steal_wait);
  for (int frame = pose + 1; frame <= native_steal_s_steal.last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 469d2c: 49cb30 finds the type-70 weapon task; within squared distance
 * 0x8000, 49cb20 retires it and 46a180 equips it with sound C[38/39]. */
static void native_steal_steal_poll(Fighter *fighter) {
  Projectile *weapon = native_steal_stolen_weapon(fighter);
  if (weapon == NULL) {
    native_steal_steal_fail(fighter);
    return;
  }
  q16 dx = weapon->pos[AXIS_X] - fighter->body.pos[AXIS_X], dz = weapon->pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
  if (q16_mul(dx, dx) + q16_mul(dz, dz) >= 0x8000) {
    return;
  }
  const WeaponDef *def = weapon->weapon;
  weapon->kind = 0;
  fighter_equip_weapon(fighter, def);
  battle_sound(SOUND_C, fighter == &g_battle.fighters[0] ? 0x26 : 0x27);
  native_steal_steal_ready(fighter);
}

static void native_steal_steal_take(Fighter *fighter) {
  native_begin(fighter, 1, native_steal_STEAL_WAIT_TICKS, native_steal_steal_ready);
  for (int frame = 1; frame <= native_steal_STEAL_WAIT_TICKS; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), native_steal_steal_poll);
  }
  native_play(fighter, false);
}

/* 469caf: an armed victim and an unarmed thief allow the D[11] query. */
static void native_steal_steal_query(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  ContactResult result = victim->armed && !fighter->armed ? battle_contact(fighter, &native_steal_steal_record) : CONTACT_MISS;
  if (result == CONTACT_MISS || result == CONTACT_BLOCK) {
    native_steal_steal_fail(fighter);
  } else {
    native_steal_steal_take(fighter);
  }
}

void quan_chi_weapon_steal(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &native_steal_s_steal);
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 37);
  native_begin(fighter, 1, 10, native_steal_steal_query);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* Reaction 108 (484060): 493370 detaches the weapon from joint 10 into task
 * 70 (484100); 48ef30 then selects 4e9838 in the air or 4e9818 grounded. */
void quan_chi_steal_reaction(Fighter *victim) {
  if (!victim->armed) {
    fighter_ready(victim);
    return;
  }
  Projectile *weapon = battle_spawn_projectile(victim, PROJECTILE_SWORD, 600, native_steal_stolen_weapon_step);
  if (weapon != NULL) {
    weapon->weapon = victim->weapon;
    Vec3 hand;
    Mat3 unused;
    fighter_joint_position(victim, 10, &hand, &unused);
    weapon->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
    weapon->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
    weapon->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
  }
  victim->armed = false;
  bool airborne = fighter_airborne(victim);
  fighter_start_move(victim, airborne ? S_KNOCKDOWN : S_HIT, airborne ? &native_steal_s_stolen_air : &native_steal_s_stolen_ground);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (airborne) {
    retail_vm_enter_owned(victim, 0x4e9838, fighter_ready, 8);
  } else {
    retail_vm_enter_owned(victim, 0x4e9818, fighter_ready, 8);
  }
}

/* Native service cluster: 486000, 487420, 49a680, 49a6a0. */
static bool n486000_spear_not_pulling(const Fighter *fighter);
static void n486000_spear_cleanup(Fighter *fighter);
static void n486000_spear_projectile(Projectile *p);
static void n486000_spear_spawn(Fighter *fighter);
static const MoveDef n486000_s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0300};
static const MoveDef n486000_s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4000};
static const MoveDef n486000_s_spear_stun = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1008};

void scorpion_spear_stun_script(Fighter *fighter) {
  retail_vm_wait_native(fighter, 64, scorpion_special_ready);
}

static bool n486000_spear_not_pulling(const Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SPEAR && &g_battle.fighters[p->owner] == fighter && p->phase == 1) {
      return false;
    }
  }
  return true;
}

/* Runs when the spear task is cancelled or completes. Pull
 * arrival installs stun before replacing the owner's move, so that victim
 * is already released from s_snared and must retain its follow-up opening. */
static void n486000_spear_cleanup(Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind != PROJECTILE_SPEAR || &g_battle.fighters[p->owner] != fighter) {
      continue;
    }
    bool pulling = p->phase == 1;
    p->kind = 0;
    Fighter *victim = fighter_peer(fighter);
    if (pulling && fighter_move(victim)->clip == 199) {
      scorpion_special_ready(victim);
    }
  }
}

static void n486000_spear_projectile(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner], *victim = fighter_peer(owner);
  bool interrupted = fighter_move(owner)->clip != 14 || !owner->retail_vm.active || owner->pending != NULL;
  if (interrupted || g_battle.contacts_off) {
    if (p->phase == 1 && fighter_move(victim)->clip == 199) {
      scorpion_special_ready(victim);
    }
    p->kind = 0;
    return;
  }
  if (p->phase == 1) {
    if (fighter_move(victim)->clip != 199 || victim->health <= 0) {
      p->kind = 0;
      return;
    }
    /* 49b170/487350: pull along the throw direction until the pair is
     * within 0.8. */
    if (fighter_distance(owner) <= q16_float(0xcccc)) {
      mk4_script_stop_body(victim);
      fighter_start_move(victim, S_HIT, &n486000_s_spear_stun);
      victim->sample_override = 0;
      victim->flags |= FF_REACTING;
      retail_vm_enter_owned(victim, 0x4e9f20, fighter_ready, 0);
      fighter_start_move(owner, S_PUNCH, &n486000_s_spear);
      owner->sample_override = 31;
      retail_vm_enter_owned(owner, 0x4ed4a8, fighter_ready, 0);
      owner->retail_vm.cleanup = n486000_spear_cleanup;
      p->kind = 0;
    } else {
      victim->body.pos[AXIS_X] -= q16_mul(p->heading[0], 0x3333);
      victim->body.pos[AXIS_Z] -= q16_mul(p->heading[1], 0x3333);
      victim->body.pos[AXIS_Y] = victim->body.ground;
      memcpy(p->pos, victim->body.pos, sizeof p->pos);
    }
    return;
  }
  if (p->phase == 2) {
    double dx = owner->body.pos[AXIS_X] - p->pos[AXIS_X], dz = owner->body.pos[AXIS_Z] - p->pos[AXIS_Z];
    double distance = sqrt(dx * dx + dz * dz);
    if (distance <= 0x3333) {
      p->kind = 0;
    } else {
      p->pos[AXIS_X] += (q16)lround(dx / distance * 0x3333);
      p->pos[AXIS_Z] += (q16)lround(dz / distance * 0x3333);
    }
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  AttackRecord record = retail_vm_attack_record(retail_vm_image(), 0x4ebeb8);
  ContactResult result = battle_contact_from(owner, &record, p->pos, p->facing);
  if ((result == CONTACT_HIT || result == CONTACT_AIR_HIT) && victim->health <= 0) {
    /* Retain the lethal contact's ordinary reaction; no live pull may park
     * the defeated controller after the round retires projectile objects. */
    p->kind = 0;
  } else if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_HIT, &n486000_s_snared);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    retail_vm_enter_owned(victim, 0x4e9d00, fighter_ready, 0);
    victim->damage_scale = 0x9999; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || hypot(p->pos[AXIS_X], p->pos[AXIS_Z]) > g_battle.arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

static void n486000_spear_spawn(Fighter *fighter) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SPEAR, 90, n486000_spear_projectile);
  if (p != NULL) {
    /* 49a6a0 launches 0.75 ahead along the owner's heading at 0.2/tick. */
    p->pos[AXIS_X] += q16_mul(p->heading[0], 0xc000);
    p->pos[AXIS_Z] += q16_mul(p->heading[1], 0xc000);
    p->pos[AXIS_Y] = -0x16666;
    p->vel[AXIS_X] = q16_mul(p->heading[0], 0x3333);
    p->vel[AXIS_Z] = q16_mul(p->heading[1], 0x3333);
  }
}

/* 4ed448/4ed47c -> 4ed470/4ed498. Normal/fast launch is at tick 10/8;
 * sample 30 follows at 15/11. Keep the existing paired-pull policy: arrival
 * replaces this program with the 4ed4a8 return, while cancellation cleans up. */

bool scorpion_can_spear(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->spear_ready && victim->hits <= 3 && victim->action != ACTION_RETAIL_1008;
}

void scorpion_spear(Fighter *fighter) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &n486000_s_spear);
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = g_battle.ticks + 90;
  retail_vm_enter_owned(fighter, 0x4ed448, scorpion_script_spear_done, 0);
  fighter->retail_vm.cleanup = n486000_spear_cleanup;
}

void scorpion_script_spear_done(Fighter *fighter) {
  if (n486000_spear_not_pulling(fighter)) {
    scorpion_special_ready(fighter);
  } else {
    retail_vm_wait_native(fighter, 1, scorpion_script_spear_done);
  }
}

void scorpion_script_spear_return(Fighter *fighter) {
  fighter->spear_ready = g_battle.ticks + 90;
  scorpion_special_ready(fighter);
}

void scorpion_script_spawn_spear(Fighter *fighter, int type) {
  require(type == 67, "Unexpected spear task");
  n486000_spear_spawn(fighter);
}

/* Native service cluster: native_bicycle. */
static bool native_bicycle_bicycle_height(const Fighter *fighter);
static void native_bicycle_bicycle_contact(Fighter *fighter);
static void native_bicycle_bicycle_done(Fighter *fighter);
static void native_bicycle_bicycle_fall(Fighter *fighter);
static void native_bicycle_bicycle_spin(Fighter *fighter);
static void native_bicycle_bicycle_script(Fighter *fighter);
static const MoveDef native_bicycle_s_bicycle = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x205};
static const AttackRecord native_bicycle_bicycle_record = {80609, -1310, 62259, 20971, 86, 86, 0, 0, 9830};

static bool native_bicycle_bicycle_height(const Fighter *fighter) {
  return fighter_height(fighter) >= 2.5f;
}

static void native_bicycle_bicycle_contact(Fighter *fighter) {
  if (battle_contact(fighter, &native_bicycle_bicycle_record) != CONTACT_MISS) {
    native_bicycle_bicycle_fall(fighter);
  }
}

static void native_bicycle_bicycle_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void native_bicycle_bicycle_fall(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->body.gravity = 0x28f;
  native_await(fighter, native_grounded, native_bicycle_bicycle_done);
}

static void native_bicycle_bicycle_spin(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_mmove_bind(fighter, 101, 9, 0);
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 0, 8, native_bicycle_bicycle_fall);
  for (int frame = 0; frame < 9; ++frame) {
    native_frame(fighter, frame, frame, native_bicycle_bicycle_contact);
  }
  native_play(fighter, false);
}

static void native_bicycle_bicycle_script(Fighter *fighter) {
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = -0x2666;
  battle_sound(SOUND_A, 6);
  battle_sound(SOUND_INDEXED, 12);
  native_await(fighter, native_bicycle_bicycle_height, native_bicycle_bicycle_spin);
}

void sonya_bicycle(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &native_bicycle_s_bicycle);
  fighter->mmove_cleanup = sonya_special_cleanup;
  native_bicycle_bicycle_script(fighter);
}

/* Native service cluster: 47b6c0. */
static void n47b6c0_cartwheel_done(Fighter *fighter);
static void n47b6c0_cartwheel_stop(Fighter *fighter);
static void n47b6c0_cartwheel_recover(Fighter *fighter);
static const MoveDef n47b6c0_s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};
static const AttackRecord n47b6c0_cartwheel_record = {75300, -45088, 72083, 89063, 121, 121, 9, 0, 4587};

static void n47b6c0_cartwheel_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void n47b6c0_cartwheel_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    native_hold(fighter, 20, n47b6c0_cartwheel_done);
  } else {
    n47b6c0_cartwheel_done(fighter);
  }
}

static void n47b6c0_cartwheel_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_finish_clip(fighter, n47b6c0_cartwheel_stop);
}

void sonya_script_cartwheel_tail(Fighter *fighter) {
  fighter->cartwheel_ready = g_battle.ticks + 48;
  native_query_rows(fighter, &n47b6c0_cartwheel_record, 9, 1, n47b6c0_cartwheel_recover);
}

void sonya_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &n47b6c0_s_cartwheel);
  fighter->mmove_cleanup = sonya_special_cleanup;
  retail_vm_enter_owned(fighter, 0x4e81d0, fighter_ready, 2);
}

bool sonya_can_cartwheel(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->cartwheel_ready;
}

/* Native service cluster: 47e0a0, 4858d0. */
static void n47e0a0_leg_stop(Fighter *fighter);
static void n47e0a0_leg_grab_done(Fighter *fighter);
static void n47e0a0_leg_grab_contact(Fighter *fighter);
static void n47e0a0_leg_grab_script(Fighter *fighter);
static const MoveDef n47e0a0_s_leg_grab = {.clip = 102, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x203};
static const MoveDef n47e0a0_s_leg_victim = {.clip = 201, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord n47e0a0_leg_record = {92405, -29491, 85852, 76677, 88, 88, 8, 1, 11796};

void sonya_script_leg_victim_end(Fighter *victim) {
  mk4_mmove_bind(victim, 185, 16, 0);
  retail_getup_roll(victim);
}

static void n47e0a0_leg_grab_done(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter_finish_mmove(fighter, fighter_ready);
}

void sonya_script_leg_grab_tail(Fighter *fighter) {
  native_finish_clip(fighter, n47e0a0_leg_grab_done);
}

static void n47e0a0_leg_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
}

static void n47e0a0_leg_grab_contact(Fighter *fighter) {
  if (battle_contact(fighter, &n47e0a0_leg_record) == CONTACT_MISS || fighter->contact == CONTACT_BLOCK) {
    fighter->action = 0x1012;
    retail_vm_enter_owned(fighter, 0x4e85a8, fighter_ready, 2);
  } else {
    mk4_mmove_pair_victim(fighter, &n47e0a0_s_leg_victim);
    retail_vm_enter_owned(fighter, 0x4e8578, fighter_ready, 2);
  }
}

static void n47e0a0_leg_grab_script(Fighter *fighter) {
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 1, 11, n47e0a0_leg_grab_contact);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, frame, frame == 7 ? n47e0a0_leg_stop : NULL);
  }
  native_play(fighter, false);
}

void sonya_leg_grab(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter->leg_grab_ready = g_battle.ticks + 64;
  fighter_start_move(fighter, S_KICK, &n47e0a0_s_leg_grab);
  battle_sound(SOUND_A, 9);
  battle_sound(SOUND_INDEXED, 15);
  fighter->mmove_cleanup = sonya_special_cleanup;
  n47e0a0_leg_grab_script(fighter);
}

bool sonya_can_leg_grab(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->leg_grab_ready && (fighter->held & mask) == chord;
}

/* Native service cluster: 47dae0. */
/* LK F,F record 4e4550 -> T4[30] 479520: action 21b, clip 109 through
 * 428750/53cf40, speed .15 and the 4797c0 cursor callback, which loops
 * samples 14..28 after the first pass. */
static const MoveDef n47dae0_2_spin = {.clip = 109, .last = 28, .flags = SF_LOCK | SF_RECORD, .action = 0x21b};
/* D[28] 4ebfc0: 12%, hit/air 158, block 3. */
static const AttackRecord n47dae0_2_spin_record = {0x12dd2, 0xf5c, 0x16872, 0x6dd2, 158, 158, 3, 0, 0x1eb8};

/* 45e8d0: at least 60 ticks since stamp 0xd and an opponent combo count
 * of at most four (45ebb0/45f5b0). */
bool tanya_can_spin_kick(const Fighter *fighter) {
  return g_battle.ticks >= fighter->spin_kick_ready && fighter_opponent(fighter)->hits <= 4;
}

static int n47dae0_2_spin_next(int pose) {
  return pose + 1 >= 29 ? 14 : pose + 1;
}

static void n47dae0_2_spin_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  bool airborne = fighter_airborne(fighter);
  mk4_script_stop_body(fighter);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  /* An interrupted recoil retains a fall; the replacing move owns landing. */
  if (airborne) {
    fighter->body.gravity = 0x28f;
  }
}

static void n47dae0_2_spin_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 479705: clear the cursor callback and motion, set no-guard, bind
 * sample 10 and let 429600 rewind one sample per visit to 0. */
static void n47dae0_2_spin_recover(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->sample_override = 10;
  native_begin(fighter, 0, 9, n47dae0_2_spin_ready);
  for (int frame = 0; frame <= 9; ++frame) {
    native_frame(fighter, frame, 9 - frame, NULL);
  }
  native_play(fighter, false);
}

static void n47dae0_2_recoil_wait(Fighter *fighter) {
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
static void n47dae0_2_spin_blocked(Fighter *fighter) {
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
  native_frame(fighter, 0, 8, n47dae0_2_recoil_wait);
  native_play(fighter, false);
}

static void n47dae0_2_spin_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &n47dae0_2_spin_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    n47dae0_2_spin_blocked(fighter);
    return;
  }
  /* An unblocked contact keeps spinning and moving for twelve visits. */
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, n47dae0_2_spin_recover);
  for (int frame = 1; frame <= 12; ++frame) {
    pose = n47dae0_2_spin_next(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}

static void n47dae0_2_spin_sounds(Fighter *fighter) {
  mk4_script_indexed_sound(fighter, 0x35);
  battle_sound(SOUND_C, 0x57);
}

/* Eight visits of startup, sounds, then fifteen D[28] queries. */
void tanya_spin_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &n47dae0_2_spin);
  fighter->mmove_cleanup = n47dae0_2_spin_cleanup;
  fighter->spin_kick_ready = g_battle.ticks + 60;
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x2666);
  native_begin(fighter, 1, 23, n47dae0_2_spin_recover);
  int pose = 0;
  for (int frame = 1; frame <= 23; ++frame) {
    pose = n47dae0_2_spin_next(pose);
    native_frame(fighter, frame, pose, frame == 8 ? n47dae0_2_spin_sounds : frame > 8 ? n47dae0_2_spin_query : NULL);
  }
  native_play(fighter, false);
}

#include "native_services.h"
#include <math.h>
#include <stdlib.h>

/* Named byte addresses of native attack records, checked against the local EXE.
 * Decode the original data through the VM; do not keep copied balance tables. */
enum {
  GROIN_PUNCH_ATTACK_RECORD = 0x4ec008,
  SHADOW_ATTACK_RECORD = 0x4ebfd8,
  POWER_ATTACK_RECORD = 0x4ebff0,
  TRIPLE_ATTACK_RECORD = 0x4ec170,
  PUNCH_ATTACK_RECORD = 0x4ec230,
  KICK_ATTACK_RECORD = 0x4ec248,
  UPPERCUT_ATTACK_RECORD = 0x4ec278,
  STOMP_ATTACK_RECORD = 0x4ec260,
  DASH_ATTACK_RECORD = 0x4ec050,
  AERIAL_ATTACK_RECORD = 0x4ec0e0,
  BICYCLE_ATTACK_RECORD = 0x4ebc60,
  WEAPON_STEAL_ATTACK_RECORD = 0x4ec140,
  AERIAL_BICYCLE_ATTACK_RECORD = 0x4ec1d0,
  CARTWHEEL_ATTACK_RECORD = 0x4ec0b0,
  LEG_ATTACK_RECORD = 0x4ebf78,
  SPIN_ATTACK_RECORD = 0x4ebfc0
};

/* Native service cluster: native_groin_punch. */
static void groin_punch_choose(Fighter *fighter);
static void groin_punch_query(Fighter *fighter);
static void groin_punch_recover(Fighter *fighter);
static void groin_punch_victim(Fighter *victim);
static const MoveDef groin_punch_move = {.clip = 438, .last = 14, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x110};
static const MoveDef groin_punch_stun = {.clip = 126, .last = 14, .flags = SF_LOCK | SF_REACTION, .action = 0x1014};

bool cage_can_groin_punch(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_BLOCK);
  unsigned excluded = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) |
                      BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && !fighter->armed && !fighter_airborne(fighter) && (fighter->held & chord) == chord && !(fighter->held & excluded) &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_PUNCH] <= 4 && g_battle.ticks - fighter->latch[BUTTON_BLOCK] <= 4;
}

static void groin_punch_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

void cage_groin_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &groin_punch_move);
  fighter->execution.cleanup = groin_punch_cleanup;
  battle_sound(SOUND_C, 9);
  native_begin(fighter, 1, 5, groin_punch_choose);
  for (int sample = 1; sample <= 5; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

static void groin_punch_victim(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &groin_punch_stun);
  victim->flags |= FF_REACTING;
  victim->body.physics = fighter_friction;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_GROIN_PUNCH_GROIN_VICTIM, mk4_script_thrown_ready);
}

static void groin_punch_contact(Fighter *fighter) {
  AttackRecord groin_punch_record = retail_vm_attack_record(battle_data(), GROIN_PUNCH_ATTACK_RECORD);

  ContactResult result = battle_contact(fighter, &groin_punch_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result != CONTACT_BLOCK) {
    fighter_interrupt((Fighter *)fighter_opponent(fighter), groin_punch_victim);
  }
  groin_punch_recover(fighter);
}

static void groin_punch_query(Fighter *fighter) {
  native_begin(fighter, 1, 3, groin_punch_recover);
  native_frame(fighter, 1, 12, groin_punch_contact);
  native_frame(fighter, 2, 13, groin_punch_contact);
  native_frame(fighter, 3, 14, NULL);
  native_play(fighter, false);
  /* Native468b8a queries D[27] at11 before advancing the next two poses. */
  groin_punch_contact(fighter);
}

static void groin_punch_recover(Fighter *fighter) {
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
  native_script_end(fighter, MK4_RETAIL_SCRIPT_GROIN_PUNCH_GROIN_RECOVER, fighter_ready);
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

static void groin_punch_avoid_reverse(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, pose, fighter_ready);
  for (int frame = 1; frame <= pose; ++frame) {
    native_frame(fighter, frame, pose - frame, NULL);
  }
  native_play(fighter, false);
}

static void groin_punch_choose(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  if (roster == 2 || roster == 9 || victim->action == 0x1014) {
    native_hold(fighter, 20, groin_punch_avoid_reverse);
    return;
  }
  native_begin(fighter, 6, 11, groin_punch_query);
  for (int sample = 6; sample <= 11; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 479b70. */
static const MoveDef cage_kicks_s_cpu_kick_script = {.clip = 308, .last = 24, .flags = SF_LOCK | SF_RECORD};

void cage_cpu_kick(Fighter *fighter);

/* Native479b70 queries D[25] at poses22/23, then holds24 for twelve
 * contact visits while descriptor4e80c8 installs47bf50 velocity decay.
 * The descriptor is data, never the adjacent powered-punch script. */

static void cage_kicks_shadow_ready(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_execution_finish(fighter, fighter_ready);
}

static void cage_kicks_shadow_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_hold(fighter, 10, cage_kicks_shadow_ready);
}

static void cage_kicks_shadow_contact(Fighter *fighter) {
  AttackRecord cage_kicks_shadow_record = retail_vm_attack_record(battle_data(), SHADOW_ATTACK_RECORD);

  if (fighter_sample(fighter) == 24) {
    fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
    fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
  }
  if (battle_contact(fighter, &cage_kicks_shadow_record) != CONTACT_MISS) {
    mk4_script_stop_body(fighter);
    fighter->sample_override = 24;
    native_hold(fighter, 10, cage_kicks_shadow_ready);
  }
}

void cage_script_cpu_kick_tail(Fighter *fighter) {
  native_begin(fighter, 22, 35, cage_kicks_shadow_recover);
  for (int frame = 22; frame <= 35; ++frame) {
    native_frame(fighter, frame, frame < 24 ? frame : 24, cage_kicks_shadow_contact);
  }
  native_play(fighter, false);
}

void cage_shadow_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_SHADOW_VICTIM, fighter_ready);
}

void cage_cpu_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &cage_kicks_s_cpu_kick_script);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_CPU_KICK, fighter_ready);
}

/* Native service cluster: 479f00, 47dae0. */
static const MoveDef cage_powered_punch_s_powered_punch_script = {.clip = 85, .last = 54, .flags = SF_LOCK | SF_RECORD};

/* Native479f00 D[26], followed by two native47a280 paired cycles.
 * 4e80f4 is a physics callback descriptor, not the adjacent105 kick. */

static void cage_powered_punch_power_cycle(Fighter *fighter);
static void cage_powered_punch_power_recover(Fighter *fighter);
static void cage_powered_punch_power_victim_fall(Fighter *fighter);

static void cage_powered_punch_power_release(Fighter *fighter) {
  if (fighter->native_followup) {
    Fighter *victim = (Fighter *)fighter_opponent(fighter);
    fighter->native_followup = false;
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    if ((retail_reaction_native(victim) == RETAIL_NATIVE_CAGE_POWER_VICTIM || retail_reaction_native(victim) == RETAIL_NATIVE_FUJIN_TRIPLE_MIDDLE_VICTIM) &&
        fighter_airborne(victim)) {
      fighter_interrupt(victim, cage_powered_punch_power_victim_fall);
    }
  }
}

static void cage_powered_punch_power_victim_ground(Fighter *fighter) {
  fighter->execution.hold = fighter_airborne(fighter);
}

static void cage_powered_punch_power_victim_fall(Fighter *fighter) {
  static const MoveDef first = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  static const MoveDef middle = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  bool second = retail_reaction_native(fighter) == RETAIL_NATIVE_FUJIN_TRIPLE_MIDDLE_VICTIM;
  int pose = fighter_sample(fighter);
  bool inherited = fighter_move(fighter)->clip == (second ? 311 : 127);
  fighter_start_move(fighter, S_HIT, second ? &middle : &first);
  fighter->flags |= FF_REACTING;
  fighter->body.gravity = 0x51e;
  fighter->body.on_land = NULL;
  /* Replace any queued capture entry as well as the current held phase.
   * Otherwise its initial stop-motion would undo interruption's gravity. */
  native_begin(fighter, 0, 0, mk4_mmove_thrown_finish);
  native_frame(fighter, 0, inherited ? pose : 0, cage_powered_punch_power_victim_ground);
  native_play(fighter, false);
}

static void cage_powered_punch_power_cleanup(Fighter *fighter) {
  cage_powered_punch_power_release(fighter);
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

static void cage_powered_punch_power_ready(Fighter *fighter) {
  bool airborne = fighter_airborne(fighter);
  q16 velocity[3], gravity = fighter->body.gravity;
  memcpy(velocity, fighter->body.vel, sizeof velocity);
  fighter_execution_finish(fighter, fighter_ready);
  /* Native46f650 restores air control without cancelling the fall. The
   * interrupted-move cleanup still owns stopping a replaced attack. */
  if (airborne) {
    memcpy(fighter->body.vel, velocity, sizeof velocity);
    fighter->body.gravity = gravity;
    fighter->body.on_land = mk4_land;
  }
}

static void cage_powered_punch_power_finish(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, first, last, cage_powered_punch_power_ready);
  for (int pose = first; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void cage_powered_punch_power_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void cage_powered_punch_power_recover(Fighter *fighter) {
  cage_powered_punch_power_release(fighter);
  fighter->body.physics = cage_powered_punch_power_decay;
  int first = fighter_sample(fighter);
  native_begin(fighter, first, 29, cage_powered_punch_power_finish);
  for (int pose = first; pose <= 29; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void cage_powered_punch_power_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (fighter->native_variant++ == 0) {
    battle_react(victim, 94);
    cage_powered_punch_power_cycle(fighter);
  } else {
    battle_react(victim, 95);
    fighter->body.gravity = 0x51e;
    cage_powered_punch_power_recover(fighter);
  }
}

static void cage_powered_punch_power_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, cage_powered_punch_power_cycle_done);
}

static void cage_powered_punch_power_cycle(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y] - 0x9999;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x4ccc);
  }
  memcpy(fighter->saved_motion, fighter->body.vel, sizeof fighter->saved_motion);
  mk4_script_stop_body(fighter);
  native_hold(fighter, 4, cage_powered_punch_power_restore);
}

static void cage_powered_punch_power_rewind(Fighter *fighter) {
  if (fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000) {
    fighter->execution.hold = true;
    return;
  }
  fighter->execution.hold = false;
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_POWER_REWIND, fighter_ready);
}

static void cage_powered_punch_power_blocked(Fighter *fighter) {
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
  native_frame(fighter, 0, 0, cage_powered_punch_power_rewind);
  native_play(fighter, false);
}

static void cage_powered_punch_power_query(Fighter *fighter) {
  AttackRecord cage_powered_punch_power_record = retail_vm_attack_record(battle_data(), POWER_ATTACK_RECORD);

  ContactResult result = battle_contact(fighter, &cage_powered_punch_power_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    cage_powered_punch_power_blocked(fighter);
  } else {
    fighter->native_followup = true;
    fighter->native_variant = 0;
    fighter->flags |= FF_PAIRED;
    cage_powered_punch_power_cycle(fighter);
  }
}

static void cage_powered_punch_power_miss(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  cage_powered_punch_power_recover(fighter);
}

void cage_script_powered_punch_tail(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = -0x1eb8;
  fighter->body.gravity = q16_mul(0x1999, -0x28f);
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 5, cage_powered_punch_power_miss);
  for (int frame = 0; frame < 6; ++frame) {
    native_frame(fighter, frame, 24, cage_powered_punch_power_query);
  }
  native_play(fighter, false);
}

void cage_power_victim(Fighter *fighter) {
  require(retail_reaction_native(fighter) == RETAIL_NATIVE_CAGE_POWER_VICTIM, "Invalid Cage powered-punch victim entry");
  static const MoveDef victim = {.clip = 127, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_HIT, &victim);
  fighter->flags |= FF_REACTING;
  mk4_script_effect(fighter, 15);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_POWER_VICTIM, fighter_ready);
}

void cage_powered_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &cage_powered_punch_s_powered_punch_script);
  fighter->native_followup = false;
  fighter->execution.cleanup = cage_powered_punch_power_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_POWERED_PUNCH, fighter_ready);
}

/* Native service cluster: native_punches. */
void cage_powered_punch(Fighter *fighter);

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */

/* Native service cluster: 47f060, 47f070. */
static const MoveDef fujin_triple_kick_triple = {.clip = 100, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x208};
static const MoveDef fujin_triple_kick_middle_victim = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};

static void fujin_triple_kick_triple_recover(Fighter *fighter);
static void fujin_triple_kick_triple_cycle(Fighter *fighter);

static void fujin_triple_kick_triple_release(Fighter *fighter) {
  if (!fighter->native_followup) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_followup = false;
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  /* An interrupted owner cannot leave a captured intermediate victim
   * hovering at the paired -.3 offset with its old zero gravity. */
  if ((retail_reaction_native(victim) == RETAIL_NATIVE_FUJIN_TRIPLE_FIRST_VICTIM ||
          retail_reaction_native(victim) == RETAIL_NATIVE_FUJIN_TRIPLE_MIDDLE_VICTIM) &&
      fighter_airborne(victim)) {
    victim->body.gravity = 0x51e;
  }
}

static void fujin_triple_kick_triple_cleanup(Fighter *fighter) {
  fujin_triple_kick_triple_release(fighter);
  mk4_script_stop_body(fighter);
}

static void fujin_triple_kick_triple_to(Fighter *fighter, int target, FighterFn end) {
  int first = fighter_sample(fighter);
  require(first <= target && target <= fighter_move(fighter)->last, "Invalid Fujin triple-kick native playback range");
  native_begin(fighter, first, target, end);
  for (int pose = first; pose <= target; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void fujin_triple_kick_triple_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fujin_triple_kick_triple_to(fighter, 44, mk4_mmove_thrown_finish);
}

static void fujin_triple_kick_triple_recover(Fighter *fighter) {
  fujin_triple_kick_triple_release(fighter);
  /* 47f280 reaches21 with the existing motion, then stops and finishes. */
  fujin_triple_kick_triple_to(fighter, 21, fujin_triple_kick_triple_finish);
}

static void fujin_triple_kick_triple_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  if (++fighter->native_variant < 3) {
    battle_react((Fighter *)fighter_opponent(fighter), fighter->native_variant == 1 ? 94 : 95);
    fujin_triple_kick_triple_cycle(fighter);
  } else {
    fujin_triple_kick_triple_recover(fighter);
  }
}

static void fujin_triple_kick_triple_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, fujin_triple_kick_triple_cycle_done);
}

static void fujin_triple_kick_triple_cycle(Fighter *fighter) {
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
  native_hold(fighter, 4, fujin_triple_kick_triple_restore);
}

static void fujin_triple_kick_triple_floor_hold(Fighter *fighter) {
  fighter->execution.hold = fighter_airborne(fighter);
}

static void fujin_triple_kick_triple_land(Fighter *fighter) {
  fighter_execution_finish(fighter, mk4_land);
}

static void fujin_triple_kick_triple_blocked(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  /* 47f480 ->490890 selects the first4e8790 recoil vector. */
  fighter_set_speed(fighter, -0x51e);
  fighter->body.vel[AXIS_Y] = -0x1999;
  fighter->body.gravity = 0x147;
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 0, fujin_triple_kick_triple_land);
  native_frame(fighter, 0, fighter_sample(fighter), fujin_triple_kick_triple_floor_hold);
  native_play(fighter, false);
}

static void fujin_triple_kick_triple_query(Fighter *fighter) {
  AttackRecord fujin_triple_kick_triple_record = retail_vm_attack_record(battle_data(), TRIPLE_ATTACK_RECORD);

  ContactResult result = battle_contact(fighter, &fujin_triple_kick_triple_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    fujin_triple_kick_triple_blocked(fighter);
  } else {
    fighter->native_variant = 0;
    fighter->native_followup = true;
    fighter->flags |= FF_PAIRED;
    fujin_triple_kick_triple_cycle(fighter);
  }
}

void fujin_triple_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 100 && fighter_sample(fighter) == 13, "Fujin triple kick needs its retail 13-pose startup");
  /* 47f070 advances before querying D[9], four times at14..17. */
  native_begin(fighter, 1, 4, fujin_triple_kick_triple_recover);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, 13 + frame, fujin_triple_kick_triple_query);
  }
  native_play(fighter, false);
}

void fujin_triple_fast(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_TRIPLE_FAST, fighter_ready);
}

void fujin_triple_victim(Fighter *fighter) {
  if (retail_reaction_native(fighter) == RETAIL_NATIVE_FUJIN_TRIPLE_MIDDLE_VICTIM) {
    fighter_start_move(fighter, S_HIT, &fujin_triple_kick_middle_victim);
    fighter->flags |= FF_REACTING;
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_TRIPLE_VICTIM, mk4_mmove_thrown_finish);
  } else {
    require(retail_reaction_native(fighter) == RETAIL_NATIVE_FUJIN_TRIPLE_FIRST_VICTIM, "Invalid Fujin triple-kick victim stage");
    /* Native488420/488440 and488830/488850 are also the exact
     * first/second axe victim entries; reuse their shared adaptations. */
    weapon_axe_reaction(fighter, false, (RetailScriptContinuation){MK4_RETAIL_SCRIPT_CAGE_POWER_VICTIM, mk4_mmove_thrown_finish});
  }
}

void fujin_triple_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &fujin_triple_kick_triple);
  fighter->native_followup = false;
  fighter->execution.cleanup = fujin_triple_kick_triple_cleanup;
  battle_sound(SOUND_C, 0x42);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_TRIPLE_KICK, fighter_ready);
}

/* Native service cluster: 4803a0. */
static const MoveDef fujin_wind_grab_wind_grab = {.clip = 10, .last = 2, .flags = SF_LOCK | SF_RECORD, .action = 0x20a};
static const MoveDef fujin_wind_grab_orbit_victim = {.clip = 8, .last = 28, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x100d};
static const MeshAnimation fujin_wind_grab_grab_meshes[] = {
    {.first = 356, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
    {.first = 350, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
};
static void fujin_wind_grab_grab_loop(Fighter *fighter);

bool fujin_can_wind_grab(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->wind_grab_ready && victim->hits <= 2 && victim->action != 0x1001 &&
         victim->action != 0x100d;
}

static void fujin_wind_grab_grab_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

static void fujin_wind_grab_orbit_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
}

static void fujin_wind_grab_grab_return(Fighter *fighter) {
  fighter_execution_finish(fighter, mk4_mmove_thrown_finish);
}

static void fujin_wind_grab_grab_stamp_and_return(Fighter *fighter) {
  fighter->wind_grab_ready = g_battle.ticks + 90;
  fujin_wind_grab_grab_return(fighter);
}

static void fujin_wind_grab_grab_effect_step(Projectile *effect) {
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

static void fujin_wind_grab_grab_effect_start(Fighter *fighter) {
  /* 4146d0 creates two wind groups at chain offsets0/6 from4d1a18.
   * Keep their original shared surfaces in the existing bounded object pool. */
  for (int i = 0; i < 2; ++i) {
    Projectile *effect = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 6000, fujin_wind_grab_grab_effect_step);
    if (effect != NULL) {
      effect->mesh_animation = &fujin_wind_grab_grab_meshes[i];
      effect->initial_mesh = fujin_wind_grab_grab_meshes[i].first;
      memset(effect->angle, 0, sizeof effect->angle);
    }
  }
}

static void fujin_wind_grab_grab_damage(Fighter *fighter) {
  fighter->action = 0;
  mk4_script_damage(fighter, 0x1eb8);
  fighter->wind_grab_ready = g_battle.ticks + 90;
  native_hold(fighter, 14, fujin_wind_grab_grab_return);
}

static void fujin_wind_grab_grab_close(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(fighter);
  if (battle_blocks_attack(victim, 0)) {
    fighter->action = 0x1015;
    battle_react(victim, 7);
    fighter->flags |= FF_NO_GUARD;
    mk4_mmove_bind(fighter, 13, 56, 0);
    native_hold(fighter, 60, fujin_wind_grab_grab_stamp_and_return);
  } else {
    mk4_script_context(fighter, 5);
    fighter->action = 0x1000;
    battle_react(victim, 98);
    battle_sound(SOUND_C, 0x3e);
    native_hold(fighter, 51, fujin_wind_grab_grab_damage);
  }
}

static void fujin_wind_grab_grab_control(Fighter *fighter) {
  if (fighter_distance(fighter) < 1.f) {
    fujin_wind_grab_grab_close(fighter);
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
    native_hold(fighter, 12, fujin_wind_grab_grab_stamp_and_return);
  }
}

static void fujin_wind_grab_grab_sound_and_control(Fighter *fighter) {
  battle_sound(SOUND_C, 0x3e);
  fujin_wind_grab_grab_control(fighter);
}

static void fujin_wind_grab_grab_loop(Fighter *fighter) {
  /* Native480730 first sounds after20 visits, then every70. The loop
   * clock is encoded in authored rows; holding LP supplies its exit gate. */
  native_begin(fighter, 0, 69, fujin_wind_grab_grab_loop);
  for (int frame = 0; frame < 70; ++frame) {
    native_frame(fighter, frame, 0, frame == 19 ? fujin_wind_grab_grab_sound_and_control : fujin_wind_grab_grab_control);
  }
  native_play(fighter, false);
}

void fujin_grab_control_start(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 11 && fighter_sample(fighter) == 0, "Wind-grab control needs its retail 11-pose entry");
  fujin_wind_grab_grab_loop(fighter);
}

static q16 fujin_wind_grab_angle_wrap(q16 angle) {
  angle %= 0x6487e;
  return angle < 0 ? angle + 0x6487e : angle;
}

static q16 fujin_wind_grab_direction_angle(q16 x, q16 z) {
  return fujin_wind_grab_angle_wrap((q16)llround(atan2((double)z, (double)x) * Q16_ONE));
}

static void fujin_wind_grab_orbit_place(Fighter *victim, const Fighter *owner, q16 angle, q16 radius) {
  double radians = q16_float(angle);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + (q16)llround(cos(radians) * radius);
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z] + (q16)llround(sin(radians) * radius);
}

static void fujin_wind_grab_orbit_release(Fighter *victim, const Fighter *owner, q16 angle) {
  fujin_wind_grab_orbit_place(victim, owner, angle, victim->saved_motion[1]);
  victim->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
  q16 x = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
  q16 z = owner->body.pos[AXIS_Z] - victim->body.pos[AXIS_Z];
  double length = hypot((double)x, (double)z);
  victim->facing = x >= 0 ? 1 : -1;
  if (length > 0) {
    victim->heading[0] = (q16)llround(x * Q16_ONE / length);
    victim->heading[1] = (q16)llround(z * Q16_ONE / length);
  }
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_ORBIT_RELEASE, fighter_ready);
}

static void fujin_wind_grab_orbit_step(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  q16 angle = victim->saved_motion[0], radius = victim->saved_motion[1];
  fujin_wind_grab_orbit_place(victim, owner, angle, radius);
  victim->saved_motion[0] = fujin_wind_grab_angle_wrap(angle + 0x4ccc);
  radius += 0x28f;
  victim->saved_motion[1] = radius > 3 * Q16_ONE ? 5 * Q16_ONE : radius;
  victim->execution.hold = true;
  if (victim->native_variant == 0) {
    q16 y = victim->body.pos[AXIS_Y] - 0x28f;
    victim->body.pos[AXIS_Y] = y < -0x2cccc ? -0x2cccc : y;
    if (owner->action != 0x1000 || owner->pending != NULL) {
      victim->native_variant = 1;
    }
  } else {
    /* Native487a69 releases toward the arena origin once the orbit comes
     * within.31 radians of that heading, then9fb0 supplies the flight. */
    q16 target = fujin_wind_grab_direction_angle(-owner->body.pos[AXIS_X], -owner->body.pos[AXIS_Z]);
    q16 difference = fujin_wind_grab_angle_wrap(angle - target);
    if (difference > 0x3243f) {
      difference = 0x6487e - difference;
    }
    if (difference <= 0x4f5c) {
      fujin_wind_grab_orbit_release(victim, owner, target);
    }
  }
}

void fujin_grab_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &fujin_wind_grab_orbit_victim);
  fighter->flags |= FF_REACTING | FF_PAIRED | FF_UNTOUCHABLE;
  fighter->execution.cleanup = fujin_wind_grab_orbit_cleanup;
  fighter->native_variant = 0;
  const Fighter *owner = fighter_opponent(fighter);
  fighter->saved_motion[0] =
      fujin_wind_grab_direction_angle(fighter->body.pos[AXIS_X] - owner->body.pos[AXIS_X], fighter->body.pos[AXIS_Z] - owner->body.pos[AXIS_Z]);
  fighter->saved_motion[1] = 0x8000;
  mk4_script_indexed_sound(fighter, 5);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 0, fujin_wind_grab_orbit_step);
  native_play(fighter, false);
}

void fujin_wind_grab(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &fujin_wind_grab_wind_grab);
  fighter->execution.cleanup = fujin_wind_grab_grab_cleanup;
  fujin_wind_grab_grab_effect_start(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FUJIN_WIND_GRAB, fighter_ready);
}

/* Native service cluster: 43afc0, 43b1a0. */
static void goro_specials_special_query(Fighter *fighter);
static void goro_specials_special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries);
static const MoveDef goro_specials_s_special_punch = {.clip = 599, .last = 56, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef goro_specials_s_special_kick = {.clip = 598, .last = 30, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef goro_specials_s_special_uppercut = {.clip = 600, .last = 36, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

/* Goro's command handlers open with 489ef0's sampled voice token and
 * 45f870, which stops the grounded body and refaces the pair before the
 * selected clip starts. */
void goro_begin_special(Fighter *fighter, StateId state, const MoveDef *move, int token) {
  battle_sound(SOUND_TOKEN, token);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
}

static void goro_specials_special_query(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

/* 43b940 advances one pose and queries D[n] per visit for its row count;
 * a contact (43ba30) keeps advancing without further queries. 428cf0 then
 * finishes the bound clip, 429670 holds its last pose one visit and 43aa50
 * returns a human controller to its stance. */
static void goro_specials_special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries) {
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && first + queries - 1 <= last, "Goro special contact rows exceed the bound clip");
  fighter->native_record_value = *record;
  fighter->native_record = &fighter->native_record_value;
  native_begin(fighter, 1, last - first + 2, fighter_ready);
  for (int frame = 1; frame <= last - first + 2; ++frame) {
    native_frame(fighter, frame, mini(last, first + frame - 1), frame <= queries ? goro_specials_special_query : NULL);
  }
  native_play(fighter, false);
}

/* 43af90 -> 4dfb60 clip 599 to 14, then 43afc0: three D[32] rows. */
void goro_special_punch(Fighter *fighter) {
  goro_begin_special(fighter, S_PUNCH, &goro_specials_s_special_punch, 0x5fa);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_SPECIAL_PUNCH, fighter_ready);
}

void goro_script_punch_tail(Fighter *fighter) {
  AttackRecord goro_specials_punch_record = retail_vm_attack_record(battle_data(), PUNCH_ATTACK_RECORD);

  goro_specials_special_tail(fighter, &goro_specials_punch_record, fighter_sample(fighter) + 1, 3);
}

/* 43b170 -> 4dfb70 clip 598 to 8, then 43b1a0: eight D[33] rows. */
void goro_special_kick(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &goro_specials_s_special_kick, 0x5fa);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_SPECIAL_KICK, fighter_ready);
}

void goro_script_kick_tail(Fighter *fighter) {
  AttackRecord goro_specials_kick_record = retail_vm_attack_record(battle_data(), KICK_ATTACK_RECORD);

  goro_specials_special_tail(fighter, &goro_specials_kick_record, fighter_sample(fighter) + 1, 8);
}

/* 43b640 binds slot 53c6bc (fixup -> descriptor 4fc99c, clip 600) at
 * pose 0 through 428750, advances nine poses with 429490, then queries
 * D[35] for five rows through 43b940 before finishing the clip. */
void goro_special_uppercut(Fighter *fighter) {
  AttackRecord goro_specials_uppercut_record = retail_vm_attack_record(battle_data(), UPPERCUT_ATTACK_RECORD);

  goro_begin_special(fighter, S_UPPERCUT, &goro_specials_s_special_uppercut, 0x5fa);
  mk4_mmove_bind(fighter, 600, 37, 0);
  fighter->native_record_value = goro_specials_uppercut_record;
  fighter->native_record = &fighter->native_record_value;
  native_begin(fighter, 0, 37, fighter_ready);
  for (int frame = 0; frame <= 37; ++frame) {
    native_frame(fighter, frame, mini(36, frame), frame >= 9 && frame < 14 ? goro_specials_special_query : NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 43b370, 43b590, 480f40. */
static void goro_stomp_leap_rise(Fighter *fighter);
static void goro_stomp_leap_fall(Fighter *fighter);
static void goro_stomp_leap_cleanup(Fighter *fighter);
static const MoveDef goro_stomp_s_leap = {.clip = 601, .last = 51, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x409};

/* D[34]: the zero-damage stomp box under the falling body; every
 * outcome selects reaction 191. 43b370 applies 30% separately. */

enum {
  GORO_STOMP_LEAP_APEX_SAMPLE = 41, /* 23 + ceil(0x3cccc / 0x3851) one-pose rising visits */
  GORO_STOMP_LEAP_RISE = -0x3851,
  GORO_STOMP_LEAP_APEX = -0x3cccc,
  GORO_STOMP_LEAP_DROP_HEIGHT = -0x40000,
  GORO_STOMP_LEAP_DROP_SPEED = 0x1999,
  GORO_STOMP_LEAP_DROP_GRAVITY = 0xa3d,
  GORO_STOMP_LEAP_DAMAGE = 0x4ccc
};

/* 480f40 sets 532254 every physics visit; 489791 then skips the close-range
 * body separation while Goro stands on his victim. */
void goro_stomp_hold_separation(Fighter *fighter) {
  fighter->flags |= FF_HOLD_SEPARATION;
}

static void goro_stomp_leap_cleanup(Fighter *fighter) {
  if (fighter->body.physics == goro_stomp_hold_separation) {
    fighter->body.physics = NULL;
  }
}

/* 43b320 (HK F,F,B and the CPU far row): voice 0x64f, action 0409, then
 * 4dfbb8 binds clip 601 and double-steps to 23 before 43b370. */
void goro_leap_stomp(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &goro_stomp_s_leap, 0x64f);
  fighter->execution.cleanup = goro_stomp_leap_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_LEAP_STOMP, fighter_ready);
}

static void goro_stomp_leap_descend(Fighter *fighter) {
  /* 43b42d: relocate above the X/Z captured at launch and drop from 4.0. */
  fighter->body.pos[AXIS_X] = fighter->saved_motion[0];
  fighter->body.pos[AXIS_Z] = fighter->saved_motion[1];
  fighter->body.pos[AXIS_Y] = GORO_STOMP_LEAP_DROP_HEIGHT;
  fighter->body.vel[AXIS_Y] = GORO_STOMP_LEAP_DROP_SPEED;
  fighter->body.gravity = GORO_STOMP_LEAP_DROP_GRAVITY;
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, GORO_STOMP_LEAP_APEX_SAMPLE, goro_stomp_leap_fall);
  native_play(fighter, false);
}

static void goro_stomp_leap_rise(Fighter *fighter) {
  if (fighter->body.pos[AXIS_Y] <= GORO_STOMP_LEAP_APEX) {
    goro_stomp_leap_descend(fighter);
  }
}

/* 43b3a4: until the body lands, query D[34] without advancing the pose.
 * A contact disturbs resting Goro objects, deals 30% and enters 4dfbd0;
 * landing first enters the miss recovery 43b620 -> 4dfc38. */
static void goro_stomp_leap_fall(Fighter *fighter) {
  AttackRecord goro_stomp_stomp_record = retail_vm_attack_record(battle_data(), STOMP_ATTACK_RECORD);

  fighter->execution.hold = true;
  if (!fighter_airborne(fighter)) {
    fighter->execution.hold = false;
    fighter->sample_override = GORO_STOMP_LEAP_APEX_SAMPLE;
    retail_disturb_resting_weapons();
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_LEAP_STOMP_MISS_LANDING, fighter_ready);
    return;
  }
  if (battle_contact(fighter, &goro_stomp_stomp_record) == CONTACT_MISS) {
    return;
  }
  fighter->execution.hold = false;
  retail_disturb_resting_weapons();
  battle_damage((Fighter *)fighter_opponent(fighter), GORO_STOMP_LEAP_DAMAGE);
  fighter->sample_override = GORO_STOMP_LEAP_APEX_SAMPLE;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_LEAP_STOMP_STOMP_HIT, fighter_ready);
}

/* 43b505 records the victim X/Z and rises at -0x3851 per visit; 43b42d
 * advances one pose per visit (4285d0) until the body passes -3.8. */
void goro_script_leap_continue(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  fighter->saved_motion[0] = victim->body.pos[AXIS_X];
  fighter->saved_motion[1] = victim->body.pos[AXIS_Z];
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = GORO_STOMP_LEAP_RISE;
  native_begin(fighter, first + 1, last, goro_stomp_leap_descend);
  for (int pose = first + 1; pose <= last; ++pose) {
    native_frame(fighter, pose, pose, goro_stomp_leap_rise);
  }
  native_play(fighter, false);
}

/* 43b590 -> 43baf0: face the victim, stop and return the controller. The
 * retail 80-visit hold polls the CPU dispatcher 4339b0; portable CPU
 * policy owns that decision after the controller returns. */
void goro_script_stomp_return(Fighter *fighter) {
  goro_stomp_leap_cleanup(fighter);
  mk4_script_stop_body(fighter);
  fighter_execution_finish(fighter, fighter_ready);
}

/* Native service cluster: 46a950, 46a970. */
/* Native service cluster: 47ab50. */
/* LP D,B record 4e4394 (flags 0x4, gate 45e910) and the near/far CPU rows
 * select 47ab10: stamp 0xc, action 0x215 and script 4e8150 (indexed 37,
 * C[78], speed 0x2666, clip 299 two poses per visit to 18, tail 47ab50). */
static const MoveDef jax_dash_punch_s_dash = {.clip = 299, .last = 34, .flags = SF_LOCK | SF_RECORD, .action = 0x215};

/* D[22] record 4ec050: 16%, reactions 157/10/165. */

static const MoveDef jax_dash_punch_s_dash_stagger = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION};
static const MoveDef jax_dash_punch_s_dash_fall = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_REACTION};
static const MoveDef jax_dash_punch_s_dash_air_fall = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};

enum {
  JAX_DASH_PUNCH_DASH_QUERIES = 15,
  JAX_DASH_PUNCH_DASH_LOOP_POSE = 23,
  JAX_DASH_PUNCH_DASH_STOP_TICKS = 5,
  JAX_DASH_PUNCH_DASH_BLOCK_TICKS = 40,
  JAX_DASH_PUNCH_DASH_FINAL_TICKS = 20
};

static void jax_dash_punch_dash_query(Fighter *fighter);
static void jax_dash_punch_dash_step_query(Fighter *fighter);

bool jax_can_dash_punch(const Fighter *fighter) {
  return g_battle.ticks >= fighter->dash_punch_ready;
}

void jax_dash_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &jax_dash_punch_s_dash);
  fighter->dash_punch_ready = g_battle.ticks + 60;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_DASH_PUNCH, fighter_ready);
}

/* 47af90, installed through descriptor 4e8174 as the group +44 callback:
 * X/Z velocity *= 0xe666, then 4297a0 advances one pose and loops the
 * final pose back to 23. The pose half is resolved in the frame rows. */
static void jax_dash_punch_dash_friction(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static int jax_dash_punch_dash_advance(int pose) {
  return pose < jax_dash_punch_s_dash.last ? pose + 1 : JAX_DASH_PUNCH_DASH_LOOP_POSE;
}

static void jax_dash_punch_dash_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->body.physics == jax_dash_punch_dash_friction) {
    fighter->body.physics = NULL;
  }
}

static void jax_dash_punch_dash_ready(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

/* While the task waits, the +44 callback keeps advancing the looping pose.
 * Rows [first, first + ticks) start with action and continue that pose. */
static int jax_dash_punch_dash_rows(Fighter *fighter, int first, int ticks, int pose, FighterFn action) {
  native_frame(fighter, first, pose, action);
  for (int frame = first + 1; frame < first + ticks; ++frame) {
    pose = jax_dash_punch_dash_advance(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  return jax_dash_punch_dash_advance(pose);
}

/* 47af10: blocked contact stops motion, removes guard and stores 0x100e. */
static void jax_dash_punch_dash_block_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->action = 0x100e;
}

/* 47ae06: camera shake 6, then 48f6e0 saves and stops the owner's motion. */
static void jax_dash_punch_dash_stop(Fighter *fighter) {
  mk4_script_shake(fighter, 6);
  mk4_script_save_motion(fighter);
}

/* 47acc0 phase 1: 48f750 restores the saved velocity, then one decay. */
static void jax_dash_punch_dash_resume(Fighter *fighter) {
  mk4_script_restore_motion(fighter);
  jax_dash_punch_dash_friction(fighter);
}

/* Phase 2 with the counter still set: reaction 157, then another stop. */
static void jax_dash_punch_dash_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  jax_dash_punch_dash_stop(fighter);
}

/* 47ae90: reaction 168 ends the sequence; stop and wait 20 ticks. */
static void jax_dash_punch_dash_final(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 168);
  mk4_script_stop_body(fighter);
}

/* The last phase-2 visit dispatches 157 and, in the same visit, 47ae90. */
static void jax_dash_punch_dash_last_pummel(Fighter *fighter) {
  battle_react((Fighter *)fighter_opponent(fighter), 157);
  jax_dash_punch_dash_final(fighter);
}

/* 47acc0 phase 0 on D[22] contact: blocked -> 47af10 (40 ticks); an
 * airborne victim (48ee60) -> 47ae90; otherwise the cursor is raised to 24
 * and the counter of two runs stop 5, resume 5, reaction 157 twice. */
static void jax_dash_punch_dash_contact(Fighter *fighter, ContactResult result) {
  int pose = fighter_sample(fighter);
  if (result == CONTACT_BLOCK) {
    native_begin(fighter, 0, JAX_DASH_PUNCH_DASH_BLOCK_TICKS - 1, jax_dash_punch_dash_ready);
    jax_dash_punch_dash_rows(fighter, 0, JAX_DASH_PUNCH_DASH_BLOCK_TICKS, pose, jax_dash_punch_dash_block_stop);
  } else if (fighter_airborne(fighter_opponent(fighter))) {
    native_begin(fighter, 0, JAX_DASH_PUNCH_DASH_FINAL_TICKS - 1, jax_dash_punch_dash_ready);
    jax_dash_punch_dash_rows(fighter, 0, JAX_DASH_PUNCH_DASH_FINAL_TICKS, pose, jax_dash_punch_dash_final);
  } else {
    int frame = 0;
    native_begin(fighter, 0, 4 * JAX_DASH_PUNCH_DASH_STOP_TICKS + JAX_DASH_PUNCH_DASH_FINAL_TICKS - 1, jax_dash_punch_dash_ready);
    pose = jax_dash_punch_dash_rows(fighter, frame, JAX_DASH_PUNCH_DASH_STOP_TICKS, maxi(pose, 24), jax_dash_punch_dash_stop);
    pose = jax_dash_punch_dash_rows(fighter, frame += JAX_DASH_PUNCH_DASH_STOP_TICKS, JAX_DASH_PUNCH_DASH_STOP_TICKS, pose, jax_dash_punch_dash_resume);
    pose = jax_dash_punch_dash_rows(fighter, frame += JAX_DASH_PUNCH_DASH_STOP_TICKS, JAX_DASH_PUNCH_DASH_STOP_TICKS, pose, jax_dash_punch_dash_pummel);
    pose = jax_dash_punch_dash_rows(fighter, frame += JAX_DASH_PUNCH_DASH_STOP_TICKS, JAX_DASH_PUNCH_DASH_STOP_TICKS, pose, jax_dash_punch_dash_resume);
    jax_dash_punch_dash_rows(fighter, frame + JAX_DASH_PUNCH_DASH_STOP_TICKS, JAX_DASH_PUNCH_DASH_FINAL_TICKS, pose, jax_dash_punch_dash_last_pummel);
  }
  native_play(fighter, false);
}

static void jax_dash_punch_dash_query(Fighter *fighter) {
  AttackRecord jax_dash_punch_dash_record = retail_vm_attack_record(battle_data(), DASH_ATTACK_RECORD);

  ContactResult result = battle_contact(fighter, &jax_dash_punch_dash_record);
  if (result != CONTACT_MISS) {
    jax_dash_punch_dash_contact(fighter, result);
  }
}

/* 47ab50 phase 1 calls 47af90 itself before each further query. */
static void jax_dash_punch_dash_step_query(Fighter *fighter) {
  jax_dash_punch_dash_friction(fighter);
  jax_dash_punch_dash_query(fighter);
}

/* Counter zero: 47af90 once more, no guard, 4914a0 removes the callback
 * and 429460 rewinds from the final stored pose to zero before return. */
static void jax_dash_punch_dash_miss(Fighter *fighter) {
  jax_dash_punch_dash_friction(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->body.physics = NULL;
}

/* 47ab50 phase 0: install 47af90, set the counter to 15 and query D[22]
 * once per visit. Both the callback and the phase-1 call advance a pose. */
void jax_dash_punch_contact(Fighter *fighter) {
  fighter->body.physics = jax_dash_punch_dash_friction;
  fighter->execution.cleanup = jax_dash_punch_dash_cleanup;
  int pose = fighter_sample(fighter);
  int last = JAX_DASH_PUNCH_DASH_QUERIES + 1 + jax_dash_punch_s_dash.last;
  native_begin(fighter, 0, last, jax_dash_punch_dash_ready);
  native_frame(fighter, 0, pose, jax_dash_punch_dash_query);
  for (int frame = 1; frame < JAX_DASH_PUNCH_DASH_QUERIES; ++frame) {
    pose = jax_dash_punch_dash_advance(jax_dash_punch_dash_advance(pose));
    native_frame(fighter, frame, pose, jax_dash_punch_dash_step_query);
  }
  native_frame(fighter, JAX_DASH_PUNCH_DASH_QUERIES, jax_dash_punch_dash_advance(jax_dash_punch_dash_advance(pose)), jax_dash_punch_dash_miss);
  for (int frame = JAX_DASH_PUNCH_DASH_QUERIES + 1; frame <= last; ++frame) {
    native_frame(fighter, frame, last - frame, NULL);
  }
  native_play(fighter, false);
}

/* 4ea4e8[157] -> 4883f0 -> 4ea028: effect 9, context sound 1, clip 311. */
void jax_dash_victim_hit(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &jax_dash_punch_s_dash_stagger);
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_JAX_DASH_VICTIM_HIT, fighter_ready);
}

/* 4ea4e8[168] -> 4867b0: disarm; an airborne victim takes 481ce0 (shake 0,
 * 4e89d8 fall and 488c00 roll getup), a grounded one the 4913a0 slide and
 * clip 130 of 4e9e38. */
void jax_dash_victim_throw(Fighter *victim) {
  mk4_script_disarm_self(victim);
  if (fighter_airborne(victim)) {
    mk4_script_shake(victim, 0);
    fighter_start_move(victim, S_KNOCKDOWN, &jax_dash_punch_s_dash_air_fall);
    retail_vm_enter(victim, MK4_RETAIL_SCRIPT_JAX_DASH_PUNCH_AIRBORNE_VICTIM_168, fighter_ready);
    return;
  }
  fighter_start_move(victim, S_KNOCKDOWN, &jax_dash_punch_s_dash_fall);
  victim->body.physics = fighter_friction;
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_JAX_DASH_PUNCH_GROUNDED_VICTIM_168, fighter_ready);
}

/* 488c00 -> 4914c0(4ea220): roll getup 191/16 at -0x147a, crouch 190/16. */

/* Native service cluster: 47bd40. */
/* 47bd10 sets action 210 and enters 4e8210: sound C60, forward .18, Y -.1,
 * gravity 41, then clip 298 advances to sample 9 before native 47bd40. */
static const MoveDef kai_aerial_punch_aerial_punch = {.clip = 298, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x210};

/* D[16] 0x4ec0e0: 13% damage, reaction 123 on the ground or in the air, no block reaction. */

enum {
  KAI_AERIAL_PUNCH_AERIAL_QUERIES = 15,
  KAI_AERIAL_PUNCH_AERIAL_CONTACT_STOP = 10,
  KAI_AERIAL_PUNCH_FALL_CLIP = 310,
  KAI_AERIAL_PUNCH_FALL_COUNT = 28,
  KAI_AERIAL_PUNCH_FALL_FIRST = 7,
  KAI_AERIAL_PUNCH_FALL_HOLD = 18
};

static void kai_aerial_punch_aerial_drag(Fighter *fighter) {
  /* 4e8250 -> 47bf50 -> 490650: scale the X/Z velocity by .9 each world step. */
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void kai_aerial_punch_aerial_cleanup(Fighter *fighter) {
  if (fighter->body.physics == kai_aerial_punch_aerial_drag) {
    fighter->body.physics = NULL;
  }
}

static void kai_aerial_punch_fall_landed(Fighter *fighter) {
  /* 480cf0 on the floor: cursor 18, 428d10 finishes clip 310, then 46f650. */
  native_begin(fighter, 1, KAI_AERIAL_PUNCH_FALL_COUNT - KAI_AERIAL_PUNCH_FALL_HOLD, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= KAI_AERIAL_PUNCH_FALL_COUNT - KAI_AERIAL_PUNCH_FALL_HOLD; ++frame) {
    native_frame(fighter, frame, KAI_AERIAL_PUNCH_FALL_HOLD + frame - 1, NULL);
  }
  native_play(fighter, false);
}

static void kai_aerial_punch_fall_check(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    kai_aerial_punch_fall_landed(fighter);
  }
}

/* Shared native 480cf0: gravity and Y velocity, action 1005, face the peer
 * and play fall clip 310 from sample 7 to its sample-18 air hold. Landing
 * at any visit plays samples 18..27 and returns through the controller. */
static void kai_aerial_punch_aerial_fall(Fighter *fighter, q16 gravity) {
  fighter->body.physics = NULL;
  fighter->body.gravity = gravity;
  fighter->action = 0x1005;
  retail_face_peer(fighter);
  mk4_mmove_bind(fighter, KAI_AERIAL_PUNCH_FALL_CLIP, KAI_AERIAL_PUNCH_FALL_COUNT, KAI_AERIAL_PUNCH_FALL_FIRST);
  native_begin(fighter, 1, KAI_AERIAL_PUNCH_FALL_HOLD - KAI_AERIAL_PUNCH_FALL_FIRST, kai_aerial_punch_fall_landed);
  for (int frame = 1; frame <= KAI_AERIAL_PUNCH_FALL_HOLD - KAI_AERIAL_PUNCH_FALL_FIRST; ++frame) {
    native_frame(fighter, frame, KAI_AERIAL_PUNCH_FALL_FIRST + frame,
        frame < KAI_AERIAL_PUNCH_FALL_HOLD - KAI_AERIAL_PUNCH_FALL_FIRST ? kai_aerial_punch_fall_check : mk4_mmove_wait_grounded);
  }
  native_play(fighter, false);
}

static void kai_aerial_punch_contact_fall(Fighter *fighter) {
  /* 47be50 resumes after its stop with 480cf0, gravity 28f and the stopped Y velocity. */
  kai_aerial_punch_aerial_fall(fighter, 0x28f);
}

static void kai_aerial_punch_aerial_hit(Fighter *fighter) {
  /* 470e80 sets action 1006; 47be50 stops all motion and sleeps ten visits. */
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_stop(fighter);
  native_hold(fighter, KAI_AERIAL_PUNCH_AERIAL_CONTACT_STOP, kai_aerial_punch_contact_fall);
}

static void kai_aerial_punch_aerial_land(Fighter *fighter) {
  /* 470e80 sets action 1006; 47bed0 runs 46fd90 landing, then 46f650. */
  fighter->action = ACTION_RETAIL_1006;
  fighter_execution_finish(fighter, mk4_land);
}

static void kai_aerial_punch_timeout_fall(Fighter *fighter) {
  /* 4914a0 removes the drag callback before 480cf0 continues the current Y velocity. */
  kai_aerial_punch_aerial_fall(fighter, 0x28f);
}

static void kai_aerial_punch_aerial_timeout(Fighter *fighter) {
  /* 47bd60: install the drag callback and gravity 28f, then 428d10 plays
   * the remaining clip-298 samples without a floor check. */
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  fighter->body.physics = kai_aerial_punch_aerial_drag;
  fighter->body.gravity = 0x28f;
  if (first >= last) {
    kai_aerial_punch_timeout_fall(fighter);
    return;
  }
  native_begin(fighter, 1, last - first, kai_aerial_punch_timeout_fall);
  for (int frame = 1; frame <= last - first; ++frame) {
    native_frame(fighter, frame, first + frame, NULL);
  }
  native_play(fighter, false);
}

static void kai_aerial_punch_aerial_first_query(Fighter *fighter) {
  AttackRecord kai_aerial_punch_aerial_contact = retail_vm_attack_record(battle_data(), AERIAL_ATTACK_RECORD);

  if (battle_contact(fighter, &kai_aerial_punch_aerial_contact) != CONTACT_MISS) {
    kai_aerial_punch_aerial_hit(fighter);
  }
}

static void kai_aerial_punch_aerial_query(Fighter *fighter) {
  /* 4704d0 checks the floor (48ef30) before every later D[16] query. */
  if (!fighter_airborne(fighter)) {
    kai_aerial_punch_aerial_land(fighter);
    return;
  }
  kai_aerial_punch_aerial_first_query(fighter);
}

static void kai_aerial_punch_aerial_expired(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    kai_aerial_punch_aerial_land(fighter);
  } else {
    kai_aerial_punch_aerial_timeout(fighter);
  }
}

void kai_aerial_punch_contact(Fighter *fighter) {
  /* 4e8238 = {15 visits, D[16], contact 47be50, floor 47bed0}, then timeout 47bd60. */
  require(fighter_move(fighter)->clip == 298 && fighter_sample(fighter) == 9, "Kai aerial punch needs its retail hop startup");
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, KAI_AERIAL_PUNCH_AERIAL_QUERIES, kai_aerial_punch_aerial_expired);
  for (int frame = 1; frame <= KAI_AERIAL_PUNCH_AERIAL_QUERIES; ++frame) {
    native_frame(fighter, frame, pose, frame == 1 ? kai_aerial_punch_aerial_first_query : kai_aerial_punch_aerial_query);
  }
  native_play(fighter, false);
}

void kai_aerial_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &kai_aerial_punch_aerial_punch);
  fighter->execution.cleanup = kai_aerial_punch_aerial_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_KAI_AERIAL_PUNCH, fighter_ready);
}

/* Native service cluster: 47eb30, 480e90. */
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static void liu_kang_bicycle_bicycle_cleanup(Fighter *fighter);
static void liu_kang_bicycle_bicycle_query(Fighter *fighter);
static void liu_kang_bicycle_bicycle_pair(Fighter *fighter);
static bool liu_kang_bicycle_bicycle_grounded(const Fighter *fighter);
static void liu_kang_bicycle_bicycle_land(Fighter *fighter);
static void liu_kang_bicycle_bicycle_descend(Fighter *fighter);
static void liu_kang_bicycle_bicycle_carry_end(Fighter *fighter);
static void liu_kang_bicycle_bicycle_contact_end(Fighter *fighter);
static void liu_kang_bicycle_bicycle_motion_pose(Fighter *fighter, float pose);
static const MoveDef liu_kang_bicycle_s_bicycle = {.clip = 68, .last = 22, .flags = RETAIL, .action = ACTION_RETAIL_0206};

static void liu_kang_bicycle_bicycle_cleanup(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->capture_action == ACTION_RETAIL_0206 && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
}

static void liu_kang_bicycle_bicycle_query(Fighter *fighter) {
  AttackRecord liu_kang_bicycle_bicycle_record = retail_vm_attack_record(battle_data(), BICYCLE_ATTACK_RECORD);

  if (!fighter_airborne(fighter) || battle_contact(fighter, &liu_kang_bicycle_bicycle_record) != CONTACT_MISS) {
    liu_kang_bicycle_bicycle_contact_end(fighter);
  }
}

static void liu_kang_bicycle_bicycle_pair(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->capture_action != ACTION_RETAIL_0206 || victim->pending != NULL || (victim->execution.frames == NULL && !victim->execution.script.active) ||
      !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
    liu_kang_bicycle_bicycle_carry_end(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xe666);
  }
}

static bool liu_kang_bicycle_bicycle_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void liu_kang_bicycle_bicycle_land(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_BICYCLE_LAND, fighter_ready);
}

static void liu_kang_bicycle_bicycle_descend(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0xc4;
    native_await(fighter, liu_kang_bicycle_bicycle_grounded, liu_kang_bicycle_bicycle_land);
  } else {
    liu_kang_bicycle_bicycle_land(fighter);
  }
}

static void liu_kang_bicycle_bicycle_carry_end(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->capture_action == ACTION_RETAIL_0206 && victim->pending == NULL && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  mk4_script_stop_body(fighter);
  liu_kang_bicycle_bicycle_descend(fighter);
}

/* The query rows end on landing, contact or their last visit. */
static void liu_kang_bicycle_bicycle_contact_end(Fighter *fighter) {
  if (fighter->contact == CONTACT_BLOCK) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_BICYCLE_CONTACT_END, fighter_ready);
    return;
  }
  if (fighter->contact != CONTACT_HIT) {
    liu_kang_bicycle_bicycle_descend(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  victim->reaction = 84;
  retail_bicycle_capture(victim);
  fighter->flags |= FF_PAIRED;
  fighter->body.pos[AXIS_Y] = mini(fighter->body.pos[AXIS_Y], -0x15999);
  fighter_set_speed(fighter, 0x10a3);
  fighter->body.vel[AXIS_Y] = fighter->body.gravity = 0;
  native_begin(fighter, 0, 57, liu_kang_bicycle_bicycle_carry_end);
  for (int frame = 0; frame < 58; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, liu_kang_bicycle_bicycle_pair);
  }
  native_play(fighter, false);
}

void liu_kang_bicycle_script(Fighter *fighter) {
  native_begin(fighter, 0, 59, liu_kang_bicycle_bicycle_contact_end);
  for (int frame = 0; frame < 60; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, liu_kang_bicycle_bicycle_query);
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
  fighter_start_move(fighter, S_KICK, &liu_kang_bicycle_s_bicycle);
  fighter->sample_override = 0;
  fighter->execution.cleanup = liu_kang_bicycle_bicycle_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LIU_KANG_BICYCLE_KICK, fighter_ready);
}

static void liu_kang_bicycle_bicycle_motion_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

void liu_kang_liu_script_bicycle_motion(Fighter *fighter) {
  if (fighter->native_motion.binding != liu_kang_liu_script_bicycle_motion) {
    native_motion_begin(fighter, 16, NULL);
    for (int frame = 0; frame < 16; ++frame) {
      native_motion_frame(fighter, frame, liu_kang_bicycle_bicycle_motion_pose, (float)(7 + (frame + 1) % 16), NULL);
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
static const MoveDef weapon_steal_move = {.clip = 7, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x401};

/* 469cea queries D[11]/4ec140 once, only against an armed victim. */

static const MoveDef stolen_weapon_ground_victim_move = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION};
static const MoveDef stolen_weapon_air_victim_move = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION};

enum {
  WEAPON_STEAL_WAIT_TICKS = 30, /* 469d1d loads 53c440 with 0x1e */
};

/* 484100: while the thief keeps action 401 the weapon homes at 0.15 along
 * the X/Z line to the thief; afterwards 494590 drops it normally. */
static void stolen_weapon_step(Projectile *weapon) {
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

static Projectile *stolen_weapon(const Fighter *thief) {
  int victim = (int)(fighter_opponent(thief) - g_battle.fighters);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SWORD && p->step == stolen_weapon_step && p->owner == victim) {
      return p;
    }
  }
  return NULL;
}

static void weapon_steal_ready(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

static void weapon_steal_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  native_hold(fighter, 16, weapon_steal_ready);
}

static void weapon_steal_wait(Fighter *fighter) {
  native_hold(fighter, 32, weapon_steal_stop);
}

/* 469d7a: stop, play clip 7 out (428d10), wait 32, stop, wait 16, 46f650. */
static void weapon_steal_fail(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  int pose = fighter_sample(fighter);
  if (pose >= weapon_steal_move.last) {
    weapon_steal_wait(fighter);
    return;
  }
  native_begin(fighter, pose + 1, weapon_steal_move.last, weapon_steal_wait);
  for (int frame = pose + 1; frame <= weapon_steal_move.last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 469d2c: 49cb30 finds the type-70 weapon task; within squared distance
 * 0x8000, 49cb20 retires it and 46a180 equips it with sound C[38/39]. */
static void weapon_steal_poll(Fighter *fighter) {
  Projectile *weapon = stolen_weapon(fighter);
  if (weapon == NULL) {
    weapon_steal_fail(fighter);
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
  weapon_steal_ready(fighter);
}

static void weapon_steal_take(Fighter *fighter) {
  native_begin(fighter, 1, WEAPON_STEAL_WAIT_TICKS, weapon_steal_ready);
  for (int frame = 1; frame <= WEAPON_STEAL_WAIT_TICKS; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), weapon_steal_poll);
  }
  native_play(fighter, false);
}

/* 469caf: an armed victim and an unarmed thief allow the D[11] query. */
static void weapon_steal_query(Fighter *fighter) {
  AttackRecord weapon_steal_record = retail_vm_attack_record(battle_data(), WEAPON_STEAL_ATTACK_RECORD);

  const Fighter *victim = fighter_opponent(fighter);
  ContactResult result = victim->armed && !fighter->armed ? battle_contact(fighter, &weapon_steal_record) : CONTACT_MISS;
  if (result == CONTACT_MISS || result == CONTACT_BLOCK) {
    weapon_steal_fail(fighter);
  } else {
    weapon_steal_take(fighter);
  }
}

void quan_chi_weapon_steal(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &weapon_steal_move);
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 37);
  native_begin(fighter, 1, 10, weapon_steal_query);
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
  Projectile *weapon = battle_spawn_projectile(victim, PROJECTILE_SWORD, 600, stolen_weapon_step);
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
  fighter_start_move(victim, airborne ? S_KNOCKDOWN : S_HIT, airborne ? &stolen_weapon_air_victim_move : &stolen_weapon_ground_victim_move);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (airborne) {
    retail_vm_enter(victim, MK4_RETAIL_SCRIPT_QUAN_CHI_WEAPON_STEAL_AIRBORNE_VICTIM, fighter_ready);
  } else {
    retail_vm_enter(victim, MK4_RETAIL_SCRIPT_QUAN_CHI_WEAPON_STEAL_GROUNDED_VICTIM, fighter_ready);
  }
}

/* Native service cluster: 486000, 487420, 49a680, 49a6a0. */
static bool scorpion_spear_spear_not_pulling(const Fighter *fighter);
static void scorpion_spear_spear_cleanup(Fighter *fighter);
static void scorpion_spear_spear_projectile(Projectile *p);
static void scorpion_spear_spear_spawn(Fighter *fighter);
static const MoveDef scorpion_spear_s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0300};
static const MoveDef scorpion_spear_s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4000};

void scorpion_spear_stun_script(Fighter *fighter) {
  retail_vm_wait_native(fighter, 64, scorpion_special_ready);
}

static bool scorpion_spear_spear_not_pulling(const Fighter *fighter) {
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
static void scorpion_spear_spear_cleanup(Fighter *fighter) {
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

static void scorpion_spear_spear_projectile(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner], *victim = fighter_peer(owner);
  bool interrupted = fighter_move(owner)->clip != 14 || !owner->execution.script.active || owner->pending != NULL;
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
      retail_spear_arrival(victim);
      fighter_start_move(owner, S_PUNCH, &scorpion_spear_s_spear);
      owner->sample_override = 31;
      retail_vm_enter(owner, MK4_RETAIL_SCRIPT_SCORPION_SPEAR_RETURN, fighter_ready);
      owner->execution.cleanup = scorpion_spear_spear_cleanup;
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
  AttackRecord record = retail_vm_attack_record(battle_data(), MK4_RETAIL_DATA_SPEAR_PROJECTILE);
  ContactResult result = battle_contact_from(owner, &record, p->pos, p->facing);
  if ((result == CONTACT_HIT || result == CONTACT_AIR_HIT) && victim->health <= 0) {
    /* Retain the lethal contact's ordinary reaction; no live pull may park
     * the defeated controller after the round retires projectile objects. */
    p->kind = 0;
  } else if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_HIT, &scorpion_spear_s_snared);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    retail_vm_enter(victim, MK4_RETAIL_SCRIPT_SCORPION_SPEAR_VICTIM_HIT, fighter_ready);
    victim->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || hypot(p->pos[AXIS_X], p->pos[AXIS_Z]) > g_battle.arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

static void scorpion_spear_spear_spawn(Fighter *fighter) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SPEAR, 90, scorpion_spear_spear_projectile);
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
  fighter_start_move(fighter, S_PUNCH, &scorpion_spear_s_spear);
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = g_battle.ticks + 90;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SCORPION_SPEAR, scorpion_script_spear_done);
  fighter->execution.cleanup = scorpion_spear_spear_cleanup;
}

void scorpion_script_spear_done(Fighter *fighter) {
  if (scorpion_spear_spear_not_pulling(fighter)) {
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
  scorpion_spear_spear_spawn(fighter);
}

/* Native service cluster: native_bicycle. */
static bool aerial_bicycle_height(const Fighter *fighter);
static void aerial_bicycle_contact(Fighter *fighter);
static void aerial_bicycle_done(Fighter *fighter);
static void aerial_bicycle_fall(Fighter *fighter);
static void aerial_bicycle_spin(Fighter *fighter);
static void aerial_bicycle_script(Fighter *fighter);
static const MoveDef aerial_bicycle_move = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x205};

static bool aerial_bicycle_height(const Fighter *fighter) {
  return fighter_height(fighter) >= 2.5f;
}

static void aerial_bicycle_contact(Fighter *fighter) {
  AttackRecord aerial_bicycle_record = retail_vm_attack_record(battle_data(), AERIAL_BICYCLE_ATTACK_RECORD);

  if (battle_contact(fighter, &aerial_bicycle_record) != CONTACT_MISS) {
    aerial_bicycle_fall(fighter);
  }
}

static void aerial_bicycle_done(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

static void aerial_bicycle_fall(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->body.gravity = 0x28f;
  native_await(fighter, native_grounded, aerial_bicycle_done);
}

static void aerial_bicycle_spin(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_mmove_bind(fighter, 101, 9, 0);
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 0, 8, aerial_bicycle_fall);
  for (int frame = 0; frame < 9; ++frame) {
    native_frame(fighter, frame, frame, aerial_bicycle_contact);
  }
  native_play(fighter, false);
}

static void aerial_bicycle_script(Fighter *fighter) {
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = -0x2666;
  battle_sound(SOUND_A, 6);
  battle_sound(SOUND_INDEXED, 12);
  native_await(fighter, aerial_bicycle_height, aerial_bicycle_spin);
}

void sonya_bicycle(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &aerial_bicycle_move);
  fighter->execution.cleanup = sonya_special_cleanup;
  aerial_bicycle_script(fighter);
}

/* Native service cluster: 47b6c0. */
static void sonya_cartwheel_cartwheel_done(Fighter *fighter);
static void sonya_cartwheel_cartwheel_stop(Fighter *fighter);
static void sonya_cartwheel_cartwheel_recover(Fighter *fighter);
static const MoveDef sonya_cartwheel_s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};

static void sonya_cartwheel_cartwheel_done(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

static void sonya_cartwheel_cartwheel_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    native_hold(fighter, 20, sonya_cartwheel_cartwheel_done);
  } else {
    sonya_cartwheel_cartwheel_done(fighter);
  }
}

static void sonya_cartwheel_cartwheel_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_finish_clip(fighter, sonya_cartwheel_cartwheel_stop);
}

void sonya_script_cartwheel_tail(Fighter *fighter) {
  AttackRecord sonya_cartwheel_cartwheel_record = retail_vm_attack_record(battle_data(), CARTWHEEL_ATTACK_RECORD);

  fighter->cartwheel_ready = g_battle.ticks + 48;
  native_query_rows(fighter, &sonya_cartwheel_cartwheel_record, 9, 1, sonya_cartwheel_cartwheel_recover);
}

void sonya_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &sonya_cartwheel_s_cartwheel);
  fighter->execution.cleanup = sonya_special_cleanup;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SONYA_CARTWHEEL, fighter_ready);
}

bool sonya_can_cartwheel(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->cartwheel_ready;
}

/* Native service cluster: 47e0a0, 4858d0. */
static void sonya_leg_grab_leg_stop(Fighter *fighter);
static void sonya_leg_grab_leg_grab_done(Fighter *fighter);
static void sonya_leg_grab_leg_grab_contact(Fighter *fighter);
static void sonya_leg_grab_leg_grab_script(Fighter *fighter);
static const MoveDef sonya_leg_grab_s_leg_grab = {.clip = 102, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x203};
static const MoveDef sonya_leg_grab_s_leg_victim = {.clip = 201, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

void sonya_script_leg_victim_end(Fighter *victim) {
  mk4_mmove_bind(victim, 185, 16, 0);
  retail_getup_roll(victim);
}

static void sonya_leg_grab_leg_grab_done(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter_execution_finish(fighter, fighter_ready);
}

void sonya_script_leg_grab_tail(Fighter *fighter) {
  native_finish_clip(fighter, sonya_leg_grab_leg_grab_done);
}

static void sonya_leg_grab_leg_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
}

static void sonya_leg_grab_leg_grab_contact(Fighter *fighter) {
  AttackRecord sonya_leg_grab_leg_record = retail_vm_attack_record(battle_data(), LEG_ATTACK_RECORD);

  if (battle_contact(fighter, &sonya_leg_grab_leg_record) == CONTACT_MISS || fighter->contact == CONTACT_BLOCK) {
    fighter->action = 0x1012;
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SONYA_LEG_GRAB_MISS_OR_BLOCK, fighter_ready);
  } else {
    mk4_mmove_pair_victim(fighter, &sonya_leg_grab_s_leg_victim);
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SONYA_LEG_GRAB_PAIRED_GRAB, fighter_ready);
  }
}

static void sonya_leg_grab_leg_grab_script(Fighter *fighter) {
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 1, 11, sonya_leg_grab_leg_grab_contact);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, frame, frame == 7 ? sonya_leg_grab_leg_stop : NULL);
  }
  native_play(fighter, false);
}

void sonya_leg_grab(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter->leg_grab_ready = g_battle.ticks + 64;
  fighter_start_move(fighter, S_KICK, &sonya_leg_grab_s_leg_grab);
  battle_sound(SOUND_A, 9);
  battle_sound(SOUND_INDEXED, 15);
  fighter->execution.cleanup = sonya_special_cleanup;
  sonya_leg_grab_leg_grab_script(fighter);
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
static const MoveDef tanya_spin_kick_spin = {.clip = 109, .last = 28, .flags = SF_LOCK | SF_RECORD, .action = 0x21b};

/* D[28] 4ebfc0: 12%, hit/air 158, block 3. */

/* 45e8d0: at least 60 ticks since stamp 0xd and an opponent combo count
 * of at most four (45ebb0/45f5b0). */
bool tanya_can_spin_kick(const Fighter *fighter) {
  return g_battle.ticks >= fighter->spin_kick_ready && fighter_opponent(fighter)->hits <= 4;
}

static int tanya_spin_kick_spin_next(int pose) {
  return pose + 1 >= 29 ? 14 : pose + 1;
}

static void tanya_spin_kick_spin_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  bool airborne = fighter_airborne(fighter);
  mk4_script_stop_body(fighter);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  /* An interrupted recoil retains a fall; the replacing move owns landing. */
  if (airborne) {
    fighter->body.gravity = 0x28f;
  }
}

static void tanya_spin_kick_spin_ready(Fighter *fighter) {
  fighter_execution_finish(fighter, fighter_ready);
}

/* 479705: clear the cursor callback and motion, set no-guard, bind
 * sample 10 and let 429600 rewind one sample per visit to 0. */
static void tanya_spin_kick_spin_recover(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  fighter->sample_override = 10;
  native_begin(fighter, 0, 9, tanya_spin_kick_spin_ready);
  for (int frame = 0; frame <= 9; ++frame) {
    native_frame(fighter, frame, 9 - frame, NULL);
  }
  native_play(fighter, false);
}

static void tanya_spin_kick_recoil_wait(Fighter *fighter) {
  if (fighter->body.vel[AXIS_Y] < 0 || fighter->body.pos[AXIS_Y] < -0x28000) {
    fighter->execution.hold = true;
    return;
  }
  fighter->execution.hold = false;
  /* 47daa0: 490720/490de0 clear spin and angles, 490bd0 faces the peer. */
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_POWER_REWIND, fighter_ready);
}

/* Shared blocked recoil 47d940: action 1010, 4e84a8 speed -.05, Y -0x23d7
 * and gravity 0x168; 47dbc0 scales X velocity by .6 for slots 7/9/12/14.
 * 47db60 holds the last sample of clip 301 with X spin -0x4ccc, lifts the
 * group .2 and installs the 4707e0 whoosh; 48f490/48f410 wait for descent
 * below -2.5. 48fed0's 4ec2c4 peer adjustment is not modelled, as in Cage's
 * port of the same recoil. */
static void tanya_spin_kick_spin_blocked(Fighter *fighter) {
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
  native_frame(fighter, 0, 8, tanya_spin_kick_recoil_wait);
  native_play(fighter, false);
}

static void tanya_spin_kick_spin_query(Fighter *fighter) {
  AttackRecord tanya_spin_kick_spin_record = retail_vm_attack_record(battle_data(), SPIN_ATTACK_RECORD);

  ContactResult result = battle_contact(fighter, &tanya_spin_kick_spin_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    tanya_spin_kick_spin_blocked(fighter);
    return;
  }
  /* An unblocked contact keeps spinning and moving for twelve visits. */
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, tanya_spin_kick_spin_recover);
  for (int frame = 1; frame <= 12; ++frame) {
    pose = tanya_spin_kick_spin_next(pose);
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}

static void tanya_spin_kick_spin_sounds(Fighter *fighter) {
  mk4_script_indexed_sound(fighter, 0x35);
  battle_sound(SOUND_C, 0x57);
}

/* Eight visits of startup, sounds, then fifteen D[28] queries. */
void tanya_spin_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &tanya_spin_kick_spin);
  fighter->execution.cleanup = tanya_spin_kick_spin_cleanup;
  fighter->spin_kick_ready = g_battle.ticks + 60;
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x2666);
  native_begin(fighter, 1, 23, tanya_spin_kick_spin_recover);
  int pose = 0;
  for (int frame = 1; frame <= 23; ++frame) {
    pose = tanya_spin_kick_spin_next(pose);
    native_frame(fighter, frame, pose, frame == 8 ? tanya_spin_kick_spin_sounds : frame > 8 ? tanya_spin_kick_spin_query : NULL);
  }
  native_play(fighter, false);
}

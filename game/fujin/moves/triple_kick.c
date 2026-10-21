#include "retail_vm.h"
#include "../character.h"

static const MoveDef triple = {.clip = 100, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x208};
static const MoveDef middle_victim = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = 0x4001};
static const AttackRecord triple_record = {0x970a, -0x1170a, 0xb851, 0xffbe, 93, 93, 8, 0, 0x30a3};
static void triple_recover(Fighter *fighter);
static void triple_cycle(Fighter *fighter);

static void triple_release(Fighter *fighter) {
  if (!fighter->native_followup) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_followup = false;
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  /* An interrupted owner cannot leave a captured intermediate victim
   * hovering at the paired -.3 offset with its old zero gravity. */
  if ((victim->reaction == 93 || victim->reaction == 94) && fighter_airborne(victim)) {
    victim->body.gravity = 0x51e;
  }
}

static void triple_cleanup(Fighter *fighter) {
  triple_release(fighter);
  mk4_script_stop_body(fighter);
}

static void triple_to(Fighter *fighter, int target, FighterFn end) {
  int first = fighter_sample(fighter);
  require(first <= target && target <= fighter_move(fighter)->last, "Invalid Fujin triple-kick native playback range");
  native_begin(fighter, first, target, end);
  for (int pose = first; pose <= target; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void triple_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  triple_to(fighter, 44, mk4_mmove_thrown_finish);
}

static void triple_recover(Fighter *fighter) {
  triple_release(fighter);
  /* 47f280 reaches21 with the existing motion, then stops and finishes. */
  triple_to(fighter, 21, triple_finish);
}

static void triple_cycle_done(Fighter *fighter) {
  fighter->sample_override = 17;
  if (++fighter->native_variant < 3) {
    battle_react((Fighter *)fighter_opponent(fighter), fighter->native_variant == 1 ? 94 : 95);
    triple_cycle(fighter);
  } else {
    triple_recover(fighter);
  }
}

static void triple_restore(Fighter *fighter) {
  for (int axis = 0; axis < 3; ++axis) {
    fighter->body.vel[axis] = axis == AXIS_Y ? fighter->saved_motion[axis] : q16_mul(fighter->saved_motion[axis], 0xe666);
  }
  native_hold(fighter, 4, triple_cycle_done);
}

static void triple_cycle(Fighter *fighter) {
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
  native_hold(fighter, 4, triple_restore);
}

static void triple_floor_hold(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void triple_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void triple_blocked(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  /* 47f480 ->490890 selects the first4e8790 recoil vector. */
  fighter_set_speed(fighter, -0x51e);
  fighter->body.vel[AXIS_Y] = -0x1999;
  fighter->body.gravity = 0x147;
  fighter->body.on_land = NULL;
  native_begin(fighter, 0, 0, triple_land);
  native_frame(fighter, 0, fighter_sample(fighter), triple_floor_hold);
  native_play(fighter, false);
}

static void triple_query(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &triple_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    triple_blocked(fighter);
  } else {
    fighter->native_variant = 0;
    fighter->native_followup = true;
    fighter->flags |= FF_PAIRED;
    triple_cycle(fighter);
  }
}

void fujin_triple_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 100 && fighter_sample(fighter) == 13, "Fujin triple kick needs its generated13-pose startup");
  /* 47f070 advances before querying D[9], four times at14..17. */
  native_begin(fighter, 1, 4, triple_recover);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, 13 + frame, triple_query);
  }
  native_play(fighter, false);
}

void fujin_triple_fast(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8770, fighter_ready, 5);
}

void fujin_triple_victim(Fighter *fighter) {
  if (fighter->reaction == 94) {
    fighter_start_move(fighter, S_HIT, &middle_victim);
    fighter->flags |= FF_REACTING;
    retail_vm_enter_owned(fighter, 0x4ea050, mk4_mmove_thrown_finish, 5);
  } else {
    require(fighter->reaction == 93 || fighter->reaction == 95, "Invalid Fujin triple-kick victim stage");
    /* Native488420/488440 and488830/488850 are also the exact
     * first/second axe victim entries; reuse their shared adaptations. */
    weapon_axe_reaction(fighter, fighter->reaction == 95, fighter->reaction == 95 ? fujin_entry_retail_script_004ea0b8 : fujin_entry_retail_script_004ea040);
  }
}

void fujin_triple_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &triple);
  fighter->native_followup = false;
  fighter->mmove_cleanup = triple_cleanup;
  battle_sound(SOUND_C, 0x42);
  retail_vm_enter_owned(fighter, 0x4e8740, fighter_ready, 5);
}

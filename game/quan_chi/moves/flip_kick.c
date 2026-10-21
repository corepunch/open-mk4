#include "retail_vm.h"
#include "../character.h"

/* HK [F,F] record 4e4858 and the near CPU row select native 47a370: action
 * 217, 490ae0(4e80f8) installs group callback 47bf50, then VM 4e8100 plays
 * clip 105 to sample 10, sets speed 0x547a and reaches sample 15. */
static const MoveDef s_flip_kick = {.clip = 105, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x217};
/* 47a500 queries D[24]/4ec020. */
static const AttackRecord flip_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 160, 160, 8, 0, 0x2666};
static const MoveDef s_flip_victim = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION};

/* 47bf50 -> 490650: scale horizontal velocity by 0xe666 every world pump. */
static void flip_decay(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

static void flip_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  if (fighter->body.physics == flip_decay) {
    fighter->body.physics = NULL;
  }
}

static void flip_ready(Fighter *fighter) {
  flip_cleanup(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 47a610 -> 428d10 plays the remaining stored samples, then 46f650. */
static void flip_finish(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  if (pose >= s_flip_kick.last) {
    flip_ready(fighter);
    return;
  }
  native_begin(fighter, pose + 1, s_flip_kick.last, flip_ready);
  for (int frame = pose + 1; frame <= s_flip_kick.last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 47a500: contact submits pose 19 (429970), stops motion (4906e0) and sets
 * no-guard (490140). A block waits five extra ticks before 47a5c0's ten. */
static void flip_contact(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &flip_record);
  if (result == CONTACT_MISS) {
    return;
  }
  fighter->sample_override = 19;
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  native_hold(fighter, result == CONTACT_BLOCK ? 15 : 10, flip_finish);
}

/* 47a3b0: four visits each query D[24] and advance one sample (429830);
 * the first three wait one tick, the fourth waits eight before 47a610. */
void quan_chi_script_flip_kick_tail(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  require(fighter_move(fighter)->clip == 105 && pose == 15, "Flip kick contact needs its selected 4e8100 startup");
  native_begin(fighter, 0, 11, flip_finish);
  for (int frame = 0; frame <= 11; ++frame) {
    native_frame(fighter, frame, pose + (frame < 4 ? frame : 4), frame < 4 ? flip_contact : NULL);
  }
  native_play(fighter, false);
}

void quan_chi_flip_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_flip_kick);
  fighter->mmove_cleanup = flip_cleanup;
  fighter->body.physics = flip_decay;
  retail_vm_enter_owned(fighter, 0x4e8100, fighter_ready, 8);
}

/* 488c00: shared roll getup after the reaction-160 landing. */
void quan_chi_script_getup_roll(Fighter *fighter) {
  mk4_mmove_getup(fighter, 0x4ea220);
}

/* Reaction 160 (481d90): 488ea0 wall recoil, 494520 disarm, then VM 4e8a48
 * launches clip 189 and lands into 488c00. */
void quan_chi_flip_reaction(Fighter *victim) {
  battle_wall_recoil(victim);
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_flip_victim);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  retail_vm_enter_owned(victim, 0x4e8a48, fighter_ready, 8);
}

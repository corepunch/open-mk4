#include "retail_vm.h"
#include "../character.h"

/* 478df0 sets action 20b and enters 4e8870: indexed cue 59, clip 551 with
 * its fast-start branch, sound C31 and forward .15 at sample 26. */
static const MoveDef lunge_kick = {.clip = 551, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x20b};
/* B[9] 0x4eba20: 14% damage, reactions 127 hit/air and 5 block. */
static const AttackRecord lunge_contact = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};

static void lunge_query(Fighter *fighter) {
  /* 470b30 skips 470dc0 after the first contact in this list. */
  if (!fighter->connected) {
    battle_contact(fighter, &lunge_contact);
  }
}

static void lunge_recover(Fighter *fighter) {
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
  native_begin(fighter, 1, 5, lunge_recover);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, 29 + frame, lunge_query);
  }
  native_play(fighter, false);
}

void kai_input_478df0(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &lunge_kick);
  retail_vm_enter_owned(fighter, 0x4e8870, fighter_ready, 11);
}

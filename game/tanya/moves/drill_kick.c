#include "../character.h"
#include "math_util.h"

/* LK F,D,B record 4e455c and all three CPU range rows -> T4[18] 47b9f0:
 * action 211 and VM 4e81f0 (indexed sound 31, speed 0x23d7, clip 103 to
 * sample 12, tail 47ba20). */
static const MoveDef drill = {.clip = 103, .last = 49, .flags = SF_LOCK | SF_RECORD, .action = 0x211};
/* D[17] 4ec0c8: 15%, hit/air/block 94/122/9. */
static const AttackRecord drill_record = {0x12624, -0x68b4, 0x11993, 0x15be7, 94, 122, 9, 0, 0x2666};

static void drill_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
}

static void drill_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 47bbd0 state 1: stop, camera shake 0, then 428cf0 finishes clip 103.
 * The 48a330 random list (tokens 0xbf/0xd7) has no portable sound bank
 * entry for 0xbf and is not queued. */
static void drill_land(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_script_shake(fighter, 0);
}

/* 47bbd0: two cursor submissions on entry, one per visit until sample 20,
 * a held visit, then the clip tail from two samples further on. */
static void drill_recover(Fighter *fighter) {
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
  native_begin(fighter, 0, count - 1, drill_ready);
  for (int frame = 0; frame < count; ++frame) {
    native_frame(fighter, frame, mini(49, poses[frame]), frame == land ? drill_land : NULL);
  }
  native_play(fighter, false);
}

/* 47ba90 state 1: count the extra hit, shake, install reaction 95 on the
 * peer through 480f80, then recover. */
static void drill_second_hit(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->hits++;
  mk4_script_shake(fighter, 0);
  battle_react(victim, 95);
  drill_recover(fighter);
}

static void drill_contact(Fighter *fighter, ContactResult result) {
  mk4_script_shake(fighter, 0);
  if (result == CONTACT_BLOCK || fighter_airborne(fighter_opponent(fighter))) {
    drill_recover(fighter);
    return;
  }
  /* 429490 advances six samples before the grounded follow-up. */
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 6, drill_second_hit);
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
    ContactResult result = battle_contact(fighter, &drill_record);
    if (result != CONTACT_MISS) {
      drill_contact(fighter, result);
      return;
    }
  }
  drill_recover(fighter);
}

void tanya_drill_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &drill);
  fighter->mmove_cleanup = drill_cleanup;
  tanya_entry_retail_script_004e81f0(fighter);
}

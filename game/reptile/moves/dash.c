#include "retail_vm.h"
#include "../character.h"

/* B,F+LK record 4e45dc (gate 45e950) and the near/medium CPU rows enter
 * 47a690: family-11 stamp, action 216, VM 4e8128 plays clip 302, binds
 * 303/0 with speed 0x2e14 and tails into 47a6d0. */
static const MoveDef dash = {.clip = 302, .last = 7, .flags = SF_LOCK | SF_RECORD, .action = 0x216};
/* 47a6d0 queries D[23] (12%, reactions 158/158/165). */
static const AttackRecord dash_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 158, 158, 165, 0, 0x1eb8};
/* B,F+LP record 4e4590 and the far CPU row enter 47afe0: action 20d,
 * VM 4e8178 on clip 106, D[21] probes and tail 47b010. */
static const MoveDef roll = {.clip = 106, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x20d};
static const AttackRecord roll_record = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 40, 10, 9, 0, 0x170a};
static const MoveDef roll_victim = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef dash_victim = {.clip = 179, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};

bool reptile_can_dash(const Fighter *fighter) {
  /* 45e950: action family 11 must be at least 70 ticks old and the peer
   * combo count (+7c, queried by 45f5b0) must not exceed four. */
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && fighter_opponent(fighter)->hits <= 4;
}

/* 47a7e0: 428750 binds clip 302 (first word of 53cf48), 429460 starts at
 * its last sample and plays it backwards, then 46f650 returns. */
static void dash_recover(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &dash);
  fighter->sample_override = 7;
  native_begin(fighter, 1, 8, mk4_return_to_stance);
  for (int frame = 1; frame <= 7; ++frame) {
    native_frame(fighter, frame, 7 - frame, NULL);
  }
  native_frame(fighter, 8, 0, NULL);
  native_play(fighter, false);
}

/* 47a9e0/47aa90 add +0xccc then -0xccc to world X, two visits each. */
static void dash_shake_out(Fighter *fighter) {
  fighter->body.pos[AXIS_X] += 0xccc;
}

static void dash_shake_back(Fighter *fighter) {
  fighter->body.pos[AXIS_X] -= 0xccc;
}

static void dash_contact(Fighter *fighter) {
  if (fighter->connected) {
    return;
  }
  ContactResult result = battle_contact(fighter, &dash_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result != CONTACT_BLOCK) {
    /* 47a890: an unblocked hit waits eight visits before 47a7e0. */
    native_hold(fighter, 8, dash_recover);
    return;
  }
  /* 47a8f0: action 100e, stop motion, no guard and eight 47a9e0 shakes. */
  fighter->action = 0x100e;
  mk4_script_stop_body(fighter);
  fighter->flags |= FF_NO_GUARD;
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 32, dash_recover);
  for (int frame = 1; frame <= 32; ++frame) {
    native_frame(fighter, frame, pose, frame % 4 == 1 ? dash_shake_out : frame % 4 == 3 ? dash_shake_back : NULL);
  }
  native_play(fighter, false);
}

/* 47a6d0: one setup visit, then sixteen D[23] queries holding 303/0.
 * Its 490bd0 steering only turns the model toward the opponent, which
 * the pair-axis heading already does. Physics 47aab0 plays a random
 * footstep from token list 4ea8b0 every five ticks; the engine has no
 * raw-token sound bank, so those cues are omitted. */
void reptile_script_dash_contact(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 0, 16, dash_recover);
  for (int frame = 0; frame <= 16; ++frame) {
    native_frame(fighter, frame, pose, frame > 0 ? dash_contact : NULL);
  }
  native_play(fighter, false);
}

void reptile_input_47a690(Fighter *fighter) {
  fighter->slide_ready = g_battle.ticks + 70;
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &dash);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e8128, fighter_ready, 10);
}

static void roll_probe(Fighter *fighter) {
  /* 47b260 queries D[21] until its local latch is set. */
  if (!fighter->connected) {
    battle_contact(fighter, &roll_record);
  }
}

/* 47b010 from 106/17: probes at 18, 19 and 18; six cycles of 428cf0
 * play-out to 21 and 4292f0 rewind to 19; 429380 reverses to zero and
 * 46f650 returns. */
void reptile_script_roll_tail(Fighter *fighter) {
  static const int cycle[] = {20, 21, 21, 20, 19};
  require(fighter_move(fighter)->clip == 106 && fighter_sample(fighter) == 17, "Roll tail needs its generated clip-106 startup");
  native_begin(fighter, 1, 3 + 6 * 5 + 20, mk4_return_to_stance);
  int frame = 1;
  native_frame(fighter, frame++, 18, roll_probe);
  native_frame(fighter, frame++, 19, roll_probe);
  native_frame(fighter, frame++, 18, roll_probe);
  for (int repeat = 0; repeat < 6; ++repeat) {
    for (int i = 0; i < 5; ++i) {
      native_frame(fighter, frame++, cycle[i], NULL);
    }
  }
  for (int sample = 18; sample >= 0; --sample) {
    native_frame(fighter, frame++, sample, NULL);
  }
  native_frame(fighter, frame, 0, NULL);
  native_play(fighter, false);
}

void reptile_input_47afe0(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &roll);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e8178, fighter_ready, 10);
}

/* Reaction 40 (483550): disarm and VM 4e9620, clip 183 launch, landing
 * wait and the shared 488b40 side getup. */
void reptile_reaction_40(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &roll_victim);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e9620, fighter_ready, 10);
}

/* Reaction 158 (483500): damage scale 0x9999 (482a50), disarm and VM
 * 4e95f0, clip 179 launch and landing. The 48a320 random grunt list
 * 4ea8f8 uses raw tokens and is omitted with the dash footsteps. */
void reptile_dash_victim(Fighter *fighter) {
  fighter->damage_scale = 0x9999;
  fighter_start_move(fighter, S_KNOCKDOWN, &dash_victim);
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4e95f0, fighter_ready, 10);
}

/* 488b80: 490350 toggles the model mirror bit, which the pair-axis
 * heading supersedes here, then 4914c0 runs the 4ea210 getup pairs. */
void reptile_dash_victim_getup(Fighter *fighter) {
  mk4_mmove_getup(fighter, 0x4ea210);
}

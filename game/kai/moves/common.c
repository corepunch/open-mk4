#include "../character.h"

static void kai_input_47bd40(Fighter *fighter);
static void kai_input_47cbf0(Fighter *fighter);
static void kai_input_47cc10(Fighter *fighter);
static void kai_contact_frame(Fighter *fighter);
static bool kai_stage_event_step(BattleEvent *event, Fighter *owner, Fighter *opponent);
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_stage_event_clip = {.clip = 0, .last = 0, .flags = SF_LOCK};
static const AttackRecord s_kai_attack_b9 = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};
static const AttackRecord s_kai_attack_b10 = {0x1851e, -0xbd2f, 0x10000, 0x9958, 127, 127, 5, 0, 0x1c28};

void kai_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void kai_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

void kai_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &kai_s_idle);
}

void kai_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &kai_s_walk);
}

void kai_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &kai_s_block);
}

void kai_input_478df0(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &kai_s_low_kick, kai_entry_retail_script_004e8870);
}

void kai_input_47bd10(Fighter *fighter) {
  kai_high_punch(fighter);
}

static void kai_input_47bd40(Fighter *fighter) {
  fighter_start_fiber(fighter, kai_entry_retail_script_004e8238, NULL);
}

void kai_input_47c520(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &kai_s_low_kick, kai_entry_retail_script_004e8320);
}

static void kai_input_47cbf0(Fighter *fighter) {
  fighter_start_fiber(fighter, kai_entry_retail_script_004e88c8, NULL);
}

static void kai_input_47cc10(Fighter *fighter) {
  fighter_start_fiber(fighter, kai_entry_retail_script_004e8358, NULL);
}

void kai_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 enters the follow-up VM root after its global task checks. */
  kai_entry_retail_script_004de358(fighter);
}

void kai_script_lk_finish(Fighter *fighter) {
  /* Retail 42bf00 coordinates global task callbacks. The portable fiber
   * scheduler owns that queue; close the actor pair at this boundary. */
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(kai_ready);
}

void kai_script_aerial_transition(Fighter *fighter) {
  /* 47bd40 resumes Kai's high-punch move at its generated follow-up stream. */
  kai_entry_retail_script_004e8238(fighter);
}

void kai_script_aerial_recovery(Fighter *fighter) {
  /* 47bf90 selects the 4e8280 recovery suffix after its aerial clip. */
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->body.gravity = 0;
  native_delay(fighter, 8);
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter->body.gravity = 0x51e;
  mk4_script_wait_land(fighter);
  fighter_fiber_finish(kai_ready);
}

static void kai_contact_frame(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

void kai_script_contact_b9_continuation(Fighter *fighter) {
  native_play_to(fighter, 29, 1);
  fighter->native_record = &s_kai_attack_b9;
  native_calls(fighter, 5, 1, kai_contact_frame);
  kai_entry_retail_script_004e88c8(fighter);
}

void kai_script_contact_b10_continuation(Fighter *fighter) {
  native_play_to(fighter, 9, 1);
  fighter->native_record = &s_kai_attack_b10;
  native_begin(fighter, 1, 3, NULL);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, frame == 1 ? 10 : 11, kai_contact_frame);
  }
  native_wait(fighter);
  kai_entry_retail_script_004e8390(fighter);
}

static bool kai_stage_event_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  int clip = event->data[0], sample = event->data[1], last = event->data[2];
  fighter_start_move(owner, S_KICK, &s_stage_event_clip);
  owner->playback_move = *fighter_move(owner);
  owner->playback_move.clip = clip;
  owner->playback_move.first = 0;
  owner->playback_move.last = last;
  owner->playback_move.sequence = NULL;
  owner->playback_move.flags = SF_LOCK;
  owner->current_move = &owner->playback_move;
  owner->sample_override = sample;
  owner->action = 0x200d;
  native_begin(owner, 1, last - sample + 2, kai_ready);
  for (int frame = 1; frame <= last - sample + 2; ++frame) {
    native_frame(owner, frame, mini(last, sample + frame), NULL);
  }
  native_play(owner, false);
  return false;
}

void kai_script_stage_event(Fighter *fighter) {
  const MoveDef *move = fighter_move(fighter);
  require((move->clip == 1 && move->last == 19) || (move->clip == 9 && move->last == 65), "Kai stage event needs clip 1 or clip 9 context");
  require(fighter->sample_override >= 0 && fighter->sample_override <= move->last, "Kai stage event needs a valid clip sample");
  BattleEvent *event = battle_event_start(fighter, kai_stage_event_step, false, true);
  event->data[0] = move->clip;
  event->data[1] = fighter->sample_override;
  event->data[2] = move->last;
}

void kai_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    kai_entry_retail_script_004de6b8(fighter);
  } else {
    kai_entry_retail_script_004de610(fighter);
  }
}

void kai_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

void kai_script_lp_extended_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 109;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown);
  battle_react(victim, 109);
  fighter_fiber_finish(kai_ready);
}

void kai_script_lp_short_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 102;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  battle_react(victim, 102);
  fighter_fiber_finish(kai_ready);
}

void kai_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(kai_ready);
    return;
  }
  require(false, "Unexpected Kai paired reaction");
}

void kai_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(kai_ready);
}

#include "../character.h"

static void kai_input_47bd40(Fighter *fighter);
static void kai_contact_frame(Fighter *fighter);
static const AttackRecord s_kai_attack_b9 = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};

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

void kai_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 enters the follow-up VM root after its global task checks. */
  kai_entry_retail_script_004de358(fighter);
}

void kai_script_lk_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
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
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_handoff(kai_entry_retail_script_004e88c8);
  }
  kai_entry_retail_script_004e88c8(fighter);
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
  require(reaction == 102 || reaction == 109, "Unexpected Kai LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void kai_script_lp_extended_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
}

void kai_script_lp_short_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
}

void kai_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Kai paired reaction");
}

void kai_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(kai_ready);
}

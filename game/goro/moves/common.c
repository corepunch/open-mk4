#include "../character.h"

static void goro_ready(Fighter *fighter);
void goro_event_progress(Fighter *fighter);

static void goro_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void goro_script_current_reaction(Fighter *fighter, int reaction) {
  require(reaction == 100, "Unexpected Goro LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void goro_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Goro opponent reaction");
}

void goro_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 selects the follow-up stream after its task-state checks. */
  goro_entry_retail_script_004de358(fighter);
}

void goro_script_lk_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, goro_ready);
}

void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    goro_entry_retail_script_004de7e8(fighter);
    return;
  }
  goro_entry_retail_script_004de6e4(fighter);
}

/* Native 43afc0/43b1a0 share 43b940: advance the pose and query D[32]
 * for three rows or D[33] for eight rows, then finish the remaining clip.
 * A contact consumes the query but preserves the remaining pose window. */
static const AttackRecord punch_contact = {0x13810, -0xe106, 0xfd2f, 0x11be7, 189, 189, 7, 0, 0x2666};
static const AttackRecord kick_contact = {0x1b2b0, -0x6b43, 0x16872, 0xa624, 190, 190, 7, 0, 0x2666};

static void contact_frame(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

static void contact_tail(Fighter *fighter, const AttackRecord *record, int frames) {
  fighter->native_record = record;
  native_calls(fighter, frames, 1, contact_frame);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(goro_ready);
}

void goro_script_punch_tail(Fighter *fighter) {
  contact_tail(fighter, &punch_contact, 3);
}

void goro_script_counter_kick_tail(Fighter *fighter) {
  contact_tail(fighter, &kick_contact, 8);
}

void goro_script_event_finish(Fighter *fighter) {
  fighter_fiber_finish(goro_ready);
}

void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action) {
  require(fighter != NULL && script != NULL && count > 0, "Invalid Goro actor event");
  fighter->playback_move = (MoveDef){.clip = clip, .last = count - 1, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = action};
  fighter_start_move(fighter, clip == 599 ? S_PUNCH : S_KICK, &fighter->playback_move);
  fighter_start_fiber(fighter, script, NULL);
}

void goro_event_progress(Fighter *fighter) {
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfb60, 599, 57, 0);
}

void goro_script_finish(Fighter *fighter) {
  if (fighter->state == S_SWEEP) {
    native_standard_sweep(fighter);
    return;
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(goro_ready);
}

void goro_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

void goro_script_lp_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, goro_ready);
}

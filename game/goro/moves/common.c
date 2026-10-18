#include "../character.h"

static void goro_ready(Fighter *fighter);
static void goro_event_progress(Fighter *fighter);

static void goro_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void goro_script_current_reaction(Fighter *fighter, int reaction) {
  /* Goro's roster entry makes battle_react select the roster-15 fallback. */
  battle_react(fighter, reaction);
}

void goro_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(goro_ready);
    return;
  }
  require(false, "Unexpected Goro opponent reaction");
}

void goro_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 selects the follow-up stream after its task-state checks. */
  goro_entry_retail_script_004de358(fighter);
}

void goro_script_lk_finish(Fighter *fighter) {
  /* The portable fiber owns the paired throw lifetime and actor return. */
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(goro_ready);
}

void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    goro_entry_retail_script_004de7e8(fighter);
    return;
  }
  goro_entry_retail_script_004de6e4(fighter);
}

void goro_script_event_finish(Fighter *fighter) {
  fighter_fiber_finish(goro_ready);
}

void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action) {
  require(fighter != NULL && script != NULL && count > 0, "Invalid Goro actor event");
  fighter->playback_move = *fighter_move(fighter);
  fighter->playback_move.clip = clip;
  fighter->playback_move.first = 0;
  fighter->playback_move.last = count - 1;
  fighter->playback_move.flags |= SF_LOCK;
  fighter->playback_move.sequence = NULL;
  fighter->playback_move.action = action != 0 ? action : fighter->action;
  fighter_start_move(fighter, fighter->state, &fighter->playback_move);
  fighter_start_fiber(fighter, script, NULL);
}

static void goro_event_progress(Fighter *fighter) {
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfb60, 599, 57, 0);
}

void goro_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(goro_ready);
}

void goro_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

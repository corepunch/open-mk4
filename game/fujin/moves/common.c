#include "../character.h"

static void fujin_ready(Fighter *fighter);
static void fujin_state_move(Fighter *fighter, int state);

static void fujin_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void fujin_script_lk_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(fujin_ready);
}

void fujin_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    fujin_entry_retail_script_004de6b8(fighter);
  } else {
    fujin_entry_retail_script_004de610(fighter);
  }
}

void fujin_script_current_reaction(Fighter *fighter, int reaction) {
  /* 480f80 -> 48f880/48fd00 installs this on the task's captured peer.
   * Replacing the owner here cancels the very stream issuing the reaction. */
  require(reaction == 102 || reaction == 109, "Unexpected Fujin LP victim release");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void fujin_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(fujin_ready);
    return;
  }
  require(false, "Unexpected Fujin opponent reaction");
}

void fujin_script_lp_extended_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fujin_ready);
}

void fujin_script_lp_short_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fujin_ready);
}

void fujin_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(fujin_ready);
}

void fujin_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void fujin_state_move(Fighter *fighter, int state) {
  fighter_start_move(fighter, state, fujin_moves[state]);
}

#include "../character.h"

static void tanya_ready(Fighter *fighter);

static void tanya_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void tanya_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void tanya_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Tanya opponent reaction");
}

void tanya_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

void tanya_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

void tanya_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 186) {
    /* 4e5798's opcode 41 installs reaction 186 (465200) on the victim. */
    tanya_second_reaction(fighter);
    return;
  }
  require(reaction == 104, "Unexpected tanya LP release reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void tanya_script_lp_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, tanya_ready);
}

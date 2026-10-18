#include "../character.h"

static void quan_chi_entry_469c80(Fighter *fighter);
static void quan_chi_entry_46a020(Fighter *fighter);
static void quan_chi_entry_47a370(Fighter *fighter);
static void quan_chi_entry_47cf30(Fighter *fighter);
static void quan_chi_entry_499740(Fighter *fighter);

void quan_chi_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void quan_chi_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void quan_chi_script_current_reaction(Fighter *fighter, int reaction) {
  require(reaction == 103, "Unexpected quan_chi LP release reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Quan Chi opponent reaction");
}

void quan_chi_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

static void quan_chi_entry_469c80(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void quan_chi_entry_46a020(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_47a370(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_47cf30(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_499740(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

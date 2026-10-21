#include "../character.h"

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

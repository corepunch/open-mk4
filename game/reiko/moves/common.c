#include "retail_vm.h"
#include "../character.h"

void reiko_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }

  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Reiko opponent reaction");
}

void reiko_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 107, "Unexpected LP victim reaction");
  battle_react(mk4_script_release_pair(fighter), reaction);
}

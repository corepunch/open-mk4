#include "retail_vm.h"
#include "../character.h"

void reptile_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Reptile opponent reaction");
}

/* 45b1f0 installs a reaction on the current task's captured peer: 102
 * releases the short LP throw victim; 4dff08 dispatches 180 to the
 * second-fatality victim. */
void reptile_script_lp_reaction(Fighter *fighter, int reaction) {
  if (reaction == 180) {
    battle_react(fighter_peer(fighter), reaction);
    return;
  }
  require(reaction == 102, "Unexpected LP victim reaction");
  battle_react(mk4_script_release_pair(fighter), reaction);
}

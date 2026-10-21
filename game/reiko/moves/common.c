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

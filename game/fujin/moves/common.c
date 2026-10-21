#include "../character.h"

void fujin_script_current_reaction(Fighter *fighter, int reaction) {
  /* 480f80 -> 48f880/48fd00 installs this on the task's captured peer.
   * Replacing the owner here cancels the very stream issuing the reaction. */
  if (reaction == 97 || reaction == 181) {
    battle_react((Fighter *)fighter_opponent(fighter), reaction);
    return;
  }
  require(reaction == 102 || reaction == 109, "Unexpected Fujin LP victim release");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void fujin_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Fujin opponent reaction");
}

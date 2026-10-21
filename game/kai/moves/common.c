#include "../character.h"

void kai_script_current_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 109, "Unexpected Kai LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void kai_script_lp_extended_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void kai_script_lp_short_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void kai_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Kai paired reaction");
}

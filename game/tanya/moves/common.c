#include "../character.h"

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
  fighter_finish_mmove(fighter, fighter_ready);
}

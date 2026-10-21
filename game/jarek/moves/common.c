#include "native_services.h"

void jarek_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    jarek_fatality_contact_hold(fighter);
    return;
  }
  require(reaction == 102 || reaction == 107, "Unexpected Jarek LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void jarek_script_opponent_reaction(Fighter *fighter, int reaction) {
  mk4_mmove_lk_release(fighter, reaction);
}

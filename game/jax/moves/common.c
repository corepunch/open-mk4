#include "native_services.h"

void jax_script_opponent_reaction(Fighter *fighter, int reaction) {
  mk4_mmove_lk_release(fighter, reaction);
}

/* 45b1f0 installs the reaction on the task peer: 183 from the head clap;
 * 100/125 end the LP slam, releasing the pair first. */
void jax_script_peer_reaction(Fighter *fighter, int reaction) {
  require(reaction == 100 || reaction == 125 || reaction == 183, "Unexpected Jax peer reaction");
  Fighter *victim = reaction == 183 ? (Fighter *)fighter_opponent(fighter) : mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

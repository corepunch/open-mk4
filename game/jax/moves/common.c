#include "../character.h"

void jax_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

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

void jax_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void jax_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void jax_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

void jax_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

void jax_script_lk_return(Fighter *fighter) {
  jax_entry_retail_script_004de358(fighter);
}

void jax_script_lk_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, jax_ready);
}

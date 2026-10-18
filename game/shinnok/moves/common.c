#include "../character.h"

void shinnok_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

void shinnok_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

void shinnok_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void shinnok_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void shinnok_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

void shinnok_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    shinnok_fatality_contact_hold(fighter);
    return;
  }
  require(reaction == 100, "Unexpected Shinnok LP release reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void shinnok_script_opponent_reaction(Fighter *fighter, int reaction) {
  mk4_mmove_lk_release(fighter, reaction);
}

void shinnok_input_target(Fighter *fighter, Button button) {
  fighter_press(fighter, button);
}

void shinnok_script_lp_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, shinnok_ready);
}

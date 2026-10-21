#include "retail_vm.h"
#include "../character.h"

static const MoveDef reptile_s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};

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
    battle_react((Fighter *)fighter_opponent(fighter), reaction);
    return;
  }
  require(reaction == 102, "Unexpected reptile LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &reptile_s_lp_thrown);
  retail_vm_enter_owned(victim, 0x4e9bd0, fighter_ready, 10);
}

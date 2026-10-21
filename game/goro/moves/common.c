#include "../character.h"

/* 4de6e4 passes reaction 100 to current-context dispatcher 45b1f0 while the
 * LP-throw victim is still captured; the owner continues its own clip. */
void goro_script_current_reaction(Fighter *fighter, int reaction) {
  require(reaction == 100, "Unexpected Goro LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void goro_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Goro opponent reaction");
  mk4_mmove_lk_release(fighter, reaction);
}

void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    goro_entry_retail_script_004de7e8(fighter);
    return;
  }
  goro_entry_retail_script_004de6e4(fighter);
}

void goro_script_lp_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 43a5c0 -> 43a610 disturbs resting world objects of Goro's own descriptor
 * type (4ecf18 word 0 = 0x6c) after his stomp landings. */
void goro_quake_resting_weapons(void) {
  weapon_hop_resting((int)goro_retail_weapon_descriptor[0]);
}

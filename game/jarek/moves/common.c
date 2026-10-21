#include "retail_vm.h"
#include "../character.h"

void jarek_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  jarek_entry_retail_script_004de664(victim);
  retail_vm_enter_owned(fighter, 0x4de668, fighter_ready, 12);
}

void jarek_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

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

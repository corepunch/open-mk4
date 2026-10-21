#include "native_services.h"

#include "retail_data.h"

/* 4de6e4 passes reaction 100 to current-context dispatcher 45b1f0 while the
 * LP-throw victim is still captured; the owner continues its own clip. */
void goro_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Goro opponent reaction");
  mk4_mmove_lk_release(fighter, reaction);
}

void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    retail_vm_enter_owned(fighter, 0x4de7e8, fighter_ready, 15);
    return;
  }
  retail_vm_enter_owned(fighter, 0x4de6e4, fighter_ready, 15);
}

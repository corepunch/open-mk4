#include "../character.h"

void goro_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

/* 467e70 is roster-generic; Goro reaches it from the CPU stage-0 finish
 * (434520 -> 4348b0 -> action 10042), not from a command record. */
void goro_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  goro_entry_retail_script_004e6950(fighter);
}

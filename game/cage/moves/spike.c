#include "../character.h"

void cage_spike_fatality(Fighter *fighter);

void cage_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void cage_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  cage_entry_retail_script_004e6950(fighter);
}

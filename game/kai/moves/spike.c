#include "../character.h"

void kai_spike_fatality(Fighter *fighter);

void kai_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void kai_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  kai_entry_retail_script_004e6950(fighter);
}

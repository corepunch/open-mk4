#include "../character.h"

void jarek_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void jarek_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  jarek_entry_retail_script_004e6950(fighter);
}

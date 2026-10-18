#include "../character.h"

void tanya_spike_fatality(Fighter *fighter);

void tanya_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void tanya_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  tanya_entry_retail_script_004e6950(fighter);
}

#include "../character.h"

void reptile_spike_fatality(Fighter *fighter);

void reptile_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void reptile_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  reptile_entry_retail_script_004e6950(fighter);
}

void reptile_entry_spike(Fighter *fighter) {
  reptile_spike_fatality(fighter);
}

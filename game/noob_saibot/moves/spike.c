#include "../character.h"

void noob_saibot_spike_fatality(Fighter *fighter);

void noob_saibot_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void noob_saibot_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  noob_saibot_entry_retail_script_004e6950(fighter);
}

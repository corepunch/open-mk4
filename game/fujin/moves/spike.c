#include "../character.h"

void fujin_script_spike_tail(Fighter *fighter);
static void fujin_spike_fatality(Fighter *fighter);
void fujin_entry_spike(Fighter *fighter);

void fujin_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void fujin_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  fujin_entry_retail_script_004e6950(fighter);
}

void fujin_entry_spike(Fighter *fighter) {
  fujin_spike_fatality(fighter);
}

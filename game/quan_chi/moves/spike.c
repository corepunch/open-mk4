#include "../character.h"

static void quan_chi_spike_fatality(Fighter *fighter);

void quan_chi_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void quan_chi_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, quan_chi_entry_retail_script_004e6950, NULL);
}

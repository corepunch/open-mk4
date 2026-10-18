#include "../character.h"

void goro_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void goro_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, goro_entry_retail_script_004e6950, NULL);
}

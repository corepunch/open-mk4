#include "../character.h"

static void reiko_spike_fatality(Fighter *fighter);

void reiko_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void reiko_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, reiko_entry_retail_script_004e6950, NULL);
}

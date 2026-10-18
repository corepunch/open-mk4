#include "../character.h"

static void fujin_script_spike_tail(Fighter *fighter);
static void fujin_spike_fatality(Fighter *fighter);
static void fujin_entry_spike(Fighter *fighter);

static void fujin_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void fujin_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, fujin_entry_retail_script_004e6950, NULL);
}

static void fujin_entry_spike(Fighter *fighter) {
  fujin_spike_fatality(fighter);
}

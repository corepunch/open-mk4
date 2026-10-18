#include "../character.h"

void raiden_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void raiden_spike_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 30 - g_battle.phase_ticks));
  raiden_entry_retail_script_004e6950(fighter);
}

void raiden_spike_fatality(Fighter *fighter) {
  raiden_arena_fatality(fighter, FINISH_SPIKE);
}

bool raiden_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && raiden_can_close_fatality(fighter);
}

#include "../character.h"

void kitana_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void kitana_spike_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 30 - g_battle.phase_ticks));
  kitana_entry_retail_script_004e6950(fighter);
}

void kitana_spike_fatality(Fighter *fighter) {
  kitana_arena_fatality(fighter, FINISH_SPIKE);
}

bool kitana_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && kitana_can_close_fatality(fighter);
}

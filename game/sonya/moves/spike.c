#include "../character.h"

void sonya_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void sonya_spike_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 30 - g_battle.phase_ticks));
  sonya_entry_retail_script_004e6950(fighter);
}

void sonya_spike_fatality(Fighter *fighter) {
  sonya_arena_fatality(fighter, FINISH_SPIKE);
}

bool sonya_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && sonya_can_close_fatality(fighter);
}

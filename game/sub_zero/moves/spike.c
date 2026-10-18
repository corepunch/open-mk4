#include "../character.h"

bool sub_zero_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && sub_zero_can_close_fatality(fighter);
}

void sub_zero_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void sub_zero_spike_start(Fighter *fighter) {
  native_hold(fighter, maxi(1, 30 - g_battle.phase_ticks), sub_zero_entry_retail_script_004e6950);
}

void sub_zero_spike_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SPIKE);
}

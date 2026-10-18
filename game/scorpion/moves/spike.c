#include "../character.h"

bool scorpion_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && scorpion_can_beast_fatality(fighter);
}

void scorpion_spike_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_SPIKE);
}

void scorpion_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void scorpion_spike_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 30 - g_battle.phase_ticks));
  scorpion_entry_retail_script_004e6950(fighter);
}

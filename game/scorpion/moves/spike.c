#include "../character.h"

void scorpion_spike_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_SPIKE);
}

void scorpion_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void scorpion_spike_start(Fighter *fighter) {
  int frames = 30 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, scorpion_entry_retail_script_004e6950);
  } else {
    scorpion_entry_retail_script_004e6950(fighter);
  }
}

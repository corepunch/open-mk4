#include "../character.h"

void sonya_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void sonya_spike_start(Fighter *fighter) {
  int frames = 30 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, sonya_entry_retail_script_004e6950);
  } else {
    sonya_entry_retail_script_004e6950(fighter);
  }
}

void sonya_spike_fatality(Fighter *fighter) {
  sonya_arena_fatality(fighter, FINISH_SPIKE);
}

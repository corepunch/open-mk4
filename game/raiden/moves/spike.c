#include "../character.h"

void raiden_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void raiden_spike_start(Fighter *fighter) {
  int frames = 30 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, raiden_entry_retail_script_004e6950);
  } else {
    raiden_entry_retail_script_004e6950(fighter);
  }
}

void raiden_spike_fatality(Fighter *fighter) {
  raiden_arena_fatality(fighter, FINISH_SPIKE);
}

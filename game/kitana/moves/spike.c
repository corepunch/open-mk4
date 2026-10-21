#include "../character.h"

void kitana_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void kitana_spike_start(Fighter *fighter) {
  int frames = 30 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, kitana_entry_retail_script_004e6950);
  } else {
    kitana_entry_retail_script_004e6950(fighter);
  }
}

void kitana_spike_fatality(Fighter *fighter) {
  kitana_arena_fatality(fighter, FINISH_SPIKE);
}

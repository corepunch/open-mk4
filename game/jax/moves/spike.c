#include "../character.h"

void jax_spike_fatality(Fighter *fighter);

void jax_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

void jax_spike_fatality(Fighter *fighter) {
  if (!mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    return;
  }

  jax_entry_retail_script_004e6950(fighter);
}

void jax_command_spike(Fighter *fighter) {
  jax_spike_fatality(fighter);
}

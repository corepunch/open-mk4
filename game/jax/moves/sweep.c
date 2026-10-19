#include "../character.h"

void jax_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &jax_s_sweep);
  fighter->sample_override = 0;
  jax_entry_retail_script_004e6798(fighter);
}

void jax_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jax_sweep);
}

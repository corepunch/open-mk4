#include "../character.h"

void jax_sweep(Fighter *fighter) {
  jax_start(fighter, S_SWEEP, &jax_s_sweep, jax_entry_retail_script_004e6798);
}

void jax_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jax_sweep);
}

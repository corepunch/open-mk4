#include "../character.h"

void jax_uppercut(Fighter *fighter) {
  jax_start(fighter, S_UPPERCUT, &jax_s_uppercut, jax_entry_retail_script_004e62a0);
}

void jax_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jax_uppercut);
}

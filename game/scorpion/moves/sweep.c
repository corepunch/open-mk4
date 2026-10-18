#include "../character.h"

void scorpion_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &scorpion_s_sweep);
  scorpion_entry_retail_script_004e6798(fighter);
}

void scorpion_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void scorpion_script_sweep_tail(Fighter *fighter) {
  native_standard_sweep(fighter);
}

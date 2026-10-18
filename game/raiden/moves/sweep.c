#include "../character.h"

void raiden_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &raiden_s_sweep);
  raiden_entry_retail_script_004e6798(fighter);
}

void raiden_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void raiden_script_sweep_tail(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

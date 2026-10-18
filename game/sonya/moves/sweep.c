#include "../character.h"

void sonya_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &sonya_s_sweep);
  sonya_entry_retail_script_004e6798(fighter);
}

void sonya_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void sonya_script_sweep_tail(Fighter *fighter) {
  native_standard_sweep(fighter);
}

#include "../character.h"

void sub_zero_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &sub_zero_s_sweep);
  sub_zero_entry_retail_script_004e6798(fighter);
}

void sub_zero_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void sub_zero_script_sweep_tail(Fighter *fighter) {
  native_standard_sweep(fighter);
}

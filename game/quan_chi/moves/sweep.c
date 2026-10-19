#include "../character.h"

void quan_chi_script_sweep_finish(Fighter *fighter) {
  native_standard_sweep(fighter);
}

void quan_chi_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &quan_chi_s_sweep);
  fighter->sample_override = 0;
  quan_chi_entry_retail_script_004e6798(fighter);
}

void quan_chi_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_sweep);
}

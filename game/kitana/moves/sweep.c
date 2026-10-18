#include "../character.h"

void kitana_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &kitana_s_sweep);
  kitana_entry_retail_script_004e6798(fighter);
}

void kitana_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void kitana_script_sweep_tail(Fighter *fighter) {
  native_standard_sweep(fighter);
}

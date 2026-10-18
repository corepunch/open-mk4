#include "../character.h"

void liu_kang_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &liu_kang_s_sweep);
  liu_kang_entry_retail_script_004e67d8(fighter);
}

void liu_kang_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void liu_kang_liu_script_sweep_tail(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

#include "../character.h"

void shinnok_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &shinnok_s_sweep);
  shinnok_entry_retail_script_004e6798(fighter);
}

void shinnok_script_sweep_finish(Fighter *fighter) {
  native_standard_sweep(fighter);
}

void shinnok_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_sweep);
}

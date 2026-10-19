#include "../character.h"

void jarek_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &jarek_s_sweep);
  fighter->sample_override = 0;
  jarek_entry_retail_script_004e6798(fighter);
}

void jarek_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_sweep);
}

void jarek_script_sweep_tail(Fighter *fighter) {
  native_standard_sweep(fighter);
}

#include "../character.h"

void reptile_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &reptile_s_sweep);
  fighter->sample_override = 0;
  reptile_entry_retail_script_004e6798(fighter);
}

void reptile_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_sweep);
}

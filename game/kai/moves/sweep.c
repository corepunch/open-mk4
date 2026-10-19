#include "../character.h"

void kai_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, kai_sweep);
}

void kai_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &kai_s_sweep);
  kai_entry_retail_script_004e6798(fighter);
}

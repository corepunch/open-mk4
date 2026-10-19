#include "../character.h"

void tanya_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &tanya_s_sweep);
  tanya_entry_retail_script_004e6798(fighter);
}

void tanya_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_sweep);
}

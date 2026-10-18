#include "../character.h"

void tanya_sweep(Fighter *fighter) {
  tanya_start_script(fighter, S_SWEEP, &tanya_s_sweep, tanya_entry_retail_script_004e6798);
}

void tanya_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_sweep);
}

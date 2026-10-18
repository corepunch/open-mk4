#include "../character.h"

void kai_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_478df0);
}

void kai_sweep(Fighter *fighter) {
  kai_start_script(fighter, S_SWEEP, &kai_s_sweep, kai_entry_retail_script_004e6798);
}

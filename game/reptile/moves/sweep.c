#include "../character.h"

void reptile_sweep(Fighter *fighter) {
  reptile_start(fighter, S_SWEEP, &reptile_s_sweep, reptile_entry_retail_script_004e6798);
}

void reptile_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_sweep);
}

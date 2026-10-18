#include "../character.h"

void reiko_sweep(Fighter *fighter) {
  reiko_start_script(fighter, S_SWEEP, &reiko_s_sweep, reiko_entry_retail_script_004e6798);
}

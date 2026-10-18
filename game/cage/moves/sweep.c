#include "../character.h"

void cage_sweep(Fighter *fighter) {
  cage_start_script(fighter, S_SWEEP, &cage_s_sweep, cage_entry_retail_script_004e6798);
}

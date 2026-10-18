#include "../character.h"

void cage_uppercut(Fighter *fighter) {
  cage_start_script(fighter, S_UPPERCUT, &cage_s_uppercut, cage_entry_retail_script_004e62a0);
}

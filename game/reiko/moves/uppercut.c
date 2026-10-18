#include "../character.h"

void reiko_uppercut(Fighter *fighter) {
  reiko_start_script(fighter, S_UPPERCUT, &reiko_s_uppercut, reiko_entry_retail_script_004e62a0);
}

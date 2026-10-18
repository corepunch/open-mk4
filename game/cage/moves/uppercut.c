#include "../character.h"

void cage_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &cage_s_uppercut);
  cage_entry_retail_script_004e62a0(fighter);
}

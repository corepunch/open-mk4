#include "../character.h"

void tanya_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &tanya_s_uppercut);
  tanya_entry_retail_script_004e62a0(fighter);
}

void tanya_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_uppercut);
}

#include "../character.h"

void tanya_uppercut(Fighter *fighter) {
  tanya_start_script(fighter, S_UPPERCUT, &tanya_s_uppercut, tanya_entry_retail_script_004e62a0);
}

void tanya_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_uppercut);
}

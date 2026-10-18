#include "../character.h"

void kai_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, kai_uppercut);
}

void kai_uppercut(Fighter *fighter) {
  kai_start_script(fighter, S_UPPERCUT, &kai_s_uppercut, kai_entry_retail_script_004e62a0);
}

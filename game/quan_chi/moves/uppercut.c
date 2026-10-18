#include "../character.h"

void quan_chi_uppercut(Fighter *fighter) {
  quan_chi_start(fighter, S_UPPERCUT, &quan_chi_s_uppercut, quan_chi_entry_retail_script_004e62a0);
}

void quan_chi_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_uppercut);
}

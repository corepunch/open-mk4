#include "../character.h"

void quan_chi_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &quan_chi_s_uppercut);
  quan_chi_entry_retail_script_004e62a0(fighter);
}

void quan_chi_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_uppercut);
}

void quan_chi_script_finish(Fighter *fighter) {
  native_uppercut_default(fighter);
}

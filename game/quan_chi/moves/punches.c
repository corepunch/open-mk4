#include "../character.h"

static void quan_chi_high_punch(Fighter *fighter);

void quan_chi_jab(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &quan_chi_s_jab, quan_chi_entry_retail_script_004e62f0);
}

static void quan_chi_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &quan_chi_s_jab);
}

void quan_chi_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_jab);
}

void quan_chi_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? quan_chi_lp_throw : quan_chi_jab);
}

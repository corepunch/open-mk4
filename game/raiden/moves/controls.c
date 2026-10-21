#include "../character.h"

void raiden_press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, mk4_can_ground_throw(fighter, false) ? raiden_enter_throw
                             : fighter->armed                     ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                                  : retail_jab);
}

void raiden_press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_sweep_state : mk4_can_ground_throw(fighter, true) ? raiden_enter_lk_throw : retail_low_kick);
}

void raiden_press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_roundhouse : retail_high_kick);
}

void raiden_cpu_close_hk(Fighter *fighter) {
  raiden_ground_attack(fighter, G_CLOSE_HK);
}

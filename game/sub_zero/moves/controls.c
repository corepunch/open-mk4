#include "../character.h"

void sub_zero_press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, mk4_can_ground_throw(fighter, false) ? retail_lp_throw
                             : fighter->armed                     ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                                  : retail_jab);
}

void sub_zero_press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_sweep_state : mk4_can_ground_throw(fighter, true) ? retail_lk_throw : retail_low_kick);
}

void sub_zero_cpu_close_hk(Fighter *fighter) {
  retail_close_high_kick(fighter);
}

#include "../character.h"

void liu_kang_press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, liu_kang_can_ground_throw(fighter, false) ? liu_kang_enter_throw
                             : fighter->armed                          ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                                       : liu_kang_jab);
}

void liu_kang_press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter)                ? liu_kang_enter_sweep
                             : liu_kang_can_ground_throw(fighter, true) ? liu_kang_enter_lk_throw
                                                                        : retail_low_kick);
}

void liu_kang_press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : retail_high_kick);
}

void liu_kang_cpu_close_hk(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_CLOSE_HK);
}

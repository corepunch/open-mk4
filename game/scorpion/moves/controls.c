#include "../character.h"

void scorpion_press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp) : retail_jab;
  fighter_interrupt(fighter, mk4_can_ground_throw(fighter, false) ? retail_lp_throw : fighter->armed ? entry : retail_jab);
}

void scorpion_press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter)           ? retail_sweep_state
                             : mk4_can_ground_throw(fighter, true) ? scorpion_enter_lk_throw
                                                                   : retail_low_kick);
}

void scorpion_press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_roundhouse : retail_high_kick);
}

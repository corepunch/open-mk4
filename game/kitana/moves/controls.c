#include "../character.h"

void kitana_press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_can_ground_throw(fighter, false) ? kitana_enter_throw
                             : fighter->armed                        ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                                     : retail_jab);
}

void kitana_press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter)              ? retail_sweep_state
                             : kitana_can_ground_throw(fighter, true) ? kitana_enter_lk_throw
                                                                      : retail_low_kick);
}

void kitana_press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? retail_roundhouse : retail_high_kick);
}

void kitana_command_shadow_low(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_low);
}

void kitana_command_shadow_up(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_up);
}

void kitana_cpu_close_hk(Fighter *fighter) {
  kitana_ground_attack(fighter, G_CLOSE_HK);
}

static void press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, can_ground_throw(fighter, false) ? enter_throw
                             : fighter->armed                 ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                              : sonya_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : sonya_lk);
}

static void press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sonya_roundhouse : sonya_kick);
}

static void cpu_close_hk(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HK);
}

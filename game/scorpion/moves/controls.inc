static void press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp) : scorpion_jab;
  fighter_interrupt(fighter, can_ground_throw(fighter, false) ? enter_throw : fighter->armed ? entry : scorpion_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : scorpion_lk);
}

static void press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_kick);
}

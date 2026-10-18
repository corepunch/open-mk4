#include "../character.h"

static void quan_chi_high_punch(Fighter *fighter);

void quan_chi_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &quan_chi_s_jab);
  quan_chi_entry_retail_script_004e62f0(fighter);
}

static void quan_chi_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    quan_chi_close_hp(fighter);
  } else {
    quan_chi_jab(fighter);
  }
}

void quan_chi_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_high_punch);
}

void quan_chi_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? quan_chi_lp_throw : quan_chi_jab);
}

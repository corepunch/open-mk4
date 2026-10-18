#include "../character.h"

void tanya_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &tanya_s_jab);
  tanya_entry_retail_script_004e62f0(fighter);
}

void tanya_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    tanya_close_hp(fighter);
  } else {
    tanya_jab(fighter);
  }
}

void tanya_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_high_punch);
}

void tanya_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? tanya_lp_throw : tanya_jab);
}

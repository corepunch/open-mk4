#include "../character.h"

static void kai_low_punch(Fighter *fighter);

void kai_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &kai_s_jab);
  kai_entry_retail_script_004e62f0(fighter);
}

void kai_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    kai_close_hp(fighter);
  } else {
    kai_jab(fighter);
  }
}

static void kai_low_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &kai_s_jab);
  kai_entry_retail_script_004e62f0(fighter);
}

void kai_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47bd10);
}

void kai_press_low_punch(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, false)) {
    fighter_interrupt(fighter, kai_lp_throw);
    return;
  }
  fighter_interrupt(fighter, kai_low_punch);
}

#include "../character.h"

void tanya_jab(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &tanya_s_jab, tanya_entry_retail_script_004e62f0);
}

void tanya_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &tanya_s_jab);
}

void tanya_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_jab);
}

void tanya_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? tanya_lp_throw : tanya_jab);
}

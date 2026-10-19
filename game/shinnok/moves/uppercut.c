#include "../character.h"

void shinnok_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &shinnok_s_uppercut);
  shinnok_entry_retail_script_004e62a0(fighter);
}

void shinnok_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_uppercut);
}

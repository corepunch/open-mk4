#include "../character.h"

void jarek_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &jarek_s_uppercut);
  jarek_entry_retail_script_004e62a0(fighter);
}

void jarek_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_uppercut);
}

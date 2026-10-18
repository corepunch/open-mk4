#include "../character.h"

static void kai_low_punch(Fighter *fighter);

void kai_jab(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &kai_s_jab, kai_entry_retail_script_004e62f0);
}

void kai_high_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &kai_s_air_attack, kai_entry_retail_script_004e8210);
}

static void kai_low_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &kai_s_jab, kai_entry_retail_script_004e62f0);
}

void kai_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47bd10);
}

void kai_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_low_punch);
}

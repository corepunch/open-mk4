#include "../character.h"

static void kai_low_kick(Fighter *fighter);

void kai_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_high_kick);
}

void kai_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47c520);
}

void kai_high_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &kai_s_high_kick, kai_entry_retail_script_004e5ca0);
}

static void kai_low_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &kai_s_low_kick, kai_entry_retail_script_004e5d90);
}

void kai_script_kick_finish(Fighter *fighter) {
  kai_entry_retail_script_004e5c70(fighter);
}

#include "../character.h"

static void tanya_low_kick(Fighter *fighter);

void tanya_script_kick_finish(Fighter *fighter) {
  tanya_entry_retail_script_004e5c70(fighter);
}

void tanya_high_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &tanya_s_kick, tanya_entry_retail_script_004e5ca0);
}

static void tanya_low_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &tanya_s_kick, tanya_entry_retail_script_004e5d90);
}

void tanya_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_high_kick);
}

void tanya_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? tanya_sweep : tanya_low_kick);
}

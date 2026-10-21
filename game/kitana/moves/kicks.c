#include "../character.h"

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */

void kitana_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &kitana_s_kick);
  kitana_entry_retail_script_004e5ca0(fighter);
}

void kitana_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kitana_roundhouse : kitana_hk);
}

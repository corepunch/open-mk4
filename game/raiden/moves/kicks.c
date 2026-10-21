#include "../character.h"

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */

void raiden_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &raiden_s_kick);
  raiden_entry_retail_script_004e5ca0(fighter);
}

void raiden_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? raiden_roundhouse : raiden_hk);
}

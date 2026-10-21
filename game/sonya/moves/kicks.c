#include "../character.h"

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */

void sonya_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &sonya_s_kick);
  sonya_entry_retail_script_004e5ca0(fighter);
}

void sonya_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sonya_roundhouse : sonya_hk);
}

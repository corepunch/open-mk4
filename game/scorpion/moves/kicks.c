#include "../character.h"

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */

void scorpion_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &scorpion_s_kick);
  scorpion_entry_retail_script_004e5ca0(fighter);
}

void scorpion_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_hk);
}

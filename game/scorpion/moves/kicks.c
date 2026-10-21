#include "../character.h"

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */

void scorpion_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_hk);
}

#include "../character.h"

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */

void kitana_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kitana_roundhouse : kitana_hk);
}

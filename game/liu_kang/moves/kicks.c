#include "../character.h"

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */

void liu_kang_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &liu_kang_s_kick);
  liu_kang_entry_retail_script_004e6c90(fighter);
}

void liu_kang_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : liu_kang_hk);
}

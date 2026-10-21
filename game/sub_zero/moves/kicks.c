#include "../character.h"

// clang-format off: Keep table entries on one row, including long rows.

// clang-format on

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */

void sub_zero_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &sub_zero_s_kick);
  sub_zero_entry_retail_script_004e5c80(fighter);
}

void sub_zero_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sub_zero_roundhouse : sub_zero_hk);
}

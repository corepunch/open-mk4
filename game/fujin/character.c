#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */

/* Retail inventory descriptor 0x4ece98, preserved as encoded EXE words. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t fujin_weapon_descriptor[] = {
  0x68,
  0x141b6b,
  0x142dc4,
  0x13b360,
  0x13edd5,
  0x13eea4,
  0x13eea7,
  0xffffcccd,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void fujin_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_mmove_thrown_finish, 5);
}

void fujin_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f58, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f80, fighter_ready, 5);
}

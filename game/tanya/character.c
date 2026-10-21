#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Retail Tanya uses the shared standing/crouching button tables. Air attacks
 * and LK throw remain explicit entries for command and CPU callers. */

/* Retail descriptor 4ecd98: preserve the encoded eight-word data block. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t tanya_weapon_descriptor[] = {
  0x6d,
  0x141bfe,
  0x142dd9,
  0x13b31e,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffcccd,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void tanya_entry_retail_script_004ed268(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed268, fighter_ready, 9);
}

void tanya_entry_retail_script_004ed2c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e81f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81f0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 9);
}

void tanya_entry_retail_script_004ed190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed190, fighter_ready, 9);
}

void tanya_entry_retail_script_004e9db8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9db8, mk4_mmove_thrown_finish, 9);
}

void tanya_entry_retail_script_004e8bb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bb0, mk4_mmove_thrown_finish, 9);
}

void tanya_entry_retail_script_004e5798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5798, fighter_ready, 9);
}

void tanya_entry_retail_script_004e57c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e57c0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e5820(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5820, fighter_ready, 9);
}

void tanya_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b38, fighter_ready, 9);
}

void tanya_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 9);
}

void tanya_entry_retail_script_004e1598(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1598, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15a8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15b8, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15d0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15e0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e15f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e15f0, fighter_ready, 9);
}

void tanya_entry_retail_script_004e1600(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1600, fighter_ready, 9);
}

void tanya_entry_retail_script_004e16d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e16d0, fighter_ready, 9);
}

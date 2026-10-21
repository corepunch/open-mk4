#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reiko_weapon_descriptor[] = {
  0x69,
  0x141b95,
  0x142dca,
  0x13b33c,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffb334,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void reiko_entry_retail_script_004de664(Fighter *fighter) {
  /* 4de664 is a490350 callback table, not a VM body. */
  fighter->facing = -fighter->facing;
}

void reiko_entry_retail_script_004de668(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de668, fighter_ready, 7);
}

void reiko_entry_retail_script_004e9b40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b40, fighter_ready, 7);
}

void reiko_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 7);
}

void reiko_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_script_thrown_ready, 7);
}

void reiko_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 7);
}

void reiko_entry_retail_script_004ed038(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed038, fighter_ready, 7);
}

void reiko_entry_retail_script_004e97f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e97f8, fighter_stop_mmove, 7);
}

void reiko_entry_retail_script_004e2cf0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2cf0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d18(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d18, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d28(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d28, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d38, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d48(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d48, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d58, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d68, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d78, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2d88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2d88, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2dc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2dc0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2dd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2dd0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2de0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2de0, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2df8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2df8, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2e08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e08, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2e18(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e18, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2e40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e40, fighter_ready, 7);
}

void reiko_entry_retail_script_004e2e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e50, fighter_ready, 7);
}

void reiko_entry_retail_script_004d2d10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2d10, fighter_ready, 7);
}

void reiko_entry_retail_script_004d2d60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2d60, fighter_ready, 7);
}

void reiko_entry_retail_script_004d2d90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2d90, fighter_ready, 7);
}

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

void fujin_entry_retail_script_004e8258(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8258, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8298(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8298, fighter_ready, 5);
}

void fujin_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 5);
}

void fujin_entry_retail_script_004ed408(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed408, fighter_ready, 5);
}

void fujin_entry_retail_script_004ed788(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed788, fujin_bow_followup_next, 5);
}

void fujin_entry_retail_script_004e9e10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9e10, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c00(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c00, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c20, fighter_ready, 5);
}

void fujin_entry_retail_script_004e5c50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c50, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9fe0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9fe0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9ff0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ff0, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8740(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8740, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8770(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8770, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_mmove_thrown_finish, 5);
}

void fujin_entry_retail_script_004ea050(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea050, mk4_mmove_thrown_finish, 5);
}

void fujin_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e8830(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8830, fighter_ready, 5);
}

void fujin_entry_retail_script_004e9fb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9fb0, fighter_ready, 5);
}

void fujin_entry_retail_script_004dfed0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfed0, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea2d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2d8, fighter_ready, 5);
}

void fujin_entry_retail_script_004ea2a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2a8, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f58, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f80, fighter_ready, 5);
}

void fujin_entry_retail_script_004e1f98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1f98, fighter_ready, 5);
}

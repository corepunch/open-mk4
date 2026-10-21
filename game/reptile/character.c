#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */

const MoveDef reptile_s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};

/* Reptile's retail inventory descriptor at 0x4ece78, preserved as encoded words. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reptile_weapon_descriptor[] = {
  0x67,
  0x141b64,
  0x142dc1,
  0x13b35a,
  0x13eeb9,
  0x13ee9e,
  0x13eea1,
  0xffff999a,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void reptile_entry_retail_script_004e8128(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8128, fighter_ready, 10);
}

void reptile_entry_retail_script_004e8178(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8178, fighter_ready, 10);
}

void reptile_entry_retail_script_004e9620(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9620, fighter_ready, 10);
}

void reptile_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, fighter_ready, 10);
}

void reptile_entry_retail_script_004ed1e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed1e8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e97f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e97f8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e5bb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5bb0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e95f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e95f0, fighter_ready, 10);
}

void reptile_entry_retail_script_004dff08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dff08, fighter_ready, 10);
}

void reptile_entry_retail_script_004ea290(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea290, fighter_ready, 10);
}

void reptile_entry_retail_script_004ea2a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2a8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e60, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e70(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e70, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e98, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ea8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ea8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2eb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2eb0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ec0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ec0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ed0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ed0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 10);
}

void reptile_entry_retail_script_004ec640(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec640, fighter_ready, 10);
}

void reptile_entry_retail_script_004e8bc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 10);
}

void reptile_entry_retail_script_004e8c98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8c98, mk4_mmove_thrown_finish, 10);
}

void reptile_entry_retail_script_004e8cc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 10);
}

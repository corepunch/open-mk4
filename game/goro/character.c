#include "retail_vm.h"
#include "character.h"

/* Encoded Goro roster descriptor retained from the EXE inventory at 0x4ecf18. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t goro_retail_weapon_descriptor[] = {
  0x6c,
  0x141bf0,
  0x142dd3,
  0x13b330,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0x0,
};
// clang-format on

/* Combat values remain the native approximation until measured against retail. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Goro's roster slot selects the shared stance tables 4e5058/4e5070/4e6ec8
 * (typed inventory), so stance presses enter the same native selectors as
 * the humanoid roster with Goro's own script slots and clip choices. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void goro_entry_retail_script_004de6e4(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6e4, fighter_ready, 15);
}

void goro_entry_retail_script_004de7e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de7e8, fighter_ready, 15);
}

void goro_entry_retail_script_004dfb60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfb60, fighter_ready, 15);
}

void goro_entry_retail_script_004dfb70(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfb70, fighter_ready, 15);
}

void goro_entry_retail_script_004dfbb8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfbb8, fighter_ready, 15);
}

void goro_entry_retail_script_004ed790(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed790, fighter_ready, 15);
}

void goro_entry_retail_script_004dfbd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfbd0, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc38, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc50, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc60, fighter_ready, 15);
}

void goro_entry_retail_script_004e8dd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8dd0, fighter_ready, 15);
}

void goro_entry_retail_script_004e8e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e50, fighter_ready, 15);
}

void goro_entry_retail_script_004e8e80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e80, fighter_ready, 15);
}

void goro_entry_retail_script_004e95c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e95c8, fighter_ready, 15);
}

void goro_entry_retail_script_004ea3e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea3e0, fighter_ready, 15);
}

void goro_entry_retail_script_004ea400(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea400, fighter_ready, 15);
}

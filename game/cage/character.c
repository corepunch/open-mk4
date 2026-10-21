#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */

const MoveDef cage_s_cpu_kick_script = {.clip = 308, .last = 24, .flags = SF_LOCK | SF_RECORD};
const MoveDef cage_s_powered_punch_script = {.clip = 85, .last = 54, .flags = SF_LOCK | SF_RECORD};

/* Contact records from Cage's 53d2b8/53d268 tables, selected by the
 * generated controller callbacks479b70/479f00. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t cage_weapon_descriptor[] = {
  0x6e,
  0x1430cb,
  0x144359,
  0x13b318,
  0x13eeb9,
  0x13ee9e,
  0x13eea1,
  0xffffb334,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void cage_entry_retail_script_004e12a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12a0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12b0, fighter_ready, 14);
}

void cage_entry_retail_script_004e5870(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5870, fighter_ready, 14);
}

void cage_entry_retail_script_004e5ba8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ba8, fighter_ready, 14);
}

void cage_entry_retail_script_004e80a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e80a8, fighter_ready, 14);
}

void cage_entry_retail_script_004e80d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e80d0, fighter_ready, 14);
}

void cage_entry_retail_script_004e9f78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9f78, mk4_script_thrown_ready, 14);
}

void cage_entry_retail_script_004ed0d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed0d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 14);
}

void cage_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 14);
}

void cage_entry_retail_script_004ec4f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec4f0, fighter_ready, 14);
}

void cage_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7f0, fighter_ready, 14);
}

void cage_entry_retail_script_004e8cc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 14);
}

void cage_entry_retail_script_004e8d38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8d38, mk4_mmove_thrown_finish, 14);
}

void cage_entry_retail_script_004e5880(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5880, fighter_ready, 14);
}

void cage_entry_retail_script_004e58d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e58d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e5900(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5900, fighter_stop_mmove, 14);
}

void cage_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b38, fighter_ready, 14);
}

void cage_entry_retail_script_004e12c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12c0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12d0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12e0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12f0, fighter_ready, 14);
}

void cage_entry_retail_script_004e1300(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1300, fighter_ready, 14);
}

void cage_entry_retail_script_004e1310(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1310, fighter_ready, 14);
}

void cage_entry_retail_script_004e1320(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1320, fighter_ready, 14);
}

void cage_entry_retail_script_004e1330(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1330, fighter_ready, 14);
}

void cage_entry_retail_script_004e1340(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1340, fighter_ready, 14);
}

void cage_entry_retail_script_004e1350(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1350, fighter_ready, 14);
}

void cage_entry_retail_script_004e1360(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1360, fighter_ready, 14);
}

void cage_entry_retail_script_004e1370(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1370, fighter_ready, 14);
}

void cage_entry_retail_script_004e1380(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1380, fighter_ready, 14);
}

void cage_entry_retail_script_004e9718(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9718, fighter_ready, 14);
}

void cage_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, fighter_ready, 14);
}

void cage_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 14);
}

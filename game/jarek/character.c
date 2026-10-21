#include "retail_vm.h"
#include "character.h"

const MoveDef jarek_s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void jarek_entry_retail_script_004de664(Fighter *fighter) {
  /* 4de664 is callback data consumed by48fcd0, not a VM stream.
   * Its native490350 toggles the captured peer facing. */
  fighter->facing = -fighter->facing;
}

void jarek_entry_retail_script_004de668(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de668, fighter_ready, 12);
}

void jarek_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 12);
}

void jarek_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 12);
}

void jarek_entry_retail_script_004e81a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81a8, fighter_ready, 12);
}

void jarek_entry_retail_script_004ed118(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed118, fighter_ready, 12);
}

void jarek_entry_retail_script_004ec4f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec4f0, fighter_ready, 12);
}

void jarek_entry_retail_script_004e8c08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8c08, mk4_mmove_thrown_finish, 12);
}

void jarek_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 12);
}

void jarek_entry_retail_script_004e9658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9658, fighter_ready, 12);
}

void jarek_entry_retail_script_004e81c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81c8, fighter_ready, 12);
}

void jarek_entry_retail_script_004d2db0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2db0, fighter_ready, 12);
}

void jarek_entry_retail_script_004d2de0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2de0, fighter_ready, 12);
}

void jarek_entry_retail_script_004d2e10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2e10, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1220(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1220, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1230(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1230, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1248(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1248, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1260(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1260, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1270, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1280(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1280, fighter_ready, 12);
}

void jarek_entry_retail_script_004e1290(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1290, fighter_ready, 12);
}

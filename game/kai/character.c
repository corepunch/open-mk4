#include "retail_vm.h"
#include "character.h"

/* Kai's move clips and contact timing come from the slot-11 retail tables.
 * Script bodies are emitted from game/kai/port.json. */
const MoveDef kai_s_low_kick = {70, 0, MOVE_END, 9, 5, 18, 12, 28, 1.55f, 0.27f, SF_ATTACK | SF_LOCK | SF_LOW, NULL, 0};

const MoveDef kai_s_air_attack = {61, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Encoded Kai weapon descriptor 0x4ecef8; attack moves remain routed through
 * the selected weapon's native inventory entry until their strike records
 * are ported. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t kai_weapon_descriptor[] = {
  0x63,
  0x141b33,
  0x142daf,
  0x13b330,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffc000,
};
// clang-format on

void kai_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 11);
}

void kai_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e8210(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8210, fighter_ready, 11);
}

void kai_entry_retail_script_004e8320(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8320, fighter_ready, 11);
}

void kai_entry_retail_script_004e8358(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8358, fighter_ready, 11);
}

void kai_entry_retail_script_004e8390(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8390, fighter_ready, 11);
}

void kai_entry_retail_script_004e8870(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8870, fighter_ready, 11);
}

void kai_entry_retail_script_004e88c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e88c8, fighter_ready, 11);
}

void kai_entry_retail_script_004e8338(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8338, fighter_ready, 11);
}

void kai_entry_retail_script_004e83a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83a0, fighter_ready, 11);
}

void kai_entry_retail_script_004e83d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e83e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83e8, fighter_ready, 11);
}

void kai_entry_retail_script_004e9308(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9308, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e9360(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9360, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ea170(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea170, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ea1c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea1c0, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ec5a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec5a8, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8b78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8b78, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8bc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8f50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8f50, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ed330(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed330, fighter_ready, 11);
}

void kai_entry_retail_script_004ed348(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed348, fighter_ready, 11);
}

void kai_entry_retail_script_004ed2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2f8, fighter_ready, 11);
}

void kai_entry_retail_script_004e1430(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1430, fighter_ready, 11);
}

void kai_entry_retail_script_004e1440(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1440, fighter_ready, 11);
}

void kai_entry_retail_script_004e1450(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1450, fighter_ready, 11);
}

void kai_entry_retail_script_004e1468(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1468, fighter_ready, 11);
}

void kai_entry_retail_script_004e1480(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1480, fighter_ready, 11);
}

void kai_entry_retail_script_004e1490(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1490, fighter_ready, 11);
}

void kai_entry_retail_script_004e14a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14a8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14b8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14c8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14e8, fighter_ready, 11);
}

void kai_entry_retail_script_004dff88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dff88, fighter_ready, 11);
}

void kai_entry_retail_script_004ea240(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea240, fighter_ready, 11);
}

void kai_entry_retail_script_004e5be0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5be0, fighter_ready, 11);
}

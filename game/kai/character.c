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

void kai_entry_retail_script_004e8390(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8390, fighter_ready, 11);
}

void kai_entry_retail_script_004e83d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83d8, fighter_ready, 11);
}

void kai_entry_retail_script_004ed330(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed330, fighter_ready, 11);
}

void kai_entry_retail_script_004ed2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2f8, fighter_ready, 11);
}

void kai_entry_retail_script_004e1430(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1430, fighter_ready, 11);
}

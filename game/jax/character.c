#include "retail_vm.h"
#include "character.h"

const MoveDef jax_s_thrown = {.clip = 145, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void jax_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_script_thrown_ready, 13);
}

void jax_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 13);
}

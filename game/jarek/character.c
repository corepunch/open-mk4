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

void jarek_entry_retail_script_004e1260(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1260, fighter_ready, 12);
}

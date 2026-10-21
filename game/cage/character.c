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

void cage_entry_retail_script_004e1350(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1350, fighter_ready, 14);
}

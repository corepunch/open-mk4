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

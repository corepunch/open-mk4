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

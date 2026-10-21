#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebaf8, B selector 2 */
/* 0x4ebb70, B selector 36 */

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */

/* Native uppercut recovery uses contact-selected pose sequences. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent frame programs after release; replacement cancels only a live pair. */

/* 49c300 spawns independently of the owner's generated recovery. */

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

#include "retail_vm.h"
#include "character.h"

/* Retail attack records; source addresses identify the supplied EXE. */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebae0, B selector 1 */
/* 0x4ebb70, B selector 36 */

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */

/* Attacks query retail records from their events (SF_RECORD). Generic
 * S_HIT/S_KNOCKDOWN entries remain shared recovery fallbacks. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Selected Liu Kang ground scripts and native tails: 4e6d08, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6e90/46f390. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* Slot 3: LP 4de678, LK 42c040/4de3b0/410/428/468/478.
 * The victim owns its released recovery, even after owner interruption/KO. */

/* LK release 4e49a8 -> 45eab0/47eaf0, scripts 4e8610/678/688.
 * Native 47eb50 holds a hit pair for 58 ticks; damage is D[6] once. */

/* Fireballs: 49bf50/90/49bfe0 -> 4ed678/6c0/6f8.
 * Projectile task 64 has a 48-tick lifetime independent of its owner. */

const MeshAnimation liu_kang_fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

/* 47edc0/4e86a8 -> 47ee00/30; D[12], six opening visits.
 * Hit and blocked exits are 4e86e8 and 4e8718 respectively. */

/* Default weapon descriptor 53d5c0[3] -> 4ece58: type 66,
 * rig 506cb0. The four weapon entries select distinct native programs. */

// clang-format off: Keep table entries on one row, including long rows.
const WeaponStrike liu_kang_sword_attacks[] = {
  {{.clip = 240, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {106168, -54394, 77987, 80609, 59, 59, 119, 0, 10485}, 0, 8, 13, 3, 4, 0, 0, 0, 34},
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {99614, -49152, 64880, 55050, 61, 61, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34},
  {{.clip = 242, .last = 30, .flags = RETAIL, .action = ACTION_RETAIL_0212}, {111411, -25552, 89784, 11796, 62, 62, 118, 0, 9830}, 8, 9, 11, 3, 8, 2, 18, 0x28f5, 34},
  {{.clip = 227, .last = 59, .flags = RETAIL, .action = ACTION_RETAIL_0112}, {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175}, 0, 16, 29, 2, 0, 8, 13, 0x1999, 31},
};
// clang-format on

/* First scene: 4e0498[3] -> 450930. Second: 4dfe90[3] ->
 * 43c3d0/4dfff8/4e0020, victim reactions 176/177. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

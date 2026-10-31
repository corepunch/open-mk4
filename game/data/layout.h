#ifndef MK4_LAYOUT_H
#define MK4_LAYOUT_H
#include "mk4_linked.h"

/* Verified MK4 record sizes. Labels come directly from the linker as dat_*
 * and scr_* constants; packed references are word offsets, never pointers. */
enum {
  MK4_ANIMATION_DESCRIPTOR_SIZE = 12,
  MK4_ANIMATION_FRAME_COUNT_OFFSET = 4,
  MK4_ATTACK_RECORD_SIZE = 24,
  MK4_ROSTER_RECORD_SIZE = 32,
  MK4_CPU_ACTION_TABLE_STRIDE = 0x40
};

#endif

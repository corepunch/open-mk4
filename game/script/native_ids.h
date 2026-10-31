#ifndef MK4_NATIVE_ADDRESSES_H
#define MK4_NATIVE_ADDRESSES_H
#include "mk4_linked.h"

/* Stable native identifiers are declared beside fighter bytecode or in the
 * shared native include, then exported by the linker. They are never pointers. */
enum {
  MK4_NATIVE_ID_BEGIN = 0x401000,
  MK4_NATIVE_ID_END = 0x4ce000
};
#endif

#ifndef MK4_SCRIPT_NATIVE_IDS_H
#define MK4_SCRIPT_NATIVE_IDS_H
#include "mk4_linked.h"

/* Stable native identifiers are declared beside fighter bytecode or in the
 * shared native include, then exported by the linker. They are never pointers. */
enum {
  native_id_begin = 0x401000,
  native_id_end = 0x4ce000
};
#endif

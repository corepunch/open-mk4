#ifndef MK4_SCRIPT_NATIVE_IDS_H
#define MK4_SCRIPT_NATIVE_IDS_H
#include "mk4_linked.h"

/* Linker-assigned references; zero is null. Never executable addresses. */
enum {
  native_id_begin = 1,
  native_id_end = MK4_NATIVE_COUNT
};
#endif

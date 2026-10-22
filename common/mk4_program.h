#ifndef MK4_PROGRAM_H
#define MK4_PROGRAM_H
#include "mk4_image.h"

/* Stable addresses preserve references from original data and native bindings.
 * Only assembled words may be fetched as script instructions or operands. */
enum {
  MK4_SCRIPT_BEGIN = 0x4d0000,
  MK4_SCRIPT_END = 0x4ef000
};

void mk4_program_load(Mk4Image *image, const char *path);
uint32_t mk4_program_word(const Mk4Image *image, uint32_t address);
#endif

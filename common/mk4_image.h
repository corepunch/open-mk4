#ifndef MK4_IMAGE_H
#define MK4_IMAGE_H
#include "binary.h"
#include "mk4_addresses.h"

/* Linked project data and wordcode. Offsets are checked indices, never pointers. */
typedef struct Mk4Image {
  Bytes bytes;
  Bytes program_words; /* bitset of words supplied by the assembled script package */
} Mk4Image;

Mk4Image mk4_image_load(const char *path);
uint32_t mk4_image_u32(const Mk4Image *image, uint32_t va);
void mk4_image_free(Mk4Image *image);
#endif

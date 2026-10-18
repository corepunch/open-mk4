#ifndef WARGODS_WIFF_H
#define WARGODS_WIFF_H
#include "binary.h"

typedef struct WgChunk {
  char tag[5];
  View data;
} WgChunk;

/* Cursor starts at 8, after WIFF and its historical expanded-size field. */
bool wg_chunk_next(View file, size_t *cursor, WgChunk *chunk);
Bytes wg_rnc_unpack(View source);
float wg_float(uint32_t value);
#endif

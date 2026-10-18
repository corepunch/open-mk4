#include "mk4_package.h"
#include "wargods_wiff.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(void) {
  assert(wg_float(0x80000000) == 0);
  assert(wg_float(0x00000000) == 1);
  assert(wg_float(0x01400000) == 3);
  Package p = {0};
  package_load(&p, "Wargods/WARGODS.DAT");
  size_t files = 0, chunks = 0, compressed = 0;
  for (size_t i = 0; i < p.count; ++i) {
    const char *dot = strrchr(p.entries[i].name, '.');
    if (!dot || strcmp(dot, ".W")) {
      continue;
    }
    View file = package_named(&p, p.entries[i].name);
    size_t cursor = 8;
    WgChunk chunk;
    while (wg_chunk_next(file, &cursor, &chunk)) {
      ++chunks;
      if (!strcmp(chunk.tag, "DOT4")) {
        view_check(chunk.data, 0, 8);
        unsigned h = view_u16(chunk.data, 4), w = view_u16(chunk.data, 6);
        if (chunk.data.n < 12 || memcmp(chunk.data.p + 8, "RNC\1", 4)) {
          view_check(chunk.data, 8, (size_t)w * h);
          continue;
        }
        Bytes pixels = wg_rnc_unpack(view_sub(chunk.data, 8, chunk.data.n - 8));
        assert(pixels.count == (size_t)w * h);
        FREE_ARRAY(pixels);
        ++compressed;
      }
    }
    ++files;
  }
  assert(files == 219 && compressed > 100);
  printf("PASS: %zu War Gods WIFF files / %zu chunks / %zu RNC textures with both CRCs\n", files, chunks, compressed);
  package_free(&p);
  return 0;
}

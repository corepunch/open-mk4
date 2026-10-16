#include "mk4_package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t name_hash(const char *path) {
  uint32_t h = 0;
  size_t i = 0;
  for (; path[i]; ++i) {
    uint8_t c = (uint8_t)path[i];
    if (c >= 'a' && c <= 'z')
      c -= 32;
    h += (uint32_t)c << ((i % 4) * 8);
  }
  return h + (uint32_t)i;
}

bool parse_package(
    View data, PackageEntry entries[PACKAGE_MAX_ENTRIES], size_t *count, const char **error) {
  *count = 0;
  *error = NULL;
#define VALIDATE(ok, message)                                                                      \
  do {                                                                                             \
    if (!(ok)) {                                                                                   \
      *error = message;                                                                            \
      return false;                                                                                \
    }                                                                                              \
  } while (0)
  VALIDATE(data.n >= PACKAGE_TABLE_SIZE, "truncated archive index");
  bool padding = false;
  for (size_t p = 0; p < PACKAGE_TABLE_SIZE; p += 12) {
    PackageEntry e = {view_u32(data, p), view_u32(data, p + 4), view_u32(data, p + 8)};
    if (!e.hash && !e.offset && !e.size) {
      padding = true;
      continue;
    }
    VALIDATE(!padding && e.hash && (!*count || e.hash > entries[*count - 1].hash),
        "invalid index ordering or padding");
    VALIDATE(e.offset >= PACKAGE_TABLE_SIZE && (uint64_t)e.offset + e.size <= data.n,
        "entry extends outside archive payload");
    for (size_t i = 0; i < *count; ++i) {
      const PackageEntry *prev = &entries[i];
      VALIDATE(!e.size || !prev->size ||
                   (uint64_t)e.offset >= (uint64_t)prev->offset + prev->size ||
                   (uint64_t)prev->offset >= (uint64_t)e.offset + e.size,
          "overlapping payload entries");
    }
    entries[(*count)++] = e;
  }
  VALIDATE(*count > 0, "empty archive index");
  return true;
#undef VALIDATE
}

void package_load(Package *p, const char *path) {
  memset(p, 0, sizeof *p);
  p->bytes = read_file(path);
  const char *error;
  if (!parse_package(view(p->bytes), p->entries, &p->count, &error)) {
    package_free(p);
    fatal("%s", error);
  }
}

void package_free(Package *p) {
  FREE_ARRAY(p->bytes);
  p->count = 0;
}

View package_get(const Package *p, uint32_t hash) {
  size_t lo = 0, hi = p->count;
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (p->entries[mid].hash < hash)
      lo = mid + 1;
    else
      hi = mid;
  }
  if (lo == p->count || p->entries[lo].hash != hash)
    fatal("Missing archive entry %u", hash);
  return view_sub(view(p->bytes), p->entries[lo].offset, p->entries[lo].size);
}

View package_geo(const Package *p, const char *name) {
  char path[1100];
  int n = snprintf(path, sizeof path, "c:\\source\\mk4\\win\\geogfx\\%s", name);
  require(n >= 0 && (size_t)n < sizeof path, "Asset name too long");
  return package_get(p, name_hash(path));
}

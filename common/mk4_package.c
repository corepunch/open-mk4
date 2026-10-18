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

static bool same_name(const char *a, const char *b) {
  while (*a && *b) {
    unsigned char ca = (unsigned char)*a++, cb = (unsigned char)*b++;
    if (ca >= 'a' && ca <= 'z') {
      ca -= 32;
    }
    if (cb >= 'a' && cb <= 'z') {
      cb -= 32;
    }
    if (ca != cb) {
      return false;
    }
  }
  return *a == *b;
}

/* Supplied WARGODS.DAT: count includes a final empty-name EOF record.
 * Each packed directory record is an absolute u32 offset and a 13-byte name.
 * Payload lengths are the differences between consecutive offsets.
 */
static bool parse_wargods(View data, PackageEntry *entries, size_t *count, const char **error) {
  uint32_t records = view_u32(data, 0);
  size_t table_end = 4 + (size_t)records * 17;
#define WG_VALIDATE(ok, message)                                                                                                                               \
  do {                                                                                                                                                         \
    if (!(ok)) {                                                                                                                                               \
      *error = message;                                                                                                                                        \
      return false;                                                                                                                                            \
    }                                                                                                                                                          \
  } while (0)
  WG_VALIDATE(records >= 2 && records <= PACKAGE_MAX_ENTRIES && table_end <= data.n, "invalid War Gods directory size");
  for (size_t i = 0; i < records; ++i) {
    size_t p = 4 + i * 17;
    uint32_t offset = view_u32(data, p);
    const uint8_t *name = data.p + p + 4;
    WG_VALIDATE(offset >= table_end && offset <= data.n, "War Gods entry outside payload");
    if (i == records - 1) {
      WG_VALIDATE(!name[0] && offset == data.n, "invalid War Gods end marker");
      for (size_t j = 0; j < 13; ++j) {
        WG_VALIDATE(!name[j], "invalid War Gods end marker");
      }
      break;
    }
    PackageEntry e = {0};
    size_t length = 0;
    while (length < 13 && name[length]) {
      uint8_t c = name[length];
      WG_VALIDATE((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '~',
          "unsafe War Gods filename");
      e.name[length++] = (char)c;
    }
    WG_VALIDATE(length && length < 13 && e.name[0] != '.', "invalid War Gods filename");
    e.hash = name_hash(e.name);
    for (size_t j = 0; j < *count; ++j) {
      WG_VALIDATE(!same_name(e.name, entries[j].name), "duplicate War Gods filename");
    }
    uint32_t next = view_u32(data, p + 17);
    WG_VALIDATE(next >= offset && next <= data.n, "invalid War Gods payload ordering");
    e.offset = offset;
    e.size = next - offset;
    entries[(*count)++] = e;
  }
  return true;
#undef WG_VALIDATE
}

bool parse_package(View data, PackageEntry entries[PACKAGE_MAX_ENTRIES], size_t *count, const char **error) {
  *count = 0;
  *error = NULL;
#define VALIDATE(ok, message)                                                                                                                                  \
  do {                                                                                                                                                         \
    if (!(ok)) {                                                                                                                                               \
      *error = message;                                                                                                                                        \
      return false;                                                                                                                                            \
    }                                                                                                                                                          \
  } while (0)
  if (data.n >= 8 && view_u32(data, 0) >= 2 && view_u32(data, 0) <= PACKAGE_MAX_ENTRIES && view_u32(data, 4) == 4 + view_u32(data, 0) * 17) {
    return parse_wargods(data, entries, count, error);
  }
  VALIDATE(data.n >= PACKAGE_TABLE_SIZE, "truncated archive index");
  bool padding = false;
  for (size_t p = 0; p < PACKAGE_TABLE_SIZE; p += 12) {
    PackageEntry e = {0};
    e.hash = view_u32(data, p);
    e.offset = view_u32(data, p + 4);
    e.size = view_u32(data, p + 8);
    if (!e.hash && !e.offset && !e.size) {
      padding = true;
      continue;
    }
    VALIDATE(!padding && e.hash && (!*count || e.hash > entries[*count - 1].hash), "invalid index ordering or padding");
    VALIDATE(e.offset >= PACKAGE_TABLE_SIZE && (uint64_t)e.offset + e.size <= data.n, "entry extends outside archive payload");
    for (size_t i = 0; i < *count; ++i) {
      const PackageEntry *prev = &entries[i];
      VALIDATE(!e.size || !prev->size || (uint64_t)e.offset >= (uint64_t)prev->offset + prev->size || (uint64_t)prev->offset >= (uint64_t)e.offset + e.size,
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
  if (p->count && p->entries[0].name[0]) {
    const PackageEntry *found = NULL;
    for (size_t i = 0; i < p->count; ++i) {
      if (p->entries[i].hash == hash) {
        require(!found, "Ambiguous named archive hash; use package_named");
        found = &p->entries[i];
      }
    }
    require(found != NULL, "Missing named archive hash");
    return view_sub(view(p->bytes), found->offset, found->size);
  }
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

View package_named(const Package *p, const char *name) {
  for (size_t i = 0; i < p->count; ++i) {
    if (same_name(p->entries[i].name, name) && name[0]) {
      return view_sub(view(p->bytes), p->entries[i].offset, p->entries[i].size);
    }
  }
  fatal("Missing archive filename %s", name);
  return (View){0};
}

View package_geo(const Package *p, const char *name) {
  char path[1100];
  int n = snprintf(path, sizeof path, "c:\\source\\mk4\\win\\geogfx\\%s", name);
  require(n >= 0 && (size_t)n < sizeof path, "Asset name too long");
  return package_get(p, name_hash(path));
}

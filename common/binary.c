#include "binary.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Noreturn void fatal(const char *format, ...) {
  va_list args;
  va_start(args, format);
  fputs("MK4: ", stderr);
  vfprintf(stderr, format, args);
  fputc('\n', stderr);
  va_end(args);
  exit(EXIT_FAILURE);
}

void require(bool condition, const char *message) {
  if (!condition)
    fatal("%s", message);
}

void *checked_alloc(size_t count, size_t size) {
  require(!size || count <= SIZE_MAX / size, "Allocation size overflow");
  void *p = calloc(count ? count : 1, size ? size : 1);
  require(p != NULL, "Out of memory");
  return p;
}

void *array_resize(void *data, size_t *count, size_t *capacity, size_t n, size_t size) {
  require(size && n <= SIZE_MAX / size, "Array size overflow");
  if (n > *capacity) {
    size_t cap = *capacity ? *capacity : 8;
    while (cap < n) {
      if (cap > SIZE_MAX / 2) {
        cap = n;
        break;
      }
      cap *= 2;
    }
    require(cap <= SIZE_MAX / size, "Array capacity overflow");
    void *p = realloc(data, cap * size);
    require(p != NULL, "Out of memory");
    data = p;
    *capacity = cap;
  }
  if (n > *count)
    memset((uint8_t *)data + *count * size, 0, (n - *count) * size);
  *count = n;
  return data;
}

View view(Bytes b) {
  return (View){b.data, b.count};
}

void view_check(View v, size_t o, size_t z) {
  require(o <= v.n && z <= v.n - o, "Truncated asset data");
}

uint8_t view_u8(View v, size_t o) {
  view_check(v, o, 1);
  return v.p[o];
}

uint16_t view_u16(View v, size_t o) {
  view_check(v, o, 2);
  return (uint16_t)v.p[o] | (uint16_t)v.p[o + 1] << 8;
}

int16_t view_s16(View v, size_t o) {
  return (int16_t)view_u16(v, o);
}

uint32_t view_u32(View v, size_t o) {
  view_check(v, o, 4);
  return (uint32_t)v.p[o] | (uint32_t)v.p[o + 1] << 8 | (uint32_t)v.p[o + 2] << 16 | (uint32_t)v.p[o + 3] << 24;
}

int32_t view_s32(View v, size_t o) {
  return (int32_t)view_u32(v, o);
}

View view_sub(View v, size_t o, size_t z) {
  view_check(v, o, z);
  return (View){v.p + o, z};
}

void view_string(View v, size_t o, char *out, size_t cap) {
  size_t n = 0;
  require(cap > 0, "Empty string buffer");
  while (o < v.n && view_u8(v, o)) {
    require(n + 1 < cap, "Unterminated asset string");
    out[n++] = (char)view_u8(v, o++);
  }
  require(o < v.n, "Unterminated asset string");
  out[n] = 0;
}

Bytes read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    fatal("Cannot open %s", path);
  require(!fseek(f, 0, SEEK_END), "Cannot seek file");
  long n = ftell(f);
  require(n >= 0 && n < 512 * 1024 * 1024, "Invalid file size");
  rewind(f);
  Bytes b = {0};
  RESIZE(b, (size_t)n);
  bool ok = !n || fread(b.data, 1, b.count, f) == b.count;
  int closed = fclose(f);
  if (!ok || closed)
    fatal("Cannot read %s", path);
  return b;
}

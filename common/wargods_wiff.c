#include "wargods_wiff.h"
#include <math.h>

static uint32_t be32(View v, size_t p) {
  view_check(v, p, 4);
  return ((uint32_t)v.p[p] << 24) | ((uint32_t)v.p[p + 1] << 16) | ((uint32_t)v.p[p + 2] << 8) | v.p[p + 3];
}

bool wg_chunk_next(View file, size_t *cursor, WgChunk *chunk) {
  require(file.n >= 8 && !memcmp(file.p, "WIFF", 4), "Invalid War Gods WIFF header");
  require(*cursor >= 8 && *cursor <= file.n, "Invalid WIFF cursor");
  if (*cursor == file.n) {
    return false;
  }
  size_t p = *cursor;
  view_check(file, p, 8);
  uint32_t size = be32(file, p + 4);
  memcpy(chunk->tag, file.p + p, 4);
  chunk->tag[4] = 0;
  chunk->data = view_sub(file, p + 8, size);
  size_t end = p + 8 + size;
  require(end <= SIZE_MAX - 3, "WIFF size overflow");
  *cursor = (end + 3) & ~(size_t)3;
  require(*cursor <= file.n, "Truncated WIFF alignment padding");
  return true;
}

typedef struct RncBits {
  View source;
  size_t cursor;
  uint16_t word;
  unsigned remaining;
} RncBits;

typedef struct RncTree {
  unsigned count, length[32], code[32];
} RncTree;

static unsigned bits(RncBits *s, unsigned count) {
  require(count <= 16, "Invalid RNC bit count");
  unsigned value = 0;
  for (unsigned k = 0; k < count; ++k) {
    if (!s->remaining) {
      /* A last partial word may contain just one meaningful byte. */
      view_check(s->source, s->cursor, 1);
      s->word = s->source.p[s->cursor];
      if (s->cursor + 1 < s->source.n) {
        s->word |= (uint16_t)s->source.p[s->cursor + 1] << 8;
      }
      s->cursor += 2;
      s->remaining = 16;
    }
    value |= (unsigned)(s->word & 1) << k;
    s->word >>= 1;
    --s->remaining;
  }
  return value;
}

static RncTree tree(RncBits *s) {
  RncTree t = {0};
  t.count = bits(s, 5);
  for (unsigned i = 0; i < t.count; ++i) {
    t.length[i] = bits(s, 4);
  }
  unsigned code = 0;
  for (unsigned n = 1; n < 16; ++n) {
    for (unsigned i = 0; i < t.count; ++i) {
      if (t.length[i] == n) {
        require(code < (1u << n), "Oversubscribed RNC Huffman tree");
        t.code[i] = code++;
      }
    }
    code <<= 1;
  }
  return t;
}

static unsigned value(RncBits *s, const RncTree *t) {
  unsigned code = 0;
  for (unsigned n = 1; n < 16; ++n) {
    code = (code << 1) | bits(s, 1);
    for (unsigned i = 0; i < t->count; ++i) {
      if (t->length[i] == n && t->code[i] == code) {
        require(i <= 17, "RNC value exceeds bounded decoder range");
        return i < 2 ? i : (1u << (i - 1)) | bits(s, i - 1);
      }
    }
  }
  fatal("Invalid RNC Huffman symbol");
}

static uint16_t crc16(View v) {
  uint16_t crc = 0;
  for (size_t i = 0; i < v.n; ++i) {
    crc ^= v.p[i];
    for (unsigned k = 0; k < 8; ++k) {
      crc = (crc >> 1) ^ ((crc & 1) ? 0xa001 : 0);
    }
  }
  return crc;
}

Bytes wg_rnc_unpack(View source) {
  view_check(source, 0, 18);
  require(!memcmp(source.p, "RNC\1", 4), "Unsupported War Gods compression method");
  uint32_t size = be32(source, 4), packed = be32(source, 8);
  require(size && size <= 32 * 1024 * 1024, "Invalid RNC expanded size");
  View payload = view_sub(source, 18, packed);
  require(crc16(payload) == ((unsigned)source.p[14] << 8 | source.p[15]), "RNC packed CRC mismatch");
  RncBits s = {payload, 0, 0, 0};
  Bytes output = {0};
  RESIZE(output, size);
  size_t used = 0;
  bits(&s, 2);
  while (used < size) {
    RncTree literal = tree(&s), distance = tree(&s), length = tree(&s);
    unsigned runs = bits(&s, 16);
    require(runs > 0, "Empty RNC block");
    size_t before = used;
    for (unsigned i = 0; i < runs; ++i) {
      unsigned n = value(&s, &literal);
      require(n <= size - used, "RNC literal exceeds output");
      if (n) {
        view_check(payload, s.cursor, n);
        memcpy(output.data + used, payload.p + s.cursor, n);
      }
      used += n;
      s.cursor += n;
      if (i + 1 < runs) {
        unsigned d = value(&s, &distance) + 1;
        n = value(&s, &length) + 2;
        require(d <= used && n <= size - used, "Invalid RNC back-reference");
        for (unsigned j = 0; j < n; ++j) {
          output.data[used] = output.data[used - d];
          ++used;
        }
      }
    }
    require(used > before, "RNC block makes no progress");
  }
  require(crc16(view(output)) == ((unsigned)source.p[12] << 8 | source.p[13]), "RNC expanded CRC mismatch");
  return output;
}

/* TMS320C3x single precision, retained in the PC resource files. */
float wg_float(uint32_t value) {
  if ((value & 0xff000000) == 0x80000000) {
    return 0;
  }
  uint32_t exponent = value & 0xff000000;
  uint32_t mantissa = value & 0x7fffff;
  bool negative = (value & 0x800000) != 0;
  if (negative) {
    mantissa ^= 0x7fffff;
  }
  uint32_t ieee = (exponent + 2 * mantissa + 0x7f000000u) >> 1;
  if (negative) {
    ieee |= 0x80000000;
  }
  float result;
  memcpy(&result, &ieee, sizeof result);
  return result;
}

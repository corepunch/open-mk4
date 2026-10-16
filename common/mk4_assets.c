#include "mk4_assets.h"
#include "mk4_package.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Image {
  Bytes bytes;
} Image;

static uint32_t image_u32(const Image *exe, uint32_t va) {
  require(va >= 0x400000, "Invalid asset VA");
  return view_u32(view(exe->bytes), va - 0x400000);
}

static int32_t image_s32(const Image *exe, uint32_t va) {
  return (int32_t)image_u32(exe, va);
}

static void image_string(const Image *exe, uint32_t va, char out[1024]) {
  require(va >= 0x400000, "Invalid string VA");
  view_string(view(exe->bytes), va - 0x400000, out, 1024);
}

static Image image_load(const char *name) {
  Bytes raw = read_file(name);
  View v = view(raw);
  require(view_u16(v, 0) == 0x5a4d, "Executable is not an MZ/PE image");
  size_t pe = view_u32(v, 0x3c);
  require(view_u32(v, pe) == 0x4550, "Executable is not PE");
  require(view_u16(v, pe + 4) == 0x14c && view_u16(v, pe + 24) == 0x10b, "Expected 32-bit x86 MK4 executable");
  require(view_u32(v, pe + 52) == 0x400000, "Unexpected executable image base");
  Image exe = {0};
  RESIZE(exe.bytes, 0x180000);
  size_t sec = pe + 24 + view_u16(v, pe + 20);
  for (size_t i = 0; i < view_u16(v, pe + 6); ++i) {
    size_t h = sec + i * 40, va = view_u32(v, h + 12), z = view_u32(v, h + 16), o = view_u32(v, h + 20);
    if (va >= exe.bytes.count)
      continue;
    require(z <= exe.bytes.count - va, "Oversized executable section");
    view_check(v, o, z);
    memcpy(exe.bytes.data + va, v.p + o, z);
  }
  require(image_u32(&exe, 0x4fad7c) == 0 && image_u32(&exe, 0x4fad80) == 13 && image_u32(&exe, 0x4fad7c + 612 * 12) == 0xffffffff,
      "Unsupported MK4 executable revision (animation table differs)");
  // clang-format off: Keep table entries on one row, including long rows.
  const uint32_t ranges[][2] = {
    {0x4d3e80, 0x4d93f0},
    {0x4d93f8, 0x4d9d60}
  };
  // clang-format on
  for (size_t i = 0; i < 2; ++i)
    for (uint32_t p = ranges[i][0]; p < ranges[i][1]; p += 8) {
      uint32_t dst = image_u32(&exe, p), src = image_u32(&exe, p + 4);
      require(dst >= 0x4d0000 && dst + 4 <= 0x580000 && (!src || (src >= 0x4d0000 && src < 0x580000)), "Invalid asset pointer fixup");
      uint32_t w = src >> 2;
      for (int k = 0; k < 4; ++k)
        exe.bytes.data[dst - 0x400000 + (size_t)k] = (uint8_t)(w >> (k * 8));
    }
  FREE_ARRAY(raw);
  return exe;
}

ARRAY_TYPE(Vertices, Vertex);

static Meshes load_meshes(View v) {
  require(v.n >= 12 && !memcmp(v.p, "0.1v", 4) && view_u32(v, 8) == v.n, "Invalid GEO header");
  size_t end = view_u32(v, 4);
  require(end >= 12 && end < v.n, "Invalid GEO texture offset");
  size_t table_end = end;
  Meshes out = {0};
  for (size_t b = 12; b + 16 <= table_end + 4; b += 16) {
    uint16_t kind = view_u16(v, b), count = view_u16(v, b + 2);
    if ((kind & 0xff) == 0x80) {
      PUSH(out, (Mesh){0});
      continue;
    }
    if ((kind & 0x3f) > 1 || !count)
      break;
    int64_t vp = (int64_t)(b) + 4 + view_s32(v, b + 4), sp = (int64_t)(b) + 8 + view_s32(v, b + 8), tp = (int64_t)(b) + 12 + view_s32(v, b + 12);
    if (vp <= (int64_t)(b) || sp <= (int64_t)(b) || vp >= (int64_t)(end) || sp >= (int64_t)(end))
      break;
    table_end = minz(table_end, (size_t)(minz(vp, sp)));
    // Header's texture offset is four bytes before the texture count;
    // those four bytes are also the final triangle's trailing UV bytes.
    require(tp >= 0 && (uint64_t)(tp) + (size_t)(count) * 8 <= (uint64_t)(end) + 4, "Invalid GEO triangle table");
    Mesh mesh = {0};
    size_t vi = (size_t)(vp), ti = 0;
    bool terminated = false;
    for (size_t s = (size_t)(sp); s + 4 <= end; s += 4) {
      int n = view_s16(v, s + 2);
      if (n < 0) {
        terminated = true;
        break;
      }
      require(n < 8192 && ti + (size_t)(n) + 1 <= count, "Invalid GEO strip count");
      Vertices verts = {0};
      for (int k = 0; k < n + 3; ++k) {
        require(vi + 12 <= (size_t)(tp), "Invalid GEO vertex stream");
        // GEO stores normal components as Z/X/Y; TransformVertex consumes X/Y/Z.
        PUSH(verts, (Vertex){{view_s16(v, vi) / 256.f, view_s16(v, vi + 2) / 256.f, view_s16(v, vi + 4) / 256.f},
                        {view_s16(v, vi + 8) / 4096.f, view_s16(v, vi + 10) / 4096.f, view_s16(v, vi + 6) / 4096.f}, 0, 0});
        vi += 12;
      }
      for (int k = 0; k <= n; ++k, ++ti) {
        Triangle t = {0};
        t.texture = view_u8(v, (size_t)(tp) + ti * 8);
        size_t uv = (size_t)(tp) + ti * 8;
        for (int j = 0; j < 3; ++j) {
          t.v[(size_t)(j)] = verts.data[(size_t)(k + j)];
          t.v[(size_t)(j)].u = view_u8(v, uv + 2 + (size_t)(j) * 2);
          t.v[(size_t)(j)].v = view_u8(v, uv + 3 + (size_t)(j) * 2);
        }
        PUSH(mesh.triangles, t);
      }
      FREE_ARRAY(verts);
    }
    require(terminated && ti == count && vi == (size_t)(tp), "Inconsistent GEO strips/vertices");
    PUSH(out, (mesh));
    require(out.count < 1024, "Too many GEO mesh blocks");
  }
  require(out.count != 0, "No GEO block records found");
  return out;
}

static Textures load_textures(View v) {
  size_t p = (size_t)(view_u32(v, 4)) + 4;
  uint32_t count = view_u32(v, p);
  p += 4;
  require(count > 0 && count <= 16, "Invalid GEO texture count");
  Textures textures = {0};
  for (uint32_t page = 0; page < count; ++page) {
    Texture t = {0};
    t.width = view_u16(v, p);
    t.height = view_u16(v, p + 2);
    size_t n = view_u32(v, p + 4);
    p += 8;
    require(t.width > 0 && t.width <= 1024 && t.height > 0 && t.height <= 1024, "Invalid texture dimensions");
    // Stream size is rounded to an even byte count. Several retail files
    // omit that last pad byte; it is not part of the decoded pixel stream.
    require(p <= v.n && n <= v.n - p + 1, "Truncated texture stream");
    size_t available = minz(n, v.n - p);
    View src = view_sub(v, p, available);
    size_t pos = 0;
    for (int y = 0; y < t.height; ++y) {
      int x = 0;
      while (x < t.width) {
        uint32_t token = view_u16(src, pos);
        pos += 2;
        int run = 1;
        uint16_t c = token;
        if (token & 0x8000) {
          c = token & 0x7fff;
          run = view_u8(src, pos++);
        }
        require(run > 0 && run <= t.width - x, "Invalid RGB555 texture run");
        // Zero is transparent. Literal and repeated colors use the same RGB555 format.
        uint32_t r = (c >> 10) & 31, g = (c >> 5) & 31, b = c & 31;
        uint32_t rgba = (c ? 0xff000000u : 0) | ((r * 255 / 31) << 16) | ((g * 255 / 31) << 8) | (b * 255 / 31);
        for (int k = 0; k < run; ++k)
          PUSH(t.pixels, rgba);
        x += run;
      }
    }
    require(pos == available || (pos + 1 == available && n % 2 == 0), "Texture stream has unexplained trailing data");
    PUSH(textures, (t));
    p += n;
  }
  return textures;
}

typedef struct Bits {
  View v;
  size_t bit;
} Bits;

static uint32_t bits_peek(const Bits *b, unsigned n) {
  require(n <= 24 && b->bit <= b->v.n * 8, "Animation bitstream overrun");
  uint32_t w = 0;
  for (unsigned i = 0; i < n && b->bit + i < b->v.n * 8; ++i)
    w |= ((view_u8(b->v, (b->bit + i) / 8) >> ((b->bit + i) % 8)) & 1u) << i;
  return w;
}

static uint32_t bits_get(Bits *b, unsigned n) {
  require(b->bit <= b->v.n * 8 && n <= b->v.n * 8 - b->bit, "Animation bitstream overrun");
  uint32_t w = bits_peek(b, n);
  b->bit += n;
  return w;
}

static void bits_skip(Bits *b, size_t n) {
  require(b->bit <= b->v.n * 8 && n <= b->v.n * 8 - b->bit, "Animation seek overrun");
  b->bit += n;
}

// Adapted from mk4decomp's Anim_DecodeBitstream (MIT; see THIRD_PARTY_NOTICES).
// A bounded bit cursor replaces the original's address-dependent DWORD reader.
static void decode_frame(View stream, View nibtable, int frames, int words, int frame, uint32_t out[24]) {
  require(words >= 3 && words <= 24, "Unsupported animation channel count");
  int bones = words - 3;
  memset(out, 0, 24 * sizeof *out);
  int keyed[63] = {0}, delta[63] = {0}, span[63] = {0}, nib[63] = {0}, extra[63] = {0};
  Bits bits = {stream, 24 * 8};
  int local = frame;
  for (;;) {
    uint32_t d = bits_get(&bits, 13);
    if (local < 16)
      break;
    local -= 16;
    bits_skip(&bits, d);
  }
  for (int i = 0; i < bones * 3; ++i) {
    int shift = (i % 3) >> 1;
    keyed[(size_t)(i)] = (int)(bits_get(&bits, 1));
    int value = (int)(bits_get(&bits, (unsigned)(11 - shift))) << shift;
    if (value & 0x400)
      value -= 0x800;
    delta[(size_t)(i)] = value;
    if (keyed[(size_t)(i)])
      span[(size_t)(i)] = (int)(bits_get(&bits, 8));
  }
  int repeats;
  if (local > 7)
    repeats = local - 7;
  else {
    int last = frames - 1, lim = 7;
    if (frame >= (last & ~15))
      lim = mini(lim, last & 15);
    repeats = lim - local;
  }
  uint32_t scalar[3] = {bits_get(&bits, 8), bits_get(&bits, 8), bits_get(&bits, 8)};
  for (int i = 0; i < bones * 3; ++i)
    if (keyed[(size_t)(i)]) {
      nib[(size_t)(i)] = (int)(bits_get(&bits, 4));
      if (!nib[(size_t)(i)]) {
        int x = (int)(bits_get(&bits, 8));
        if (!repeats)
          extra[(size_t)(i)] = x;
      }
    }
  uint32_t d = bits_get(&bits, 12);
  if (local > 7)
    bits_skip(&bits, d);
  while (repeats-- > 0) {
    for (int i = 0; i < 3; ++i) {
      uint32_t x = bits_get(&bits, 8);
      if (!repeats)
        scalar[(size_t)(i)] = x;
    }
    for (int i = 0; i < bones * 3; ++i)
      if (keyed[(size_t)(i)]) {
        uint32_t token = view_u16(nibtable, bits_peek(&bits, 8) * 2);
        unsigned used = token & 15;
        require(used > 0 && used <= 8, "Invalid animation prefix code");
        bits_skip(&bits, used);
        nib[(size_t)(i)] = (nib[(size_t)(i)] + (token >> 7)) & 15;
        if (!nib[(size_t)(i)]) {
          int x = (int)(bits_get(&bits, 8));
          if (!repeats)
            extra[(size_t)(i)] = x;
        }
      }
  }
  for (int i = 0; i < 3; ++i)
    out[(size_t)(i)] = ((scalar[(size_t)(i)] * view_u32(stream, (size_t)(i) * 8)) >> 8) + view_u32(stream, (size_t)(i) * 8 + 4);
  for (int i = 0; i < bones; ++i) {
    uint32_t packed = 0;
    for (int k = 0; k < 3; ++k) {
      size_t j = (size_t)(i * 3 + k);
      int value = delta[j];
      if (keyed[j])
        value = nib[j] ? span[j] * (nib[j] - 1) + delta[j] : (int)((int8_t)(extra[j])) * 8;
      packed |= (uint32_t)((value & 0x7ff) >> (k >> 1)) << (k * 11);
    }
    out[(size_t)(i + 3)] = packed;
  }
}

static Geometry load_geometry(View v) {
  Geometry g = {0};
  g.meshes = load_meshes(v);
  g.textures = load_textures(v);
  RESIZE(g.sprites, g.meshes.count);
  for (size_t i = 0; i < g.meshes.count; ++i) {
    size_t p = 12 + i * 16;
    if ((view_u16(v, p) & 0xff) == 0x80) {
      Sprite sp = {0};
      sp.texture = view_u8(v, p + 2);
      sp.width = view_s16(v, p + 4);
      sp.height = view_s16(v, p + 6);
      sp.origin_x = view_s16(v, p + 8);
      sp.origin_y = view_s16(v, p + 10);
      sp.u = view_u8(v, p + 12);
      sp.v = view_u8(v, p + 13);
      sp.w = view_u8(v, p + 14);
      sp.h = view_u8(v, p + 15);
      require((size_t)sp.texture < g.textures.count && sp.w > 0 && sp.h > 0 && sp.u + sp.w <= g.textures.data[sp.texture].width &&
                  sp.v + sp.h <= g.textures.data[sp.texture].height,
          "Invalid GEO sprite");
      g.sprites.data[i] = sp;
    } else
      for (size_t j = 0; j < g.meshes.data[i].triangles.count; ++j) {
        int texture = g.meshes.data[i].triangles.data[j].texture;
        require(texture >= 0 && (size_t)texture < g.textures.count, "Invalid GEO texture page");
      }
  }
  return g;
}

static Clips load_clips(const Package *archive, const Image *exe) {
  View ap0 = package_get(archive, 0x2da2df95), ap1 = package_get(archive, 0x2da3df95);
  Clips clips = {0};
  RESIZE(clips, MK4_CLIP_COUNT);
  bool seen[MK4_CLIP_COUNT] = {0};
  size_t offset = 0;
  for (size_t p = 0x200;; p += 4) {
    uint16_t tag = view_u16(ap0, p);
    if (tag == 0xffff)
      break;
    int id = tag & 0x7fff;
    size_t size = view_u16(ap0, p + 2);
    require(id < (int)clips.count && !seen[id], "Invalid animation index");
    seen[id] = true;
    uint32_t desc = 0x4fad7c + (uint32_t)id * 12;
    int frames = (int)image_u32(exe, desc + 4), words = (int)(image_u32(exe, desc + 8) >> 16);
    require(frames > 0 && frames <= 600 && words >= 3 && words <= 24, "Invalid animation descriptor");
    View stream = view_sub(ap1, offset, size);
    offset += size;
    Clip *clip = &clips.data[id];
    clip->id = id;
    if (!(tag & 0x8000))
      require(size == (size_t)words * (size_t)frames * 4, "Invalid uncompressed clip size");
    RESIZE(clip->frames, (size_t)frames);
    for (int f = 0; f < frames; ++f) {
      uint32_t record[24] = {0};
      if (tag & 0x8000)
        decode_frame(stream, view_sub(ap0, 0, 512), frames, words, f, record);
      else
        for (int w = 0; w < words; ++w)
          record[w] = view_u32(stream, (size_t)(f * words + w) * 4);
      Frame *out = &clip->frames.data[f];
      out->translation = (Vec3){(int32_t)record[0] / 65536.f, (int32_t)record[1] / 65536.f, (int32_t)record[2] / 65536.f};
      RESIZE(out->angles, (size_t)(words - 3));
      for (int w = 3; w < words; ++w) {
        uint32_t v = record[w];
        out->angles.data[w - 3] = (Vec3){(float)((v & 0x7ff) * 2), (float)(((v >> 11) & 0x7ff) * 2), (float)((v >> 22) * 4)};
      }
    }
  }
  require(offset == ap1.n, "Incomplete animation pack");
  for (size_t i = 0; i < clips.count; ++i)
    require(seen[i], "Incomplete animation pack");
  return clips;
}

typedef struct RigReader {
  const Image *exe;
  uint32_t cursor, slot;
  size_t meshes;
  Bones bones;
} RigReader;

static void rig_walk(RigReader *r, int parent, int depth) {
  require(depth < 32 && r->bones.count < MK4_MAX_BONES, "Invalid skeleton hierarchy");
  uint32_t flags = image_u32(r->exe, r->cursor);
  require(image_u32(r->exe, r->cursor + 4) == r->slot, "Skeleton asset slot mismatch");
  Bone b = {0};
  b.parent = parent;
  b.flags = (flags & 0xfffff) | (parent < 0 ? 0x09000000u : 0x0a000000u);
  /* Required post-build setup: root kind 9 (C), descendants kind 10 (B).
     * MStackPushTwoEntryChainCall / ScaledMaskOrStore preserve other flags. */
  b.kind = (int)((b.flags >> 24) & 7);
  int id = image_s32(r->exe, r->cursor) >> 20;
  b.id = id;
  b.channel = (b.flags & 0x100) ? id - 2 : -1;
  b.mesh = (int)image_u32(r->exe, r->cursor + 24);
  require(b.mesh >= 0 && (size_t)b.mesh < r->meshes, "Skeleton mesh index out of range");
  b.translation = (Vec3){image_s32(r->exe, r->cursor + 12) / 65536.f, image_s32(r->exe, r->cursor + 16) / 65536.f, image_s32(r->exe, r->cursor + 20) / 65536.f};
  int index = (int)r->bones.count;
  PUSH(r->bones, b);
  r->cursor += 28;
  if (!(flags & 8))
    for (;;) {
      uint32_t f = image_u32(r->exe, r->cursor);
      rig_walk(r, index, depth + 1);
      if (f & 16)
        break;
    }
}

static Bones load_rig(const Image *exe, uint32_t start, uint32_t slot, size_t meshes) {
  RigReader r = {.exe = exe, .cursor = start, .slot = slot, .meshes = meshes};
  rig_walk(&r, -1, 0);
  return r.bones;
}

Vec3 vec_add(Vec3 a, Vec3 b) {
  return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 vec_sub(Vec3 a, Vec3 b) {
  return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 vec_scale(Vec3 a, float b) {
  return (Vec3){a.x * b, a.y * b, a.z * b};
}

Mat3 mat3_identity(void) {
  return (Mat3){{1, 0, 0, 0, 1, 0, 0, 0, 1}};
}

Vec3 mat3_apply(Mat3 a, Vec3 v) {
  return (Vec3){a.m[0] * v.x + a.m[1] * v.y + a.m[2] * v.z, a.m[3] * v.x + a.m[4] * v.y + a.m[5] * v.z, a.m[6] * v.x + a.m[7] * v.y + a.m[8] * v.z};
}

Mat3 mat3_mul(Mat3 a, Mat3 b) {
  Mat3 c = {0};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        c.m[i * 3 + j] += a.m[i * 3 + k] * b.m[k * 3 + j];
  return c;
}

Mat3 rotation(int kind, Vec3 bam) {
  const float tau = 6.283185307179586f;
  // Direct transform setters negate the 12-bit angles before lookup.
  float a = -bam.x * tau / 4096, b = -bam.y * tau / 4096, c = -bam.z * tau / 4096;
  // AppInit_PostJoy builds this table with FCOS, despite its historical
  // g_sinTable name. The phase-shifted lookup is therefore SIN, not -COS.
  // Keep the original builders' raw lookup formulas to avoid that trap.
  float x = cosf(a), xr = sinf(a), y = cosf(b), yr = sinf(b), z = cosf(c), zr = sinf(c);
  if (kind == 1) {
    float p = yr * xr, q = -yr * x;
    return (Mat3){{z * y, -zr * y, yr, z * p + zr * x, z * x - zr * p, -xr * y, z * q + zr * xr, xr * z - zr * q, y * x}};
  }
  if (kind == 2) {
    float r = yr * z, t = zr * yr;
    return (Mat3){{z * y, xr * r - zr * x, x * r + zr * xr, zr * y, xr * t + z * x, x * t - xr * z, -yr, xr * y, y * x}};
  }
  float t = yr * xr, u = xr * y;
  return (Mat3){{zr * t + z * y, z * t - zr * y, yr * x, zr * x, z * x, -xr, zr * u - yr * z, z * u + zr * yr, y * x}};
}

void assets_load(Assets *assets, const char *archive_path, const char *executable) {
  require(!assets->characters.count && !assets->clips.count, "Assets already loaded");
  Package archive;
  package_load(&archive, archive_path);
  Image exe = image_load(executable);
  assets->clips = load_clips(&archive, &exe);
  for (int i = 0; i < MK4_ROSTER_COUNT; ++i) {
    uint32_t rec = 0x4eb2e8 + (uint32_t)i * 32, slot = image_u32(&exe, rec) * 4;
    uint32_t asset = image_u32(&exe, slot), name = image_u32(&exe, asset);
    Character ch = {0};
    image_string(&exe, image_u32(&exe, 0x4ee1f0 + (uint32_t)i * 4), ch.name);
    image_string(&exe, name, ch.file);
    for (size_t k = 0; ch.name[k]; ++k)
      if (ch.name[k] >= 'a' && ch.name[k] <= 'z')
        ch.name[k] -= 32;
    View data = package_geo(&archive, ch.file);
    ch.meshes = load_meshes(data);
    Textures pages = load_textures(data);
    ch.texture = pages.data[0];
    for (size_t j = 1; j < pages.count; ++j)
      FREE_ARRAY(pages.data[j].pixels);
    FREE_ARRAY(pages);
    ch.bones = load_rig(&exe, image_u32(&exe, rec + 12) * 4, slot, ch.meshes.count);
    ch.scale = (float)image_u32(&exe, rec + 28) / 65536.f;
    /* StateDispatchTable special roster-ID stance remapping. */
    int stance_index = i == 16 ? 2 : i == 17 ? 7 : i;
    uint32_t stance = image_u32(&exe, 0x53d408 + (uint32_t)stance_index * 4) * 4;
    require(stance >= 0x4fad7c && (stance - 0x4fad7c) % 12 == 0 && stance < 0x4fad7c + 612 * 12, "Invalid default stance descriptor");
    ch.idle = (int)((stance - 0x4fad7c) / 12);
    Pose initial[MK4_MAX_BONES];
    pose(&ch, &assets->clips.data[ch.idle], 0, initial);
    for (size_t n = 0; n < ch.bones.count; ++n)
      if (ch.bones.data[n].channel == 0) {
        Vec3 face = mat3_apply(initial[n].rotation, (Vec3){0, 0, -1});
        ch.front_yaw = atan2f(-face.x, face.z);
        break;
      }
    PUSH(assets->characters, ch);
  }
  /* Scorpion fatality II: 0x41ea50 selects rig 0x50cb28, whose asset
   * slot 0x50c2e8 resolves to sc_fat2_.geo. Reuse the normal rig decoder. */
  Character *beast = &assets->scorpion_beast;
  image_string(&exe, image_u32(&exe, image_u32(&exe, 0x50c2e8)), beast->file);
  snprintf(beast->name, sizeof beast->name, "SCORPION CREATURE");
  View beast_data = package_geo(&archive, beast->file);
  beast->meshes = load_meshes(beast_data);
  Textures beast_pages = load_textures(beast_data);
  beast->texture = beast_pages.data[0];
  for (size_t i = 1; i < beast_pages.count; ++i) {
    FREE_ARRAY(beast_pages.data[i].pixels);
  }
  FREE_ARRAY(beast_pages);
  beast->bones = load_rig(&exe, 0x50cb28, 0x50c2e8, beast->meshes.count);
  beast->idle = 578;
  beast->scale = 1.f;
  int beast_channels = 0;
  for (size_t i = 0; i < beast->bones.count; ++i) {
    /* This rig animates IDs 1..21, including its root. Fighter rigs start
     * at ID 2; the stream walks animated IDs in ascending order. */
    beast->bones.data[i].channel++;
    beast_channels = maxi(beast_channels, beast->bones.data[i].channel + 1);
  }
  for (int c = 578; c <= 581; ++c) {
    require((int)assets->clips.data[c].frames.data[0].angles.count == beast_channels, "Scorpion creature animation/rig mismatch");
  }
  package_free(&archive);
  FREE_ARRAY(exe.bytes);
}

void assets_load_battle(Assets *assets, const char *archive_path, const char *executable) {
  require(assets->characters.count == MK4_ROSTER_COUNT && !assets->arenas.count, "Load fighters before battle assets");
  Package archive;
  package_load(&archive, archive_path);
  Image exe = image_load(executable);
  // clang-format off: Keep table entries on one row, including long rows.
  const char *names[] = {
    "GORO'S LAIR",
    "THE WELL",
    "ELDER GODS",
    "THE TOMB",
    "WIND WORLD",
    "REPTILE'S LAIR",
    "SHAOLIN TEMPLE",
    "THE LIVING FOREST",
    "THE PRISON",
    "ICE PIT",
    "THE SKULL"
  };
  // clang-format on
  /* June 1998 stage table, verified against the supplied executable. */
  for (size_t i = 0; i < MK4_ARENA_COUNT; ++i) {
    uint32_t rec = 0x4dd960 + (uint32_t)i * 64, slot = image_u32(&exe, rec) * 4;
    Arena a = {0};
    snprintf(a.name, sizeof a.name, "%s", names[i]);
    image_string(&exe, image_u32(&exe, image_u32(&exe, slot)), a.file);
    a.geometry = load_geometry(package_geo(&archive, a.file));
    a.nodes = load_rig(&exe, image_u32(&exe, rec + 12) * 4, slot, a.geometry.meshes.count);
    for (size_t j = 0; j < a.nodes.count; ++j)
      a.nodes.data[j].channel = -1;
    PUSH(assets->arenas, a);
  }
  assets->hud.font = load_geometry(package_geo(&archive, "mk4font_.geo"));
  assets->hud.effects = load_geometry(package_geo(&archive, "shared_g.geo"));
  // clang-format off: Keep table entries on one row, including long rows.
  const char *sprite_names[] = {
    "LIU KANG",
    "RAIDEN",
    "SONYA",
    "SCORPION",
    "SUB ZERO",
    "FUJIN",
    "JAREK",
    "KAI",
    "TANYA",
    "MILEENA",
    "NOOB SAIBOT",
    "QUAN CHI",
    "REPTILE",
    "SHINNOK",
    "JAX",
    "CAGE",
    "REIKO",
    "GORO",
    "KITANA"
  };
  // clang-format on
  for (size_t i = 0; i < assets->characters.count; ++i) {
    assets->hud.names[i] = -1;
    for (int j = 0; j < 19; ++j)
      if (!strcmp(assets->characters.data[i].name, sprite_names[j]))
        assets->hud.names[i] = j;
    require(assets->hud.names[i] >= 0, "Missing HUD name");
  }
  package_free(&archive);
  FREE_ARRAY(exe.bytes);
}

static void pose_frames(const Character *ch, const Frame *f0, const Frame *f1, float t, Pose out[MK4_MAX_BONES]) {
  require(ch->bones.count <= MK4_MAX_BONES, "Too many rig bones");
  for (size_t i = 0; i < ch->bones.count; ++i) {
    const Bone *bone = &ch->bones.data[i];
    Pose p;
    Vec3 angle = {0};
    if (bone->channel >= 0 && (size_t)bone->channel < f0->angles.count) {
      Vec3 v0 = f0->angles.data[bone->channel], v1 = f1->angles.data[bone->channel];
      angle = (Vec3){v0.x + remainderf(v1.x - v0.x, 4096.f) * t, v0.y + remainderf(v1.y - v0.y, 4096.f) * t, v0.z + remainderf(v1.z - v0.z, 4096.f) * t};
    }
    p.rotation = bone->channel >= 0 ? rotation(bone->kind, angle) : mat3_identity();
    p.position = bone->translation;
    if (bone->parent < 0)
      p.position = vec_add(vec_add(p.position, vec_scale(f0->translation, 1 - t)), vec_scale(f1->translation, t));
    if (bone->parent >= 0) {
      require((size_t)bone->parent < i, "Skeleton is not in parent order");
      const Pose *par = &out[bone->parent];
      p.position = vec_add(par->position, mat3_apply(par->rotation, p.position));
      p.rotation = mat3_mul(par->rotation, p.rotation);
    }
    out[i] = p;
  }
}

void pose(const Character *ch, const Clip *clip, double frame, Pose out[MK4_MAX_BONES]) {
  require(clip->frames.count > 0, "Empty animation clip");
  double f = fmod(fmax(0., frame), (double)clip->frames.count);
  size_t a = (size_t)f, b = (a + 1) % clip->frames.count;
  pose_frames(ch, &clip->frames.data[a], &clip->frames.data[b], (float)(f - (double)a), out);
}

void pose_blend(const Character *ch, const Clip *from, int sample, const Clip *to, int target, float fraction, Pose out[MK4_MAX_BONES]) {
  require(from->frames.count > 0 && to->frames.count > 0, "Empty blend clip");
  const Frame *a = &from->frames.data[(size_t)sample % from->frames.count];
  const Frame *b = &to->frames.data[(size_t)target % to->frames.count];
  require(a->angles.count == b->angles.count, "Blend clips have different rigs");
  pose_frames(ch, a, b, fraction, out);
}

static void meshes_free(Meshes *meshes) {
  for (size_t i = 0; i < meshes->count; ++i)
    FREE_ARRAY(meshes->data[i].triangles);
  FREE_ARRAY(*meshes);
}

static void geometry_free(Geometry *g) {
  meshes_free(&g->meshes);
  for (size_t i = 0; i < g->textures.count; ++i)
    FREE_ARRAY(g->textures.data[i].pixels);
  FREE_ARRAY(g->textures);
  FREE_ARRAY(g->sprites);
}

void assets_free(Assets *a) {
  for (size_t i = 0; i < a->characters.count; ++i) {
    Character *ch = &a->characters.data[i];
    meshes_free(&ch->meshes);
    FREE_ARRAY(ch->texture.pixels);
    FREE_ARRAY(ch->bones);
  }
  meshes_free(&a->scorpion_beast.meshes);
  FREE_ARRAY(a->scorpion_beast.texture.pixels);
  FREE_ARRAY(a->scorpion_beast.bones);
  for (size_t i = 0; i < a->clips.count; ++i) {
    Frames *frames = &a->clips.data[i].frames;
    for (size_t j = 0; j < frames->count; ++j)
      FREE_ARRAY(frames->data[j].angles);
    FREE_ARRAY(*frames);
  }
  for (size_t i = 0; i < a->arenas.count; ++i) {
    geometry_free(&a->arenas.data[i].geometry);
    FREE_ARRAY(a->arenas.data[i].nodes);
  }
  geometry_free(&a->hud.font);
  geometry_free(&a->hud.effects);
  FREE_ARRAY(a->characters);
  FREE_ARRAY(a->clips);
  FREE_ARRAY(a->arenas);
  memset(a, 0, sizeof *a);
}

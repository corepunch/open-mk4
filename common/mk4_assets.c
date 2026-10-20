#include "mk4_assets.h"
#include "mk4_package.h"
#include "mk4_image.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int32_t image_s32(const Mk4Image *exe, uint32_t va) {
  return (int32_t)mk4_image_u32(exe, va);
}

static void image_string(const Mk4Image *exe, uint32_t va, char out[1024]) {
  require(va >= 0x400000, "Invalid string VA");
  view_string(view(exe->bytes), va - 0x400000, out, 1024);
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
    Mesh mesh = {.flags = kind};
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

static Clips load_clips(const Package *archive, const Mk4Image *exe) {
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
    int frames = (int)mk4_image_u32(exe, desc + 4), words = (int)(mk4_image_u32(exe, desc + 8) >> 16);
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
  const Mk4Image *exe;
  uint32_t cursor, slot;
  size_t meshes;
  Bones bones;
} RigReader;

static void rig_walk(RigReader *r, int parent, int depth) {
  require(depth < 32 && r->bones.count < MK4_MAX_BONES, "Invalid skeleton hierarchy");
  uint32_t flags = mk4_image_u32(r->exe, r->cursor);
  require(mk4_image_u32(r->exe, r->cursor + 4) == r->slot, "Skeleton asset slot mismatch");
  Bone b = {0};
  b.parent = parent;
  b.flags = (flags & 0xfffff) | (parent < 0 ? 0x09000000u : 0x0a000000u);
  /* Required post-build setup: root kind 9 (C), descendants kind 10 (B).
     * MStackPushTwoEntryChainCall / ScaledMaskOrStore preserve other flags. */
  b.kind = (int)((b.flags >> 24) & 7);
  int id = image_s32(r->exe, r->cursor) >> 20;
  b.id = id;
  b.channel = (b.flags & 0x100) ? id - 2 : -1;
  b.mesh = (int)mk4_image_u32(r->exe, r->cursor + 24);
  require(b.mesh >= 0 && (size_t)b.mesh < r->meshes, "Skeleton mesh index out of range");
  b.translation = (Vec3){image_s32(r->exe, r->cursor + 12) / 65536.f, image_s32(r->exe, r->cursor + 16) / 65536.f, image_s32(r->exe, r->cursor + 20) / 65536.f};
  int index = (int)r->bones.count;
  PUSH(r->bones, b);
  r->cursor += 28;
  if (!(flags & 8))
    for (;;) {
      uint32_t f = mk4_image_u32(r->exe, r->cursor);
      rig_walk(r, index, depth + 1);
      if (f & 16)
        break;
    }
}

static Bones load_rig(const Mk4Image *exe, uint32_t start, uint32_t slot, size_t meshes) {
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
  Mk4Image exe = mk4_image_load(executable);
  assets->clips = load_clips(&archive, &exe);
  for (int i = 0; i < MK4_ROSTER_COUNT; ++i) {
    uint32_t rec = 0x4eb2e8 + (uint32_t)i * 32, slot = mk4_image_u32(&exe, rec) * 4;
    uint32_t asset = mk4_image_u32(&exe, slot), name = mk4_image_u32(&exe, asset);
    Character ch = {0};
    image_string(&exe, mk4_image_u32(&exe, 0x4ee1f0 + (uint32_t)i * 4), ch.name);
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
    ch.bones = load_rig(&exe, mk4_image_u32(&exe, rec + 12) * 4, slot, ch.meshes.count);
    ch.scale = (float)mk4_image_u32(&exe, rec + 28) / 65536.f;
    /* StateDispatchTable special roster-ID stance remapping. */
    int stance_index = i == 16 ? 2 : i == 17 ? 7 : i;
    uint32_t stance = mk4_image_u32(&exe, 0x53d408 + (uint32_t)stance_index * 4) * 4;
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
    /* 48ce00 selects roster +0x14 when freezing; these alternate
     * meshes close the exposed cuts used by detached body parts. */
    Character cut = ch;
    uint32_t cut_root = mk4_image_u32(&exe, rec + 20) * 4;
    uint32_t cut_slot = mk4_image_u32(&exe, cut_root + 4);
    image_string(&exe, mk4_image_u32(&exe, mk4_image_u32(&exe, cut_slot)), cut.file);
    if (strcmp(cut.file, ch.file) != 0) {
      /* Kitana's recovered freeze row selects Tanya's cut geometry. */
      View cut_data = package_geo(&archive, cut.file);
      cut.meshes = load_meshes(cut_data);
      Textures cut_pages = load_textures(cut_data);
      cut.texture = cut_pages.data[0];
      for (size_t j = 1; j < cut_pages.count; ++j) {
        FREE_ARRAY(cut_pages.data[j].pixels);
      }
      FREE_ARRAY(cut_pages);
    }
    cut.bones = load_rig(&exe, cut_root, cut_slot, cut.meshes.count);
    PUSH(assets->cut_characters, cut);
  }
  /* 446620 adds rig 5145bc below the detached head. Its mesh data
   * is in sz_geo.geo; clip 386 also animates the original head first. */
  Character spine = assets->characters.data[4];
  spine.bones = load_rig(&exe, 0x5145bc, 0x511d1c, spine.meshes.count);
  spine.idle = 386;
  for (size_t i = 0; i < spine.bones.count; ++i) {
    spine.bones.data[i].channel++;
  }
  spine.bones.data[0].kind = 2;                         /* attached below the head: descendant B */
  spine.bones.data[0].translation.z = 0x13b6 / 65536.f; /* 4465a0 */
  PUSH(assets->scene_models, spine);
  /* 47f4d0 uses the shared ice statue, not the owner's body meshes. */
  assets->hud.effects = load_geometry(package_geo(&archive, "shared_g.geo"));
  assets->hud.effects2 = load_geometry(package_geo(&archive, "share2_g.geo"));
  /*450730 selects the six authored tornado surfaces through slot5061d4. */
  assets->hud.fujin_wind = load_geometry(package_geo(&archive, "fj_fat_g.geo"));
  /* 5061bc resolves to ja_fat_g.geo, the geometry of Jarek's fatality props. */
  assets->hud.jarek_fatality = load_geometry(package_geo(&archive, "ja_fat_g.geo"));
  Character statue = assets->characters.data[4];
  snprintf(statue.file, sizeof statue.file, "shared_g.geo");
  statue.meshes = assets->hud.effects.meshes;
  statue.texture = assets->hud.effects.textures.data[0];
  statue.material_pages = assets->hud.effects.textures;
  statue.bones = load_rig(&exe, 0x50686c, 0x50609c, statue.meshes.count);
  PUSH(assets->scene_models, statue);
  const uint32_t weapon_descriptors[] = {
      0x4ecdf8, 0x4ecdb8, 0x4ece58, 0x4ece18, 0x4ece38, 0x4eced8, 0x4ecef8, 0x4ecf58, 0x4ecdd8, 0x4eceb8, 0x4ece98, 0x4ece78, 0x4ecd98, 0x4ecf38};
  for (size_t i = 0; i < sizeof weapon_descriptors / sizeof weapon_descriptors[0]; ++i) {
    uint32_t descriptor = weapon_descriptors[i];
    uint32_t root = mk4_image_u32(&exe, descriptor + 4) * 4;
    Character weapon = statue;
    if (descriptor == 0x4ecf58 || descriptor == 0x4ecf38) { /* Cage and Quan Chi rigs are in share2_g.geo. */
      snprintf(weapon.file, sizeof weapon.file, "share2_g.geo");
      weapon.meshes = assets->hud.effects2.meshes;
      weapon.material_pages = assets->hud.effects2.textures;
      weapon.texture = assets->hud.effects2.textures.data[0];
    }
    weapon.bones = load_rig(&exe, root, mk4_image_u32(&exe, root + 4), weapon.meshes.count);
    weapon.bones.data[0].translation.y = image_s32(&exe, descriptor + 28) / 65536.f;
    PUSH(assets->scene_models, weapon);
  }
  /*41d560/41d770 replaces a wind-torn victim with the original
   *share2 flesh rig50c444 before4818f0 separates its limbs. */
  Character flesh = statue;
  snprintf(flesh.file, sizeof flesh.file, "share2_g.geo");
  flesh.meshes = assets->hud.effects2.meshes;
  flesh.texture = assets->hud.effects2.textures.data[0];
  flesh.material_pages = assets->hud.effects2.textures;
  flesh.bones = load_rig(&exe, 0x50c444, 0x50c2a0, flesh.meshes.count);
  PUSH(assets->scene_models, flesh);
  for (uint32_t part = 0; part < 14; ++part) {
    uint32_t root = mk4_image_u32(&exe, 0x4ea424 + part * 12) * 4;
    Character skin = flesh;
    skin.bones = load_rig(&exe, root, mk4_image_u32(&exe, root + 4), skin.meshes.count);
    PUSH(assets->scene_models, skin);
  }
  /* Quan Chi fatality I: 44ccf0 copies the victim's own leg joints
   * 12 -> 14 -> 16 (with their unnamed caps) into a separate three-channel
   * rig, which clips 422/423 animate after the leg is torn off. */
  require(assets->scene_models.count == SCENE_MODEL_SEVERED_LEGS, "Severed leg rigs follow the wind pieces");
  for (size_t i = 0; i < assets->characters.count; ++i) {
    const Character *source = &assets->characters.data[i];
    Character leg = *source;
    leg.bones = (Bones){0};
    int mapped[MK4_MAX_BONES];
    for (size_t n = 0; n < source->bones.count; ++n) {
      const Bone *bone = &source->bones.data[n];
      int parent = bone->parent >= 0 ? mapped[bone->parent] : -1;
      mapped[n] = -1;
      if (bone->id != 12 && parent < 0) {
        continue;
      }
      Bone copy = *bone;
      copy.parent = bone->id == 12 ? -1 : parent;
      copy.kind = bone->id == 12 ? 9 : 10;
      copy.flags = (copy.flags & 0xfffff) | (uint32_t)copy.kind << 24;
      copy.channel = bone->id == 12 ? 0 : bone->id == 14 ? 1 : bone->id == 16 ? 2 : -1;
      if (bone->id == 12) {
        copy.translation = (Vec3){0, 0, 0};
      }
      mapped[n] = (int)leg.bones.count;
      PUSH(leg.bones, copy);
    }
    require(leg.bones.count >= 3, "Fighter rig lacks the leg joints 12/14/16");
    PUSH(assets->scene_models, leg);
  }
  /* Scorpion fatality II: 0x41ea50 selects rig 0x50cb28, whose asset
   * slot 0x50c2e8 resolves to sc_fat2_.geo. Reuse the normal rig decoder. */
  Character *beast = &assets->scorpion_beast;
  image_string(&exe, mk4_image_u32(&exe, mk4_image_u32(&exe, 0x50c2e8)), beast->file);
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
  /* 450930 creates the original Liu Kang dragon rig 508204. Its
   * asset slot 5061b0 resolves to lk_fat_g.geo; IDs start at one. */
  Character *dragon = &assets->liu_dragon;
  image_string(&exe, mk4_image_u32(&exe, mk4_image_u32(&exe, 0x5061b0)), dragon->file);
  snprintf(dragon->name, sizeof dragon->name, "LIU KANG DRAGON");
  View dragon_data = package_geo(&archive, dragon->file);
  dragon->meshes = load_meshes(dragon_data);
  dragon->material_pages = load_textures(dragon_data);
  dragon->texture = dragon->material_pages.data[0];
  dragon->bones = load_rig(&exe, 0x508204, 0x5061b0, dragon->meshes.count);
  dragon->idle = 418;
  dragon->scale = 1.f;
  int dragon_channels = 0;
  for (size_t i = 0; i < dragon->bones.count; ++i) {
    if (dragon->bones.data[i].flags & 0x100) {
      dragon->bones.data[i].channel++;
    }
    dragon_channels = maxi(dragon_channels, dragon->bones.data[i].channel + 1);
  }
  require((int)assets->clips.data[418].frames.data[0].angles.count == dragon_channels, "Liu Kang dragon animation/rig mismatch");
  /* 4556d0 and 43caa0 create rig 5083c4. Its asset slot 5061e0 resolves
   * to nf_fat_g.geo; fatality I plays clip 434 and II plays clip 541. */
  Character *hand = &assets->shinnok_hand;
  image_string(&exe, mk4_image_u32(&exe, mk4_image_u32(&exe, 0x5061e0)), hand->file);
  snprintf(hand->name, sizeof hand->name, "SHINNOK HAND");
  View hand_data = package_geo(&archive, hand->file);
  hand->meshes = load_meshes(hand_data);
  hand->material_pages = load_textures(hand_data);
  hand->texture = hand->material_pages.data[0];
  hand->bones = load_rig(&exe, 0x5083c4, 0x5061e0, hand->meshes.count);
  hand->idle = 541;
  hand->scale = 1.f;
  int hand_channels = 0;
  for (size_t i = 0; i < hand->bones.count; ++i) {
    /* Like the dragon, the clip stream starts at ID one: animated
     * joints 2..17 occupy channels 1..16 of the 17-channel clips. */
    if (hand->bones.data[i].flags & 0x100) {
      hand->bones.data[i].channel++;
    }
    hand_channels = maxi(hand_channels, hand->bones.data[i].channel + 1);
  }
  require(
      (int)assets->clips.data[434].frames.data[0].angles.count == hand_channels && (int)assets->clips.data[541].frames.data[0].angles.count == hand_channels,
      "Shinnok hand animation/rig mismatch");
  package_free(&archive);
  mk4_image_free(&exe);
}

void assets_load_battle(Assets *assets, const char *archive_path, const char *executable) {
  require(assets->characters.count == MK4_ROSTER_COUNT && !assets->arenas.count, "Load fighters before battle assets");
  Package archive;
  package_load(&archive, archive_path);
  Mk4Image exe = mk4_image_load(executable);
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
    uint32_t rec = 0x4dd960 + (uint32_t)i * 64, slot = mk4_image_u32(&exe, rec) * 4;
    Arena a = {0};
    snprintf(a.name, sizeof a.name, "%s", names[i]);
    image_string(&exe, mk4_image_u32(&exe, mk4_image_u32(&exe, slot)), a.file);
    a.geometry = load_geometry(package_geo(&archive, a.file));
    a.nodes = load_rig(&exe, mk4_image_u32(&exe, rec + 12) * 4, slot, a.geometry.meshes.count);
    for (size_t j = 0; j < a.nodes.count; ++j)
      a.nodes.data[j].channel = -1;
    PUSH(assets->arenas, a);
  }
  assets->hud.font = load_geometry(package_geo(&archive, "mk4font_.geo"));
  assets->hud.title_scene.textures = load_textures(package_geo(&archive, "mklogo_g.geo"));
  assets->hud.select_scene.textures = load_textures(package_geo(&archive, "select_g.geo"));
  sound_bank_load(&assets->audio, &archive);
  /* 48a0d0 accepts indexed selectors 0..65 at 4ead30. Do not alias B. */
  const uint32_t sound_tables[] = {0x4ea9b0, 0x4e77b8, 0x4eaa18, 0x4ead30, mk4_image_u32(&exe, 0x53d0a4) * 4, mk4_image_u32(&exe, 0x53d000 + 8 * 4) * 4,
      mk4_image_u32(&exe, 0x53d0a0) * 4, 0x4eaaf8, 0};
  const int sound_counts[] = {51, 184, 113, 66, MK4_ROSTER_COUNT, MK4_ROSTER_COUNT, MK4_ROSTER_COUNT, 20, 0};
  for (int bank = 0; bank < SOUND_BANK_COUNT; ++bank) {
    assets->audio.event_counts[bank] = sound_counts[bank];
    for (int i = 0; i < sound_counts[bank]; ++i) {
      /* 48a230 maps Kitana/Noob voices to Sonya/Reiko. */
      int selector = (bank == SOUND_IMPACT_VOICE || bank == SOUND_DRAW_VOICE) ? (i == 16 ? 2 : i == 17 ? 7 : i) : i;
      if (bank == SOUND_THROW_VOICE) {
        /* 489ff0: flat nonzero selector-8 list, indexed through the signed
         * roster remap (Kitana uses roster 10 for this voice family). */
        int roster = i == 16 ? 10 : i;
        selector = view_u8(view(exe.bytes), 0x4ead18 - 0x400000 + (uint32_t)roster);
      }
      assets->audio.events[bank][i] = view_u16(view(exe.bytes), sound_tables[bank] - 0x400000 + (uint32_t)selector * 2);
    }
  }
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
  mk4_image_free(&exe);
}

const Texture *character_texture(const Character *character, int page) {
  if (character->material_pages.count > 0) {
    require(page >= 0 && (size_t)page < character->material_pages.count, "Invalid model material page");
    return &character->material_pages.data[page];
  }
  return &character->texture;
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

void pose_static(const Character *ch, Pose out[MK4_MAX_BONES]) {
  const Frame bind = {0};
  pose_frames(ch, &bind, &bind, 0, out);
}

void pose(const Character *ch, const Clip *clip, double frame, Pose out[MK4_MAX_BONES]) {
  require(clip->frames.count > 0, "Empty animation clip");
  double f = fmod(fmax(0., frame), (double)clip->frames.count);
  size_t a = (size_t)f, b = (a + 1) % clip->frames.count;
  pose_frames(ch, &clip->frames.data[a], &clip->frames.data[b], (float)(f - (double)a), out);
}

void pose_blend(const Character *ch, const Clip *from, int sample, const Clip *to, int target, float fraction, Pose out[MK4_MAX_BONES]) {
  pose_blend_frames(ch, from, sample, to, target, fraction, out);
}

void pose_blend_frames(const Character *ch, const Clip *from, double from_frame, const Clip *to, double to_frame, float fraction, Pose out[MK4_MAX_BONES]) {
  require(from->frames.count > 0 && to->frames.count > 0, "Empty blend clip");
  double source = fmod(fmax(0., from_frame), (double)from->frames.count);
  double target = fmod(fmax(0., to_frame), (double)to->frames.count);
  size_t source_a = (size_t)source, source_b = (source_a + 1) % from->frames.count;
  size_t target_a = (size_t)target, target_b = (target_a + 1) % to->frames.count;
  float source_fraction = (float)(source - (double)source_a);
  float target_fraction = (float)(target - (double)target_a);
  fraction = fmaxf(0.f, fminf(1.f, fraction));
  const Frame *sa = &from->frames.data[source_a], *sb = &from->frames.data[source_b];
  const Frame *ta = &to->frames.data[target_a], *tb = &to->frames.data[target_b];
  require(sa->angles.count == ta->angles.count && ta->angles.count == tb->angles.count, "Blend clips have different rigs");
  require(ch->bones.count <= MK4_MAX_BONES, "Too many rig bones");
  for (size_t i = 0; i < ch->bones.count; ++i) {
    const Bone *bone = &ch->bones.data[i];
    Pose p;
    Vec3 angle = {0};
    if (bone->channel >= 0 && (size_t)bone->channel < sa->angles.count) {
      Vec3 s0 = sa->angles.data[bone->channel], s1 = sb->angles.data[bone->channel];
      Vec3 t0 = ta->angles.data[bone->channel], t1 = tb->angles.data[bone->channel];
      Vec3 source_angle = {
          s0.x + remainderf(s1.x - s0.x, 4096.f) * source_fraction,
          s0.y + remainderf(s1.y - s0.y, 4096.f) * source_fraction,
          s0.z + remainderf(s1.z - s0.z, 4096.f) * source_fraction,
      };
      Vec3 target_angle = {
          t0.x + remainderf(t1.x - t0.x, 4096.f) * target_fraction,
          t0.y + remainderf(t1.y - t0.y, 4096.f) * target_fraction,
          t0.z + remainderf(t1.z - t0.z, 4096.f) * target_fraction,
      };
      angle = (Vec3){
          source_angle.x + remainderf(target_angle.x - source_angle.x, 4096.f) * fraction,
          source_angle.y + remainderf(target_angle.y - source_angle.y, 4096.f) * fraction,
          source_angle.z + remainderf(target_angle.z - source_angle.z, 4096.f) * fraction,
      };
    }
    p.rotation = bone->channel >= 0 ? rotation(bone->kind, angle) : mat3_identity();
    p.position = bone->translation;
    if (bone->parent < 0) {
      Vec3 source_root = vec_add(vec_scale(sa->translation, 1.f - source_fraction), vec_scale(sb->translation, source_fraction));
      Vec3 target_root = vec_add(vec_scale(ta->translation, 1.f - target_fraction), vec_scale(tb->translation, target_fraction));
      p.position = vec_add(p.position, vec_add(vec_scale(source_root, 1.f - fraction), vec_scale(target_root, fraction)));
    }
    if (bone->parent >= 0) {
      require((size_t)bone->parent < i, "Skeleton is not in parent order");
      const Pose *parent = &out[bone->parent];
      p.position = vec_add(parent->position, mat3_apply(parent->rotation, p.position));
      p.rotation = mat3_mul(parent->rotation, p.rotation);
    }
    out[i] = p;
  }
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
  sound_bank_free(&a->audio);
  for (size_t i = 0; i < a->scene_models.count; ++i) {
    FREE_ARRAY(a->scene_models.data[i].bones);
  }
  FREE_ARRAY(a->scene_models);
  for (size_t i = 0; i < a->cut_characters.count; ++i) {
    Character *cut = &a->cut_characters.data[i];
    const Character *normal = &a->characters.data[i];
    if (cut->meshes.data != normal->meshes.data) {
      meshes_free(&cut->meshes);
      FREE_ARRAY(cut->texture.pixels);
    }
    FREE_ARRAY(cut->bones);
  }
  FREE_ARRAY(a->cut_characters);
  for (size_t i = 0; i < a->characters.count; ++i) {
    Character *ch = &a->characters.data[i];
    meshes_free(&ch->meshes);
    FREE_ARRAY(ch->texture.pixels);
    FREE_ARRAY(ch->bones);
  }
  meshes_free(&a->liu_dragon.meshes);
  for (size_t i = 0; i < a->liu_dragon.material_pages.count; ++i) {
    FREE_ARRAY(a->liu_dragon.material_pages.data[i].pixels);
  }
  FREE_ARRAY(a->liu_dragon.material_pages);
  FREE_ARRAY(a->liu_dragon.bones);
  meshes_free(&a->shinnok_hand.meshes);
  for (size_t i = 0; i < a->shinnok_hand.material_pages.count; ++i) {
    FREE_ARRAY(a->shinnok_hand.material_pages.data[i].pixels);
  }
  FREE_ARRAY(a->shinnok_hand.material_pages);
  FREE_ARRAY(a->shinnok_hand.bones);
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
  geometry_free(&a->hud.effects2);
  geometry_free(&a->hud.fujin_wind);
  geometry_free(&a->hud.jarek_fatality);
  geometry_free(&a->hud.title_scene);
  geometry_free(&a->hud.select_scene);
  FREE_ARRAY(a->characters);
  FREE_ARRAY(a->clips);
  FREE_ARRAY(a->arenas);
  memset(a, 0, sizeof *a);
}

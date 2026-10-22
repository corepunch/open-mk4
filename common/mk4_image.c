#include "mk4_image.h"

uint32_t mk4_image_u32(const Mk4Image *image, uint32_t va) {
  require(va >= MK4_RETAIL_DATA_IMAGE_BASE && va % 4 == 0, "Invalid retail data VA");
  return view_u32(view(image->bytes), va - MK4_RETAIL_DATA_IMAGE_BASE);
}

Mk4Image mk4_image_load(const char *name) {
  Bytes raw = read_file(name);
  View v = view(raw);
  require(view_u16(v, 0) == 0x5a4d, "Executable is not an MZ/PE image");
  size_t pe = view_u32(v, 0x3c);
  require(view_u32(v, pe) == 0x4550, "Executable is not PE");
  require(view_u16(v, pe + 4) == 0x14c && view_u16(v, pe + 24) == 0x10b, "Expected 32-bit x86 MK4 executable");
  require(view_u32(v, pe + 52) == MK4_RETAIL_DATA_IMAGE_BASE, "Unexpected executable image base");
  Mk4Image exe = {0};
  RESIZE(exe.bytes, 0x180000);
  size_t sec = pe + 24 + view_u16(v, pe + 20);
  for (size_t i = 0; i < view_u16(v, pe + 6); ++i) {
    size_t h = sec + i * 40, va = view_u32(v, h + 12), z = view_u32(v, h + 16), o = view_u32(v, h + 20);
    if (va >= exe.bytes.count) {
      continue;
    }
    require(z <= exe.bytes.count - va, "Oversized executable section");
    view_check(v, o, z);
    memcpy(exe.bytes.data + va, v.p + o, z);
  }
  require(mk4_image_u32(&exe, MK4_RETAIL_DATA_ANIMATION_TABLE) == 0 && mk4_image_u32(&exe, MK4_RETAIL_DATA_ANIMATION_CLIP_ZERO_FRAME_COUNT_WORD) == 13 &&
              mk4_image_u32(&exe, MK4_RETAIL_DATA_ANIMATION_TABLE + 612 * 12) == 0xffffffff,
      "Unsupported MK4 executable revision (animation table differs)");
  // clang-format off: Keep table entries on one row, including long rows.
  const uint32_t ranges[][2] = {
    {MK4_RETAIL_DATA_ASSET_FIXUPS_PRIMARY_BEGIN, MK4_RETAIL_DATA_ASSET_FIXUPS_PRIMARY_END},
    {MK4_RETAIL_DATA_ASSET_FIXUPS_SECONDARY_BEGIN, MK4_RETAIL_DATA_ASSET_FIXUPS_SECONDARY_END}
  };
  // clang-format on
  for (size_t i = 0; i < 2; ++i) {
    for (uint32_t p = ranges[i][0]; p < ranges[i][1]; p += 8) {
      uint32_t dst = mk4_image_u32(&exe, p), src = mk4_image_u32(&exe, p + 4);
      require(dst >= MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN && dst + 4 <= MK4_RETAIL_DATA_IMAGE_END &&
                  (!src || (src >= MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN && src < MK4_RETAIL_DATA_IMAGE_END)),
          "Invalid asset pointer fixup");
      uint32_t w = src >> 2;
      for (int k = 0; k < 4; ++k) {
        exe.bytes.data[dst - MK4_RETAIL_DATA_IMAGE_BASE + (size_t)k] = (uint8_t)(w >> (k * 8));
      }
    }
  }
  FREE_ARRAY(raw);
  return exe;
}

void mk4_image_free(Mk4Image *image) {
  FREE_ARRAY(image->bytes);
}

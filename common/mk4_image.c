#include "mk4_image.h"

uint32_t mk4_image_u32(const Mk4Image *image, uint32_t address) {
  require(address >= MK4_DATA_IMAGE_BASE && address % 4 == 0, "Invalid MK4 data offset");
  return view_u32(view(image->bytes), address - MK4_DATA_IMAGE_BASE);
}

Mk4Image mk4_image_load(const char *name) {
  Bytes raw = read_file(name);
  View v = view(raw);
  view_check(v, 0, 52);
  require(memcmp(v.p, "MK4D", 4) == 0 && view_u32(v, 4) == 1, "Unsupported MK4 data package");
  uint32_t size = view_u32(v, 8), begin = view_u32(v, 12), end = view_u32(v, 16);
  require(size > 0 && size <= 16 * 1024 * 1024 && size % 4 == 0 && begin < end && end == size && begin % 4 == 0, "Invalid MK4 data package layout");
  size_t mask_size = ((size_t)size + 31) / 32;
  require(v.n == 52 + (size_t)size + mask_size, "Invalid MK4 data package size");
  require(memcmp(v.p + 20, MK4_LAYOUT_ID, 32) == 0 && size == MK4_DATA_IMAGE_END - MK4_DATA_IMAGE_BASE && begin == SCRIPT_ADDRESS_BEGIN - MK4_DATA_IMAGE_BASE &&
              end == SCRIPT_ADDRESS_END - MK4_DATA_IMAGE_BASE,
      "MK4 data layout differs: rebuild the application with this source package");
  Mk4Image image = {0};
  RESIZE(image.bytes, size);
  RESIZE(image.program_words, mask_size);
  memcpy(image.bytes.data, v.p + 52, size);
  memcpy(image.program_words.data, v.p + 52 + size, mask_size);
  for (size_t word = 0; word < mask_size * 8; ++word) {
    if (image.program_words.data[word / 8] & (1u << (word % 8))) {
      require(word * 4 >= begin && word * 4 < end, "Script word mask outside linked program");
    }
  }
  FREE_ARRAY(raw);
  return image;
}

void mk4_image_free(Mk4Image *image) {
  FREE_ARRAY(image->bytes);
  FREE_ARRAY(image->program_words);
}

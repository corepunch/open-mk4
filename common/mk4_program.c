#include "mk4_program.h"

void mk4_program_load(Mk4Image *image, const char *path) {
  Bytes bytes = read_file(path);
  View package = view(bytes);
  view_check(package, 0, 12);
  require(memcmp(package.p, "MK4S", 4) == 0 && view_u32(package, 4) == 1, "Unsupported MK4 script package");
  uint32_t count = view_u32(package, 8);
  require(count > 0 && count <= (MK4_SCRIPT_END - MK4_SCRIPT_BEGIN) / 4 && package.n == 12 + (size_t)count * 8, "Invalid MK4 script package size");
  require(image->bytes.count >= MK4_SCRIPT_END - MK4_RETAIL_DATA_IMAGE_BASE, "Script package needs a loaded MK4 data image");
  RESIZE(image->program_words, (image->bytes.count + 31) / 32);
  memset(image->program_words.data, 0, image->program_words.count);
  uint32_t previous = 0;
  for (uint32_t i = 0; i < count; ++i) {
    uint32_t address = view_u32(package, 12 + (size_t)i * 8);
    require(address >= MK4_SCRIPT_BEGIN && address < MK4_SCRIPT_END && address % 4 == 0 && address > previous, "Invalid or duplicate MK4 script address");
    previous = address;
    size_t offset = address - MK4_RETAIL_DATA_IMAGE_BASE;
    memcpy(image->bytes.data + offset, package.p + 16 + (size_t)i * 8, 4);
    image->program_words.data[offset / 32] |= (uint8_t)(1u << (offset / 4 % 8));
  }
  FREE_ARRAY(bytes);
}

uint32_t mk4_program_word(const Mk4Image *image, uint32_t address) {
  require(address >= MK4_RETAIL_DATA_IMAGE_BASE && address % 4 == 0, "Invalid MK4 script word address");
  size_t offset = address - MK4_RETAIL_DATA_IMAGE_BASE;
  if (offset / 32 >= image->program_words.count || !(image->program_words.data[offset / 32] & (1u << (offset / 4 % 8)))) {
    fatal("Script word %08x absent from assembled MK4 program", address);
  }
  return mk4_image_u32(image, address);
}

#include "mk4_program.h"

uint32_t mk4_program_word(const Mk4Image *image, uint32_t address) {
  require(address >= MK4_DATA_IMAGE_BASE && address % 4 == 0, "Invalid MK4 script word address");
  size_t offset = address - MK4_DATA_IMAGE_BASE;
  if (offset / 32 >= image->program_words.count || !(image->program_words.data[offset / 32] & (1u << (offset / 4 % 8)))) {
    fatal("Script word %08x absent from assembled MK4 program", address);
  }
  return mk4_image_u32(image, address);
}

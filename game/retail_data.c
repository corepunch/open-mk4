#include "retail_data.h"

unsigned retail_character_slot(unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail character");
  return character == 16 ? 2 : character == 17 ? 7 : character;
}

uint32_t retail_action_target(const Mk4Image *image, unsigned flags, unsigned action) {
  // clang-format off: Keep table entries on one row, including long rows.
  static const uint32_t dispatch[][2] = {
    {1, 0x4e6ef8},
    {2, 0x4ed828},
    {4, 0x4e8910},
    {8, 0x4e7008},
    {0x20, 0x4dfc90}
  };
  // clang-format on
  require(action < 256, "Invalid retail action index");
  for (size_t i = 0; i < sizeof dispatch / sizeof *dispatch; ++i) {
    if (flags & dispatch[i][0]) {
      return mk4_image_u32(image, dispatch[i][1] + action * 4);
    }
  }
  return 0;
}

RetailCommandCursor retail_command_cursor(const Mk4Image *image, unsigned character, unsigned player, Button button, bool released) {
  require(character < MK4_ROSTER_COUNT && player < 2, "Invalid retail command owner");
  uint32_t table = 0;
  switch (button) {
    case BUTTON_UP:
      table = player ? 0x53cb10 : 0x53c888;
      break;
    case BUTTON_HIGH_PUNCH:
      table = player ? 0x53cb58 : 0x53c8d0;
      break;
    case BUTTON_LOW_PUNCH:
      table = player ? 0x53cba0 : 0x53c918;
      break;
    case BUTTON_BLOCK:
      table = player ? 0x53cbe8 : 0x53c960;
      break;
    case BUTTON_HIGH_KICK:
      table = player ? 0x53cc30 : 0x53c9a8;
      break;
    case BUTTON_LOW_KICK:
      table = released ? (player ? 0x53ca80 : 0x53ca38) : (player ? 0x53cc78 : 0x53c9f0);
      break;
    case BUTTON_RUN:
      table = player ? 0x53ccc0 : 0x53cac8;
      break;
    default:
      return (RetailCommandCursor){0};
  }
  if (released && button != BUTTON_LOW_KICK) {
    return (RetailCommandCursor){0};
  }
  uint32_t packed = mk4_image_u32(image, table + character * 4);
  require(!packed || (packed >= 0x4d0000 / 4 && packed < 0x4ef000 / 4), "Invalid retail command list");
  return (RetailCommandCursor){.address = packed * 4, .remaining = 256};
}

bool retail_command_next(const Mk4Image *image, RetailCommandCursor *cursor, RetailCommand *command) {
  if (cursor->address == 0) {
    return false;
  }
  require(cursor->remaining > 0 && cursor->address >= 0x4d0000 && cursor->address + 12 <= 0x4ef000, "Unterminated retail command list");
  uint32_t pattern = mk4_image_u32(image, cursor->address);
  if (pattern == 0) {
    cursor->address = 0;
    return false;
  }
  uint32_t word = mk4_image_u32(image, cursor->address + 8);
  unsigned flags = (word >> 16) & 255u, predicate = word >> 24;
  *command = (RetailCommand){.pattern = pattern,
      .extra = mk4_image_u32(image, cursor->address + 4),
      .word = word,
      .flags = flags,
      .window = (word >> 8) & 255u,
      .target = retail_action_target(image, flags, word & 255u),
      .predicate = predicate ? mk4_image_u32(image, 0x4e4ec0 + predicate * 4) : 0};
  cursor->address += 12;
  cursor->remaining--;
  return true;
}

uint32_t retail_cpu_target(const Mk4Image *image, unsigned character, unsigned distance) {
  require(distance < 4, "Invalid retail CPU table");
  uint32_t word = mk4_image_u32(image, 0x4df8e8 + distance * 0x40 + retail_character_slot(character) * 4);
  return retail_action_target(image, (word >> 16) & 255u, word & 255u);
}

int retail_stance_clip(const Mk4Image *image, unsigned character, bool walking) {
  uint32_t packed = mk4_image_u32(image, (walking ? 0x53d388 : 0x53d408) + retail_character_slot(character) * 4);
  require(packed >= 0x4fad7c / 4 && packed < (0x4fad7c + MK4_CLIP_COUNT * 12) / 4 && (packed * 4 - 0x4fad7c) % 12 == 0, "Invalid retail stance descriptor");
  return (int)((packed * 4 - 0x4fad7c) / 12);
}

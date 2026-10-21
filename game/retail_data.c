#include "retail_data.h"
#include "retail_native.h"

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
  require(!packed || (packed >= 0x4d0000 / 4 && packed < 0x580000 / 4), "Invalid retail command list");
  return (RetailCommandCursor){.address = packed * 4, .remaining = 256};
}

bool retail_command_next(const Mk4Image *image, RetailCommandCursor *cursor, RetailCommand *command) {
  if (cursor->address == 0) {
    return false;
  }
  require(cursor->remaining > 0 && cursor->address >= 0x4d0000 && cursor->address + 12 <= 0x580000, "Unterminated retail command list");
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

/* Compatibility view for UI and existing callers that inspect controls. The
 * input matcher itself walks the selected player list directly on every press. */
void retail_bind_commands(Controls *controls, const Mk4Image *image, unsigned character) {
  static CommandDef records[MK4_ROSTER_COUNT][64];
  require(character < MK4_ROSTER_COUNT, "Invalid retail controls owner");
  unsigned count = 0;
  for (Button button = BUTTON_UP; button < BUTTON_COUNT; button++) {
    for (unsigned released = 0; released < 2; ++released) {
      RetailCommandCursor cursor = retail_command_cursor(image, character, 0, button, released != 0);
      RetailCommand source;
      while (retail_command_next(image, &cursor, &source)) {
        require(count < 64, "Retail command list exceeds controls view capacity");
        CommandDef *record = &records[character][count++];
        *record = (CommandDef){.button = button,
            .window = (int)source.window,
            .flags = (source.flags & (CMD_AIR | CMD_BLOCK_OK)) | (released ? CMD_RELEASE : 0),
            .predicate = retail_native_predicate(source.predicate),
            .enter = retail_native_entry(character, source.target)};
        for (uint32_t pattern = source.pattern == 0xffffffffu ? 0 : source.pattern; pattern; pattern >>= 8) {
          require(record->count < COMMAND_TOKENS, "Retail pattern exceeds controls view capacity");
          CommandToken token;
          switch (pattern & 255u) {
            case 1:
              token = TOKEN_UP;
              break;
            case 2:
              token = TOKEN_DOWN;
              break;
            case 0xaa:
              token = TOKEN_BACK;
              break;
            case 0xdd:
              token = TOKEN_FORWARD;
              break;
            default:
              fatal("Invalid retail command direction %02x", pattern & 255u);
          }
          record->tokens[record->count++] = token;
        }
      }
    }
  }
  controls->commands = records[character];
  controls->command_count = (int)count;
}

void retail_roster_read(const Mk4Image *image, unsigned character, RetailRoster *roster) {
  require(character < MK4_ROSTER_COUNT && roster != NULL, "Invalid retail roster destination");
  uint32_t name = mk4_image_u32(image, 0x4ee1f0 + character * 4);
  View memory = view(image->bytes);
  require(name >= 0x400000, "Invalid retail character name");
  size_t i = 0;
  for (; i + 1 < sizeof roster->name; ++i) {
    unsigned char ch = view_u8(memory, name - 0x400000 + i);
    roster->name[i] = (char)(ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch);
    if (ch == 0) {
      break;
    }
  }
  require(i + 1 < sizeof roster->name, "Unterminated retail character name");
  uint32_t record = 0x4eb2e8 + character * 32;
  uint32_t slot = mk4_image_u32(image, record) * 4;
  uint32_t root = mk4_image_u32(image, record + 12) * 4;
  require(mk4_image_u32(image, root + 4) == slot, "Retail roster rig slot mismatch");
  roster->ground = (q16)mk4_image_u32(image, root + 16);
  roster->idle = retail_stance_clip(image, character, false);
  roster->walk = retail_stance_clip(image, character, true);
}

uint32_t retail_finish_target(const Mk4Image *image, unsigned character, bool second) {
  uint32_t target = mk4_image_u32(image, (second ? 0x4dfe90 : 0x4e0498) + retail_character_slot(character) * 4);
  require(target == 0 || (target >= 0x400000 && target < 0x4d0000), "Invalid retail scene native target");
  return target;
}

q16 retail_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second) {
  /* 43460d maps the boss to14 for CPU approach distance, independently of
   * its scene support. Hidden scene aliases retain their shared data slot. */
  unsigned slot = character == 15 ? 14 : retail_character_slot(character);
  q16 distance = (q16)mk4_image_u32(image, (second ? 0x4dfe50 : 0x4dfe10) + slot * 4);
  require(distance >= 0 && distance <= 16 * Q16_ONE, "Invalid retail CPU finish distance");
  return distance;
}

uint32_t retail_reaction_target(const Mk4Image *image, unsigned reaction) {
  require(reaction < (0x4ea7f0 - 0x4ea4e8) / 4, "Invalid retail reaction selector");
  uint32_t target = mk4_image_u32(image, 0x4ea4e8 + reaction * 4);
  require(target == 0 || (target >= 0x400000 && target < 0x4d0000), "Invalid retail reaction native");
  return target;
}

uint32_t retail_lk_target(const Mk4Image *image, unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid native LK roster slot");
  /* Native48e490 remaps17 to4, unlike the script selector's17 to7. */
  unsigned slot = character == 16 ? 2 : character == 17 ? 4 : character;
  uint32_t target = mk4_image_u32(image, 0x4ddfb8 + slot * 4);
  require(target >= 0x400000 && target < 0x4d0000, "Invalid native LK entry");
  return target;
}

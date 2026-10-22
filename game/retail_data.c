#include "retail_layout.h"
#include "retail_data.h"
#include "retail_native.h"
#include <stdio.h>

unsigned retail_character_slot(unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail character");
  return character == 16 ? 2 : character == 17 ? 7 : character;
}

/* Native48e490 and close-entry46be40 use a different Noob alias from scripts. */
unsigned retail_native_character_slot(unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail native character");
  return character == 16 ? 2 : character == 17 ? 4 : character;
}

uint32_t retail_action_target(const Mk4Image *image, unsigned flags, unsigned action) {
  // clang-format off: Keep table entries on one row, including long rows.
  static const uint32_t dispatch[][2] = {
    {1, RETAIL_ACTION_TABLE_1},
    {2, RETAIL_ACTION_TABLE_2},
    {4, RETAIL_ACTION_TABLE_4},
    {8, RETAIL_ACTION_TABLE_8},
    {0x20, RETAIL_ACTION_TABLE_32}
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
      table = player ? RETAIL_P2_UP_COMMAND_TABLE : RETAIL_P1_UP_COMMAND_TABLE;
      break;
    case BUTTON_HIGH_PUNCH:
      table = player ? RETAIL_P2_HP_COMMAND_TABLE : RETAIL_P1_HP_COMMAND_TABLE;
      break;
    case BUTTON_LOW_PUNCH:
      table = player ? RETAIL_P2_LP_COMMAND_TABLE : RETAIL_P1_LP_COMMAND_TABLE;
      break;
    case BUTTON_BLOCK:
      table = player ? RETAIL_P2_BLOCK_COMMAND_TABLE : RETAIL_P1_BLOCK_COMMAND_TABLE;
      break;
    case BUTTON_HIGH_KICK:
      table = player ? RETAIL_P2_HK_COMMAND_TABLE : RETAIL_P1_HK_COMMAND_TABLE;
      break;
    case BUTTON_LOW_KICK:
      table = released ? (player ? RETAIL_P2_LK_RELEASE_COMMAND_TABLE : RETAIL_P1_LK_RELEASE_COMMAND_TABLE)
                       : (player ? RETAIL_P2_LK_COMMAND_TABLE : RETAIL_P1_LK_COMMAND_TABLE);
      break;
    case BUTTON_RUN:
      table = player ? RETAIL_P2_RUN_COMMAND_TABLE : RETAIL_P1_RUN_COMMAND_TABLE;
      break;
    default:
      return (RetailCommandCursor){0};
  }
  if (released && button != BUTTON_LOW_KICK) {
    return (RetailCommandCursor){0};
  }
  uint32_t packed = mk4_image_u32(image, table + character * 4);
  require(
      !packed || (packed >= MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000 / 4 && packed < MK4_RETAIL_DATA_IMAGE_END_580000 / 4), "Invalid retail command list");
  return (RetailCommandCursor){.address = packed * 4, .remaining = 256};
}

bool retail_command_next(const Mk4Image *image, RetailCommandCursor *cursor, RetailCommand *command) {
  if (cursor->address == 0) {
    return false;
  }
  require(cursor->remaining > 0 && cursor->address >= MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000 && cursor->address + 12 <= MK4_RETAIL_DATA_IMAGE_END_580000,
      "Unterminated retail command list");
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
      .predicate = predicate ? mk4_image_u32(image, MK4_RETAIL_DATA_COMMAND_NEXT_4E4EC0 + predicate * 4) : 0};
  cursor->address += 12;
  cursor->remaining--;
  return true;
}

uint32_t retail_cpu_target(const Mk4Image *image, unsigned character, unsigned distance) {
  require(distance < 4, "Invalid retail CPU table");
  uint32_t word = mk4_image_u32(image, RETAIL_CPU_ACTION_TABLES + distance * RETAIL_CPU_ACTION_TABLE_STRIDE + retail_character_slot(character) * 4);
  return retail_action_target(image, (word >> 16) & 255u, word & 255u);
}

int retail_stance_clip(const Mk4Image *image, unsigned character, bool walking) {
  uint32_t packed = mk4_image_u32(image, (walking ? RETAIL_WALK_ANIMATION_TABLE : RETAIL_IDLE_ANIMATION_TABLE) + retail_character_slot(character) * 4);
  require(packed >= RETAIL_ANIMATION_TABLE / 4 && packed < (RETAIL_ANIMATION_TABLE + MK4_CLIP_COUNT * RETAIL_ANIMATION_DESCRIPTOR_SIZE) / 4 &&
              (packed * 4 - RETAIL_ANIMATION_TABLE) % RETAIL_ANIMATION_DESCRIPTOR_SIZE == 0,
      "Invalid retail stance descriptor");
  return (int)((packed * 4 - RETAIL_ANIMATION_TABLE) / RETAIL_ANIMATION_DESCRIPTOR_SIZE);
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
            .enter = retail_native_entry(source.target)};
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
  uint32_t name = mk4_image_u32(image, RETAIL_ROSTER_NAME_TABLE + character * 4);
  View memory = view(image->bytes);
  require(name >= MK4_RETAIL_DATA_IMAGE_BASE_400000, "Invalid retail character name");
  size_t i = 0;
  for (; i + 1 < sizeof roster->name; ++i) {
    unsigned char ch = view_u8(memory, name - MK4_RETAIL_DATA_IMAGE_BASE_400000 + i);
    roster->name[i] = (char)(ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch);
    if (ch == 0) {
      break;
    }
  }
  require(i + 1 < sizeof roster->name, "Unterminated retail character name");
  uint32_t record = RETAIL_ROSTER_RECORD_TABLE + character * RETAIL_ROSTER_RECORD_SIZE;
  uint32_t slot = mk4_image_u32(image, record) * 4;
  uint32_t root = mk4_image_u32(image, record + 12) * 4;
  require(mk4_image_u32(image, root + 4) == slot, "Retail roster rig slot mismatch");
  roster->ground = (q16)mk4_image_u32(image, root + 16);
  roster->idle = retail_stance_clip(image, character, false);
  roster->walk = retail_stance_clip(image, character, true);
}

uint32_t retail_finish_target(const Mk4Image *image, unsigned character, bool second) {
  uint32_t target = mk4_image_u32(image, (second ? RETAIL_SECOND_FINISH_TABLE : RETAIL_FIRST_FINISH_TABLE) + retail_character_slot(character) * 4);
  require(target == 0 || (target >= MK4_RETAIL_DATA_IMAGE_BASE_400000 && target < MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000),
      "Invalid retail scene native target");
  return target;
}

q16 retail_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second) {
  /* 43460d maps the boss to14 for CPU approach distance, independently of
   * its scene support. Hidden scene aliases retain their shared data slot. */
  unsigned slot = character == 15 ? 14 : retail_character_slot(character);
  q16 distance = (q16)mk4_image_u32(image, (second ? RETAIL_SECOND_FINISH_DISTANCE_TABLE : RETAIL_FIRST_FINISH_DISTANCE_TABLE) + slot * 4);
  require(distance >= 0 && distance <= 16 * Q16_ONE, "Invalid retail CPU finish distance");
  return distance;
}

uint32_t retail_reaction_target(const Mk4Image *image, unsigned reaction) {
  require(reaction < RETAIL_REACTION_COUNT, "Invalid retail reaction selector");
  uint32_t target = mk4_image_u32(image, RETAIL_REACTION_TABLE + reaction * 4);
  require(
      target == 0 || (target >= MK4_RETAIL_DATA_IMAGE_BASE_400000 && target < MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000), "Invalid retail reaction native");
  return target;
}

uint32_t retail_lk_target(const Mk4Image *image, unsigned character) {
  unsigned slot = retail_native_character_slot(character);
  uint32_t target = mk4_image_u32(image, RETAIL_LK_THROW_TABLE + slot * 4);
  require(target >= MK4_RETAIL_DATA_IMAGE_BASE_400000 && target < MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000, "Invalid native LK entry");
  return target;
}

/* UI text is a view of the loaded command lists, not another fighter ruleset.
 * Retail has no command display names here, so use shared action labels. */
void retail_bind_move_list(CharacterDef *definition, const Mk4Image *image, unsigned character) {
  enum {
    CAPACITY = 64,
    INPUT_SIZE = 64
  };

  static MoveListEntry entries[MK4_ROSTER_COUNT][CAPACITY];
  static char inputs[MK4_ROSTER_COUNT][CAPACITY][INPUT_SIZE];
  static const char *buttons[BUTTON_COUNT] = {"U", "D", "L", "R", "HP", "LP", "BL", "HK", "LK", "RUN"};
  require(character < MK4_ROSTER_COUNT, "Invalid command display owner");
  unsigned count = 0;
  for (unsigned finish = 0; finish < 2; ++finish) {
    for (Button button = BUTTON_UP; button < BUTTON_COUNT; ++button) {
      for (unsigned released = 0; released < 2; ++released) {
        RetailCommandCursor cursor = retail_command_cursor(image, character, 0, button, released != 0);
        RetailCommand command;
        while (retail_command_next(image, &cursor, &command)) {
          unsigned action = command.word & 255;
          bool finisher = (command.flags & 1) && action >= 64 && action <= 67;
          if (finisher != (finish != 0) || (command.pattern == 0xffffffffu && !released)) {
            continue;
          }
          require(count < CAPACITY, "Retail command display capacity exceeded");
          char *input = inputs[character][count];
          size_t used = 0;
          if (released) {
            used = (size_t)snprintf(input, INPUT_SIZE, "RELEASE ");
          }
          uint32_t pattern = command.pattern == 0xffffffffu ? 0 : command.pattern;
          for (; pattern; pattern >>= 8) {
            const char *direction = (pattern & 255) == 1 ? "U" : (pattern & 255) == 2 ? "D" : (pattern & 255) == 0xaa ? "B" : "F";
            int written = snprintf(input + used, INPUT_SIZE - used, "%s%s", direction, pattern >> 8 ? "," : "+");
            require(written >= 0 && (size_t)written < INPUT_SIZE - used, "Retail command display input too long");
            used += (size_t)written;
          }
          int written = snprintf(input + used, INPUT_SIZE - used, "%s", buttons[button]);
          require(written >= 0 && (size_t)written < INPUT_SIZE - used, "Retail command display button too long");
          const char *name = finisher                              ? (action == 64      ? "FATALITY 1"
                                                                         : action == 67 ? "FATALITY 2"
                                                                                        : "STAGE FINISH")
                             : (command.flags & 1) && action == 41 ? "WEAPON"
                                                                   : "SPECIAL";
          entries[character][count] = (MoveListEntry){finisher ? MOVE_LIST_FINISHER : MOVE_LIST_SPECIAL, name, input};
          count++;
        }
      }
    }
  }
  definition->move_list = entries[character];
  definition->move_list_count = (int)count;
}

/* Native493080/StrikeAnim4Picker indexes the default inventory with the raw
 * roster slot. Script, stance and native action aliases do not apply here. */
void retail_weapon_read(const Mk4Image *image, unsigned character, RetailWeaponInventory *inventory) {
  require(character < MK4_ROSTER_COUNT && inventory != NULL, "Invalid retail inventory owner");
  uint32_t address = mk4_image_u32(image, RETAIL_WEAPON_INVENTORY_TABLE + character * 4) * 4;
  require(address >= MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN_4D0000 && address <= MK4_RETAIL_SCRIPT_SCRIPT_END_4EF000 - 32 && address % 4 == 0,
      "Invalid retail inventory descriptor");
  *inventory = (RetailWeaponInventory){.address = address,
      .type = mk4_image_u32(image, address),
      .object_template = mk4_image_u32(image, address + 4) * 4,
      .hand_binding = mk4_image_u32(image, address + 8) * 4,
      .buttons = mk4_image_u32(image, address + 12) * 4,
      .idle = mk4_image_u32(image, address + 16) * 4,
      .forward = mk4_image_u32(image, address + 20) * 4,
      .backward = mk4_image_u32(image, address + 24) * 4,
      .reserved = mk4_image_u32(image, address + 28)};
}

/* Native lists use a zero first word as their terminator. Keep the caller's
 * engine capacity as a bound, independent of the authored row count. */
unsigned retail_terminated_rows(const Mk4Image *image, uint32_t table, unsigned stride, unsigned limit) {
  require(stride > 0 && stride % 4 == 0, "Invalid retail table row stride");
  for (unsigned count = 0; count <= limit; ++count) {
    if (mk4_image_u32(image, table + count * stride) == 0) {
      return count;
    }
  }
  require(false, "Retail table exceeds its engine capacity or lacks a terminator");
  return 0;
}

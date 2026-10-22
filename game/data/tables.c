#include "data/layout.h"
#include "data/tables.h"

unsigned mk4_character_slot(unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail character");
  return character == 16 ? 2 : character == 17 ? 7 : character;
}

/* Native48e490 and close-entry46be40 use a different Noob alias from scripts. */
unsigned mk4_native_character_slot(unsigned character) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail native character");
  return character == 16 ? 2 : character == 17 ? 4 : character;
}

uint32_t mk4_action_target(const Mk4Image *image, unsigned flags, unsigned action) {
  // clang-format off: Keep table entries on one row, including long rows.
  static const uint32_t dispatch[][2] = {
    {1, MK4_ACTION_TABLE_1},
    {2, MK4_ACTION_TABLE_2},
    {4, MK4_ACTION_TABLE_4},
    {8, MK4_ACTION_TABLE_8},
    {0x20, MK4_ACTION_TABLE_32}
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

Mk4CommandCursor mk4_command_cursor(const Mk4Image *image, unsigned character, unsigned player, Button button, bool released) {
  require(character < MK4_ROSTER_COUNT && player < 2, "Invalid retail command owner");
  uint32_t table = 0;
  switch (button) {
    case BUTTON_UP:
      table = player ? MK4_P2_UP_COMMAND_TABLE : MK4_P1_UP_COMMAND_TABLE;
      break;
    case BUTTON_HIGH_PUNCH:
      table = player ? MK4_P2_HP_COMMAND_TABLE : MK4_P1_HP_COMMAND_TABLE;
      break;
    case BUTTON_LOW_PUNCH:
      table = player ? MK4_P2_LP_COMMAND_TABLE : MK4_P1_LP_COMMAND_TABLE;
      break;
    case BUTTON_BLOCK:
      table = player ? MK4_P2_BLOCK_COMMAND_TABLE : MK4_P1_BLOCK_COMMAND_TABLE;
      break;
    case BUTTON_HIGH_KICK:
      table = player ? MK4_P2_HK_COMMAND_TABLE : MK4_P1_HK_COMMAND_TABLE;
      break;
    case BUTTON_LOW_KICK:
      table = released ? (player ? MK4_P2_LK_RELEASE_COMMAND_TABLE : MK4_P1_LK_RELEASE_COMMAND_TABLE)
                       : (player ? MK4_P2_LK_COMMAND_TABLE : MK4_P1_LK_COMMAND_TABLE);
      break;
    case BUTTON_RUN:
      table = player ? MK4_P2_RUN_COMMAND_TABLE : MK4_P1_RUN_COMMAND_TABLE;
      break;
    default:
      return (Mk4CommandCursor){0};
  }
  if (released && button != BUTTON_LOW_KICK) {
    return (Mk4CommandCursor){0};
  }
  uint32_t packed = mk4_image_u32(image, table + character * 4);
  require(!packed || (packed >= MK4_DATA_PACKED_POINTERS_BEGIN / 4 && packed < MK4_DATA_IMAGE_END / 4), "Invalid retail command list");
  return (Mk4CommandCursor){.address = packed * 4, .remaining = 256};
}

bool mk4_command_next(const Mk4Image *image, Mk4CommandCursor *cursor, Mk4Command *command) {
  if (cursor->address == 0) {
    return false;
  }
  require(cursor->remaining > 0 && cursor->address >= MK4_DATA_PACKED_POINTERS_BEGIN && cursor->address + 12 <= MK4_DATA_IMAGE_END,
      "Unterminated retail command list");
  uint32_t pattern = mk4_image_u32(image, cursor->address);
  if (pattern == 0) {
    cursor->address = 0;
    return false;
  }
  uint32_t word = mk4_image_u32(image, cursor->address + 8);
  unsigned flags = (word >> 16) & 255u, predicate = word >> 24;
  *command = (Mk4Command){.pattern = pattern,
      .extra = mk4_image_u32(image, cursor->address + 4),
      .word = word,
      .flags = flags,
      .window = (word >> 8) & 255u,
      .target = mk4_action_target(image, flags, word & 255u),
      .predicate = predicate ? mk4_image_u32(image, MK4_DATA_COMMAND_NEXT + predicate * 4) : 0};
  cursor->address += 12;
  cursor->remaining--;
  return true;
}

uint32_t mk4_cpu_target(const Mk4Image *image, unsigned character, unsigned distance) {
  require(distance < 4, "Invalid retail CPU table");
  uint32_t word = mk4_image_u32(image, MK4_CPU_ACTION_TABLES + distance * MK4_CPU_ACTION_TABLE_STRIDE + mk4_character_slot(character) * 4);
  return mk4_action_target(image, (word >> 16) & 255u, word & 255u);
}

int mk4_stance_clip(const Mk4Image *image, unsigned character, bool walking) {
  uint32_t packed = mk4_image_u32(image, (walking ? MK4_WALK_ANIMATION_TABLE : MK4_IDLE_ANIMATION_TABLE) + mk4_character_slot(character) * 4);
  require(packed >= MK4_ANIMATION_TABLE / 4 && packed < (MK4_ANIMATION_TABLE + MK4_CLIP_COUNT * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4 &&
              (packed * 4 - MK4_ANIMATION_TABLE) % MK4_ANIMATION_DESCRIPTOR_SIZE == 0,
      "Invalid retail stance descriptor");
  return (int)((packed * 4 - MK4_ANIMATION_TABLE) / MK4_ANIMATION_DESCRIPTOR_SIZE);
}

void mk4_roster_read(const Mk4Image *image, unsigned character, Mk4Roster *roster) {
  require(character < MK4_ROSTER_COUNT && roster != NULL, "Invalid retail roster destination");
  uint32_t name = mk4_image_u32(image, MK4_ROSTER_NAME_TABLE + character * 4);
  View memory = view(image->bytes);
  require(name >= MK4_DATA_IMAGE_BASE, "Invalid retail character name");
  size_t i = 0;
  for (; i + 1 < sizeof roster->name; ++i) {
    unsigned char ch = view_u8(memory, name - MK4_DATA_IMAGE_BASE + i);
    roster->name[i] = (char)(ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch);
    if (ch == 0) {
      break;
    }
  }
  require(i + 1 < sizeof roster->name, "Unterminated retail character name");
  uint32_t record = MK4_ROSTER_RECORD_TABLE + character * MK4_ROSTER_RECORD_SIZE;
  uint32_t slot = mk4_image_u32(image, record) * 4;
  uint32_t root = mk4_image_u32(image, record + 12) * 4;
  require(mk4_image_u32(image, root + 4) == slot, "Retail roster rig slot mismatch");
  roster->ground = (q16)mk4_image_u32(image, root + 16);
  roster->idle = mk4_stance_clip(image, character, false);
  roster->walk = mk4_stance_clip(image, character, true);
}

uint32_t mk4_finish_target(const Mk4Image *image, unsigned character, bool second) {
  uint32_t target = mk4_image_u32(image, (second ? MK4_SECOND_FINISH_TABLE : MK4_FIRST_FINISH_TABLE) + mk4_character_slot(character) * 4);
  require(target == 0 || (target >= MK4_DATA_IMAGE_BASE && target < MK4_DATA_PACKED_POINTERS_BEGIN), "Invalid retail scene native target");
  return target;
}

q16 mk4_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second) {
  /* 43460d maps the boss to14 for CPU approach distance, independently of
   * its scene support. Hidden scene aliases retain their shared data slot. */
  unsigned slot = character == 15 ? 14 : mk4_character_slot(character);
  q16 distance = (q16)mk4_image_u32(image, (second ? MK4_SECOND_FINISH_DISTANCE_TABLE : MK4_FIRST_FINISH_DISTANCE_TABLE) + slot * 4);
  require(distance >= 0 && distance <= 16 * Q16_ONE, "Invalid retail CPU finish distance");
  return distance;
}

uint32_t mk4_reaction_target(const Mk4Image *image, unsigned reaction) {
  require(reaction < MK4_REACTION_COUNT, "Invalid retail reaction selector");
  uint32_t target = mk4_image_u32(image, MK4_REACTION_TABLE + reaction * 4);
  require(target == 0 || (target >= MK4_DATA_IMAGE_BASE && target < MK4_DATA_PACKED_POINTERS_BEGIN), "Invalid retail reaction native");
  return target;
}

uint32_t mk4_lk_target(const Mk4Image *image, unsigned character) {
  unsigned slot = mk4_native_character_slot(character);
  uint32_t target = mk4_image_u32(image, MK4_LK_THROW_TABLE + slot * 4);
  require(target >= MK4_DATA_IMAGE_BASE && target < MK4_DATA_PACKED_POINTERS_BEGIN, "Invalid native LK entry");
  return target;
}

/* Native493080/StrikeAnim4Picker indexes the default inventory with the raw
 * roster slot. Script, stance and native action aliases do not apply here. */
void mk4_weapon_read(const Mk4Image *image, unsigned character, Mk4WeaponInventory *inventory) {
  require(character < MK4_ROSTER_COUNT && inventory != NULL, "Invalid retail inventory owner");
  uint32_t address = mk4_image_u32(image, MK4_WEAPON_INVENTORY_TABLE + character * 4) * 4;
  require(address >= MK4_DATA_PACKED_POINTERS_BEGIN && address <= MK4_SCRIPT_REGION_END - 32 && address % 4 == 0, "Invalid retail inventory descriptor");
  *inventory = (Mk4WeaponInventory){.address = address,
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
unsigned mk4_terminated_rows(const Mk4Image *image, uint32_t table, unsigned stride, unsigned limit) {
  require(stride > 0 && stride % 4 == 0, "Invalid retail table row stride");
  for (unsigned count = 0; count <= limit; ++count) {
    if (mk4_image_u32(image, table + count * stride) == 0) {
      return count;
    }
  }
  require(false, "Retail table exceeds its engine capacity or lacks a terminator");
  return 0;
}

/* Native475090 scans signed indices; any negative entry terminates the list. */
bool mk4_index_list_contains(const Mk4Image *image, uint32_t table, unsigned value, unsigned limit) {
  for (unsigned index = 0; index <= limit; ++index) {
    int32_t candidate = (int32_t)mk4_image_u32(image, table + index * 4);
    if (candidate < 0) {
      return false;
    }
    if ((unsigned)candidate == value) {
      return true;
    }
  }
  require(false, "Retail index list exceeds its engine capacity or lacks a terminator");
  return false;
}

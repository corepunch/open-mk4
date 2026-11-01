#include "script/native_ids.h"
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
    {1, dat_action_table_1},
    {2, dat_action_table_2},
    {4, dat_action_table_4},
    {8, dat_action_table_8},
    {0x20, dat_action_table_32}
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
      table = player ? dat_p2_up_command_table : dat_p1_up_command_table;
      break;
    case BUTTON_HIGH_PUNCH:
      table = player ? dat_p2_hp_command_table : dat_p1_hp_command_table;
      break;
    case BUTTON_LOW_PUNCH:
      table = player ? dat_p2_lp_command_table : dat_p1_lp_command_table;
      break;
    case BUTTON_BLOCK:
      table = player ? dat_p2_block_command_table : dat_p1_block_command_table;
      break;
    case BUTTON_HIGH_KICK:
      table = player ? dat_p2_hk_command_table : dat_p1_hk_command_table;
      break;
    case BUTTON_LOW_KICK:
      table = released ? (player ? dat_p2_lk_release_command_table : dat_p1_lk_release_command_table)
                       : (player ? dat_p2_lk_command_table : dat_p1_lk_command_table);
      break;
    case BUTTON_RUN:
      table = player ? dat_p2_run_command_table : dat_p1_run_command_table;
      break;
    default:
      return (Mk4CommandCursor){0};
  }
  if (released && button != BUTTON_LOW_KICK) {
    return (Mk4CommandCursor){0};
  }
  uint32_t packed = mk4_image_u32(image, table + character * 4);
  require(!packed || (packed >= dat_packed_pointers_begin / 4 && packed < MK4_DATA_IMAGE_END / 4), "Invalid retail command list");
  return (Mk4CommandCursor){.address = packed * 4, .remaining = 256};
}

bool mk4_command_next(const Mk4Image *image, Mk4CommandCursor *cursor, Mk4Command *command) {
  if (cursor->address == 0) {
    return false;
  }
  require(
      cursor->remaining > 0 && cursor->address >= dat_packed_pointers_begin && cursor->address + 12 <= MK4_DATA_IMAGE_END, "Unterminated retail command list");
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
      .predicate = predicate ? mk4_image_u32(image, dat_command_next + predicate * 4) : 0};
  cursor->address += 12;
  cursor->remaining--;
  return true;
}

uint32_t mk4_cpu_target(const Mk4Image *image, unsigned character, unsigned distance) {
  require(distance < 4, "Invalid retail CPU table");
  uint32_t word = mk4_image_u32(image, dat_cpu_action_tables + distance * MK4_CPU_ACTION_TABLE_STRIDE + mk4_character_slot(character) * 4);
  return mk4_action_target(image, (word >> 16) & 255u, word & 255u);
}

/* HitReactionStateCluster / PackedAdvanceCallTailJmp: a count followed by
 * native callbacks. Duplicate entries are intentional selection weights. */
Mk4CpuChoices mk4_cpu_choices(const Mk4Image *image, Mk4CpuRange range) {
  static const uint32_t tables[] = {dat_cpu_choices_close, dat_cpu_choices_near, dat_cpu_choices_medium, dat_cpu_choices_far};
  require(range >= MK4_CPU_CLOSE && range <= MK4_CPU_FAR, "Invalid CPU distance band");
  uint32_t table = tables[range];
  Mk4CpuChoices choices = {.count = mk4_image_u32(image, table)};
  require(choices.count > 0 && choices.count <= sizeof choices.targets / sizeof *choices.targets, "Invalid counted CPU choice table");
  for (unsigned i = 0; i < choices.count; ++i) {
    choices.targets[i] = mk4_image_u32(image, table + (i + 1) * 4);
  }
  return choices;
}

/* Local 438000 selects one of five distance/callback pairs, then 438630
 * approaches until strictly inside that distance before invoking the callback. */
Mk4CpuApproach mk4_cpu_approach(const Mk4Image *image, unsigned index) {
  require(index < (dat_cpu_near_attacks_end - dat_cpu_near_attacks) / 8, "Invalid CPU approach row");
  uint32_t row = dat_cpu_near_attacks + index * 8;
  Mk4CpuApproach approach = {(q16)mk4_image_u32(image, row), mk4_image_u32(image, row + 4)};
  require(approach.distance > 0 && approach.target != 0, "Invalid CPU approach parameters");
  return approach;
}

Mk4CpuWeights mk4_cpu_weights(const Mk4Image *image) {
  return (Mk4CpuWeights){(int32_t)mk4_image_u32(image, dat_cpu_reaction_weights), (int32_t)mk4_image_u32(image, dat_cpu_reaction_weights + 4),
      (int32_t)mk4_image_u32(image, dat_cpu_reaction_weights + 8), (int32_t)mk4_image_u32(image, dat_cpu_reaction_weights + 12)};
}

int mk4_stance_clip(const Mk4Image *image, unsigned character, bool walking) {
  uint32_t packed = mk4_image_u32(image, (walking ? dat_walk_animation_table : dat_idle_animation_table) + mk4_character_slot(character) * 4);
  require(packed >= dat_animation_table / 4 && packed < (dat_animation_table + MK4_CLIP_COUNT * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4 &&
              (packed * 4 - dat_animation_table) % MK4_ANIMATION_DESCRIPTOR_SIZE == 0,
      "Invalid retail stance descriptor");
  return (int)((packed * 4 - dat_animation_table) / MK4_ANIMATION_DESCRIPTOR_SIZE);
}

void mk4_roster_read(const Mk4Image *image, unsigned character, Mk4Roster *roster) {
  require(character < MK4_ROSTER_COUNT && roster != NULL, "Invalid retail roster destination");
  uint32_t name = mk4_image_u32(image, dat_roster_name_table + character * 4);
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
  uint32_t record = dat_roster_record_table + character * MK4_ROSTER_RECORD_SIZE;
  uint32_t slot = mk4_image_u32(image, record) * 4;
  uint32_t root = mk4_image_u32(image, record + 12) * 4;
  require(mk4_image_u32(image, root + 4) == slot, "Retail roster rig slot mismatch");
  roster->ground = (q16)mk4_image_u32(image, root + 16);
  roster->idle = mk4_stance_clip(image, character, false);
  roster->walk = mk4_stance_clip(image, character, true);
}

uint32_t mk4_finish_target(const Mk4Image *image, unsigned character, bool second) {
  uint32_t target = mk4_image_u32(image, (second ? dat_second_finish_table : dat_first_finish_table) + mk4_character_slot(character) * 4);
  require(target == 0 || (target >= native_id_begin && target < native_id_end), "Invalid retail scene native target");
  return target;
}

q16 mk4_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second) {
  /* 43460d maps the boss to14 for CPU approach distance, independently of
   * its scene support. Hidden scene aliases retain their shared data slot. */
  unsigned slot = character == 15 ? 14 : mk4_character_slot(character);
  q16 distance = (q16)mk4_image_u32(image, (second ? dat_second_finish_distance_table : dat_first_finish_distance_table) + slot * 4);
  require(distance >= 0 && distance <= 16 * Q16_ONE, "Invalid retail CPU finish distance");
  return distance;
}

uint32_t mk4_reaction_target(const Mk4Image *image, unsigned reaction) {
  require(reaction < MK4_REACTION_COUNT, "Invalid retail reaction selector");
  uint32_t target = mk4_image_u32(image, dat_reaction_table + reaction * 4);
  require(target == 0 || (target >= native_id_begin && target < native_id_end), "Invalid retail reaction native");
  return target;
}

uint32_t mk4_lk_target(const Mk4Image *image, unsigned character) {
  unsigned slot = mk4_native_character_slot(character);
  uint32_t target = mk4_image_u32(image, dat_lk_throw_table + slot * 4);
  require(target >= native_id_begin && target < native_id_end, "Invalid native LK entry");
  return target;
}

/* Native493080/StrikeAnim4Picker indexes the default inventory with the raw
 * roster slot. Script, stance and native action aliases do not apply here. */
void mk4_weapon_read(const Mk4Image *image, unsigned character, Mk4WeaponInventory *inventory) {
  require(character < MK4_ROSTER_COUNT && inventory != NULL, "Invalid retail inventory owner");
  uint32_t address = mk4_image_u32(image, dat_weapon_inventory_table + character * 4) * 4;
  require(address >= dat_packed_pointers_begin && address <= dat_region_end - 32 && address % 4 == 0, "Invalid retail inventory descriptor");
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

#ifndef MK4_DATA_TABLES_H
#define MK4_DATA_TABLES_H
#include "data/layout.h"
#include "game.h"

enum {
  MK4_REACTION_COUNT = (MK4_REACTION_TABLE_END - MK4_REACTION_TABLE) / 4
};

typedef struct Mk4Roster {
  char name[1024];
  q16 ground;
  int idle, walk;
} Mk4Roster;

typedef struct Mk4WeaponInventory {
  uint32_t address, type;
  uint32_t object_template, hand_binding, buttons;
  uint32_t idle, forward, backward;
  uint32_t reserved;
} Mk4WeaponInventory;

typedef struct Mk4CommandCursor {
  uint32_t address;
  unsigned remaining;
} Mk4CommandCursor;

typedef struct Mk4Command {
  uint32_t pattern, extra, word, target, predicate;
  unsigned flags, window;
} Mk4Command;

/* Addresses refer to the checked local image, never host function pointers. */
unsigned mk4_terminated_rows(const Mk4Image *image, uint32_t table, unsigned stride, unsigned limit);
bool mk4_index_list_contains(const Mk4Image *image, uint32_t table, unsigned value, unsigned limit);
void mk4_weapon_read(const Mk4Image *image, unsigned character, Mk4WeaponInventory *inventory);
void mk4_roster_read(const Mk4Image *image, unsigned character, Mk4Roster *roster);
unsigned mk4_character_slot(unsigned character);
unsigned mk4_native_character_slot(unsigned character);
uint32_t mk4_action_target(const Mk4Image *image, unsigned flags, unsigned action);
Mk4CommandCursor mk4_command_cursor(const Mk4Image *image, unsigned character, unsigned player, Button button, bool released);
bool mk4_command_next(const Mk4Image *image, Mk4CommandCursor *cursor, Mk4Command *command);
uint32_t mk4_cpu_target(const Mk4Image *image, unsigned character, unsigned distance);
int mk4_stance_clip(const Mk4Image *image, unsigned character, bool walking);
uint32_t mk4_finish_target(const Mk4Image *image, unsigned character, bool second);
q16 mk4_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second);
uint32_t mk4_reaction_target(const Mk4Image *image, unsigned reaction);
uint32_t mk4_lk_target(const Mk4Image *image, unsigned character);
#endif

#ifndef MK4_RETAIL_DATA_H
#define MK4_RETAIL_DATA_H
#include "game.h"

typedef struct RetailRoster {
  char name[1024];
  q16 ground;
  int idle, walk;
} RetailRoster;

typedef struct RetailCommandCursor {
  uint32_t address;
  unsigned remaining;
} RetailCommandCursor;

typedef struct RetailCommand {
  uint32_t pattern, extra, word, target, predicate;
  unsigned flags, window;
} RetailCommand;

/* Addresses refer to the checked local image, never host function pointers. */
void retail_roster_read(const Mk4Image *image, unsigned character, RetailRoster *roster);
unsigned retail_character_slot(unsigned character);
unsigned retail_native_character_slot(unsigned character);
uint32_t retail_action_target(const Mk4Image *image, unsigned flags, unsigned action);
RetailCommandCursor retail_command_cursor(const Mk4Image *image, unsigned character, unsigned player, Button button, bool released);
bool retail_command_next(const Mk4Image *image, RetailCommandCursor *cursor, RetailCommand *command);
uint32_t retail_cpu_target(const Mk4Image *image, unsigned character, unsigned distance);
void retail_bind_commands(Controls *controls, const Mk4Image *image, unsigned character);
int retail_stance_clip(const Mk4Image *image, unsigned character, bool walking);
uint32_t retail_finish_target(const Mk4Image *image, unsigned character, bool second);
q16 retail_cpu_finish_distance(const Mk4Image *image, unsigned character, bool second);
uint32_t retail_reaction_target(const Mk4Image *image, unsigned reaction);
uint32_t retail_lk_target(const Mk4Image *image, unsigned character);
#endif

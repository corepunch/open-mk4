#ifndef MK4_RETAIL_DATA_H
#define MK4_RETAIL_DATA_H
#include "game.h"

typedef struct RetailCommandCursor {
  uint32_t address;
  unsigned remaining;
} RetailCommandCursor;

typedef struct RetailCommand {
  uint32_t pattern, extra, word, target, predicate;
  unsigned flags, window;
} RetailCommand;

/* Addresses refer to the checked local image, never host function pointers. */
unsigned retail_character_slot(unsigned character);
uint32_t retail_action_target(const Mk4Image *image, unsigned flags, unsigned action);
RetailCommandCursor retail_command_cursor(const Mk4Image *image, unsigned character, unsigned player, Button button, bool released);
bool retail_command_next(const Mk4Image *image, RetailCommandCursor *cursor, RetailCommand *command);
uint32_t retail_cpu_target(const Mk4Image *image, unsigned character, unsigned distance);
void retail_bind_commands(Controls *controls, const Mk4Image *image, unsigned character);
int retail_stance_clip(const Mk4Image *image, unsigned character, bool walking);
#endif

#ifndef MK4_HOST_COMMANDS_H
#define MK4_HOST_COMMANDS_H

typedef enum MenuCommand {
  MENU_NONE,
  MENU_UP,
  MENU_DOWN,
  MENU_LEFT,
  MENU_RIGHT,
  MENU_CONFIRM,
  MENU_BACK
} MenuCommand;

typedef enum ShellCommand {
  SHELL_NONE,
  SHELL_QUIT,
  SHELL_PAUSE,
  SHELL_RESET,
  SHELL_REMATCH,
  SHELL_NEXT_P1,
  SHELL_NEXT_P2,
  SHELL_TOGGLE_CPU,
  SHELL_NEXT_ARENA
} ShellCommand;

typedef struct InputCommands {
  MenuCommand menu;
  ShellCommand shell;
} InputCommands;

#endif

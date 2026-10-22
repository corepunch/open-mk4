#ifndef MK4_MENU_H
#define MK4_MENU_H

#include "renderer.h"
#include "host_commands.h"
#include <stdbool.h>

typedef enum MenuScreen {
  MENU_MODE,
  MENU_SELECT,
  MENU_ARENA
} MenuScreen;

typedef enum MenuMode {
  MENU_ARCADE,
  MENU_TEAM,
  MENU_ENDURANCE,
  MENU_TOURNAMENT,
  MENU_PRACTICE,
  MENU_EXIT
} MenuMode;

typedef struct FrontendMenu {
  MenuScreen screen;
  MenuMode mode;
  int p1;
  int p2;
  int arena;
  int selecting_player;
  int cursor;
} FrontendMenu;

void menu_init(FrontendMenu *menu);
/* Returns true when a command starts a match. */
bool menu_command(FrontendMenu *menu, MenuCommand command, bool *quit);
/* Animation time is supplied by the host, in seconds. */
void menu_draw(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds);

#endif

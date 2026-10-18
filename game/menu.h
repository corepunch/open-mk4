#ifndef MK4_MENU_H
#define MK4_MENU_H

#include "renderer.h"
#include <stdbool.h>

typedef enum MenuScreen {
  MENU_TITLE,
  MENU_MODE,
  MENU_SELECT,
  MENU_ARENA
} MenuScreen;

typedef enum MenuMode {
  MENU_ARCADE,
  MENU_VERSUS
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
/* Returns true when a key starts a match. */
bool menu_key(FrontendMenu *menu, int keycode, bool *quit);
void menu_draw(Canvas *canvas, const Assets *assets, const FrontendMenu *menu);

#endif

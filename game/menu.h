#ifndef MK4_MENU_H
#define MK4_MENU_H

#include "renderer.h"
#include "host_commands.h"
#include <stdbool.h>

typedef enum MenuScreen {
  MENU_MODE,
  MENU_SELECT,
  MENU_ARENA,
  MENU_TOWER,
  MENU_CONTINUE,
  MENU_COMPLETE
} MenuScreen;

typedef enum MenuMode {
  MENU_ARCADE,
  MENU_TEAM,
  MENU_ENDURANCE,
  MENU_TOURNAMENT,
  MENU_PRACTICE,
  MENU_EXIT
} MenuMode;

enum {
  MENU_TOWERS = 5,
  MENU_TOWER_FLOORS = 16
};

typedef struct MenuTower {
  char name[32];
  int count;
  uint32_t opponents[MENU_TOWER_FLOORS];
} MenuTower;

typedef struct FrontendMenu {
  MenuScreen screen;
  MenuMode mode;
  int p1;
  int p2;
  int arena;
  int selecting_player;
  int cursor;
  MenuTower towers[MENU_TOWERS];
  int tower, floor, face;
  bool climbing;
} FrontendMenu;

void menu_init(FrontendMenu *menu, const Mk4Image *image);
int menu_tower_opponent(const FrontendMenu *menu, int tower, int floor);
/* Called once after the match presentation completes. Defeat retains the floor. */
void menu_match_finished(FrontendMenu *menu, bool won);
/* Returns true when a command starts a match. */
bool menu_command(FrontendMenu *menu, MenuCommand command, bool *quit);
/* Animation time is supplied by the host, in seconds. */
void menu_draw(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds);

#endif

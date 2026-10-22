#include "game.h"

const Mk4Image *battle_data(void) {
  require(g_battle.data != NULL, "MK4 match data is not bound");
  return g_battle.data;
}

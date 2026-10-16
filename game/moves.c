#include "game.h"

void fighter_start_move(Fighter *fighter, int state, const MoveDef *move) {
  require(move != NULL && state >= 0 && state < S_COUNT, "Invalid move entry");
  fighter->state = state;
  fighter->current_move = move;
  fighter->ticks = 0;
  fighter->connected = false;
}

void fighter_enter_state(Battle *battle, Fighter *fighter, int state) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(state >= 0 && state < def->move_count, "Invalid character state");
  if (fighter->state == state && fighter->current_move != NULL) {
    return;
  }
  if (def->enter != NULL && def->enter[state] != NULL) {
    def->enter[state](battle, fighter);
  } else {
    fighter_start_move(fighter, state, def->moves[state]);
  }
}

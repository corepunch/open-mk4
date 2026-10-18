#include "../character.h"

void jarek_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &jarek_s_crouch);
}

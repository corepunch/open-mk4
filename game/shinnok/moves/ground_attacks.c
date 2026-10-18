#include "../character.h"

void shinnok_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &shinnok_s_crouch);
}

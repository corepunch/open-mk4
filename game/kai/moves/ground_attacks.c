#include "../character.h"

void kai_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &kai_s_crouch);
}

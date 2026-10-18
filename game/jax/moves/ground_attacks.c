#include "../character.h"

void jax_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &jax_s_crouch);
}

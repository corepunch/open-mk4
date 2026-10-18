#include "../character.h"

static const MoveDef s_low_kick = {.clip = 70, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};

void jax_high_kick(Fighter *fighter) {
  jax_start(fighter, S_KICK, &jax_s_high_kick, jax_entry_retail_script_004e5ca0);
}

void jax_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_low_kick);
  fighter->action = ACTION_RETAIL_0102;
  jax_entry_retail_script_004e5d90(fighter);
}

#include "../character.h"

void jax_lk_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_lk_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void jax_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &jax_s_throw, &jax_s_thrown, jax_entry_retail_script_004de798);
}

void jax_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &s_lk_throw, &s_lk_thrown, jax_entry_retail_script_004de2f8);
}

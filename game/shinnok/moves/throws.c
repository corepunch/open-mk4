#include "../character.h"

static void shinnok_ground_throw(Fighter *fighter, FighterFn script);
void shinnok_lk_throw(Fighter *fighter);
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static void shinnok_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_mmove_throw_start(fighter, &shinnok_s_throw, &s_thrown, script);
}

void shinnok_lk_throw(Fighter *fighter) {
  shinnok_ground_throw(fighter, shinnok_entry_retail_script_004de2f8);
}

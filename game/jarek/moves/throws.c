#include "../character.h"

static void jarek_ground_throw(Fighter *fighter, FighterFn script);
void jarek_lk_throw(Fighter *fighter);
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static void jarek_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_mmove_throw_start(fighter, &jarek_s_throw, &s_thrown, script);
}

void jarek_lk_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, jarek_entry_retail_script_004de2f8);
}

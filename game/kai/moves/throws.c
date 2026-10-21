#include "../character.h"

void kai_lk_throw(Fighter *fighter);
static const MoveDef s_thrown = {157, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};

void kai_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &kai_s_throw, &s_thrown, kai_entry_retail_script_004de2f8);
}

#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void tanya_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &tanya_s_throw, &s_ground_thrown, tanya_entry_retail_script_004de2f8);
}

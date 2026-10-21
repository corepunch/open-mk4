#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void cage_lk_throw(Fighter *fighter);

void cage_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &cage_s_throw, &s_ground_thrown, cage_entry_retail_script_004de798);
}

void cage_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &cage_s_throw, &s_ground_thrown, cage_entry_retail_script_004de2f8);
}

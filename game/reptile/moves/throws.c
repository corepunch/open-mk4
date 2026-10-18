#include "../character.h"

void reptile_lk_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_RECORD | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};

void reptile_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &reptile_s_throw, &reptile_s_lp_thrown, reptile_entry_retail_script_004de798);
}

void reptile_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &s_lk_throw, &s_lk_thrown, reptile_entry_retail_script_004de2f8);
}

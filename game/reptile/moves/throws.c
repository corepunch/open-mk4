#include "../character.h"

void reptile_lk_throw(Fighter *fighter);
static void reptile_air_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};

void reptile_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &reptile_s_throw, &reptile_s_lp_thrown, reptile_entry_retail_script_004de798);
}

void reptile_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, reptile_entry_retail_script_004de2f8);
}

static void reptile_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, reptile_entry_retail_script_004e5e28, reptile_entry_retail_script_004e9910);
}

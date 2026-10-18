#include "../character.h"

void fujin_lk_throw(Fighter *fighter);
static void fujin_lp_throw(Fighter *fighter);
static void fujin_air_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};

void fujin_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, fujin_entry_retail_script_004de2f8);
}

static void fujin_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &fujin_s_throw, &s_lp_thrown, fujin_entry_retail_script_004de798);
}

static void fujin_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, fujin_entry_retail_script_004e5e28, fujin_entry_retail_script_004e9910);
}

void fujin_enter_throw(Fighter *fighter) {
  fujin_lp_throw(fighter);
}

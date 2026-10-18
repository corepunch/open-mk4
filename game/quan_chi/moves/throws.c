#include "../character.h"

void quan_chi_lk_throw(Fighter *fighter);
static void quan_chi_air_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION};

void quan_chi_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &quan_chi_s_throw, &s_lk_thrown, quan_chi_entry_retail_script_004de798);
}

void quan_chi_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, quan_chi_entry_retail_script_004de2f8);
}

static void quan_chi_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, quan_chi_entry_retail_script_004e5e28, quan_chi_entry_retail_script_004e9910);
}

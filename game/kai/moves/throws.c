#include "../character.h"

void kai_lk_throw(Fighter *fighter);
static void kai_air_throw(Fighter *fighter);
static void kai_script_thrown_return(Fighter *fighter);
static const MoveDef s_thrown = {157, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};

void kai_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &kai_s_throw, &s_thrown, kai_entry_retail_script_004de798);
}

void kai_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &kai_s_throw, &s_thrown, kai_entry_retail_script_004de2f8);
}

static void kai_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, kai_entry_retail_script_004e5e28, kai_entry_retail_script_004e9910);
}

static void kai_script_thrown_return(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

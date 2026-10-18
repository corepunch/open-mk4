#include "../character.h"

static void shinnok_ground_throw(Fighter *fighter, FighterFn script);
static void shinnok_lk_throw(Fighter *fighter);
static void shinnok_air_throw(Fighter *fighter);
static void shinnok_throw_reaction(Fighter *fighter);
static void shinnok_press_air_throw(Fighter *fighter);
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static void shinnok_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_script_throw_start(fighter, &shinnok_s_throw, &s_thrown, script);
}

void shinnok_lp_throw(Fighter *fighter) {
  shinnok_ground_throw(fighter, shinnok_entry_retail_script_004de798);
}

static void shinnok_lk_throw(Fighter *fighter) {
  shinnok_ground_throw(fighter, shinnok_entry_retail_script_004de2f8);
}

static void shinnok_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, shinnok_entry_retail_script_004e5e28, shinnok_entry_retail_script_004e9910);
}

static void shinnok_throw_reaction(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static void shinnok_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_air_throw);
}

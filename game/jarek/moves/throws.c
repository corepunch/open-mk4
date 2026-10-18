#include "../character.h"

static void jarek_ground_throw(Fighter *fighter, FighterFn script);
void jarek_lk_throw(Fighter *fighter);
static void jarek_air_throw(Fighter *fighter);
static void jarek_throw_reaction(Fighter *fighter);
static void jarek_press_air_throw(Fighter *fighter);
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static void jarek_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_script_throw_start(fighter, &jarek_s_throw, &s_thrown, script);
}

void jarek_lp_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, jarek_entry_retail_script_004de798);
}

void jarek_lk_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, jarek_entry_retail_script_004de2f8);
}

static void jarek_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, jarek_entry_retail_script_004e5e28, jarek_entry_retail_script_004e9910);
}

static void jarek_throw_reaction(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static void jarek_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_throw);
}

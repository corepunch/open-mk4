#include "../character.h"

static void reiko_lk_throw(Fighter *fighter);
static void reiko_air_throw(Fighter *fighter);
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

void reiko_lp_throw(Fighter *fighter) {
  reiko_start_script(fighter, S_THROW, &reiko_s_throw, reiko_entry_retail_script_004de798);
}

static void reiko_lk_throw(Fighter *fighter) {
  reiko_start_script(fighter, S_THROW, &reiko_s_throw, reiko_entry_retail_script_004de2f8);
}

static void reiko_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, reiko_entry_retail_script_004e5e28, NULL);
}

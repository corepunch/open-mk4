#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

void tanya_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &tanya_s_throw, &s_ground_thrown, tanya_entry_retail_script_004de798);
}

void tanya_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &tanya_s_throw, &s_ground_thrown, tanya_entry_retail_script_004de2f8);
}

void tanya_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, tanya_entry_retail_script_004e5e28, NULL);
}

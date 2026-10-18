#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void noob_saibot_lk_throw(Fighter *fighter);
void noob_saibot_air_throw(Fighter *fighter);
static const MoveDef s_noob_air_throw = {.clip = 74, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_noob_air_thrown = {.clip = 150, .last = 27, .flags = SF_LOCK, .action = 0x4003};

void noob_saibot_script_air_throw_continue(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  noob_saibot_entry_retail_script_004e5ea0(fighter);
}

void noob_saibot_script_air_throw_end(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_damage(victim, 0x30a3);
  fighter_fiber_finish(noob_saibot_ready);
}

void noob_saibot_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de798);
}

void noob_saibot_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de2f8);
}

void noob_saibot_air_throw(Fighter *fighter) {
  mk4_air_throw_generated_with_moves(
      fighter, &s_noob_air_throw, &s_noob_air_thrown, noob_saibot_entry_retail_script_004e5e78, noob_saibot_entry_retail_script_004e9910);
}

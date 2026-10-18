#include "../character.h"

static void noob_saibot_script_air_release(Fighter *fighter);
static void noob_saibot_air_hp(Fighter *fighter);
static void noob_saibot_air_lp(Fighter *fighter);
static void noob_saibot_air_kick(Fighter *fighter);
static void noob_saibot_air_diagonal_hk(Fighter *fighter);
static void noob_saibot_air_diagonal_lk(Fighter *fighter);

static void noob_saibot_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, noob_saibot_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  noob_saibot_entry_retail_script_004e5e50(fighter);
}

void noob_saibot_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void noob_saibot_air_hp(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &noob_saibot_s_jab, noob_saibot_entry_retail_script_004e6658);
}

static void noob_saibot_air_lp(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &noob_saibot_s_jab, noob_saibot_entry_retail_script_004e6710);
}

static void noob_saibot_air_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e61f8);
}

static void noob_saibot_air_diagonal_hk(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e6500);
}

static void noob_saibot_air_diagonal_lk(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e63d8);
}

void noob_saibot_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_air_hp);
}

void noob_saibot_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_air_lp);
}

void noob_saibot_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? noob_saibot_air_diagonal_hk : noob_saibot_air_kick);
}

void noob_saibot_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? noob_saibot_air_diagonal_lk : noob_saibot_air_kick);
}

#include "../character.h"

static void goro_air_hp(Fighter *fighter);
static void goro_air_lp(Fighter *fighter);
static void goro_air_kick(Fighter *fighter);
static void goro_air_diagonal_hk(Fighter *fighter);
static void goro_air_diagonal_lk(Fighter *fighter);

void goro_script_lp_pair_start(Fighter *fighter) {
  mk4_script_pair_victim(fighter, &goro_s_knockdown);
}

void goro_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, goro_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  goro_entry_retail_script_004e5e50(fighter);
}

void goro_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void goro_air_hp(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &goro_s_jab, goro_entry_retail_script_004e6658);
}

static void goro_air_lp(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &goro_s_jab, goro_entry_retail_script_004e6710);
}

static void goro_air_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e61f8);
}

static void goro_air_diagonal_hk(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e6500);
}

static void goro_air_diagonal_lk(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e63d8);
}

void goro_enter_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_hp);
}

void goro_enter_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_lp);
}

void goro_enter_air_hk(Fighter *fighter) {
  bool diagonal = fighter->body.vel[AXIS_X] != 0 || fighter->body.vel[AXIS_Z] != 0;
  fighter_interrupt(fighter, diagonal ? goro_air_diagonal_hk : goro_air_kick);
}

void goro_enter_air_lk(Fighter *fighter) {
  bool diagonal = fighter->body.vel[AXIS_X] != 0 || fighter->body.vel[AXIS_Z] != 0;
  fighter_interrupt(fighter, diagonal ? goro_air_diagonal_lk : goro_air_kick);
}

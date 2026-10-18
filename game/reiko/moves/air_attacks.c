#include "../character.h"

static void reiko_air_hp(Fighter *fighter);
static void reiko_air_lp(Fighter *fighter);
static void reiko_air_kick(Fighter *fighter);
static void reiko_air_diagonal_hk(Fighter *fighter);
static void reiko_air_diagonal_lk(Fighter *fighter);

void reiko_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, reiko_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  reiko_entry_retail_script_004e5e50(fighter);
}

void reiko_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void reiko_air_hp(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &reiko_s_jab, reiko_entry_retail_script_004e6658);
}

static void reiko_air_lp(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &reiko_s_jab, reiko_entry_retail_script_004e6710);
}

static void reiko_air_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e61f8);
}

static void reiko_air_diagonal_hk(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e6500);
}

static void reiko_air_diagonal_lk(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e63d8);
}

void reiko_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_air_hp);
}

void reiko_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_air_lp);
}

void reiko_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reiko_air_diagonal_hk : reiko_air_kick);
}

void reiko_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reiko_air_diagonal_lk : reiko_air_kick);
}

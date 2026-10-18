#include "../character.h"

static void cage_air_hp(Fighter *fighter);
static void cage_air_lp(Fighter *fighter);
static void cage_air_kick(Fighter *fighter);
static void cage_air_diagonal_hk(Fighter *fighter);
static void cage_air_diagonal_lk(Fighter *fighter);

void cage_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, cage_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  cage_entry_retail_script_004e5e50(fighter);
}

void cage_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void cage_air_hp(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &cage_s_jab, cage_entry_retail_script_004e6658);
}

static void cage_air_lp(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &cage_s_jab, cage_entry_retail_script_004e6710);
}

static void cage_air_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_kick, cage_entry_retail_script_004e61f8);
}

static void cage_air_diagonal_hk(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_kick, cage_entry_retail_script_004e6500);
}

static void cage_air_diagonal_lk(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_kick, cage_entry_retail_script_004e63d8);
}

void cage_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_air_hp);
}

void cage_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_air_lp);
}

void cage_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? cage_air_diagonal_hk : cage_air_kick);
}

void cage_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? cage_air_diagonal_lk : cage_air_kick);
}

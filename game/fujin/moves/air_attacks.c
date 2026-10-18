#include "../character.h"

static void fujin_air_hp(Fighter *fighter);
static void fujin_air_lp(Fighter *fighter);
static void fujin_air_kick(Fighter *fighter);
static void fujin_air_diagonal_hk(Fighter *fighter);
static void fujin_air_diagonal_lk(Fighter *fighter);
static void fujin_enter_air_punch(Fighter *fighter);

void fujin_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, fujin_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  fujin_entry_retail_script_004e5e50(fighter);
}

void fujin_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void fujin_air_hp(Fighter *fighter) {
  fujin_start(fighter, S_PUNCH, &fujin_s_jab, fujin_entry_retail_script_004e6658);
}

static void fujin_air_lp(Fighter *fighter) {
  fujin_start(fighter, S_PUNCH, &fujin_s_jab, fujin_entry_retail_script_004e6710);
}

static void fujin_air_kick(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &fujin_s_kick, fujin_entry_retail_script_004e61f8);
}

static void fujin_air_diagonal_hk(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &fujin_s_kick, fujin_entry_retail_script_004e6500);
}

static void fujin_air_diagonal_lk(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &fujin_s_kick, fujin_entry_retail_script_004e63d8);
}

static void fujin_enter_air_punch(Fighter *fighter) {
  fujin_air_hp(fighter);
}

void fujin_press_air_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_air_punch);
}

void fujin_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_air_lp);
}

void fujin_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? fujin_air_diagonal_hk : fujin_air_kick);
}

void fujin_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? fujin_air_diagonal_lk : fujin_air_kick);
}

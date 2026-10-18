#include "../character.h"

static void jarek_air_hp(Fighter *fighter);
static void jarek_air_lp(Fighter *fighter);
static void jarek_air_kick(Fighter *fighter);
static void jarek_air_diagonal_hk(Fighter *fighter);
static void jarek_air_diagonal_lk(Fighter *fighter);
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};

static void jarek_air_hp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, jarek_entry_retail_script_004e6658);
}

static void jarek_air_lp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, jarek_entry_retail_script_004e6710);
}

static void jarek_air_kick(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, jarek_entry_retail_script_004e61f8);
}

static void jarek_air_diagonal_hk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, jarek_entry_retail_script_004e6500);
}

static void jarek_air_diagonal_lk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, jarek_entry_retail_script_004e63d8);
}

void jarek_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  jarek_entry_retail_script_004e5e50(fighter);
}

void jarek_script_air_finish(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(mk4_land);
}

void jarek_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_hp);
}

void jarek_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_lp);
}

void jarek_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_kick);
}

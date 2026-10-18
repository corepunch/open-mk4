#include "../character.h"

static void jax_air_hp(Fighter *fighter);
static void jax_air_lp(Fighter *fighter);
static void jax_air_kick(Fighter *fighter);
static void jax_air_diagonal_hk(Fighter *fighter);
static void jax_air_diagonal_lk(Fighter *fighter);
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};

void jax_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void jax_air_hp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, jax_entry_retail_script_004e6658);
}

static void jax_air_lp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, jax_entry_retail_script_004e6710);
}

static void jax_air_kick(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, jax_entry_retail_script_004e61f8);
}

static void jax_air_diagonal_hk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, jax_entry_retail_script_004e6500);
}

static void jax_air_diagonal_lk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, jax_entry_retail_script_004e63d8);
}

void jax_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_hp);
}

void jax_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_lp);
}

void jax_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_kick);
}

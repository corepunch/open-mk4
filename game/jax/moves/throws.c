#include "../character.h"

void jax_lk_throw(Fighter *fighter);
static void jax_air_throw(Fighter *fighter);
static void jax_press_air_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_lk_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

static void jax_air_grab_victim(Fighter *victim) {
  jax_entry_retail_script_004e98e8(victim);
}

void jax_script_air_throw_ground_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_damage(victim, 0x2147);
  mk4_script_pair_cleanup(fighter);
  static const MoveDef released = {.clip = 140, .last = 62, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
  fighter_start_move(victim, S_KNOCKDOWN, &released);
  victim->reaction = 166;
  fighter_start_fiber(victim, jax_air_grab_victim, NULL);
  fighter->flags |= FF_PAIR_RELEASE;
  jax_entry_retail_script_004e5e18(fighter);
}

void jax_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &jax_s_throw, &jax_s_thrown, jax_entry_retail_script_004de798);
}

void jax_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &s_lk_throw, &s_lk_thrown, jax_entry_retail_script_004de2f8);
}

static void jax_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, jax_entry_retail_script_004e5e28, jax_entry_retail_script_004e9910);
}

void jax_script_air_throw_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  jax_entry_retail_script_004e5e50(fighter);
}

static void jax_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_throw);
}

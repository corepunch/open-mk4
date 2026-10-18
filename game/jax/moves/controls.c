#include "../character.h"

static void jax_script_cpu_496f80(Fighter *fighter);
static void jax_cpu_medium(Fighter *fighter);
static void jax_command_run(Fighter *fighter);
static void jax_command_medium(Fighter *fighter);

static void jax_script_cpu_496f80(Fighter *fighter) {
  fighter_start_fiber(fighter, jax_entry_retail_script_004ed090, NULL);
}

static void jax_cpu_medium(Fighter *fighter) {
  fighter->action = 0x101;
  fighter->script_phase = 0;
  battle_cpu_step(fighter);
}

void jax_press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? fighter->weapon->lp : (fighter_distance(fighter) <= 0.7f ? jax_lp_throw : jax_jab);
  fighter_interrupt(fighter, entry);
}

void jax_press_hk(Fighter *fighter) {
  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, jax_close_hk);
    return;
  }

  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_high_kick);
}

void jax_press_lk(Fighter *fighter) {
  FighterFn entry = fighter_holds_back(fighter) ? jax_sweep : jax_low_kick;
  fighter_interrupt(fighter, entry);
}

static void jax_command_run(Fighter *fighter) {
  mk4_walk(fighter);
}

static void jax_command_medium(Fighter *fighter) {
  jax_cpu_medium(fighter);
}

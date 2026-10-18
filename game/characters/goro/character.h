#ifndef MK4_CHARACTER_GORO_PRIVATE_H
#define MK4_CHARACTER_GORO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void goro_ready(Fighter *fighter);
static void goro_script_current_reaction(Fighter *fighter, int reaction);
static void goro_script_opponent_reaction(Fighter *fighter, int reaction);
static void goro_script_lk_return(Fighter *fighter);
static void goro_script_lk_finish(Fighter *fighter);
static void goro_script_lp_pair_start(Fighter *fighter);
static void goro_script_lp_phase_repeat(Fighter *fighter);
static void goro_script_event_finish(Fighter *fighter);
static void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action);
static void goro_event_progress(Fighter *fighter);
static void goro_event_counter_kick(Fighter *fighter);
static void goro_event_counter_sweep(Fighter *fighter);
static void goro_script_fan_cycle(Fighter *fighter);
static void goro_script_fan_victim_end(Fighter *fighter);
static void goro_script_fan_victory(Fighter *fighter);
static void goro_script_fan_tail(Fighter *fighter);
static void goro_script_air_release(Fighter *fighter);
static void goro_script_spike_tail(Fighter *fighter);
static void goro_script_finish(Fighter *fighter);
static void goro_script_kick_finish(Fighter *fighter);
static void goro_script_air_finish(Fighter *fighter);
static void goro_script_roundhouse_start(Fighter *fighter);
void goro_fan_fatality(Fighter *fighter);
void goro_spike_fatality(Fighter *fighter);
static void goro_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void goro_cpu_projectile_step(Projectile *projectile);
static void goro_cpu_projectile_spawn(Fighter *fighter, int task_type);
static void goro_cpu_projectile(Fighter *fighter);
void goro_roundhouse(Fighter *fighter);
void goro_lk_throw(Fighter *fighter);
static void goro_jab(Fighter *fighter);
static void goro_lp_throw(Fighter *fighter);
static void goro_high_punch(Fighter *fighter);
static void goro_uppercut(Fighter *fighter);
static void goro_sweep(Fighter *fighter);
static void goro_high_kick(Fighter *fighter);
static void goro_low_kick(Fighter *fighter);
static void goro_air_throw(Fighter *fighter);
static void goro_air_hp(Fighter *fighter);
static void goro_air_lp(Fighter *fighter);
static void goro_air_kick(Fighter *fighter);
static void goro_air_diagonal_hk(Fighter *fighter);
static void goro_air_diagonal_lk(Fighter *fighter);
static void goro_enter_jab(Fighter *fighter);
static void goro_enter_low_punch(Fighter *fighter);
static void goro_enter_uppercut(Fighter *fighter);
static void goro_enter_sweep(Fighter *fighter);
static void goro_enter_high_kick(Fighter *fighter);
static void goro_enter_low_kick(Fighter *fighter);
static void goro_enter_air_hp(Fighter *fighter);
static void goro_enter_air_lp(Fighter *fighter);
static void goro_enter_air_hk(Fighter *fighter);
static void goro_enter_air_lk(Fighter *fighter);
#endif

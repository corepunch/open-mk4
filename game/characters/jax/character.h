#ifndef MK4_CHARACTER_JAX_PRIVATE_H
#define MK4_CHARACTER_JAX_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void jax_ready(Fighter *fighter);
static bool jax_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static bool jax_contact_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void jax_script_fatality_1_scene_signal(Fighter *fighter);
static void jax_script_fatality_1_finish(Fighter *fighter);
static void jax_script_fatality_2_task(Fighter *fighter, int task_type);
static void jax_script_fatality_2_scene_finish(Fighter *fighter);
static void jax_script_air_throw_ground_release(Fighter *fighter);
static void jax_script_jab_contact(Fighter *fighter);
static void jax_script_jab_followup(Fighter *fighter);
static void jax_script_cpu_projectile_step(Projectile *projectile);
static void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type);
static void jax_script_opponent_reaction(Fighter *fighter, int reaction);
static void jax_script_current_reaction(Fighter *fighter, int reaction);
static void jax_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
static void jax_idle(Fighter *fighter);
static void jax_walk(Fighter *fighter);
static void jax_crouch(Fighter *fighter);
static void jax_block(Fighter *fighter);
static void jax_jab(Fighter *fighter);
static void jax_high_punch(Fighter *fighter);
static void jax_high_kick(Fighter *fighter);
static void jax_low_kick(Fighter *fighter);
static void jax_sweep(Fighter *fighter);
static void jax_uppercut(Fighter *fighter);
static void jax_roundhouse(Fighter *fighter);
static void jax_script_roundhouse_start(Fighter *fighter);
static void jax_script_roundhouse_tail(Fighter *fighter);
static void jax_script_finish(Fighter *fighter);
static void jax_script_air_finish(Fighter *fighter);
static void jax_air_hp(Fighter *fighter);
static void jax_air_lp(Fighter *fighter);
static void jax_air_kick(Fighter *fighter);
static void jax_air_diagonal_hk(Fighter *fighter);
static void jax_air_diagonal_lk(Fighter *fighter);
static void jax_lp_throw(Fighter *fighter);
static void jax_lk_throw(Fighter *fighter);
static void jax_air_throw(Fighter *fighter);
static void jax_script_air_throw_release(Fighter *fighter);
static void jax_fan_start(Fighter *fighter);
static void jax_fan_tail(Fighter *fighter);
static void jax_spike_tail(Fighter *fighter);
static void jax_script_input_47ab10(Fighter *fighter);
static void jax_script_input_47ab50(Fighter *fighter);
static void jax_script_cpu_496f80(Fighter *fighter);
static void jax_script_input_46a990(Fighter *fighter);
static void jax_script_input_46a9f0(Fighter *fighter);
static void jax_script_weapon_draw_finish(Fighter *fighter);
static void jax_fan_fatality(Fighter *fighter);
static void jax_spike_fatality(Fighter *fighter);
static void jax_script_fatality_1_tail(Fighter *fighter);
static void jax_script_fatality_1_actor_tail(Fighter *fighter);
static void jax_script_fatality_1_final_tail(Fighter *fighter);
static void jax_script_fatality_1_actor_variant_a(Fighter *fighter);
static void jax_script_fatality_1_actor_variant_b(Fighter *fighter);
static void jax_script_fatality_1_scene_task_a(Fighter *fighter);
static void jax_script_fatality_1_scene_task_b(Fighter *fighter);
static void jax_script_lk_return(Fighter *fighter);
static void jax_script_lk_finish(Fighter *fighter);
static void jax_script_fatality_1_victim(Fighter *fighter);
static void jax_script_fatality_1_victim_alt(Fighter *fighter);
static void jax_script_fatality_2_tail(Fighter *fighter);
static void jax_script_fatality_2_victim(Fighter *fighter);
static void jax_fatality_1(Fighter *fighter);
static void jax_fatality_2(Fighter *fighter);
static void jax_cpu_medium(Fighter *fighter);
static void jax_weapon_hp(Fighter *fighter);
static void jax_weapon_lp(Fighter *fighter);
static void jax_weapon_back_hp(Fighter *fighter);
static void jax_weapon_back_lp(Fighter *fighter);
static void jax_weapon_equip(Fighter *fighter);
static void jax_weapon_action(Fighter *fighter);
static void jax_press_hp(Fighter *fighter);
static void jax_press_lp(Fighter *fighter);
static void jax_press_hk(Fighter *fighter);
static void jax_press_lk(Fighter *fighter);
static void jax_press_uppercut(Fighter *fighter);
static void jax_press_sweep(Fighter *fighter);
static void jax_press_air_hp(Fighter *fighter);
static void jax_press_air_lp(Fighter *fighter);
static void jax_press_air_kick(Fighter *fighter);
static void jax_press_air_throw(Fighter *fighter);
static void jax_command_weapon(Fighter *fighter);
static void jax_command_spike(Fighter *fighter);
static void jax_command_fan(Fighter *fighter);
static void jax_command_run(Fighter *fighter);
static void jax_command_sidestep(Fighter *fighter);
static void jax_command_medium(Fighter *fighter);
#endif

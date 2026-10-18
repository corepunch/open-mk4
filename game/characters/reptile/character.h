#ifndef MK4_CHARACTER_REPTILE_PRIVATE_H
#define MK4_CHARACTER_REPTILE_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void reptile_clone_step(Projectile *projectile);
static bool reptile_visibility_event(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static BattleEvent *reptile_find_event(Fighter *fighter, BattleEventFn step, int kind);
static void reptile_script_visibility_start(Fighter *fighter);
static void reptile_script_visibility_extend(Fighter *fighter);
static void reptile_script_visibility_end(Fighter *fighter);
static void reptile_script_visibility_phase(Fighter *fighter);
static bool reptile_clone_pose(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static bool reptile_clone_contact(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static bool reptile_task_76_event(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static void reptile_task_76_spawn(Fighter *fighter, int task_type);
static void reptile_ready(Fighter *fighter);
static void reptile_script_lk_return(Fighter *fighter);
static void reptile_script_47a6d0(Fighter *fighter);
static void reptile_script_47ab50(Fighter *fighter);
static void reptile_script_47b260(Fighter *fighter);
static void reptile_loop_frame(Fighter *fighter);
static void reptile_script_47b010(Fighter *fighter);
static void reptile_script_reaction_side_getup(Fighter *fighter);
static void reptile_script_lp_select(Fighter *fighter);
static void reptile_script_lp_end(Fighter *fighter);
static void reptile_script_lp_return(Fighter *fighter);
static void reptile_script_current_reaction(Fighter *fighter, int reaction);
void reptile_reaction_40(Fighter *fighter);
static void reptile_script_opponent_reaction(Fighter *fighter, int reaction);
static void reptile_script_fan_cycle(Fighter *fighter);
static void reptile_script_fan_victim_end(Fighter *fighter);
static void reptile_script_fan_tail(Fighter *fighter);
static void reptile_script_air_release(Fighter *fighter);
static void reptile_script_finish(Fighter *fighter);
static void reptile_script_air_finish(Fighter *fighter);
static void reptile_script_roundhouse_start(Fighter *fighter);
static void reptile_script_spike_tail(Fighter *fighter);
static void reptile_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void reptile_lp_throw(Fighter *fighter);
static void reptile_lk_throw(Fighter *fighter);
static void reptile_jab(Fighter *fighter);
static void reptile_high_kick(Fighter *fighter);
static void reptile_low_kick(Fighter *fighter);
static void reptile_sweep(Fighter *fighter);
static void reptile_uppercut(Fighter *fighter);
static void reptile_roundhouse(Fighter *fighter);
static void reptile_input_47a690(Fighter *fighter);
static void reptile_input_47afe0(Fighter *fighter);
static void reptile_input_4988d0(Fighter *fighter);
static void reptile_air_hp(Fighter *fighter);
static void reptile_air_lp(Fighter *fighter);
static void reptile_air_kick(Fighter *fighter);
static void reptile_air_diagonal_hk(Fighter *fighter);
static void reptile_air_diagonal_lk(Fighter *fighter);
static void reptile_air_throw(Fighter *fighter);
static void reptile_fan_fatality(Fighter *fighter);
static void reptile_spike_fatality(Fighter *fighter);
static void reptile_entry_fan(Fighter *fighter);
static void reptile_entry_spike(Fighter *fighter);
static void reptile_entry_block(Fighter *fighter);
static void reptile_entry_high_punch(Fighter *fighter);
static void reptile_entry_low_punch(Fighter *fighter);
static void reptile_press_punch(Fighter *fighter);
static void reptile_press_low_punch(Fighter *fighter);
static void reptile_press_kick(Fighter *fighter);
static void reptile_press_low_kick(Fighter *fighter);
static void reptile_press_uppercut(Fighter *fighter);
static void reptile_press_sweep(Fighter *fighter);
static void reptile_press_air_hp(Fighter *fighter);
static void reptile_press_air_lp(Fighter *fighter);
static void reptile_press_air_hk(Fighter *fighter);
static void reptile_press_air_lk(Fighter *fighter);
#endif

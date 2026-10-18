#ifndef MK4_CHARACTER_CAGE_PRIVATE_H
#define MK4_CHARACTER_CAGE_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void cage_ready(Fighter *fighter);
static void cage_start_event_recovery(BattleEvent *event, int frames, int kind);
static bool cage_combo_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void cage_script_cpu_kick_tail(Fighter *fighter);
static void cage_script_powered_punch_tail(Fighter *fighter);
static void cage_script_powered_punch_finish_tail(Fighter *fighter);
static bool cage_fatality_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void cage_script_fatality_1_signal(Fighter *fighter);
static bool cage_fatality_2_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void cage_script_fatality_2_continue(Fighter *fighter);
static void cage_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void cage_script_lp_return(Fighter *fighter);
static void cage_script_fan_cycle(Fighter *fighter);
static void cage_script_fan_victim_end(Fighter *fighter);
static void cage_script_fan_victory(Fighter *fighter);
static void cage_script_fan_tail(Fighter *fighter);
static void cage_script_air_release(Fighter *fighter);
static void cage_script_spike_tail(Fighter *fighter);
static void cage_script_kick_finish(Fighter *fighter);
static void cage_script_finish(Fighter *fighter);
static void cage_script_air_finish(Fighter *fighter);
static void cage_script_roundhouse_start(Fighter *fighter);
static void cage_script_fatality_1_continue(Fighter *fighter);
static void cage_script_lk_throw_return(Fighter *fighter);
static void cage_script_opponent_reaction(Fighter *fighter, int reaction);
static void cage_lp_throw(Fighter *fighter);
static void cage_lk_throw(Fighter *fighter);
static void cage_jab(Fighter *fighter);
static void cage_uppercut(Fighter *fighter);
static void cage_sweep(Fighter *fighter);
static void cage_air_hp(Fighter *fighter);
static void cage_air_lp(Fighter *fighter);
static void cage_air_kick(Fighter *fighter);
static void cage_air_diagonal_hk(Fighter *fighter);
static void cage_air_diagonal_lk(Fighter *fighter);
static void cage_air_throw(Fighter *fighter);
static void cage_fan_fatality(Fighter *fighter);
static void cage_spike_fatality(Fighter *fighter);
static void cage_fatality_1(Fighter *fighter);
static void cage_fatality_2(Fighter *fighter);
static void cage_shadow_kick(Fighter *fighter);
static void cage_powered_punch(Fighter *fighter);
static void cage_cpu_kick(Fighter *fighter);
static void cage_cpu_projectile(Fighter *fighter);
static void cage_high_punch(Fighter *fighter);
static void cage_high_kick(Fighter *fighter);
static void cage_low_kick(Fighter *fighter);
static void cage_roundhouse(Fighter *fighter);
static void cage_press_high_punch(Fighter *fighter);
static void cage_press_low_punch(Fighter *fighter);
static void cage_press_high_kick(Fighter *fighter);
static void cage_press_low_kick(Fighter *fighter);
static void cage_press_crouching_hp(Fighter *fighter);
static void cage_press_crouching_kick(Fighter *fighter);
static void cage_press_air_hp(Fighter *fighter);
static void cage_press_air_lp(Fighter *fighter);
static void cage_press_air_hk(Fighter *fighter);
static void cage_press_air_lk(Fighter *fighter);
#endif

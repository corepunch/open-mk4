#ifndef MK4_CHARACTER_REIKO_PRIVATE_H
#define MK4_CHARACTER_REIKO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void reiko_ready(Fighter *fighter);
static void reiko_script_lk_return(Fighter *fighter);
static void reiko_script_lp_select(Fighter *fighter);
static void reiko_script_lp_end(Fighter *fighter);
static void reiko_script_lp_return(Fighter *fighter);
static void reiko_script_fan_cycle(Fighter *fighter);
static void reiko_script_fan_victim_end(Fighter *fighter);
static void reiko_script_fan_victory(Fighter *fighter);
static void reiko_script_fan_tail(Fighter *fighter);
static void reiko_script_air_release(Fighter *fighter);
static void reiko_script_spike_tail(Fighter *fighter);
static void reiko_script_finish(Fighter *fighter);
static void reiko_script_kick_finish(Fighter *fighter);
static void reiko_script_air_finish(Fighter *fighter);
static void reiko_script_roundhouse_start(Fighter *fighter);
static void reiko_script_opponent_reaction(Fighter *fighter, int reaction);
static void reiko_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void reiko_high_punch(Fighter *fighter);
static void reiko_lp_throw(Fighter *fighter);
static void reiko_lk_throw(Fighter *fighter);
static void reiko_jab(Fighter *fighter);
static void reiko_uppercut(Fighter *fighter);
static void reiko_sweep(Fighter *fighter);
static void reiko_air_hp(Fighter *fighter);
static void reiko_air_lp(Fighter *fighter);
static void reiko_air_kick(Fighter *fighter);
static void reiko_air_diagonal_hk(Fighter *fighter);
static void reiko_air_diagonal_lk(Fighter *fighter);
static void reiko_air_throw(Fighter *fighter);
static void reiko_fan_fatality(Fighter *fighter);
static void reiko_spike_fatality(Fighter *fighter);
static void reiko_high_kick(Fighter *fighter);
static void reiko_low_kick(Fighter *fighter);
static void reiko_roundhouse(Fighter *fighter);
static void reiko_press_high_punch(Fighter *fighter);
static void reiko_press_low_punch(Fighter *fighter);
static void reiko_press_high_kick(Fighter *fighter);
static void reiko_press_low_kick(Fighter *fighter);
static void reiko_press_crouching_hp(Fighter *fighter);
static void reiko_press_crouching_kick(Fighter *fighter);
static void reiko_press_air_hp(Fighter *fighter);
static void reiko_press_air_lp(Fighter *fighter);
static void reiko_press_air_hk(Fighter *fighter);
static void reiko_press_air_lk(Fighter *fighter);
#endif

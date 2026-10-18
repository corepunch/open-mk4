#ifndef MK4_CHARACTER_FUJIN_PRIVATE_H
#define MK4_CHARACTER_FUJIN_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void fujin_ready(Fighter *fighter);
static void fujin_script_lk_return(Fighter *fighter);
static void fujin_script_lp_select(Fighter *fighter);
static void fujin_script_current_reaction(Fighter *fighter, int reaction);
static void fujin_script_opponent_reaction(Fighter *fighter, int reaction);
static void fujin_script_lp_extended_finish(Fighter *fighter);
static void fujin_script_lp_short_finish(Fighter *fighter);
static void fujin_script_fan_cycle(Fighter *fighter);
static void fujin_script_fan_victim_end(Fighter *fighter);
static void fujin_script_fan_victory(Fighter *fighter);
static void fujin_script_fan_tail(Fighter *fighter);
static void fujin_script_kick_finish(Fighter *fighter);
static void fujin_script_air_release(Fighter *fighter);
static void fujin_script_air_finish(Fighter *fighter);
static void fujin_script_finish(Fighter *fighter);
static void fujin_script_roundhouse_start(Fighter *fighter);
static void fujin_script_spike_tail(Fighter *fighter);
static void fujin_low_kick(Fighter *fighter);
static void fujin_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void fujin_lk_throw(Fighter *fighter);
static void fujin_lp_throw(Fighter *fighter);
static void fujin_high_kick(Fighter *fighter);
static void fujin_sweep(Fighter *fighter);
static void fujin_uppercut(Fighter *fighter);
static void fujin_roundhouse(Fighter *fighter);
static void fujin_air_hp(Fighter *fighter);
static void fujin_air_lp(Fighter *fighter);
static void fujin_air_kick(Fighter *fighter);
static void fujin_air_diagonal_hk(Fighter *fighter);
static void fujin_air_diagonal_lk(Fighter *fighter);
static void fujin_air_throw(Fighter *fighter);
static void fujin_fan_fatality(Fighter *fighter);
static void fujin_spike_fatality(Fighter *fighter);
static void fujin_entry_roundhouse(Fighter *fighter);
static void fujin_entry_low_kick(Fighter *fighter);
static void fujin_entry_sweep(Fighter *fighter);
static void fujin_entry_high_punch(Fighter *fighter);
static void fujin_entry_fan(Fighter *fighter);
static void fujin_entry_spike(Fighter *fighter);
static void fujin_jab(Fighter *fighter);
static void fujin_state_move(Fighter *fighter, int state);
static void fujin_enter_punch(Fighter *fighter);
static void fujin_enter_air_punch(Fighter *fighter);
static void fujin_enter_kick(Fighter *fighter);
static void fujin_enter_sweep(Fighter *fighter);
static void fujin_enter_uppercut(Fighter *fighter);
static void fujin_enter_throw(Fighter *fighter);
static void fujin_press_punch(Fighter *fighter);
static void fujin_press_low_punch(Fighter *fighter);
static void fujin_press_air_punch(Fighter *fighter);
static void fujin_press_kick(Fighter *fighter);
static void fujin_press_low_kick(Fighter *fighter);
static void fujin_press_uppercut(Fighter *fighter);
static void fujin_press_roundhouse(Fighter *fighter);
static void fujin_press_air_lp(Fighter *fighter);
static void fujin_press_air_hk(Fighter *fighter);
static void fujin_press_air_lk(Fighter *fighter);
static void fujin_press_sweep(Fighter *fighter);
#endif

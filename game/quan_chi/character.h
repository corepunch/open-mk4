#ifndef MK4_CHARACTER_QUAN_CHI_PRIVATE_H
#define MK4_CHARACTER_QUAN_CHI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void quan_chi_ready(Fighter *fighter);
static void quan_chi_script_lk_return(Fighter *fighter);
static void quan_chi_script_fan_cycle(Fighter *fighter);
static void quan_chi_script_fan_victim_end(Fighter *fighter);
static void quan_chi_script_fan_victory(Fighter *fighter);
static void quan_chi_script_fan_tail(Fighter *fighter);
static void quan_chi_script_current_reaction(Fighter *fighter, int reaction);
static void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction);
static void quan_chi_script_kick_finish(Fighter *fighter);
static void quan_chi_script_air_release(Fighter *fighter);
static void quan_chi_script_air_finish(Fighter *fighter);
static void quan_chi_script_finish(Fighter *fighter);
static void quan_chi_script_sweep_finish(Fighter *fighter);
static void quan_chi_script_roundhouse_start(Fighter *fighter);
static void quan_chi_script_spike_tail(Fighter *fighter);
static void quan_chi_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void quan_chi_lp_throw(Fighter *fighter);
static void quan_chi_lk_throw(Fighter *fighter);
static void quan_chi_jab(Fighter *fighter);
static void quan_chi_high_punch(Fighter *fighter);
static void quan_chi_high_kick(Fighter *fighter);
static void quan_chi_low_kick(Fighter *fighter);
static void quan_chi_sweep(Fighter *fighter);
static void quan_chi_uppercut(Fighter *fighter);
static void quan_chi_roundhouse(Fighter *fighter);
static void quan_chi_air_hp(Fighter *fighter);
static void quan_chi_air_lp(Fighter *fighter);
static void quan_chi_air_kick(Fighter *fighter);
static void quan_chi_air_diagonal_hk(Fighter *fighter);
static void quan_chi_air_diagonal_lk(Fighter *fighter);
static void quan_chi_air_throw(Fighter *fighter);
static void quan_chi_fan_fatality(Fighter *fighter);
static void quan_chi_spike_fatality(Fighter *fighter);
static void quan_chi_entry_469c80(Fighter *fighter);
static void quan_chi_entry_46a020(Fighter *fighter);
static void quan_chi_weapon_draw(Fighter *fighter);
static void quan_chi_entry_47a370(Fighter *fighter);
static void quan_chi_entry_47cf30(Fighter *fighter);
static void quan_chi_entry_499740(Fighter *fighter);
static void press_high_punch(Fighter *fighter);
static void press_low_punch(Fighter *fighter);
static void press_high_kick(Fighter *fighter);
static void press_low_kick(Fighter *fighter);
static void press_uppercut(Fighter *fighter);
static void press_sweep(Fighter *fighter);
static void press_air_hp(Fighter *fighter);
static void press_air_lp(Fighter *fighter);
static void press_air_hk(Fighter *fighter);
static void press_air_lk(Fighter *fighter);
#endif

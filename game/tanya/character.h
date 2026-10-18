#ifndef MK4_CHARACTER_TANYA_PRIVATE_H
#define MK4_CHARACTER_TANYA_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void tanya_ready(Fighter *fighter);
static void tanya_script_lk_return(Fighter *fighter);
static void tanya_script_fan_cycle(Fighter *fighter);
static void tanya_script_fan_victim_end(Fighter *fighter);
static void tanya_script_fan_victory(Fighter *fighter);
static void tanya_script_fan_tail(Fighter *fighter);
static void tanya_script_opponent_reaction(Fighter *fighter, int reaction);
static void tanya_script_air_release(Fighter *fighter);
static void tanya_script_spike_tail(Fighter *fighter);
static void tanya_script_finish(Fighter *fighter);
static void tanya_script_kick_finish(Fighter *fighter);
static void tanya_script_air_finish(Fighter *fighter);
static void tanya_script_roundhouse_start(Fighter *fighter);
static void tanya_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void tanya_lp_throw(Fighter *fighter);
void tanya_lk_throw(Fighter *fighter);
static void tanya_jab(Fighter *fighter);
static void tanya_high_kick(Fighter *fighter);
static void tanya_low_kick(Fighter *fighter);
static void tanya_sweep(Fighter *fighter);
static void tanya_uppercut(Fighter *fighter);
void tanya_roundhouse(Fighter *fighter);
static void tanya_air_hp(Fighter *fighter);
static void tanya_air_lp(Fighter *fighter);
static void tanya_air_kick(Fighter *fighter);
void tanya_air_diagonal_hk(Fighter *fighter);
static void tanya_air_diagonal_lk(Fighter *fighter);
void tanya_air_throw(Fighter *fighter);
void tanya_fan_fatality(Fighter *fighter);
void tanya_spike_fatality(Fighter *fighter);
void tanya_high_punch(Fighter *fighter);
static void tanya_press_high_punch(Fighter *fighter);
static void tanya_press_low_punch(Fighter *fighter);
static void tanya_press_high_kick(Fighter *fighter);
static void tanya_press_low_kick(Fighter *fighter);
static void tanya_press_uppercut(Fighter *fighter);
static void tanya_press_sweep(Fighter *fighter);
static void tanya_press_air_hp(Fighter *fighter);
static void tanya_press_air_lp(Fighter *fighter);
static void tanya_press_air_hk(Fighter *fighter);
static void tanya_press_air_lk(Fighter *fighter);
static void tanya_script_current_reaction(Fighter *fighter, int reaction);
#endif

#ifndef MK4_CHARACTER_NOOB_SAIBOT_PRIVATE_H
#define MK4_CHARACTER_NOOB_SAIBOT_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void noob_saibot_ready(Fighter *fighter);
static void noob_saibot_script_lk_return(Fighter *fighter);
static void noob_saibot_script_lp_select(Fighter *fighter);
static void noob_saibot_script_lp_end(Fighter *fighter);
static void noob_saibot_script_lp_return(Fighter *fighter);
static void noob_saibot_script_fan_cycle(Fighter *fighter);
static void noob_saibot_script_fan_victim_end(Fighter *fighter);
static void noob_saibot_script_fan_victory(Fighter *fighter);
static void noob_saibot_script_fan_tail(Fighter *fighter);
static void noob_saibot_script_air_release(Fighter *fighter);
static void noob_saibot_script_air_throw_continue(Fighter *fighter);
static void noob_saibot_script_air_throw_end(Fighter *fighter);
static void noob_saibot_script_spike_tail(Fighter *fighter);
static void noob_saibot_script_finish(Fighter *fighter);
static void noob_saibot_script_kick_finish(Fighter *fighter);
static void noob_saibot_script_air_finish(Fighter *fighter);
static void noob_saibot_script_roundhouse_start(Fighter *fighter);
static void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction);
static void noob_saibot_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void noob_saibot_high_punch(Fighter *fighter);
static void noob_saibot_lp_throw(Fighter *fighter);
static void noob_saibot_lk_throw(Fighter *fighter);
static void noob_saibot_jab(Fighter *fighter);
static void noob_saibot_uppercut(Fighter *fighter);
static void noob_saibot_sweep(Fighter *fighter);
static void noob_saibot_air_hp(Fighter *fighter);
static void noob_saibot_air_lp(Fighter *fighter);
static void noob_saibot_air_kick(Fighter *fighter);
static void noob_saibot_air_diagonal_hk(Fighter *fighter);
static void noob_saibot_air_diagonal_lk(Fighter *fighter);
static void noob_saibot_air_throw(Fighter *fighter);
static void noob_saibot_shadow_low(Fighter *fighter);
static void noob_saibot_shadow_up(Fighter *fighter);
static bool noob_saibot_shadow_up_enabled(const Fighter *fighter);
void noob_saibot_command_498b70(Fighter *fighter);
void noob_saibot_command_498e70(Fighter *fighter);
static void noob_saibot_shadow_task_low(Fighter *fighter, int task_type);
static void noob_saibot_shadow_task_up(Fighter *fighter, int task_type);
static void noob_saibot_shadow_install_physics(Fighter *fighter);
static void noob_saibot_fan_fatality(Fighter *fighter);
static void noob_saibot_spike_fatality(Fighter *fighter);
static void noob_saibot_high_kick(Fighter *fighter);
static void noob_saibot_low_kick(Fighter *fighter);
static void noob_saibot_roundhouse(Fighter *fighter);
static void noob_saibot_press_high_punch(Fighter *fighter);
static void noob_saibot_press_low_punch(Fighter *fighter);
static void noob_saibot_press_high_kick(Fighter *fighter);
static void noob_saibot_press_low_kick(Fighter *fighter);
static void noob_saibot_press_crouching_hp(Fighter *fighter);
static void noob_saibot_press_crouching_kick(Fighter *fighter);
static void noob_saibot_press_air_hp(Fighter *fighter);
static void noob_saibot_press_air_lp(Fighter *fighter);
static void noob_saibot_press_air_hk(Fighter *fighter);
static void noob_saibot_press_air_lk(Fighter *fighter);
#endif

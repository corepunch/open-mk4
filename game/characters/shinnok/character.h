#ifndef MK4_CHARACTER_SHINNOK_PRIVATE_H
#define MK4_CHARACTER_SHINNOK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void shinnok_ready(Fighter *fighter);
static void shinnok_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
static void shinnok_idle(Fighter *fighter);
static void shinnok_walk(Fighter *fighter);
static void shinnok_crouch(Fighter *fighter);
static void shinnok_block(Fighter *fighter);
static void shinnok_jab(Fighter *fighter);
static void shinnok_high_punch(Fighter *fighter);
static void shinnok_high_kick(Fighter *fighter);
static void shinnok_low_kick(Fighter *fighter);
static void shinnok_sweep(Fighter *fighter);
static void shinnok_uppercut(Fighter *fighter);
static void shinnok_roundhouse(Fighter *fighter);
static void shinnok_air_hp(Fighter *fighter);
static void shinnok_air_lp(Fighter *fighter);
static void shinnok_air_kick(Fighter *fighter);
static void shinnok_air_diagonal_hk(Fighter *fighter);
static void shinnok_air_diagonal_lk(Fighter *fighter);
static void shinnok_ground_throw(Fighter *fighter, FighterFn script);
static void shinnok_lp_throw(Fighter *fighter);
static void shinnok_lk_throw(Fighter *fighter);
static void shinnok_air_throw(Fighter *fighter);
static void shinnok_script_air_release(Fighter *fighter);
static void shinnok_fan_start(Fighter *fighter);
static void shinnok_fan_tail(Fighter *fighter);
static void shinnok_fan_victory(Fighter *fighter);
static void shinnok_fan_fatality(Fighter *fighter);
static void shinnok_spike_fatality(Fighter *fighter);
static void shinnok_throw_reaction(Fighter *fighter);
static void shinnok_script_current_reaction(Fighter *fighter, int reaction);
static void shinnok_script_opponent_reaction(Fighter *fighter, int reaction);
static void shinnok_fatality_contact_hold(Fighter *fighter);
static void shinnok_script_kick_finish(Fighter *fighter);
static void shinnok_script_air_finish(Fighter *fighter);
static void shinnok_uppercut_frame(Fighter *fighter);
static void shinnok_script_uppercut_finish(Fighter *fighter);
static void shinnok_script_sweep_finish(Fighter *fighter);
static void shinnok_script_roundhouse_start(Fighter *fighter);
static void shinnok_script_roundhouse_finish(Fighter *fighter);
static void shinnok_press_punch(Fighter *fighter);
static void shinnok_press_low_punch(Fighter *fighter);
static void shinnok_press_kick(Fighter *fighter);
static void shinnok_press_low_kick(Fighter *fighter);
static void shinnok_press_uppercut(Fighter *fighter);
static void shinnok_press_sweep(Fighter *fighter);
static void shinnok_press_air_hp(Fighter *fighter);
static void shinnok_press_air_lp(Fighter *fighter);
static void shinnok_press_air_kick(Fighter *fighter);
static void shinnok_press_air_throw(Fighter *fighter);
static void shinnok_input_target(Fighter *fighter, Button button);
static void shinnok_command_469a00(Fighter *fighter);
static void shinnok_command_469a10(Fighter *fighter);
static void shinnok_command_469a20(Fighter *fighter);
static void shinnok_command_469a30(Fighter *fighter);
static void shinnok_command_469a40(Fighter *fighter);
static void shinnok_command_469a50(Fighter *fighter);
static void shinnok_command_469a60(Fighter *fighter);
static void shinnok_command_469a70(Fighter *fighter);
static void shinnok_command_469a80(Fighter *fighter);
static void shinnok_command_469a90(Fighter *fighter);
static void shinnok_command_469aa0(Fighter *fighter);
static void shinnok_command_469ab0(Fighter *fighter);
static void shinnok_command_469ac0(Fighter *fighter);
static void shinnok_command_469ad0(Fighter *fighter);
static void shinnok_command_467cd0(Fighter *fighter);
static void shinnok_command_467ce0(Fighter *fighter);
static void shinnok_command_46d1d0(Fighter *fighter);
static void shinnok_weapon_hp(Fighter *fighter);
static void shinnok_weapon_lp(Fighter *fighter);
static void shinnok_weapon_back_hp(Fighter *fighter);
static void shinnok_weapon_back_lp(Fighter *fighter);
static void shinnok_weapon_equip(Fighter *fighter);
#endif

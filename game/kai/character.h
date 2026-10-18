#ifndef MK4_CHARACTER_KAI_PRIVATE_H
#define MK4_CHARACTER_KAI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include <string.h>

/* Private callbacks and entries; only character.c includes this header. */
static void kai_ready(Fighter *fighter);
static void kai_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
static void kai_idle(Fighter *fighter);
static void kai_walk(Fighter *fighter);
static void kai_crouch(Fighter *fighter);
static void kai_block(Fighter *fighter);
static void kai_jab(Fighter *fighter);
static void kai_high_punch(Fighter *fighter);
static void kai_low_punch(Fighter *fighter);
static void kai_draw_weapon(Fighter *fighter);
static void kai_input_478df0(Fighter *fighter);
static void kai_input_47bd10(Fighter *fighter);
static void kai_input_47bd40(Fighter *fighter);
static void kai_input_47c520(Fighter *fighter);
static void kai_input_47cbf0(Fighter *fighter);
static void kai_input_47cc10(Fighter *fighter);
static void kai_press_high_punch(Fighter *fighter);
static void kai_press_low_punch(Fighter *fighter);
static void kai_press_high_kick(Fighter *fighter);
static void kai_press_low_kick(Fighter *fighter);
static void kai_press_uppercut(Fighter *fighter);
static void kai_press_sweep(Fighter *fighter);
static void kai_press_air_hp(Fighter *fighter);
static void kai_press_air_lp(Fighter *fighter);
static void kai_press_air_hk(Fighter *fighter);
static void kai_press_air_lk(Fighter *fighter);
static void kai_high_kick(Fighter *fighter);
static void kai_low_kick(Fighter *fighter);
static void kai_sweep(Fighter *fighter);
static void kai_uppercut(Fighter *fighter);
static void kai_roundhouse(Fighter *fighter);
static void kai_air_hp(Fighter *fighter);
static void kai_air_lp(Fighter *fighter);
static void kai_air_kick(Fighter *fighter);
static void kai_air_diagonal_hk(Fighter *fighter);
static void kai_air_diagonal_lk(Fighter *fighter);
static void kai_lp_throw(Fighter *fighter);
static void kai_lk_throw(Fighter *fighter);
static void kai_air_throw(Fighter *fighter);
static void kai_fan_start(Fighter *fighter);
static void kai_fan_fatality(Fighter *fighter);
static void kai_spike_fatality(Fighter *fighter);
static void kai_script_lk_return(Fighter *fighter);
static void kai_script_lk_finish(Fighter *fighter);
static void kai_script_aerial_transition(Fighter *fighter);
static void kai_script_aerial_recovery(Fighter *fighter);
static void kai_contact_frame(Fighter *fighter);
static void kai_script_contact_b9_continuation(Fighter *fighter);
static void kai_script_contact_b10_continuation(Fighter *fighter);
static bool kai_stage_event_step(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void kai_script_stage_event(Fighter *fighter);
static void kai_script_lp_select(Fighter *fighter);
static void kai_script_current_reaction(Fighter *fighter, int reaction);
static void kai_script_lp_extended_finish(Fighter *fighter);
static void kai_script_lp_short_finish(Fighter *fighter);
static void kai_script_opponent_reaction(Fighter *fighter, int reaction);
static void kai_script_draw_equip(Fighter *fighter);
static void kai_script_air_release(Fighter *fighter);
static void kai_script_fan_tail(Fighter *fighter);
static void kai_script_fan_cycle(Fighter *fighter);
static void kai_script_fan_victory(Fighter *fighter);
static void kai_script_fan_victim_end(Fighter *fighter);
static void kai_script_kick_finish(Fighter *fighter);
static void kai_script_air_finish(Fighter *fighter);
static void kai_script_finish(Fighter *fighter);
static void kai_script_roundhouse_start(Fighter *fighter);
static void kai_script_spike_tail(Fighter *fighter);
static void kai_script_thrown_return(Fighter *fighter);
#endif

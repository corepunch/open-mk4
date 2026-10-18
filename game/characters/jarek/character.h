#ifndef MK4_CHARACTER_JAREK_PRIVATE_H
#define MK4_CHARACTER_JAREK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

/* Private callbacks and entries; only character.c includes this header. */
static void jarek_ready(Fighter *fighter);
static void jarek_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
static void jarek_idle(Fighter *fighter);
static void jarek_walk(Fighter *fighter);
static void jarek_crouch(Fighter *fighter);
static void jarek_block(Fighter *fighter);
static void jarek_jab(Fighter *fighter);
static void jarek_high_punch(Fighter *fighter);
static void jarek_high_kick(Fighter *fighter);
static void jarek_low_kick(Fighter *fighter);
static void jarek_sweep(Fighter *fighter);
static void jarek_uppercut(Fighter *fighter);
static void jarek_roundhouse(Fighter *fighter);
static void jarek_air_hp(Fighter *fighter);
static void jarek_air_lp(Fighter *fighter);
static void jarek_air_kick(Fighter *fighter);
static void jarek_air_diagonal_hk(Fighter *fighter);
static void jarek_air_diagonal_lk(Fighter *fighter);
static void jarek_ground_throw(Fighter *fighter, FighterFn script);
static void jarek_script_lk_return(Fighter *fighter);
static void jarek_script_lp_select(Fighter *fighter);
static void jarek_script_lp_end(Fighter *fighter);
static void jarek_script_lp_return(Fighter *fighter);
static void jarek_lp_throw(Fighter *fighter);
static void jarek_lk_throw(Fighter *fighter);
static void jarek_air_throw(Fighter *fighter);
static void jarek_script_air_release(Fighter *fighter);
static void jarek_fan_start(Fighter *fighter);
static void jarek_fan_tail(Fighter *fighter);
static void jarek_fan_victory(Fighter *fighter);
static void jarek_fan_fatality(Fighter *fighter);
static void jarek_spike_fatality(Fighter *fighter);
static void jarek_throw_reaction(Fighter *fighter);
static void jarek_script_current_reaction(Fighter *fighter, int reaction);
static void jarek_script_opponent_reaction(Fighter *fighter, int reaction);
static void jarek_fatality_contact_hold(Fighter *fighter);
static void jarek_start_contact_recovery(BattleEvent *event, Fighter *fighter, int frames);
static bool jarek_contact_event(BattleEvent *event, Fighter *fighter, Fighter *victim);
static void jarek_contact_dispatch(Fighter *fighter, int stage);
static void jarek_script_contact_25(Fighter *fighter);
static void jarek_script_contact_26(Fighter *fighter);
static void jarek_script_contact_24(Fighter *fighter);
static bool jarek_fan_event(BattleEvent *event, Fighter *fighter, Fighter *victim);
static void jarek_script_fan_scene(Fighter *fighter);
static void jarek_script_fan_victim_scene(Fighter *fighter);
static bool jarek_fan_finish_event(BattleEvent *event, Fighter *fighter, Fighter *victim);
static bool jarek_fan_finish_pose(BattleEvent *event, Fighter *fighter, Fighter *victim);
static void jarek_script_fan_finish(Fighter *fighter);
static void jarek_ai_projectile_step(Projectile *projectile);
static void jarek_ai_projectile_spawn(Fighter *fighter, int task_type);
static void jarek_kick_frame(Fighter *fighter);
static void jarek_script_kick_finish(Fighter *fighter);
static void jarek_script_low_kick_avoid(Fighter *fighter);
static void jarek_script_low_kick_clear(Fighter *fighter);
static void jarek_script_low_kick_finish(Fighter *fighter);
static void jarek_script_air_finish(Fighter *fighter);
static void jarek_uppercut_frame(Fighter *fighter);
static void jarek_script_uppercut_finish(Fighter *fighter);
static void jarek_script_sweep_finish(Fighter *fighter);
static void jarek_script_roundhouse_start(Fighter *fighter);
static void jarek_script_roundhouse_finish(Fighter *fighter);
static void jarek_press_punch(Fighter *fighter);
static void jarek_press_low_punch(Fighter *fighter);
static void jarek_press_kick(Fighter *fighter);
static void jarek_press_low_kick(Fighter *fighter);
static void jarek_press_uppercut(Fighter *fighter);
static void jarek_press_sweep(Fighter *fighter);
static void jarek_press_air_hp(Fighter *fighter);
static void jarek_press_air_lp(Fighter *fighter);
static void jarek_press_air_kick(Fighter *fighter);
static void jarek_press_air_throw(Fighter *fighter);
static void jarek_input_target(Fighter *fighter, Button button);
static void jarek_command_469a00(Fighter *fighter);
static void jarek_command_469a10(Fighter *fighter);
static void jarek_command_469a20(Fighter *fighter);
static void jarek_command_469a30(Fighter *fighter);
static void jarek_command_469a40(Fighter *fighter);
static void jarek_command_469a50(Fighter *fighter);
static void jarek_command_469a60(Fighter *fighter);
static void jarek_command_469a70(Fighter *fighter);
static void jarek_command_469a80(Fighter *fighter);
static void jarek_command_469a90(Fighter *fighter);
static void jarek_command_469aa0(Fighter *fighter);
static void jarek_command_469ab0(Fighter *fighter);
static void jarek_command_469ac0(Fighter *fighter);
static void jarek_command_469ad0(Fighter *fighter);
static void jarek_command_467cd0(Fighter *fighter);
static void jarek_command_467ce0(Fighter *fighter);
static void jarek_command_46d1d0(Fighter *fighter);
static void jarek_command_4797f0(Fighter *fighter);
static void jarek_command_47b310(Fighter *fighter);
static void jarek_command_47b5d0(Fighter *fighter);
static void jarek_command_497540(Fighter *fighter);
static void jarek_script_weapon_draw_finish(Fighter *fighter);
static void jarek_script_47b600(Fighter *fighter);
static void jarek_weapon_hp(Fighter *fighter);
static void jarek_weapon_lp(Fighter *fighter);
static void jarek_weapon_back_hp(Fighter *fighter);
static void jarek_weapon_back_lp(Fighter *fighter);
static void jarek_weapon_equip(Fighter *fighter);
static void jarek_weapon_action(Fighter *fighter);
#endif

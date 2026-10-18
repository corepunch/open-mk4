#ifndef MK4_SHINNOK_PRIVATE_H
#define MK4_SHINNOK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block shinnok_s_block
#define s_crouch shinnok_s_crouch
#define s_high_kick shinnok_s_high_kick
#define s_idle shinnok_s_idle
#define s_jab shinnok_s_jab
#define s_sweep shinnok_s_sweep
#define s_throw shinnok_s_throw
#define s_uppercut shinnok_s_uppercut
#define s_walk shinnok_s_walk

void shinnok_entry_retail_script_004de2f8(Fighter *fighter);
void shinnok_entry_retail_script_004de798(Fighter *fighter);
void shinnok_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *shinnok_retail_script_004e0138_repeat_phase(void);
void shinnok_entry_retail_script_004e0148(Fighter *fighter);
void shinnok_entry_retail_script_004e0158(Fighter *fighter);
void shinnok_entry_retail_script_004e0188(Fighter *fighter);
void shinnok_entry_retail_script_004e01a0(Fighter *fighter);
void shinnok_entry_retail_script_004e5c70(Fighter *fighter);
void shinnok_entry_retail_script_004e5ca0(Fighter *fighter);
void shinnok_entry_retail_script_004e5d90(Fighter *fighter);
void shinnok_entry_retail_script_004e5e28(Fighter *fighter);
void shinnok_entry_retail_script_004e5e50(Fighter *fighter);
void shinnok_entry_retail_script_004e61f8(Fighter *fighter);
void shinnok_entry_retail_script_004e62a0(Fighter *fighter);
void shinnok_entry_retail_script_004e62f0(Fighter *fighter);
void shinnok_entry_retail_script_004e63d8(Fighter *fighter);
void shinnok_entry_retail_script_004e6500(Fighter *fighter);
void shinnok_entry_retail_script_004e6658(Fighter *fighter);
void shinnok_entry_retail_script_004e6710(Fighter *fighter);
void shinnok_entry_retail_script_004e6798(Fighter *fighter);
void shinnok_entry_retail_script_004e6800(Fighter *fighter);
void shinnok_entry_retail_script_004e6860(Fighter *fighter);
void shinnok_entry_retail_script_004e6878(Fighter *fighter);
void shinnok_entry_retail_script_004e6950(Fighter *fighter);
void shinnok_entry_retail_script_004e9910(Fighter *fighter);
extern const MoveDef shinnok_s_block;
extern const MoveDef shinnok_s_crouch;
extern const MoveDef shinnok_s_high_kick;
extern const MoveDef shinnok_s_idle;
extern const MoveDef shinnok_s_jab;
extern const MoveDef shinnok_s_sweep;
extern const MoveDef shinnok_s_throw;
extern const MoveDef shinnok_s_uppercut;
extern const MoveDef shinnok_s_walk;
void shinnok_block(Fighter *fighter);
void shinnok_crouch(Fighter *fighter);
void shinnok_fan_fatality(Fighter *fighter);
void shinnok_fan_tail(Fighter *fighter);
void shinnok_fan_victory(Fighter *fighter);
void shinnok_fatality_contact_hold(Fighter *fighter);
void shinnok_high_kick(Fighter *fighter);
void shinnok_idle(Fighter *fighter);
void shinnok_input_target(Fighter *fighter, Button button);
void shinnok_jab(Fighter *fighter);
void shinnok_lp_throw(Fighter *fighter);
void shinnok_press_air_hp(Fighter *fighter);
void shinnok_press_air_kick(Fighter *fighter);
void shinnok_press_air_lp(Fighter *fighter);
void shinnok_press_kick(Fighter *fighter);
void shinnok_press_low_kick(Fighter *fighter);
void shinnok_press_low_punch(Fighter *fighter);
void shinnok_press_punch(Fighter *fighter);
void shinnok_press_sweep(Fighter *fighter);
void shinnok_press_uppercut(Fighter *fighter);
void shinnok_ready(Fighter *fighter);
void shinnok_script_air_finish(Fighter *fighter);
void shinnok_script_air_release(Fighter *fighter);
void shinnok_script_current_reaction(Fighter *fighter, int reaction);
void shinnok_script_kick_finish(Fighter *fighter);
void shinnok_script_opponent_reaction(Fighter *fighter, int reaction);
void shinnok_script_roundhouse_finish(Fighter *fighter);
void shinnok_script_roundhouse_start(Fighter *fighter);
void shinnok_script_sweep_finish(Fighter *fighter);
void shinnok_script_uppercut_finish(Fighter *fighter);
void shinnok_spike_fatality(Fighter *fighter);
void shinnok_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
void shinnok_sweep(Fighter *fighter);
void shinnok_uppercut(Fighter *fighter);
void shinnok_walk(Fighter *fighter);
extern const WeaponDef shinnok_weapon;
void shinnok_weapon_back_hp(Fighter *fighter);
void shinnok_weapon_back_lp(Fighter *fighter);
void shinnok_weapon_equip(Fighter *fighter);
void shinnok_weapon_hp(Fighter *fighter);
void shinnok_weapon_lp(Fighter *fighter);

extern const ButtonTable shinnok_diagonal_air;
void shinnok_press_air_hk(Fighter *fighter);
void shinnok_press_air_lk(Fighter *fighter);
void shinnok_close_hp(Fighter *fighter);
void shinnok_combo_hp(Fighter *fighter);
void shinnok_close_hk(Fighter *fighter);
void shinnok_press_air_ground_hp(Fighter *fighter);
void shinnok_script_close_hp_tail(Fighter *fighter);
void shinnok_script_combo_hp_tail(Fighter *fighter);
void shinnok_script_close_hk_tail(Fighter *fighter);
void shinnok_entry_retail_script_004e61e0(Fighter *fighter);
void shinnok_entry_retail_script_004e69b0(Fighter *fighter);
void shinnok_entry_retail_script_004e6190(Fighter *fighter);
void shinnok_lk_throw(Fighter *fighter);
void shinnok_fan_fatality(Fighter *fighter);
void shinnok_spike_fatality(Fighter *fighter);
#endif

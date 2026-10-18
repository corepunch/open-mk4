#ifndef MK4_FUJIN_PRIVATE_H
#define MK4_FUJIN_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_jab fujin_s_jab
#define s_kick fujin_s_kick
#define s_knockdown fujin_s_knockdown
#define s_sweep fujin_s_sweep
#define s_throw fujin_s_throw
#define s_uppercut fujin_s_uppercut

void fujin_enter_sweep(Fighter *fighter);
void fujin_enter_throw(Fighter *fighter);
void fujin_press_air_hk(Fighter *fighter);
void fujin_press_air_lk(Fighter *fighter);
void fujin_press_air_lp(Fighter *fighter);
void fujin_press_air_punch(Fighter *fighter);
void fujin_press_kick(Fighter *fighter);
void fujin_press_low_kick(Fighter *fighter);
void fujin_press_low_punch(Fighter *fighter);
void fujin_press_punch(Fighter *fighter);
void fujin_press_sweep(Fighter *fighter);
void fujin_press_uppercut(Fighter *fighter);
void fujin_script_air_finish(Fighter *fighter);
void fujin_script_current_reaction(Fighter *fighter, int reaction);
void fujin_script_fan_tail(Fighter *fighter);
void fujin_script_finish(Fighter *fighter);
void fujin_script_lk_return(Fighter *fighter);
void fujin_script_lp_extended_finish(Fighter *fighter);
void fujin_script_lp_select(Fighter *fighter);
void fujin_script_lp_short_finish(Fighter *fighter);
void fujin_script_opponent_reaction(Fighter *fighter, int reaction);
void fujin_script_roundhouse_start(Fighter *fighter);
void fujin_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
extern const MoveDef *const fujin_moves[13];
void fujin_entry_retail_script_004de2f8(Fighter *fighter);
void fujin_entry_retail_script_004de610(Fighter *fighter);
void fujin_entry_retail_script_004de6b8(Fighter *fighter);
void fujin_entry_retail_script_004de798(Fighter *fighter);
void fujin_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *fujin_retail_script_004e0138_repeat_phase(void);
void fujin_entry_retail_script_004e0148(Fighter *fighter);
void fujin_entry_retail_script_004e0158(Fighter *fighter);
void fujin_entry_retail_script_004e0188(Fighter *fighter);
void fujin_entry_retail_script_004e01a0(Fighter *fighter);
void fujin_entry_retail_script_004e5d20(Fighter *fighter);
void fujin_entry_retail_script_004e5ca0(Fighter *fighter);
void fujin_entry_retail_script_004e5d90(Fighter *fighter);
void fujin_entry_retail_script_004e61f8(Fighter *fighter);
void fujin_entry_retail_script_004e62a0(Fighter *fighter);
void fujin_entry_retail_script_004e62f0(Fighter *fighter);
void fujin_entry_retail_script_004e63d8(Fighter *fighter);
void fujin_entry_retail_script_004e6500(Fighter *fighter);
void fujin_entry_retail_script_004e6658(Fighter *fighter);
void fujin_entry_retail_script_004e6710(Fighter *fighter);
void fujin_entry_retail_script_004e6798(Fighter *fighter);
void fujin_entry_retail_script_004e6800(Fighter *fighter);
void fujin_entry_retail_script_004e6890(Fighter *fighter);
void fujin_entry_retail_script_004e68a8(Fighter *fighter);
void fujin_entry_retail_script_004e6950(Fighter *fighter);
extern const MoveDef fujin_s_jab;
extern const MoveDef fujin_s_kick;
extern const MoveDef fujin_s_knockdown;
extern const MoveDef fujin_s_sweep;
extern const MoveDef fujin_s_throw;
extern const MoveDef fujin_s_uppercut;

extern const ButtonTable fujin_diagonal_air;
void fujin_press_air_hk(Fighter *fighter);
void fujin_press_air_lk(Fighter *fighter);
void fujin_close_hp(Fighter *fighter);
void fujin_combo_hp(Fighter *fighter);
void fujin_close_hk(Fighter *fighter);
void fujin_press_air_ground_hp(Fighter *fighter);
void fujin_script_close_hp_tail(Fighter *fighter);
void fujin_script_combo_hp_tail(Fighter *fighter);
void fujin_script_close_hk_tail(Fighter *fighter);
void fujin_entry_retail_script_004e61e0(Fighter *fighter);
void fujin_entry_retail_script_004e69b0(Fighter *fighter);
void fujin_entry_retail_script_004e6190(Fighter *fighter);
void fujin_lk_throw(Fighter *fighter);
void fujin_entry_fan(Fighter *fighter);
void fujin_entry_spike(Fighter *fighter);
void fujin_roundhouse(Fighter *fighter);
void fujin_script_roundhouse_finish(Fighter *fighter);
void fujin_entry_roundhouse(Fighter *fighter);
void fujin_press_roundhouse(Fighter *fighter);
bool fujin_can_dive(const Fighter *fighter);
void fujin_dive(Fighter *fighter);
void fujin_dive_contact(Fighter *fighter);
void fujin_dive_land(Fighter *fighter);
void fujin_entry_retail_script_004e8258(Fighter *fighter);
void fujin_entry_retail_script_004e8298(Fighter *fighter);
void fujin_script_fan_cycle(Fighter *fighter);
void fujin_script_fan_victim_end(Fighter *fighter);
void fujin_script_fan_victory(Fighter *fighter);
void fujin_script_spike_tail(Fighter *fighter);
void fujin_entry_retail_script_004e0138(Fighter *fighter);
#endif

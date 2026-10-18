#ifndef MK4_QUAN_CHI_PRIVATE_H
#define MK4_QUAN_CHI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define press_air_hk quan_chi_press_air_hk
#define press_air_hp quan_chi_press_air_hp
#define press_air_lk quan_chi_press_air_lk
#define press_air_lp quan_chi_press_air_lp
#define press_high_kick quan_chi_press_high_kick
#define press_high_punch quan_chi_press_high_punch
#define press_low_kick quan_chi_press_low_kick
#define press_low_punch quan_chi_press_low_punch
#define press_sweep quan_chi_press_sweep
#define press_uppercut quan_chi_press_uppercut
#define s_high_kick quan_chi_s_high_kick
#define s_jab quan_chi_s_jab
#define s_sweep quan_chi_s_sweep
#define s_throw quan_chi_s_throw
#define s_uppercut quan_chi_s_uppercut

void quan_chi_press_air_hk(Fighter *fighter);
void quan_chi_press_air_hp(Fighter *fighter);
void quan_chi_press_air_lk(Fighter *fighter);
void quan_chi_press_air_lp(Fighter *fighter);
void quan_chi_press_high_kick(Fighter *fighter);
void quan_chi_press_high_punch(Fighter *fighter);
void quan_chi_press_low_kick(Fighter *fighter);
void quan_chi_press_low_punch(Fighter *fighter);
void quan_chi_press_sweep(Fighter *fighter);
void quan_chi_press_uppercut(Fighter *fighter);
void quan_chi_high_kick(Fighter *fighter);
void quan_chi_jab(Fighter *fighter);
void quan_chi_lp_throw(Fighter *fighter);
void quan_chi_ready(Fighter *fighter);
void quan_chi_script_air_finish(Fighter *fighter);
void quan_chi_script_air_release(Fighter *fighter);
void quan_chi_script_current_reaction(Fighter *fighter, int reaction);
void quan_chi_script_fan_cycle(Fighter *fighter);
void quan_chi_script_fan_tail(Fighter *fighter);
void quan_chi_script_fan_victim_end(Fighter *fighter);
void quan_chi_script_fan_victory(Fighter *fighter);
void quan_chi_script_finish(Fighter *fighter);
void quan_chi_script_kick_finish(Fighter *fighter);
void quan_chi_script_lk_return(Fighter *fighter);
void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction);
void quan_chi_script_roundhouse_start(Fighter *fighter);
void quan_chi_script_spike_tail(Fighter *fighter);
void quan_chi_script_sweep_finish(Fighter *fighter);
void quan_chi_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void quan_chi_sweep(Fighter *fighter);
void quan_chi_uppercut(Fighter *fighter);
void quan_chi_entry_retail_script_004de2f8(Fighter *fighter);
void quan_chi_entry_retail_script_004de798(Fighter *fighter);
void quan_chi_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *quan_chi_retail_script_004e0138_repeat_phase(void);
void quan_chi_entry_retail_script_004e0148(Fighter *fighter);
void quan_chi_entry_retail_script_004e0158(Fighter *fighter);
void quan_chi_entry_retail_script_004e0188(Fighter *fighter);
void quan_chi_entry_retail_script_004e01a0(Fighter *fighter);
void quan_chi_entry_retail_script_004e5c70(Fighter *fighter);
void quan_chi_entry_retail_script_004e5ca0(Fighter *fighter);
void quan_chi_entry_retail_script_004e5d90(Fighter *fighter);
void quan_chi_entry_retail_script_004e5e28(Fighter *fighter);
void quan_chi_entry_retail_script_004e5e50(Fighter *fighter);
void quan_chi_entry_retail_script_004e61f8(Fighter *fighter);
void quan_chi_entry_retail_script_004e62a0(Fighter *fighter);
void quan_chi_entry_retail_script_004e62f0(Fighter *fighter);
void quan_chi_entry_retail_script_004e63d8(Fighter *fighter);
void quan_chi_entry_retail_script_004e6500(Fighter *fighter);
void quan_chi_entry_retail_script_004e6658(Fighter *fighter);
void quan_chi_entry_retail_script_004e6710(Fighter *fighter);
void quan_chi_entry_retail_script_004e6798(Fighter *fighter);
void quan_chi_entry_retail_script_004e6800(Fighter *fighter);
void quan_chi_entry_retail_script_004e6860(Fighter *fighter);
void quan_chi_entry_retail_script_004e6878(Fighter *fighter);
void quan_chi_entry_retail_script_004e6950(Fighter *fighter);
void quan_chi_entry_retail_script_004e9910(Fighter *fighter);
extern const MoveDef quan_chi_s_high_kick;
extern const MoveDef quan_chi_s_jab;
extern const MoveDef quan_chi_s_sweep;
extern const MoveDef quan_chi_s_throw;
extern const MoveDef quan_chi_s_uppercut;

extern const ButtonTable quan_chi_diagonal_air;
void quan_chi_press_air_hk(Fighter *fighter);
void quan_chi_press_air_lk(Fighter *fighter);
#endif

#ifndef MK4_TANYA_PRIVATE_H
#define MK4_TANYA_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_jab tanya_s_jab
#define s_kick tanya_s_kick
#define s_sweep tanya_s_sweep
#define s_throw tanya_s_throw
#define s_uppercut tanya_s_uppercut

void tanya_entry_retail_script_004de2f8(Fighter *fighter);
void tanya_entry_retail_script_004de798(Fighter *fighter);
void tanya_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *tanya_retail_script_004e0138_repeat_phase(void);
void tanya_entry_retail_script_004e0148(Fighter *fighter);
void tanya_entry_retail_script_004e0158(Fighter *fighter);
void tanya_entry_retail_script_004e0188(Fighter *fighter);
void tanya_entry_retail_script_004e01a0(Fighter *fighter);
void tanya_entry_retail_script_004e5c70(Fighter *fighter);
void tanya_entry_retail_script_004e5ca0(Fighter *fighter);
void tanya_entry_retail_script_004e5d90(Fighter *fighter);
void tanya_entry_retail_script_004e5e28(Fighter *fighter);
void tanya_entry_retail_script_004e5e50(Fighter *fighter);
void tanya_entry_retail_script_004e61f8(Fighter *fighter);
void tanya_entry_retail_script_004e62a0(Fighter *fighter);
void tanya_entry_retail_script_004e62f0(Fighter *fighter);
void tanya_entry_retail_script_004e63d8(Fighter *fighter);
void tanya_entry_retail_script_004e6500(Fighter *fighter);
void tanya_entry_retail_script_004e6658(Fighter *fighter);
void tanya_entry_retail_script_004e6710(Fighter *fighter);
void tanya_entry_retail_script_004e6798(Fighter *fighter);
void tanya_entry_retail_script_004e6800(Fighter *fighter);
void tanya_entry_retail_script_004e6860(Fighter *fighter);
void tanya_entry_retail_script_004e6878(Fighter *fighter);
void tanya_entry_retail_script_004e6950(Fighter *fighter);
void tanya_entry_retail_script_004e9910(Fighter *fighter);
extern const MoveDef tanya_s_jab;
extern const MoveDef tanya_s_kick;
extern const MoveDef tanya_s_sweep;
extern const MoveDef tanya_s_throw;
extern const MoveDef tanya_s_uppercut;
void tanya_high_kick(Fighter *fighter);
void tanya_jab(Fighter *fighter);
void tanya_lp_throw(Fighter *fighter);
void tanya_press_air_hk(Fighter *fighter);
void tanya_press_air_hp(Fighter *fighter);
void tanya_press_air_lk(Fighter *fighter);
void tanya_press_air_lp(Fighter *fighter);
void tanya_press_high_kick(Fighter *fighter);
void tanya_press_high_punch(Fighter *fighter);
void tanya_press_low_kick(Fighter *fighter);
void tanya_press_low_punch(Fighter *fighter);
void tanya_press_sweep(Fighter *fighter);
void tanya_press_uppercut(Fighter *fighter);
void tanya_script_air_finish(Fighter *fighter);
void tanya_script_air_release(Fighter *fighter);
void tanya_script_current_reaction(Fighter *fighter, int reaction);
void tanya_script_fan_cycle(Fighter *fighter);
void tanya_script_fan_tail(Fighter *fighter);
void tanya_script_fan_victim_end(Fighter *fighter);
void tanya_script_fan_victory(Fighter *fighter);
void tanya_script_finish(Fighter *fighter);
void tanya_script_kick_finish(Fighter *fighter);
void tanya_script_lk_return(Fighter *fighter);
void tanya_script_opponent_reaction(Fighter *fighter, int reaction);
void tanya_script_roundhouse_start(Fighter *fighter);
void tanya_script_spike_tail(Fighter *fighter);
void tanya_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void tanya_sweep(Fighter *fighter);
void tanya_uppercut(Fighter *fighter);

extern const ButtonTable tanya_diagonal_air;
void tanya_press_air_hk(Fighter *fighter);
void tanya_press_air_lk(Fighter *fighter);
void tanya_close_hp(Fighter *fighter);
void tanya_combo_hp(Fighter *fighter);
void tanya_close_hk(Fighter *fighter);
void tanya_press_air_ground_hp(Fighter *fighter);
void tanya_script_close_hp_tail(Fighter *fighter);
void tanya_script_combo_hp_tail(Fighter *fighter);
void tanya_script_close_hk_tail(Fighter *fighter);
void tanya_entry_retail_script_004e61e0(Fighter *fighter);
void tanya_entry_retail_script_004e69b0(Fighter *fighter);
void tanya_entry_retail_script_004e6190(Fighter *fighter);
void tanya_lk_throw(Fighter *fighter);
#endif

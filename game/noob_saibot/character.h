#ifndef MK4_NOOB_SAIBOT_PRIVATE_H
#define MK4_NOOB_SAIBOT_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_jab noob_saibot_s_jab
#define s_kick noob_saibot_s_kick
#define s_sweep noob_saibot_s_sweep
#define s_throw noob_saibot_s_throw
#define s_uppercut noob_saibot_s_uppercut

void noob_saibot_command_498b70(Fighter *fighter);
void noob_saibot_command_498e70(Fighter *fighter);
void noob_saibot_lp_throw(Fighter *fighter);
void noob_saibot_press_air_hk(Fighter *fighter);
void noob_saibot_press_air_hp(Fighter *fighter);
void noob_saibot_press_air_lk(Fighter *fighter);
void noob_saibot_press_air_lp(Fighter *fighter);
void noob_saibot_press_crouching_hp(Fighter *fighter);
void noob_saibot_press_crouching_kick(Fighter *fighter);
void noob_saibot_press_high_kick(Fighter *fighter);
void noob_saibot_press_high_punch(Fighter *fighter);
void noob_saibot_press_low_kick(Fighter *fighter);
void noob_saibot_press_low_punch(Fighter *fighter);
void noob_saibot_ready(Fighter *fighter);
void noob_saibot_script_air_finish(Fighter *fighter);
void noob_saibot_script_air_throw_continue(Fighter *fighter);
void noob_saibot_script_air_throw_end(Fighter *fighter);
void noob_saibot_script_fan_cycle(Fighter *fighter);
void noob_saibot_script_fan_tail(Fighter *fighter);
void noob_saibot_script_fan_victim_end(Fighter *fighter);
void noob_saibot_script_fan_victory(Fighter *fighter);
void noob_saibot_script_finish(Fighter *fighter);
void noob_saibot_script_kick_finish(Fighter *fighter);
void noob_saibot_script_lk_return(Fighter *fighter);
void noob_saibot_script_lp_end(Fighter *fighter);
void noob_saibot_script_lp_return(Fighter *fighter);
void noob_saibot_script_lp_select(Fighter *fighter);
void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction);
void noob_saibot_script_roundhouse_start(Fighter *fighter);
void noob_saibot_script_spike_tail(Fighter *fighter);
void noob_saibot_shadow_install_physics(Fighter *fighter);
void noob_saibot_shadow_low(Fighter *fighter);
void noob_saibot_shadow_task_low(Fighter *fighter, int task_type);
void noob_saibot_shadow_task_up(Fighter *fighter, int task_type);
void noob_saibot_shadow_up(Fighter *fighter);
bool noob_saibot_shadow_up_enabled(const Fighter *fighter);
void noob_saibot_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void noob_saibot_sweep(Fighter *fighter);
void noob_saibot_uppercut(Fighter *fighter);
void noob_saibot_entry_retail_script_004de2f8(Fighter *fighter);
void noob_saibot_entry_retail_script_004de640(Fighter *fighter);
void noob_saibot_entry_retail_script_004de664(Fighter *fighter);
void noob_saibot_entry_retail_script_004de668(Fighter *fighter);
void noob_saibot_entry_retail_script_004de6b8(Fighter *fighter);
void noob_saibot_entry_retail_script_004de798(Fighter *fighter);
void noob_saibot_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *noob_saibot_retail_script_004e0138_repeat_phase(void);
void noob_saibot_entry_retail_script_004e0148(Fighter *fighter);
void noob_saibot_entry_retail_script_004e0158(Fighter *fighter);
void noob_saibot_entry_retail_script_004e0188(Fighter *fighter);
void noob_saibot_entry_retail_script_004e01a0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5c70(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5ca0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5d90(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5e50(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5e78(Fighter *fighter);
void noob_saibot_entry_retail_script_004e5ea0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e61f8(Fighter *fighter);
void noob_saibot_entry_retail_script_004e62a0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e62f0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e63d8(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6500(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6658(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6710(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6798(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6800(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6860(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6878(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6950(Fighter *fighter);
void noob_saibot_entry_retail_script_004e9910(Fighter *fighter);
void noob_saibot_entry_retail_script_004e9b40(Fighter *fighter);
void noob_saibot_entry_retail_script_004e9bd0(Fighter *fighter);
void noob_saibot_entry_retail_script_004ed268(Fighter *fighter);
void noob_saibot_entry_retail_script_004ed2c0(Fighter *fighter);
extern const MoveDef noob_saibot_s_jab;
extern const MoveDef noob_saibot_s_kick;
extern const MoveDef noob_saibot_s_sweep;
extern const MoveDef noob_saibot_s_throw;
extern const MoveDef noob_saibot_s_uppercut;

extern const ButtonTable noob_saibot_diagonal_air;
void noob_saibot_press_air_hk(Fighter *fighter);
void noob_saibot_press_air_lk(Fighter *fighter);
void noob_saibot_close_hp(Fighter *fighter);
void noob_saibot_combo_hp(Fighter *fighter);
void noob_saibot_close_hk(Fighter *fighter);
void noob_saibot_press_air_ground_hp(Fighter *fighter);
void noob_saibot_script_close_hp_tail(Fighter *fighter);
void noob_saibot_script_combo_hp_tail(Fighter *fighter);
void noob_saibot_script_close_hk_tail(Fighter *fighter);
void noob_saibot_entry_retail_script_004e61e0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e69b0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e6190(Fighter *fighter);
#endif

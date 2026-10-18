#ifndef MK4_GORO_PRIVATE_H
#define MK4_GORO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_jab goro_s_jab
#define s_kick goro_s_kick
#define s_knockdown goro_s_knockdown
#define s_sweep goro_s_sweep
#define s_throw goro_s_throw
#define s_uppercut goro_s_uppercut

void goro_air_throw(Fighter *fighter);
void goro_cpu_projectile(Fighter *fighter);
void goro_cpu_projectile_spawn(Fighter *fighter, int task_type);
void goro_enter_air_hk(Fighter *fighter);
void goro_enter_air_hp(Fighter *fighter);
void goro_enter_air_lk(Fighter *fighter);
void goro_enter_air_lp(Fighter *fighter);
void goro_enter_high_kick(Fighter *fighter);
void goro_enter_jab(Fighter *fighter);
void goro_enter_low_kick(Fighter *fighter);
void goro_enter_low_punch(Fighter *fighter);
void goro_enter_sweep(Fighter *fighter);
void goro_enter_uppercut(Fighter *fighter);
void goro_lp_throw(Fighter *fighter);
void goro_lk_throw(Fighter *fighter);
void goro_script_air_finish(Fighter *fighter);
void goro_script_air_release(Fighter *fighter);
void goro_script_current_reaction(Fighter *fighter, int reaction);
void goro_script_event_finish(Fighter *fighter);
void goro_script_fan_cycle(Fighter *fighter);
void goro_script_fan_tail(Fighter *fighter);
void goro_script_fan_victim_end(Fighter *fighter);
void goro_script_fan_victory(Fighter *fighter);
void goro_script_finish(Fighter *fighter);
void goro_script_kick_finish(Fighter *fighter);
void goro_script_lk_finish(Fighter *fighter);
void goro_script_lk_return(Fighter *fighter);
void goro_script_lp_pair_start(Fighter *fighter);
void goro_script_lp_phase_repeat(Fighter *fighter);
void goro_script_opponent_reaction(Fighter *fighter, int reaction);
void goro_script_roundhouse_start(Fighter *fighter);
void goro_script_spike_tail(Fighter *fighter);
void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action);
void goro_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void goro_entry_retail_script_004de2f8(Fighter *fighter);
void goro_entry_retail_script_004de358(Fighter *fighter);
void goro_entry_retail_script_004de6e4(Fighter *fighter);
void goro_entry_retail_script_004de798(Fighter *fighter);
void goro_entry_retail_script_004de7e8(Fighter *fighter);
void goro_entry_retail_script_004dfb60(Fighter *fighter);
void goro_entry_retail_script_004dfb70(Fighter *fighter);
void goro_entry_retail_script_004dfbb8(Fighter *fighter);
void goro_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *goro_retail_script_004e0138_repeat_phase(void);
void goro_entry_retail_script_004e0148(Fighter *fighter);
void goro_entry_retail_script_004e0158(Fighter *fighter);
void goro_entry_retail_script_004e0188(Fighter *fighter);
void goro_entry_retail_script_004e01a0(Fighter *fighter);
void goro_entry_retail_script_004e5c70(Fighter *fighter);
void goro_entry_retail_script_004e5ca0(Fighter *fighter);
void goro_entry_retail_script_004e5d90(Fighter *fighter);
void goro_entry_retail_script_004e5e28(Fighter *fighter);
void goro_entry_retail_script_004e5e50(Fighter *fighter);
void goro_entry_retail_script_004e61f8(Fighter *fighter);
void goro_entry_retail_script_004e62a0(Fighter *fighter);
void goro_entry_retail_script_004e62f0(Fighter *fighter);
void goro_entry_retail_script_004e63d8(Fighter *fighter);
void goro_entry_retail_script_004e6500(Fighter *fighter);
void goro_entry_retail_script_004e6658(Fighter *fighter);
void goro_entry_retail_script_004e6710(Fighter *fighter);
void goro_entry_retail_script_004e6798(Fighter *fighter);
void goro_entry_retail_script_004e6800(Fighter *fighter);
void goro_entry_retail_script_004e6860(Fighter *fighter);
void goro_entry_retail_script_004e6878(Fighter *fighter);
void goro_entry_retail_script_004e6950(Fighter *fighter);
void goro_entry_retail_script_004e9910(Fighter *fighter);
void goro_entry_retail_script_004ed790(Fighter *fighter);
extern const MoveDef goro_s_jab;
extern const MoveDef goro_s_kick;
extern const MoveDef goro_s_knockdown;
extern const MoveDef goro_s_sweep;
extern const MoveDef goro_s_throw;
extern const MoveDef goro_s_uppercut;

void goro_event_progress(Fighter *fighter);
void goro_event_counter_kick(Fighter *fighter);
void goro_script_punch_tail(Fighter *fighter);
void goro_script_counter_kick_tail(Fighter *fighter);
#endif

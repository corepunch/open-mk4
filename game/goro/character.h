#include "retail_native.h"
#ifndef MK4_GORO_PRIVATE_H
#define MK4_GORO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_knockdown goro_s_knockdown

extern const MoveDef goro_s_knockdown;

void goro_air_throw(Fighter *fighter);
void goro_press_air_hk(Fighter *fighter);
void goro_press_air_hp(Fighter *fighter);
void goro_press_air_lk(Fighter *fighter);
void goro_press_air_lp(Fighter *fighter);

void goro_script_air_finish(Fighter *fighter);
void goro_script_air_getup(Fighter *fighter);
void goro_script_air_land(Fighter *fighter);
void goro_script_air_release(Fighter *fighter);
void goro_script_current_reaction(Fighter *fighter, int reaction);
void goro_script_lp_finish(Fighter *fighter);
void goro_script_lp_phase_repeat(Fighter *fighter);
void goro_script_opponent_reaction(Fighter *fighter, int reaction);

extern const uint32_t goro_retail_weapon_descriptor[];
void goro_begin_special(Fighter *fighter, StateId state, const MoveDef *move, int token);
void goro_fireball(Fighter *fighter);
void goro_fireball_spawn(Fighter *fighter, int task_type);
void goro_kick_victim(Fighter *victim);
void goro_leap_stomp(Fighter *fighter);
void goro_press_air_block(Fighter *fighter);
void goro_punch_victim(Fighter *victim);
void goro_quake_resting_weapons(void);
void goro_quake_stomp(Fighter *fighter);
void goro_quake_victim(Fighter *victim);
void goro_script_kick_tail(Fighter *fighter);
void goro_script_leap_continue(Fighter *fighter);
void goro_script_punch_tail(Fighter *fighter);
void goro_script_quake_land(Fighter *fighter);
void goro_script_stomp_return(Fighter *fighter);
void goro_script_stomp_victim_hold(Fighter *victim);
void goro_script_uppercut_victim_slide(Fighter *victim);
void goro_special_kick(Fighter *fighter);
void goro_special_punch(Fighter *fighter);
void goro_special_uppercut(Fighter *fighter);
void goro_stomp_hold_separation(Fighter *fighter);
void goro_stomp_victim(Fighter *victim);
void goro_uppercut_victim(Fighter *victim);
void goro_victim_finish_getup(Fighter *victim);
void goro_victim_getup(Fighter *victim);

void goro_entry_retail_script_004de6e4(Fighter *fighter);
void goro_entry_retail_script_004de7e8(Fighter *fighter);
void goro_entry_retail_script_004dfb60(Fighter *fighter);
void goro_entry_retail_script_004dfb70(Fighter *fighter);
void goro_entry_retail_script_004dfbb8(Fighter *fighter);
void goro_entry_retail_script_004e5e28(Fighter *fighter);
void goro_entry_retail_script_004e5e50(Fighter *fighter);
void goro_entry_retail_script_004e61f8(Fighter *fighter);
void goro_entry_retail_script_004e6270(Fighter *fighter);
void goro_entry_retail_script_004e63d8(Fighter *fighter);
void goro_entry_retail_script_004e6500(Fighter *fighter);
void goro_entry_retail_script_004e6658(Fighter *fighter);
void goro_entry_retail_script_004e6710(Fighter *fighter);
void goro_entry_retail_script_004e9910(Fighter *fighter);
void goro_entry_retail_script_004ed790(Fighter *fighter);
void goro_entry_retail_script_004dfbd0(Fighter *fighter);
void goro_entry_retail_script_004dfc38(Fighter *fighter);
void goro_entry_retail_script_004dfc50(Fighter *fighter);
void goro_entry_retail_script_004dfc60(Fighter *fighter);
void goro_entry_retail_script_004e8dd0(Fighter *fighter);
void goro_entry_retail_script_004e8e50(Fighter *fighter);
void goro_entry_retail_script_004e8e80(Fighter *fighter);
void goro_entry_retail_script_004e95c8(Fighter *fighter);
void goro_entry_retail_script_004ea3e0(Fighter *fighter);
void goro_entry_retail_script_004ea400(Fighter *fighter);
#endif

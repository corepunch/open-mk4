#include "retail_native.h"
#ifndef MK4_FUJIN_PRIVATE_H
#define MK4_FUJIN_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_knockdown fujin_s_knockdown
#define s_throw fujin_s_throw

void fujin_enter_throw(Fighter *fighter);
void fujin_press_air_hk(Fighter *fighter);
void fujin_press_air_lk(Fighter *fighter);
void fujin_press_air_lp(Fighter *fighter);
void fujin_press_air_punch(Fighter *fighter);
void fujin_press_kick(Fighter *fighter);
void fujin_press_low_kick(Fighter *fighter);
void fujin_press_low_punch(Fighter *fighter);
void fujin_press_punch(Fighter *fighter);
void fujin_script_air_finish(Fighter *fighter);
void fujin_script_current_reaction(Fighter *fighter, int reaction);
void fujin_script_lp_extended_finish(Fighter *fighter);
void fujin_script_lp_short_finish(Fighter *fighter);
void fujin_script_opponent_reaction(Fighter *fighter, int reaction);
extern const MoveDef *const fujin_moves[13];
void fujin_entry_retail_script_004de2f8(Fighter *fighter);
void fujin_entry_retail_script_004de798(Fighter *fighter);
void fujin_entry_retail_script_004e61f8(Fighter *fighter);
void fujin_entry_retail_script_004e63d8(Fighter *fighter);
void fujin_entry_retail_script_004e6500(Fighter *fighter);
void fujin_entry_retail_script_004e6658(Fighter *fighter);
void fujin_entry_retail_script_004e6710(Fighter *fighter);
extern const MoveDef fujin_s_knockdown;
extern const MoveDef fujin_s_throw;

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
bool fujin_can_dive(const Fighter *fighter);
void fujin_dive(Fighter *fighter);
void fujin_dive_contact(Fighter *fighter);
void fujin_dive_land(Fighter *fighter);
void fujin_entry_retail_script_004e8258(Fighter *fighter);
void fujin_entry_retail_script_004e8298(Fighter *fighter);
void fujin_weapon_action(Fighter *fighter);
void fujin_draw_equip(Fighter *fighter);
void fujin_equip_crossbow(Fighter *fighter);
void fujin_bow_finish(Fighter *fighter);
void fujin_bow_followup_next(Fighter *fighter);
void fujin_bolt_victim(Fighter *fighter);
void fujin_entry_retail_script_004e62c0(Fighter *fighter);
void fujin_entry_retail_script_004e62d8(Fighter *fighter);
void fujin_entry_retail_script_004e66d0(Fighter *fighter);
void fujin_entry_retail_script_004e66f0(Fighter *fighter);
void fujin_entry_retail_script_004ed408(Fighter *fighter);
void fujin_entry_retail_script_004ed788(Fighter *fighter);
void fujin_entry_retail_script_004e9e10(Fighter *fighter);
extern const uint32_t fujin_weapon_descriptor[];
bool fujin_can_wind(const Fighter *fighter);
bool fujin_can_wind_slam(const Fighter *fighter);
void fujin_wind(Fighter *fighter);
void fujin_wind_slam(Fighter *fighter);
void fujin_wind_victim(Fighter *fighter);
void fujin_wind_slam_victim(Fighter *fighter);
void fujin_wind_recovery(Fighter *fighter);
void fujin_wind_getup(Fighter *fighter);
void fujin_wind_crouch_return(Fighter *fighter);
void fujin_entry_retail_script_004e5c00(Fighter *fighter);
void fujin_entry_retail_script_004e5c20(Fighter *fighter);
void fujin_entry_retail_script_004e5c50(Fighter *fighter);
void fujin_entry_retail_script_004e9fe0(Fighter *fighter);
void fujin_entry_retail_script_004e9ff0(Fighter *fighter);
void fujin_triple_kick(Fighter *fighter);
void fujin_triple_contact(Fighter *fighter);
void fujin_triple_fast(Fighter *fighter);
void fujin_triple_victim(Fighter *fighter);
void fujin_entry_retail_script_004e8740(Fighter *fighter);
void fujin_entry_retail_script_004e8770(Fighter *fighter);
void fujin_entry_retail_script_004ea040(Fighter *fighter);
void fujin_entry_retail_script_004ea050(Fighter *fighter);
void fujin_entry_retail_script_004ea0b8(Fighter *fighter);
bool fujin_can_wind_grab(const Fighter *fighter);
void fujin_wind_grab(Fighter *fighter);
void fujin_grab_control_start(Fighter *fighter);
void fujin_grab_victim(Fighter *fighter);
void fujin_entry_retail_script_004e8830(Fighter *fighter);
void fujin_entry_retail_script_004e9fb0(Fighter *fighter);
void fujin_second_fatality(Fighter *fighter);
void fujin_second_victim(Fighter *fighter);
void fujin_second_burst(Fighter *fighter);
void fujin_second_finish(Fighter *fighter);
void fujin_second_face(Fighter *fighter);
void fujin_second_event(Fighter *fighter, int selector);
void fujin_second_blood(Fighter *fighter, int task_type);
void fujin_entry_retail_script_004dfed0(Fighter *fighter);
void fujin_entry_retail_script_004ea2d8(Fighter *fighter);
void fujin_entry_retail_script_004ea2a8(Fighter *fighter);
bool fujin_can_first_fatality(const Fighter *fighter);
void fujin_first_fatality(Fighter *fighter);
void fujin_first_loop_start(Fighter *fighter);
void fujin_first_cycle(Fighter *fighter);
void fujin_first_shoot(Fighter *fighter);
void fujin_first_fire_end(Fighter *fighter);
void fujin_entry_retail_script_004e1f58(Fighter *fighter);
void fujin_entry_retail_script_004e1f68(Fighter *fighter);
void fujin_entry_retail_script_004e1f80(Fighter *fighter);
void fujin_entry_retail_script_004e1f98(Fighter *fighter);
#endif

#include "retail_native.h"
#ifndef MK4_JAX_PRIVATE_H
#define MK4_JAX_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block jax_s_block
#define s_crouch jax_s_crouch
#define s_throw jax_s_throw
#define s_thrown jax_s_thrown

void jax_air_throw(Fighter *fighter);
void jax_air_throw_fall(Fighter *fighter);
void jax_air_throw_getup(Fighter *fighter);
void jax_air_throw_release(Fighter *fighter);
void jax_lp_slam_first(Fighter *fighter);
void jax_lp_slam_second(Fighter *fighter);
void jax_lp_slam_third(Fighter *fighter);
void jax_lp_throw_return(Fighter *fighter);
void jax_throw_grunt(void);
void jax_entry_retail_script_004de518(Fighter *fighter);
void jax_entry_retail_script_004de538(Fighter *fighter);
void jax_entry_retail_script_004de558(Fighter *fighter);
void jax_entry_retail_script_004de568(Fighter *fighter);
bool jax_can_fatality_1(const Fighter *fighter);
void jax_fatality_1(Fighter *fighter);
void jax_fatality_1_first_arm(Fighter *fighter);
void jax_fatality_1_grab(Fighter *fighter);
void jax_fatality_1_second_arm(Fighter *fighter);
void jax_fatality_1_victory(Fighter *fighter);
void jax_fatality_2(Fighter *fighter);
void jax_fatality_2_approach(Fighter *fighter);
void jax_fatality_2_finish(Fighter *fighter);
void jax_fatality_2_flash(Fighter *fighter, int task_type);
void jax_fatality_2_sparks(Fighter *fighter, int task_type);
void jax_fatality_2_victim_pause(Fighter *fighter);
void jax_fatality_task_end(Fighter *fighter);
void jax_script_peer_reaction(Fighter *fighter, int reaction);
void jax_entry_retail_script_004e13f0(Fighter *fighter);
void jax_ground_pound(Fighter *fighter);
bool jax_can_dash_punch(const Fighter *fighter);
void jax_dash_punch(Fighter *fighter);
void jax_dash_punch_contact(Fighter *fighter);
void jax_dash_victim_getup(Fighter *fighter);
void jax_entry_retail_script_004ea028(Fighter *fighter);
void jax_entry_retail_script_004e9e38(Fighter *fighter);
void jax_entry_retail_script_004e89d8(Fighter *fighter);
bool jax_can_missile(const Fighter *fighter);
void jax_missile(Fighter *fighter);
void jax_missile_spawn(Fighter *fighter, int task_type);
void jax_fan_tail(Fighter *fighter);
void jax_lp_throw(Fighter *fighter);
void jax_press_air_hp(Fighter *fighter);
void jax_press_air_kick(Fighter *fighter);
void jax_press_air_lp(Fighter *fighter);
void jax_press_hk(Fighter *fighter);
void jax_press_hp(Fighter *fighter);
void jax_press_lk(Fighter *fighter);
void jax_press_lp(Fighter *fighter);
void jax_script_air_finish(Fighter *fighter);
void jax_script_lk_finish(Fighter *fighter);
void jax_script_lk_return(Fighter *fighter);
void jax_script_opponent_reaction(Fighter *fighter, int reaction);
void jax_weapon_action(Fighter *fighter);
void jax_entry_retail_script_004de2f8(Fighter *fighter);
void jax_entry_retail_script_004de358(Fighter *fighter);
void jax_entry_retail_script_004de798(Fighter *fighter);
void jax_entry_retail_script_004e1390(Fighter *fighter);
void jax_entry_retail_script_004e13a0(Fighter *fighter);
void jax_entry_retail_script_004e13b0(Fighter *fighter);
void jax_entry_retail_script_004e13c0(Fighter *fighter);
void jax_entry_retail_script_004e13d0(Fighter *fighter);
void jax_entry_retail_script_004e13e0(Fighter *fighter);
void jax_entry_retail_script_004e5a80(Fighter *fighter);
void jax_entry_retail_script_004e5a98(Fighter *fighter);
void jax_entry_retail_script_004e5b10(Fighter *fighter);
void jax_entry_retail_script_004e5b38(Fighter *fighter);
void jax_entry_retail_script_004e5dd8(Fighter *fighter);
void jax_entry_retail_script_004e5df8(Fighter *fighter);
void jax_entry_retail_script_004e61f8(Fighter *fighter);
void jax_entry_retail_script_004e62c0(Fighter *fighter);
void jax_entry_retail_script_004e62d8(Fighter *fighter);
void jax_entry_retail_script_004e63d8(Fighter *fighter);
void jax_entry_retail_script_004e6500(Fighter *fighter);
void jax_entry_retail_script_004e6658(Fighter *fighter);
void jax_entry_retail_script_004e6710(Fighter *fighter);
void jax_entry_retail_script_004e8150(Fighter *fighter);
void jax_entry_retail_script_004ed090(Fighter *fighter);
extern const MoveDef jax_s_block;
extern const MoveDef jax_s_crouch;
extern const MoveDef jax_s_throw;
extern const MoveDef jax_s_thrown;

extern const ButtonTable jax_diagonal_air;
void jax_press_air_hk(Fighter *fighter);
void jax_press_air_lk(Fighter *fighter);
void jax_close_hp(Fighter *fighter);
void jax_combo_hp(Fighter *fighter);
void jax_close_hk(Fighter *fighter);
void jax_script_close_hp_tail(Fighter *fighter);
void jax_script_combo_hp_tail(Fighter *fighter);
void jax_script_close_hk_tail(Fighter *fighter);
void jax_entry_retail_script_004e61e0(Fighter *fighter);
void jax_entry_retail_script_004e69b0(Fighter *fighter);
void jax_entry_retail_script_004e6190(Fighter *fighter);
void jax_lk_throw(Fighter *fighter);
void jax_entry_retail_script_004e5e18(Fighter *fighter);
void jax_entry_retail_script_004e98e8(Fighter *fighter);
void jax_script_weapon_draw_finish(Fighter *fighter);
void jax_command_weapon(Fighter *fighter);
void jax_entry_retail_script_004e66d0(Fighter *fighter);
void jax_entry_retail_script_004e66f0(Fighter *fighter);
void jax_entry_retail_script_004ea040(Fighter *fighter);
void jax_entry_retail_script_004ea0b8(Fighter *fighter);
#endif

#include "retail_native.h"
#ifndef MK4_JAREK_PRIVATE_H
#define MK4_JAREK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block jarek_s_block
#define s_crouch jarek_s_crouch
#define s_high_kick jarek_s_high_kick
#define s_jab jarek_s_jab
#define s_low_kick jarek_s_low_kick
#define s_sweep jarek_s_sweep
#define s_throw jarek_s_throw
#define s_uppercut jarek_s_uppercut

/* Character-owned finisher ids, outside the shared FINISH_* range. */
enum {
  JAREK_EVENT_FATALITY_1 = 25,
  JAREK_EVENT_FATALITY_2
};

void jarek_fatality_contact_hold(Fighter *fighter);
void jarek_lp_throw(Fighter *fighter);
void jarek_press_air_hp(Fighter *fighter);
void jarek_press_air_kick(Fighter *fighter);
void jarek_press_air_hk(Fighter *fighter);
void jarek_press_air_lk(Fighter *fighter);
void jarek_press_air_lp(Fighter *fighter);
void jarek_press_kick(Fighter *fighter);
void jarek_press_low_kick(Fighter *fighter);
void jarek_press_low_punch(Fighter *fighter);
void jarek_press_punch(Fighter *fighter);
void jarek_press_uppercut(Fighter *fighter);
void jarek_script_air_finish(Fighter *fighter);
void jarek_script_current_reaction(Fighter *fighter, int reaction);
void jarek_script_lk_return(Fighter *fighter);
void jarek_script_low_kick_avoid(Fighter *fighter);
void jarek_script_low_kick_clear(Fighter *fighter);
void jarek_script_lp_end(Fighter *fighter);
void jarek_script_lp_return(Fighter *fighter);
void jarek_script_lp_select(Fighter *fighter);
void jarek_script_opponent_reaction(Fighter *fighter, int reaction);
void jarek_script_roundhouse_finish(Fighter *fighter);
void jarek_script_roundhouse_start(Fighter *fighter);
void jarek_script_weapon_draw_finish(Fighter *fighter);
void jarek_spike_fatality(Fighter *fighter);
void jarek_cannonball(Fighter *fighter);
void jarek_fatality_1(Fighter *fighter);
void jarek_first_heart(Fighter *fighter);
void jarek_first_victim_rip(Fighter *fighter);
void jarek_first_victim_impact(Fighter *fighter);
void jarek_first_victim_end(Fighter *fighter);
void jarek_first_victim_done(Fighter *fighter);
void jarek_entry_retail_script_004e1220(Fighter *fighter);
void jarek_entry_retail_script_004e1230(Fighter *fighter);
void jarek_entry_retail_script_004e1248(Fighter *fighter);
void jarek_entry_retail_script_004e1260(Fighter *fighter);
void jarek_entry_retail_script_004e1270(Fighter *fighter);
void jarek_entry_retail_script_004e1280(Fighter *fighter);
void jarek_entry_retail_script_004e1290(Fighter *fighter);
void jarek_fatality_2(Fighter *fighter);
void jarek_second_glow(Fighter *fighter);
void jarek_second_handoff(Fighter *fighter);
void jarek_second_head_fall(Fighter *fighter, int task_type);
void jarek_second_victim_hold(Fighter *fighter);
void jarek_entry_retail_script_004d2db0(Fighter *fighter);
void jarek_entry_retail_script_004d2de0(Fighter *fighter);
void jarek_entry_retail_script_004d2e10(Fighter *fighter);
void jarek_quake(Fighter *fighter);
void jarek_blade(Fighter *fighter);
void jarek_blade_spawn(Fighter *fighter, int task_type);
void jarek_quake_tail(Fighter *fighter);
void jarek_entry_retail_script_004e81c8(Fighter *fighter);
void jarek_upward_ball(Fighter *fighter);
void jarek_ball_land(Fighter *fighter);
void jarek_entry_retail_script_004e84b8(Fighter *fighter);
void jarek_entry_retail_script_004e9658(Fighter *fighter);
void jarek_uppercut(Fighter *fighter);
void jarek_weapon_action(Fighter *fighter);
void jarek_entry_retail_script_004de2f8(Fighter *fighter);
void jarek_entry_retail_script_004de640(Fighter *fighter);
void jarek_entry_retail_script_004de664(Fighter *fighter);
void jarek_entry_retail_script_004de668(Fighter *fighter);
void jarek_entry_retail_script_004de6b8(Fighter *fighter);
void jarek_entry_retail_script_004de798(Fighter *fighter);
void jarek_entry_retail_script_004e6950(Fighter *fighter);
void jarek_entry_retail_script_004e61f8(Fighter *fighter);
void jarek_entry_retail_script_004e62a0(Fighter *fighter);
void jarek_entry_retail_script_004e62c0(Fighter *fighter);
void jarek_entry_retail_script_004e62d8(Fighter *fighter);
void jarek_entry_retail_script_004e63d8(Fighter *fighter);
void jarek_entry_retail_script_004e6500(Fighter *fighter);
void jarek_entry_retail_script_004e6658(Fighter *fighter);
void jarek_entry_retail_script_004e6710(Fighter *fighter);
void jarek_entry_retail_script_004e6800(Fighter *fighter);
void jarek_entry_retail_script_004e6860(Fighter *fighter);
void jarek_entry_retail_script_004e6878(Fighter *fighter);
void jarek_entry_retail_script_004e6950(Fighter *fighter);
void jarek_entry_retail_script_004e81a8(Fighter *fighter);
void jarek_entry_retail_script_004ed118(Fighter *fighter);
extern const MoveDef jarek_s_block;
extern const MoveDef jarek_s_crouch;
extern const MoveDef jarek_s_high_kick;
extern const MoveDef jarek_s_jab;
extern const MoveDef jarek_s_low_kick;
extern const MoveDef jarek_s_sweep;
extern const MoveDef jarek_s_throw;
extern const MoveDef jarek_s_uppercut;

void jarek_close_hp(Fighter *fighter);
void jarek_combo_hp(Fighter *fighter);
void jarek_close_hk(Fighter *fighter);
void jarek_press_air_ground_hp(Fighter *fighter);
void jarek_script_close_hp_tail(Fighter *fighter);
void jarek_script_combo_hp_tail(Fighter *fighter);
void jarek_script_close_hk_tail(Fighter *fighter);
void jarek_entry_retail_script_004e61e0(Fighter *fighter);
void jarek_entry_retail_script_004e69b0(Fighter *fighter);
void jarek_entry_retail_script_004e6190(Fighter *fighter);
void jarek_lk_throw(Fighter *fighter);
void jarek_spike_fatality(Fighter *fighter);
void jarek_roundhouse(Fighter *fighter);
void jarek_script_roundhouse_finish(Fighter *fighter);
void jarek_entry_retail_script_004e6270(Fighter *fighter);
void jarek_weapon_upper_contact(Fighter *fighter);
void jarek_weapon_reaction(Fighter *fighter);
void jarek_entry_retail_script_004ec4f0(Fighter *fighter);
void jarek_entry_retail_script_004e66d0(Fighter *fighter);
void jarek_entry_retail_script_004e66f0(Fighter *fighter);
void jarek_entry_retail_script_004e8c08(Fighter *fighter);
#endif

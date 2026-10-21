#include "retail_native.h"
#ifndef MK4_QUAN_CHI_PRIVATE_H
#define MK4_QUAN_CHI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
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
void quan_chi_script_air_finish(Fighter *fighter);
void quan_chi_script_air_release(Fighter *fighter);
void quan_chi_script_air_land(Fighter *fighter);
void quan_chi_script_air_getup(Fighter *fighter);
void quan_chi_script_current_reaction(Fighter *fighter, int reaction);
void quan_chi_script_finish(Fighter *fighter);
void quan_chi_script_lk_return(Fighter *fighter);
void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction);
void quan_chi_script_roundhouse_start(Fighter *fighter);
void quan_chi_script_spike_tail(Fighter *fighter);
void quan_chi_script_sweep_finish(Fighter *fighter);
void quan_chi_sweep(Fighter *fighter);
void quan_chi_uppercut(Fighter *fighter);
void quan_chi_entry_retail_script_004de2f8(Fighter *fighter);
void quan_chi_entry_retail_script_004de798(Fighter *fighter);
void quan_chi_entry_retail_script_004e6998(Fighter *fighter);
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
void quan_chi_close_hp(Fighter *fighter);
void quan_chi_combo_hp(Fighter *fighter);
void quan_chi_close_hk(Fighter *fighter);
void quan_chi_press_air_ground_hp(Fighter *fighter);
void quan_chi_script_close_hp_tail(Fighter *fighter);
void quan_chi_script_combo_hp_tail(Fighter *fighter);
void quan_chi_script_close_hk_tail(Fighter *fighter);
void quan_chi_entry_retail_script_004e61e0(Fighter *fighter);
void quan_chi_entry_retail_script_004e69b0(Fighter *fighter);
void quan_chi_entry_retail_script_004e6190(Fighter *fighter);
void quan_chi_lk_throw(Fighter *fighter);
void quan_chi_spike_fatality(Fighter *fighter);
void quan_chi_air_throw(Fighter *fighter);
void quan_chi_roundhouse(Fighter *fighter);
void quan_chi_script_roundhouse_finish(Fighter *fighter);
void quan_chi_entry_retail_script_004e6270(Fighter *fighter);
void quan_chi_skull(Fighter *fighter);
void quan_chi_script_skull_release(Fighter *fighter);
void quan_chi_entry_retail_script_004ed3a0(Fighter *fighter);
void quan_chi_entry_retail_script_004ed3c0(Fighter *fighter);
void quan_chi_entry_retail_script_004e9cd8(Fighter *fighter);
void quan_chi_flip_kick(Fighter *fighter);
void quan_chi_script_flip_kick_tail(Fighter *fighter);
void quan_chi_script_getup_roll(Fighter *fighter);
void quan_chi_entry_retail_script_004e8100(Fighter *fighter);
void quan_chi_entry_retail_script_004e8a48(Fighter *fighter);
void quan_chi_teleport_stomp(Fighter *fighter);
void quan_chi_script_teleport_land(Fighter *fighter);
void quan_chi_script_stomp_victim(Fighter *fighter);
void quan_chi_entry_retail_script_004e9868(Fighter *fighter);
void quan_chi_entry_retail_script_004e8818(Fighter *fighter);
void quan_chi_weapon_steal(Fighter *fighter);
void quan_chi_entry_retail_script_004e9818(Fighter *fighter);
void quan_chi_entry_retail_script_004e9838(Fighter *fighter);
void quan_chi_weapon_action(Fighter *fighter);
void quan_chi_script_draw_equip(Fighter *fighter);
void quan_chi_armed_idle(Fighter *fighter);
void quan_chi_armed_walk(Fighter *fighter);
void quan_chi_armed_crouch(Fighter *fighter);
void quan_chi_armed_block(Fighter *fighter);
extern const uint32_t quan_chi_weapon_descriptor[];
void quan_chi_entry_retail_script_004e62c0(Fighter *fighter);
void quan_chi_entry_retail_script_004e62d8(Fighter *fighter);
void quan_chi_entry_retail_script_004e66d0(Fighter *fighter);
void quan_chi_entry_retail_script_004e66f0(Fighter *fighter);
void quan_chi_first_fatality(Fighter *fighter);
void quan_chi_second_fatality(Fighter *fighter);
bool quan_chi_can_first_fatality(const Fighter *fighter);
void quan_chi_fatality_cue_40(Fighter *fighter);
void quan_chi_fatality_leg(Fighter *fighter);
void quan_chi_fatality_hit_179(Fighter *fighter);
void quan_chi_fatality_hit_241(Fighter *fighter);
void quan_chi_fatality_beat(Fighter *fighter);
void quan_chi_entry_retail_script_004e14f8(Fighter *fighter);
void quan_chi_entry_retail_script_004e1508(Fighter *fighter);
void quan_chi_entry_retail_script_004e1518(Fighter *fighter);
void quan_chi_entry_retail_script_004e1528(Fighter *fighter);
void quan_chi_entry_retail_script_004e1538(Fighter *fighter);
void quan_chi_entry_retail_script_004e1558(Fighter *fighter);
void quan_chi_entry_retail_script_004e1568(Fighter *fighter);
void quan_chi_entry_retail_script_004e1580(Fighter *fighter);

/* Roster-unique finisher identity for the first fatality scene (44bcd0).
 * The second fatality (43be30) runs the victim roster's own scene and id. */
enum {
  QUAN_CHI_FINISH_LEG = 31
};
#endif

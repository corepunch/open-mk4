#include "retail_native.h"
#ifndef MK4_REPTILE_PRIVATE_H
#define MK4_REPTILE_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */

enum {
  REPTILE_EVENT_FATALITY_1 = 33,
  REPTILE_EVENT_FATALITY_2
};

#define s_jab reptile_s_jab
#define s_kick reptile_s_kick
#define s_lp_thrown reptile_s_lp_thrown
#define s_sweep reptile_s_sweep
#define s_throw reptile_s_throw
#define s_uppercut reptile_s_uppercut

void reptile_lp_throw(Fighter *fighter);
void reptile_press_air_hk(Fighter *fighter);
void reptile_press_air_hp(Fighter *fighter);
void reptile_press_air_lk(Fighter *fighter);
void reptile_press_air_lp(Fighter *fighter);
void reptile_press_kick(Fighter *fighter);
void reptile_press_low_kick(Fighter *fighter);
void reptile_press_low_punch(Fighter *fighter);
void reptile_press_punch(Fighter *fighter);
void reptile_press_sweep(Fighter *fighter);
void reptile_press_uppercut(Fighter *fighter);
void reptile_script_air_finish(Fighter *fighter);
void reptile_script_finish(Fighter *fighter);
void reptile_script_lk_return(Fighter *fighter);
void reptile_script_lp_return(Fighter *fighter);
void reptile_script_opponent_reaction(Fighter *fighter, int reaction);
void reptile_script_roundhouse_start(Fighter *fighter);
void reptile_script_spike_tail(Fighter *fighter);
void reptile_sweep(Fighter *fighter);
void reptile_entry_retail_script_004de2f8(Fighter *fighter);
void reptile_entry_retail_script_004de6b8(Fighter *fighter);
void reptile_entry_retail_script_004de798(Fighter *fighter);
void reptile_entry_retail_script_004e5ca0(Fighter *fighter);
void reptile_entry_retail_script_004e5d90(Fighter *fighter);
void reptile_entry_retail_script_004e61f8(Fighter *fighter);
void reptile_entry_retail_script_004e62a0(Fighter *fighter);
void reptile_entry_retail_script_004e62f0(Fighter *fighter);
void reptile_entry_retail_script_004e63d8(Fighter *fighter);
void reptile_entry_retail_script_004e6500(Fighter *fighter);
void reptile_entry_retail_script_004e6658(Fighter *fighter);
void reptile_entry_retail_script_004e6710(Fighter *fighter);
void reptile_entry_retail_script_004e6798(Fighter *fighter);
void reptile_entry_retail_script_004e6800(Fighter *fighter);
void reptile_entry_retail_script_004e6860(Fighter *fighter);
void reptile_entry_retail_script_004e6878(Fighter *fighter);
void reptile_entry_retail_script_004e6950(Fighter *fighter);
void reptile_entry_retail_script_004e8128(Fighter *fighter);
void reptile_entry_retail_script_004e8178(Fighter *fighter);
void reptile_entry_retail_script_004e9620(Fighter *fighter);
void reptile_entry_retail_script_004e9bd0(Fighter *fighter);
void reptile_entry_retail_script_004ed1e8(Fighter *fighter);
extern const MoveDef reptile_s_jab;
extern const MoveDef reptile_s_kick;
extern const MoveDef reptile_s_lp_thrown;
extern const MoveDef reptile_s_sweep;
extern const MoveDef reptile_s_throw;
extern const MoveDef reptile_s_uppercut;

extern const ButtonTable reptile_diagonal_air;
void reptile_press_air_hk(Fighter *fighter);
void reptile_press_air_lk(Fighter *fighter);
void reptile_close_hp(Fighter *fighter);
void reptile_combo_hp(Fighter *fighter);
void reptile_close_hk(Fighter *fighter);
void reptile_press_air_ground_hp(Fighter *fighter);
void reptile_script_close_hp_tail(Fighter *fighter);
void reptile_script_combo_hp_tail(Fighter *fighter);
void reptile_script_close_hk_tail(Fighter *fighter);
void reptile_entry_retail_script_004e61e0(Fighter *fighter);
void reptile_entry_retail_script_004e69b0(Fighter *fighter);
void reptile_entry_retail_script_004e6190(Fighter *fighter);
void reptile_lk_throw(Fighter *fighter);
void reptile_entry_spike(Fighter *fighter);
void reptile_roundhouse(Fighter *fighter);
void reptile_script_roundhouse_finish(Fighter *fighter);
void reptile_input_47a690(Fighter *fighter);
void reptile_input_47afe0(Fighter *fighter);
bool reptile_can_dash(const Fighter *fighter);
void reptile_entry_retail_script_004e6270(Fighter *fighter);
void reptile_entry_retail_script_004e97f8(Fighter *fighter);
bool reptile_can_spit(const Fighter *fighter);
void reptile_acid_spit(Fighter *fighter);
void reptile_acid_spawn(Fighter *fighter, int task_type);
void reptile_acid_victim(Fighter *fighter);
void reptile_acid_victim_tail(Fighter *fighter);
void reptile_script_acid_drool(Fighter *fighter);
void reptile_script_head_close(Fighter *fighter);
void reptile_script_head_open(Fighter *fighter);
void reptile_script_head_wide(Fighter *fighter);
void reptile_entry_retail_script_004e5bb0(Fighter *fighter);
bool reptile_can_invisibility(const Fighter *fighter);
void reptile_invisibility(Fighter *fighter);
void reptile_script_invisibility(Fighter *fighter);
void reptile_script_dash_contact(Fighter *fighter);
void reptile_script_roll_tail(Fighter *fighter);
void reptile_script_kick_tail(Fighter *fighter);
void reptile_dash_victim(Fighter *fighter);
void reptile_dash_victim_getup(Fighter *fighter);
void reptile_reaction_40(Fighter *fighter);
void reptile_entry_retail_script_004e5c70(Fighter *fighter);
void reptile_entry_retail_script_004e95f0(Fighter *fighter);
void reptile_entry_retail_script_004dff08(Fighter *fighter);
void reptile_entry_retail_script_004ea290(Fighter *fighter);
void reptile_entry_retail_script_004ea2a8(Fighter *fighter);
void reptile_fatality_2(Fighter *fighter);
void reptile_second_acid(Fighter *fighter, int task_type);
void reptile_second_event(Fighter *fighter, int selector);
void reptile_second_face(Fighter *fighter);
void reptile_second_finish(Fighter *fighter);
void reptile_second_hover(Fighter *fighter, int task_type);
void reptile_second_melt(Fighter *fighter);
void reptile_second_victim(Fighter *fighter);
void reptile_entry_retail_script_004e2e60(Fighter *fighter);
void reptile_entry_retail_script_004e2e70(Fighter *fighter);
void reptile_entry_retail_script_004e2e98(Fighter *fighter);
void reptile_entry_retail_script_004e2ea8(Fighter *fighter);
void reptile_entry_retail_script_004e2eb0(Fighter *fighter);
void reptile_entry_retail_script_004e2ec0(Fighter *fighter);
void reptile_entry_retail_script_004e2ed0(Fighter *fighter);
bool reptile_can_first_fatality(const Fighter *fighter);
void reptile_fatality_1(Fighter *fighter);
void reptile_first_owner_bite(Fighter *fighter);
void reptile_first_owner_cycle(Fighter *fighter);
void reptile_first_owner_loop(Fighter *fighter);
void reptile_first_victim_bite(Fighter *fighter);
void reptile_first_victim_collapse(Fighter *fighter);
void reptile_first_victim_shake(Fighter *fighter);
void reptile_entry_retail_script_004e62c0(Fighter *fighter);
void reptile_entry_retail_script_004e62d8(Fighter *fighter);
void reptile_entry_retail_script_004e66d0(Fighter *fighter);
void reptile_entry_retail_script_004e66f0(Fighter *fighter);
void reptile_entry_retail_script_004ec640(Fighter *fighter);
void reptile_entry_retail_script_004e8bc0(Fighter *fighter);
void reptile_entry_retail_script_004e8c98(Fighter *fighter);
void reptile_entry_retail_script_004e8cc0(Fighter *fighter);
void reptile_armed_block(Fighter *fighter);
void reptile_armed_crouch(Fighter *fighter);
void reptile_armed_idle(Fighter *fighter);
void reptile_armed_walk(Fighter *fighter);
void reptile_script_draw_equip(Fighter *fighter);
void reptile_weapon_action(Fighter *fighter);
void reptile_weapon_face_peer(Fighter *fighter);
void reptile_weapon_flurry(Fighter *fighter);
void reptile_weapon_getup_back(Fighter *fighter);
void reptile_weapon_reaction(Fighter *fighter);
void reptile_weapon_stagger_tail(Fighter *fighter);
extern const uint32_t reptile_weapon_descriptor[];
#endif

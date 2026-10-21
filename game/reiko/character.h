#include "retail_native.h"
#ifndef MK4_REIKO_PRIVATE_H
#define MK4_REIKO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Finisher IDs are unique across the roster (coordinator allocation). */
enum {
  REIKO_FINISH_1 = 27,
  REIKO_FINISH_2
};

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_throw reiko_s_throw

void reiko_lp_throw(Fighter *fighter);
void reiko_press_air_hk(Fighter *fighter);
void reiko_press_air_hp(Fighter *fighter);
void reiko_press_air_lk(Fighter *fighter);
void reiko_press_air_lp(Fighter *fighter);
void reiko_press_crouching_hp(Fighter *fighter);
void reiko_press_crouching_kick(Fighter *fighter);
void reiko_press_high_kick(Fighter *fighter);
void reiko_press_high_punch(Fighter *fighter);
void reiko_press_low_kick(Fighter *fighter);
void reiko_press_low_punch(Fighter *fighter);
void reiko_script_air_finish(Fighter *fighter);
void reiko_script_lk_return(Fighter *fighter);
void reiko_script_lp_end(Fighter *fighter);
void reiko_script_lp_return(Fighter *fighter);
void reiko_script_lp_select(Fighter *fighter);
void reiko_script_opponent_reaction(Fighter *fighter, int reaction);
void reiko_entry_retail_script_004de2f8(Fighter *fighter);
void reiko_entry_retail_script_004de640(Fighter *fighter);
void reiko_entry_retail_script_004de664(Fighter *fighter);
void reiko_entry_retail_script_004de668(Fighter *fighter);
void reiko_entry_retail_script_004de6b8(Fighter *fighter);
void reiko_entry_retail_script_004de798(Fighter *fighter);
void reiko_entry_retail_script_004e61f8(Fighter *fighter);
void reiko_entry_retail_script_004e63d8(Fighter *fighter);
void reiko_entry_retail_script_004e6500(Fighter *fighter);
void reiko_entry_retail_script_004e6658(Fighter *fighter);
void reiko_entry_retail_script_004e6710(Fighter *fighter);
void reiko_entry_retail_script_004e9b40(Fighter *fighter);
void reiko_entry_retail_script_004e9bd0(Fighter *fighter);
extern const MoveDef reiko_s_throw;

extern const ButtonTable reiko_diagonal_air;
void reiko_press_air_hk(Fighter *fighter);
void reiko_press_air_lk(Fighter *fighter);
void reiko_close_hp(Fighter *fighter);
void reiko_combo_hp(Fighter *fighter);
void reiko_close_hk(Fighter *fighter);
void reiko_press_air_ground_hp(Fighter *fighter);
void reiko_script_close_hp_tail(Fighter *fighter);
void reiko_script_combo_hp_tail(Fighter *fighter);
void reiko_script_close_hk_tail(Fighter *fighter);
void reiko_entry_retail_script_004e61e0(Fighter *fighter);
void reiko_entry_retail_script_004e69b0(Fighter *fighter);
void reiko_entry_retail_script_004e6190(Fighter *fighter);
void reiko_lk_throw(Fighter *fighter);
void reiko_weapon_action(Fighter *fighter);
void reiko_script_weapon_draw_finish(Fighter *fighter);
void reiko_entry_retail_script_004e62c0(Fighter *fighter);
void reiko_entry_retail_script_004e62d8(Fighter *fighter);
void reiko_entry_retail_script_004e66d0(Fighter *fighter);
void reiko_entry_retail_script_004e66f0(Fighter *fighter);
void reiko_entry_retail_script_004ea040(Fighter *fighter);
void reiko_entry_retail_script_004ea0b8(Fighter *fighter);
void reiko_entry_retail_script_004e82b8(Fighter *fighter);
void reiko_entry_retail_script_004e82e0(Fighter *fighter);
void reiko_entry_retail_script_004e8310(Fighter *fighter);
void reiko_entry_retail_script_004e8818(Fighter *fighter);
void reiko_entry_retail_script_004e9880(Fighter *fighter);
void reiko_entry_retail_script_004e9880(Fighter *fighter);
void reiko_teleport(Fighter *fighter);
void reiko_script_teleport_vanish(Fighter *fighter);
void reiko_script_teleport_slam_fall(Fighter *fighter);
void reiko_script_teleport_slam_drop(Fighter *fighter);
void reiko_script_teleport_land(Fighter *fighter);
void reiko_slam_tail(Fighter *fighter);
void reiko_slam_released_fall(Fighter *fighter);
void reiko_slam_reaction(Fighter *victim);
void reiko_flip_kick(Fighter *fighter);
bool reiko_can_flip_kick(const Fighter *fighter);
void reiko_entry_retail_script_004ed038(Fighter *fighter);
void reiko_entry_retail_script_004e97f8(Fighter *fighter);
void reiko_star(Fighter *fighter);
void reiko_star_spawn(Fighter *fighter, int task_type);
bool reiko_can_star(const Fighter *fighter);
void reiko_script_star_victim_tail(Fighter *fighter);
void reiko_orbit(Fighter *fighter);
bool reiko_can_orbit(const Fighter *fighter);
void reiko_entry_retail_script_004e2cf0(Fighter *fighter);
void reiko_entry_retail_script_004e2d18(Fighter *fighter);
void reiko_entry_retail_script_004e2d28(Fighter *fighter);
void reiko_entry_retail_script_004e2d38(Fighter *fighter);
void reiko_entry_retail_script_004e2d48(Fighter *fighter);
void reiko_entry_retail_script_004e2d58(Fighter *fighter);
void reiko_entry_retail_script_004e2d68(Fighter *fighter);
void reiko_entry_retail_script_004e2d78(Fighter *fighter);
void reiko_entry_retail_script_004e2d88(Fighter *fighter);
void reiko_entry_retail_script_004e2dc0(Fighter *fighter);
void reiko_entry_retail_script_004e2dd0(Fighter *fighter);
void reiko_entry_retail_script_004e2de0(Fighter *fighter);
void reiko_entry_retail_script_004e2df8(Fighter *fighter);
void reiko_entry_retail_script_004e2e08(Fighter *fighter);
void reiko_entry_retail_script_004e2e18(Fighter *fighter);
void reiko_entry_retail_script_004e2e40(Fighter *fighter);
void reiko_entry_retail_script_004e2e50(Fighter *fighter);
void reiko_first_head_59(Fighter *fighter);
void reiko_first_head_83(Fighter *fighter);
void reiko_first_left_arm_56(Fighter *fighter);
void reiko_first_left_arm_land(Fighter *fighter);
void reiko_first_piece_end(Fighter *fighter);
void reiko_first_right_arm_44(Fighter *fighter);
void reiko_first_right_arm_land(Fighter *fighter);
void reiko_first_tear(Fighter *fighter);
void reiko_first_torso_29(Fighter *fighter);
void reiko_first_torso_end(Fighter *fighter);
void reiko_first_victim_118(Fighter *fighter);
void reiko_first_victim_165(Fighter *fighter);
void reiko_first_victory(Fighter *fighter);
void reiko_first_fatality(Fighter *fighter);
bool reiko_can_first_fatality(const Fighter *fighter);
void reiko_entry_retail_script_004d2d10(Fighter *fighter);
void reiko_entry_retail_script_004d2d60(Fighter *fighter);
void reiko_entry_retail_script_004d2d90(Fighter *fighter);
void reiko_second_fatality(Fighter *fighter);
void reiko_second_stars(Fighter *fighter, int task_type);
void reiko_second_owner_park(Fighter *fighter);
void reiko_second_victim_fall(Fighter *fighter);
void reiko_second_victim_end(Fighter *fighter);
#endif

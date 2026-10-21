#include "retail_native.h"
#ifndef MK4_SONYA_PRIVATE_H
#define MK4_SONYA_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define can_cartwheel sonya_can_cartwheel
#define can_first_fatality sonya_can_first_fatality
#define can_leg_grab sonya_can_leg_grab
#define can_second_fatality retail_can_finish_range
#define cpu_air_hk sonya_cpu_air_hk
#define cpu_air_lk sonya_cpu_air_lk
#define cpu_close_hk sonya_cpu_close_hk
#define cpu_close_hp sonya_cpu_close_hp
#define enter_lk_throw retail_lk_throw
#define enter_throw retail_lp_throw
#define enter_victory sonya_enter_victory
#define first_fatality sonya_first_fatality
#define lk_victim sonya_lk_victim
#define press_air_hk sonya_press_air_hk
#define press_air_hp sonya_press_air_hp
#define press_air_lk sonya_press_air_lk
#define press_air_lp sonya_press_air_lp
#define press_combo_hk sonya_press_combo_hk
#define press_combo_hp sonya_press_combo_hp
#define press_knee_hp sonya_press_knee_hp
#define s_knockdown sonya_s_knockdown
#define s_victory sonya_s_victory
#define scene_end_watch sonya_scene_end_watch
#define script_air_diagonal_tail sonya_script_air_diagonal_tail
#define script_air_getup sonya_script_air_getup
#define script_air_hp_tail sonya_script_air_hp_tail
#define script_air_kick_tail sonya_script_air_kick_tail
#define script_air_lk_tail sonya_script_air_lk_tail
#define script_air_lp_tail sonya_script_air_lp_tail
#define script_air_release sonya_script_air_release
#define script_cartwheel_tail sonya_script_cartwheel_tail
#define script_close_hp_tail sonya_script_close_hp_tail
#define script_combo_hp_tail sonya_script_combo_hp_tail
#define script_crouch_lk_tail sonya_script_crouch_lk_tail
#define script_draw_equip sonya_script_draw_equip
#define script_first_release sonya_script_first_release
#define script_leg_grab_tail sonya_script_leg_grab_tail
#define script_leg_victim_end sonya_script_leg_victim_end
#define script_reaction sonya_script_reaction
#define script_ring_flash sonya_script_ring_flash
#define script_ring_spawn sonya_script_ring_spawn
#define script_second_branch sonya_script_second_branch
#define script_second_cycle_end sonya_script_second_cycle_end
#define script_second_open sonya_script_second_open
#define script_second_pause sonya_script_second_pause
#define script_second_repeat sonya_script_second_repeat
#define script_second_victim_end sonya_script_second_victim_end
#define script_square_wave_tail sonya_script_square_wave_tail
#define script_throw_getup sonya_script_throw_getup
#define script_weapon_hp_tail sonya_script_weapon_hp_tail
#define script_weapon_lp_tail sonya_script_weapon_lp_tail
#define script_weapon_victim_getup sonya_script_weapon_victim_getup
#define second_fatality sonya_second_fatality
#define special_cleanup sonya_special_cleanup
#define square_victim sonya_square_victim

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD

bool sonya_can_cartwheel(const Fighter *fighter);
bool sonya_can_first_fatality(const Fighter *fighter);
bool sonya_can_leg_grab(const Fighter *fighter);
void sonya_cpu_air_hk(Fighter *fighter);
void sonya_cpu_air_lk(Fighter *fighter);
void sonya_cpu_close_hk(Fighter *fighter);
void sonya_cpu_close_hp(Fighter *fighter);
void sonya_enter_victory(Fighter *fighter);
void sonya_first_fatality(Fighter *fighter);
void sonya_lk_victim(Fighter *victim);
void sonya_press_air_hk(Fighter *fighter);
void sonya_press_air_hp(Fighter *fighter);
void sonya_press_air_lk(Fighter *fighter);
void sonya_press_air_lp(Fighter *fighter);
void sonya_entry_retail_script_004e01c0(Fighter *fighter);
void sonya_entry_retail_script_004e01d0(Fighter *fighter);
void sonya_entry_retail_script_004e5930(Fighter *fighter);
void sonya_entry_retail_script_004e5940(Fighter *fighter);
void sonya_entry_retail_script_004e5960(Fighter *fighter);
void sonya_entry_retail_script_004e5980(Fighter *fighter);
void sonya_entry_retail_script_004e5990(Fighter *fighter);
void sonya_entry_retail_script_004e59a8(Fighter *fighter);
void sonya_entry_retail_script_004e59c8(Fighter *fighter);
void sonya_entry_retail_script_004e5a20(Fighter *fighter);
void sonya_entry_retail_script_004e5ed0(Fighter *fighter);
void sonya_entry_retail_script_004e62c0(Fighter *fighter);
void sonya_entry_retail_script_004e62d8(Fighter *fighter);
void sonya_entry_retail_script_004e6a40(Fighter *fighter);
void sonya_entry_retail_script_004e81d0(Fighter *fighter);
void sonya_entry_retail_script_004e8578(Fighter *fighter);
void sonya_entry_retail_script_004e85a8(Fighter *fighter);
void sonya_entry_retail_script_004e85c0(Fighter *fighter);
void sonya_entry_retail_script_004e8b38(Fighter *fighter);
void sonya_entry_retail_script_004e8f50(Fighter *fighter);
void sonya_entry_retail_script_004e9110(Fighter *fighter);
void sonya_entry_retail_script_004e98a0(Fighter *fighter);
void sonya_entry_retail_script_004e9a68(Fighter *fighter);
void sonya_entry_retail_script_004e9b88(Fighter *fighter);
void sonya_entry_retail_script_004e9de8(Fighter *fighter);
void sonya_entry_retail_script_004ea0b8(Fighter *fighter);
void sonya_entry_retail_script_004ea130(Fighter *fighter);
void sonya_entry_retail_script_004ec7a0(Fighter *fighter);
void sonya_entry_retail_script_004ec7f0(Fighter *fighter);
void sonya_entry_retail_script_004ed4c0(Fighter *fighter);
extern const MoveDef sonya_s_knockdown;
extern const MoveDef sonya_s_victory;
void sonya_scene_end_watch(Fighter *fighter);
void sonya_script_air_diagonal_tail(Fighter *fighter);
void sonya_script_air_getup(Fighter *victim);
void sonya_script_air_hp_tail(Fighter *fighter);
void sonya_script_air_kick_tail(Fighter *fighter);
void sonya_script_air_lk_tail(Fighter *fighter);
void sonya_script_air_lp_tail(Fighter *fighter);
void sonya_script_air_release(Fighter *fighter);
void sonya_script_cartwheel_tail(Fighter *fighter);
void sonya_script_draw_equip(Fighter *fighter);
void sonya_script_first_release(Fighter *fighter);
void sonya_script_leg_grab_tail(Fighter *fighter);
void sonya_script_leg_victim_end(Fighter *victim);
void sonya_script_reaction(Fighter *fighter, int reaction);
void sonya_script_ring_flash(Fighter *fighter, int task_type);
void sonya_script_ring_spawn(Fighter *fighter, int task_type);
void sonya_script_second_branch(Fighter *fighter);
void sonya_script_second_cycle_end(Fighter *fighter);
void sonya_script_second_open(Fighter *fighter);
void sonya_script_second_pause(Fighter *fighter);
void sonya_script_second_repeat(Fighter *fighter);
void sonya_script_second_victim_end(Fighter *victim);
void sonya_script_square_wave_tail(Fighter *fighter);
void sonya_script_throw_getup(Fighter *victim);
void sonya_script_weapon_hp_tail(Fighter *fighter);
void sonya_script_weapon_lp_tail(Fighter *fighter);
void sonya_script_weapon_victim_getup(Fighter *victim);
void sonya_second_fatality(Fighter *fighter);
void sonya_air_throw(Fighter *fighter);
void sonya_bicycle(Fighter *fighter);
void sonya_cartwheel(Fighter *fighter);
void sonya_hk(Fighter *fighter);
void sonya_leg_grab(Fighter *fighter);
void sonya_ring(Fighter *fighter);
void sonya_square_wave(Fighter *fighter);
void sonya_weapon(Fighter *fighter);
void sonya_special_cleanup(Fighter *fighter);
void sonya_square_victim(Fighter *victim);

#endif

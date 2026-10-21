#include "retail_native.h"
#ifndef MK4_KITANA_PRIVATE_H
#define MK4_KITANA_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define air_hp kitana_air_hp
#define can_ground_throw kitana_can_ground_throw
#define cpu_air_hk kitana_cpu_air_hk
#define cpu_air_lk kitana_cpu_air_lk
#define cpu_close_hk kitana_cpu_close_hk
#define cpu_close_hp kitana_cpu_close_hp
#define enter_lk_throw kitana_enter_lk_throw
#define enter_throw kitana_enter_throw
#define enter_victory kitana_enter_victory
#define ground_attack kitana_ground_attack
#define ground_native_next kitana_ground_native_next
#define ground_native_tail kitana_ground_native_tail
#define lk_victim kitana_lk_victim
#define press_air_hk kitana_press_air_hk
#define press_air_hp kitana_press_air_hp
#define press_air_lk kitana_press_air_lk
#define press_air_lp kitana_press_air_lp
#define press_combo_hk kitana_press_combo_hk
#define press_combo_hp kitana_press_combo_hp
#define press_crouch_hk kitana_press_crouch_hk
#define press_crouch_lk kitana_press_crouch_lk
#define press_crouch_lp kitana_press_crouch_lp
#define press_jab kitana_press_jab
#define press_kick kitana_press_kick
#define press_knee_hp kitana_press_knee_hp
#define press_lk kitana_press_lk
#define press_lp kitana_press_lp
#define s_knockdown kitana_s_knockdown
#define s_throw kitana_s_throw
#define s_victory kitana_s_victory
#define scene_end_watch kitana_scene_end_watch
#define script_air_diagonal_tail kitana_script_air_diagonal_tail
#define script_air_getup kitana_script_air_getup
#define script_air_hp_tail kitana_script_air_hp_tail
#define script_air_kick_tail kitana_script_air_kick_tail
#define script_air_lk_tail kitana_script_air_lk_tail
#define script_air_lp_tail kitana_script_air_lp_tail
#define script_air_release kitana_script_air_release
#define script_cartwheel_tail kitana_script_cartwheel_tail
#define script_close_hp_tail kitana_script_close_hp_tail
#define script_combo_hp_tail kitana_script_combo_hp_tail
#define script_crouch_lk_tail kitana_script_crouch_lk_tail
#define script_draw_equip kitana_script_draw_equip
#define script_first_release kitana_script_first_release
#define script_knee_tail kitana_script_knee_tail
#define script_leg_grab_tail kitana_script_leg_grab_tail
#define script_leg_victim_end kitana_script_leg_victim_end
#define script_lk_return kitana_script_lk_return
#define script_reaction kitana_script_reaction
#define script_ring_flash kitana_script_ring_flash
#define script_ring_spawn kitana_script_ring_spawn
#define script_second_branch kitana_script_second_branch
#define script_second_cycle_end kitana_script_second_cycle_end
#define script_second_open kitana_script_second_open
#define script_second_pause kitana_script_second_pause
#define script_second_repeat kitana_script_second_repeat
#define script_second_victim_end kitana_script_second_victim_end
#define script_square_wave_tail kitana_script_square_wave_tail
#define script_throw_getup kitana_script_throw_getup
#define script_weapon_hp_tail kitana_script_weapon_hp_tail
#define script_weapon_lp_tail kitana_script_weapon_lp_tail
#define script_weapon_victim_getup kitana_script_weapon_victim_getup
#define special_cleanup kitana_special_cleanup
#define square_victim kitana_square_victim

enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK
};

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

void kitana_air_hp(Fighter *fighter);
bool kitana_can_ground_throw(const Fighter *fighter, bool low_kick);
void kitana_cpu_air_hk(Fighter *fighter);
void kitana_cpu_air_lk(Fighter *fighter);
void kitana_cpu_close_hk(Fighter *fighter);
void kitana_cpu_close_hp(Fighter *fighter);
void kitana_enter_lk_throw(Fighter *fighter);
void kitana_enter_throw(Fighter *fighter);
void kitana_enter_victory(Fighter *fighter);
void kitana_ground_attack(Fighter *fighter, int index);
void kitana_ground_native_next(Fighter *fighter);
void kitana_ground_native_tail(Fighter *fighter);
void kitana_air_throw(Fighter *fighter);
void kitana_bicycle(Fighter *fighter);
void kitana_cartwheel(Fighter *fighter);
void kitana_command_shadow_low(Fighter *fighter);
void kitana_command_shadow_up(Fighter *fighter);
void kitana_crouch_lp(Fighter *fighter);
void kitana_hk(Fighter *fighter);
void kitana_hp(Fighter *fighter);
void kitana_leg_grab(Fighter *fighter);
void kitana_ring(Fighter *fighter);
void kitana_shadow_install_physics(Fighter *fighter);
void kitana_shadow_low(Fighter *fighter);
void kitana_shadow_task_low(Fighter *fighter, int task_type);
void kitana_shadow_task_up(Fighter *fighter, int task_type);
void kitana_shadow_up(Fighter *fighter);
void kitana_square_wave(Fighter *fighter);
void kitana_weapon(Fighter *fighter);
void kitana_lk_victim(Fighter *victim);
void kitana_press_air_hk(Fighter *fighter);
void kitana_press_air_hp(Fighter *fighter);
void kitana_press_air_lk(Fighter *fighter);
void kitana_press_air_lp(Fighter *fighter);
void kitana_press_combo_hk(Fighter *fighter);
void kitana_press_combo_hp(Fighter *fighter);
void kitana_press_crouch_hk(Fighter *fighter);
void kitana_press_crouch_lk(Fighter *fighter);
void kitana_press_crouch_lp(Fighter *fighter);
void kitana_press_jab(Fighter *fighter);
void kitana_press_kick(Fighter *fighter);
void kitana_press_knee_hp(Fighter *fighter);
void kitana_press_lk(Fighter *fighter);
void kitana_press_lp(Fighter *fighter);
void kitana_entry_retail_script_004de360(Fighter *fighter);
void kitana_entry_retail_script_004de798(Fighter *fighter);
void kitana_entry_retail_script_004e01c0(Fighter *fighter);
void kitana_entry_retail_script_004e01d0(Fighter *fighter);
void kitana_entry_retail_script_004e5930(Fighter *fighter);
void kitana_entry_retail_script_004e5940(Fighter *fighter);
void kitana_entry_retail_script_004e5960(Fighter *fighter);
void kitana_entry_retail_script_004e5980(Fighter *fighter);
void kitana_entry_retail_script_004e5990(Fighter *fighter);
void kitana_entry_retail_script_004e59a8(Fighter *fighter);
void kitana_entry_retail_script_004e59c8(Fighter *fighter);
void kitana_entry_retail_script_004e5a20(Fighter *fighter);
void kitana_entry_retail_script_004e5d90(Fighter *fighter);
void kitana_entry_retail_script_004e5ed0(Fighter *fighter);
void kitana_entry_retail_script_004e6020(Fighter *fighter);
void kitana_entry_retail_script_004e6158(Fighter *fighter);
void kitana_entry_retail_script_004e6190(Fighter *fighter);
void kitana_entry_retail_script_004e61e0(Fighter *fighter);
void kitana_entry_retail_script_004e61f8(Fighter *fighter);
void kitana_entry_retail_script_004e62c0(Fighter *fighter);
void kitana_entry_retail_script_004e62d8(Fighter *fighter);
void kitana_entry_retail_script_004e63d8(Fighter *fighter);
void kitana_entry_retail_script_004e6500(Fighter *fighter);
void kitana_entry_retail_script_004e6658(Fighter *fighter);
void kitana_entry_retail_script_004e66d0(Fighter *fighter);
void kitana_entry_retail_script_004e66f0(Fighter *fighter);
void kitana_entry_retail_script_004e6710(Fighter *fighter);
void kitana_entry_retail_script_004e69b0(Fighter *fighter);
void kitana_entry_retail_script_004e6a40(Fighter *fighter);
void kitana_entry_retail_script_004e81d0(Fighter *fighter);
void kitana_entry_retail_script_004e8578(Fighter *fighter);
void kitana_entry_retail_script_004e85a8(Fighter *fighter);
void kitana_entry_retail_script_004e85c0(Fighter *fighter);
void kitana_entry_retail_script_004e8b38(Fighter *fighter);
void kitana_entry_retail_script_004e8f50(Fighter *fighter);
void kitana_entry_retail_script_004e9110(Fighter *fighter);
void kitana_entry_retail_script_004e98a0(Fighter *fighter);
void kitana_entry_retail_script_004e9a68(Fighter *fighter);
void kitana_entry_retail_script_004e9b88(Fighter *fighter);
void kitana_entry_retail_script_004e9de8(Fighter *fighter);
void kitana_entry_retail_script_004ea0b8(Fighter *fighter);
void kitana_entry_retail_script_004ea130(Fighter *fighter);
void kitana_entry_retail_script_004ec7a0(Fighter *fighter);
void kitana_entry_retail_script_004ec7f0(Fighter *fighter);
void kitana_entry_retail_script_004ed268(Fighter *fighter);
void kitana_entry_retail_script_004ed2c0(Fighter *fighter);
void kitana_entry_retail_script_004ed4c0(Fighter *fighter);
extern const MoveDef kitana_s_knockdown;
extern const MoveDef kitana_s_throw;
extern const MoveDef kitana_s_victory;
void kitana_scene_end_watch(Fighter *fighter);
void kitana_script_air_diagonal_tail(Fighter *fighter);
void kitana_script_air_getup(Fighter *victim);
void kitana_script_air_hp_tail(Fighter *fighter);
void kitana_script_air_kick_tail(Fighter *fighter);
void kitana_script_air_lk_tail(Fighter *fighter);
void kitana_script_air_lp_tail(Fighter *fighter);
void kitana_script_air_release(Fighter *fighter);
void kitana_script_cartwheel_tail(Fighter *fighter);
void kitana_script_close_hp_tail(Fighter *fighter);
void kitana_script_combo_hp_tail(Fighter *fighter);
void kitana_script_crouch_lk_tail(Fighter *fighter);
void kitana_script_draw_equip(Fighter *fighter);
void kitana_script_first_release(Fighter *fighter);
void kitana_script_knee_tail(Fighter *fighter);
void kitana_script_leg_grab_tail(Fighter *fighter);
void kitana_script_leg_victim_end(Fighter *victim);
void kitana_script_lk_return(Fighter *fighter);
void kitana_script_reaction(Fighter *fighter, int reaction);
void kitana_script_ring_flash(Fighter *fighter, int task_type);
void kitana_script_ring_spawn(Fighter *fighter, int task_type);
void kitana_script_second_branch(Fighter *fighter);
void kitana_script_second_cycle_end(Fighter *fighter);
void kitana_script_second_open(Fighter *fighter);
void kitana_script_second_pause(Fighter *fighter);
void kitana_script_second_repeat(Fighter *fighter);
void kitana_script_second_victim_end(Fighter *victim);
void kitana_script_square_wave_tail(Fighter *fighter);
void kitana_script_throw_getup(Fighter *victim);
void kitana_script_weapon_hp_tail(Fighter *fighter);
void kitana_script_weapon_lp_tail(Fighter *fighter);
void kitana_script_weapon_victim_getup(Fighter *victim);
void kitana_special_cleanup(Fighter *fighter);
void kitana_square_victim(Fighter *victim);

#endif

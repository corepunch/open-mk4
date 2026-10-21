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
#define air_hp sonya_air_hp
#define arena_fatality sonya_arena_fatality
#define armed_block sonya_armed_block
#define armed_crouch sonya_armed_crouch
#define armed_idle sonya_armed_idle
#define armed_walk sonya_armed_walk
#define can_cartwheel sonya_can_cartwheel
#define can_first_fatality sonya_can_first_fatality
#define can_ground_throw sonya_can_ground_throw
#define can_leg_grab sonya_can_leg_grab
#define can_second_fatality retail_can_finish_range
#define cpu_air_hk sonya_cpu_air_hk
#define cpu_air_lk sonya_cpu_air_lk
#define cpu_close_hk sonya_cpu_close_hk
#define cpu_close_hp sonya_cpu_close_hp
#define enter_lk_throw sonya_enter_lk_throw
#define enter_sweep sonya_enter_sweep
#define enter_throw sonya_enter_throw
#define enter_uppercut sonya_enter_uppercut
#define enter_victory sonya_enter_victory
#define first_fatality sonya_first_fatality
#define ground_attack sonya_ground_attack
#define ground_native_next sonya_ground_native_next
#define ground_native_tail sonya_ground_native_tail
#define hp_script sonya_hp_script
#define lk_victim sonya_lk_victim
#define press_air_hk sonya_press_air_hk
#define press_air_hp sonya_press_air_hp
#define press_air_lk sonya_press_air_lk
#define press_air_lp sonya_press_air_lp
#define press_combo_hk sonya_press_combo_hk
#define press_combo_hp sonya_press_combo_hp
#define press_crouch_hk sonya_press_crouch_hk
#define press_crouch_lk sonya_press_crouch_lk
#define press_crouch_lp sonya_press_crouch_lp
#define press_jab sonya_press_jab
#define press_kick sonya_press_kick
#define press_knee_hp sonya_press_knee_hp
#define press_lk sonya_press_lk
#define press_lp sonya_press_lp
#define press_uppercut sonya_press_uppercut
#define s_hp sonya_s_hp
#define s_jab sonya_s_jab
#define s_kick sonya_s_kick
#define s_knockdown sonya_s_knockdown
#define s_sweep sonya_s_sweep
#define s_throw sonya_s_throw
#define s_uppercut sonya_s_uppercut
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
#define script_knee_tail sonya_script_knee_tail
#define script_leg_grab_tail sonya_script_leg_grab_tail
#define script_leg_victim_end sonya_script_leg_victim_end
#define script_lk_return sonya_script_lk_return
#define script_reaction sonya_script_reaction
#define script_ring_flash sonya_script_ring_flash
#define script_ring_spawn sonya_script_ring_spawn
#define script_roundhouse_start sonya_script_roundhouse_start
#define script_roundhouse_tail sonya_script_roundhouse_tail
#define script_second_branch sonya_script_second_branch
#define script_second_cycle_end sonya_script_second_cycle_end
#define script_second_open sonya_script_second_open
#define script_second_pause sonya_script_second_pause
#define script_second_repeat sonya_script_second_repeat
#define script_second_victim_end sonya_script_second_victim_end
#define script_spike_tail sonya_script_spike_tail
#define script_square_wave_tail sonya_script_square_wave_tail
#define script_sweep_tail sonya_script_sweep_tail
#define script_throw_getup sonya_script_throw_getup
#define script_weapon_hp_tail sonya_script_weapon_hp_tail
#define script_weapon_lp_tail sonya_script_weapon_lp_tail
#define script_weapon_victim_getup sonya_script_weapon_victim_getup
#define second_fatality sonya_second_fatality
#define special_cleanup sonya_special_cleanup
#define spike_start sonya_spike_start
#define square_victim sonya_square_victim
#define uppercut_script sonya_uppercut_script

enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK,
  G_ROUNDHOUSE
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

void sonya_air_hp(Fighter *fighter);
void sonya_arena_fatality(Fighter *fighter, int id);
void sonya_armed_block(Fighter *fighter);
void sonya_armed_crouch(Fighter *fighter);
void sonya_armed_idle(Fighter *fighter);
void sonya_armed_walk(Fighter *fighter);
bool sonya_can_cartwheel(const Fighter *fighter);
bool sonya_can_first_fatality(const Fighter *fighter);
bool sonya_can_ground_throw(const Fighter *fighter, bool low_kick);
bool sonya_can_leg_grab(const Fighter *fighter);
void sonya_cpu_air_hk(Fighter *fighter);
void sonya_cpu_air_lk(Fighter *fighter);
void sonya_cpu_close_hk(Fighter *fighter);
void sonya_cpu_close_hp(Fighter *fighter);
void sonya_enter_lk_throw(Fighter *fighter);
void sonya_enter_sweep(Fighter *fighter);
void sonya_enter_throw(Fighter *fighter);
void sonya_enter_uppercut(Fighter *fighter);
void sonya_enter_victory(Fighter *fighter);
void sonya_first_fatality(Fighter *fighter);
void sonya_ground_attack(Fighter *fighter, int index);
void sonya_ground_native_next(Fighter *fighter);
void sonya_ground_native_tail(Fighter *fighter);
void sonya_hp_script(Fighter *fighter);
void sonya_lk_victim(Fighter *victim);
void sonya_press_air_hk(Fighter *fighter);
void sonya_press_air_hp(Fighter *fighter);
void sonya_press_air_lk(Fighter *fighter);
void sonya_press_air_lp(Fighter *fighter);
void sonya_press_combo_hk(Fighter *fighter);
void sonya_press_combo_hp(Fighter *fighter);
void sonya_press_crouch_hk(Fighter *fighter);
void sonya_press_crouch_lk(Fighter *fighter);
void sonya_press_crouch_lp(Fighter *fighter);
void sonya_press_jab(Fighter *fighter);
void sonya_press_kick(Fighter *fighter);
void sonya_press_knee_hp(Fighter *fighter);
void sonya_press_lk(Fighter *fighter);
void sonya_press_lp(Fighter *fighter);
void sonya_press_uppercut(Fighter *fighter);
void sonya_entry_retail_script_004de360(Fighter *fighter);
void sonya_entry_retail_script_004de798(Fighter *fighter);
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
void sonya_entry_retail_script_004e5ca0(Fighter *fighter);
void sonya_entry_retail_script_004e5d20(Fighter *fighter);
void sonya_entry_retail_script_004e5d90(Fighter *fighter);
void sonya_entry_retail_script_004e5ed0(Fighter *fighter);
void sonya_entry_retail_script_004e6020(Fighter *fighter);
void sonya_entry_retail_script_004e6158(Fighter *fighter);
void sonya_entry_retail_script_004e6190(Fighter *fighter);
void sonya_entry_retail_script_004e61e0(Fighter *fighter);
void sonya_entry_retail_script_004e61f8(Fighter *fighter);
void sonya_entry_retail_script_004e62a0(Fighter *fighter);
void sonya_entry_retail_script_004e62c0(Fighter *fighter);
void sonya_entry_retail_script_004e62d8(Fighter *fighter);
void sonya_entry_retail_script_004e62f0(Fighter *fighter);
void sonya_entry_retail_script_004e63d8(Fighter *fighter);
void sonya_entry_retail_script_004e6500(Fighter *fighter);
void sonya_entry_retail_script_004e6658(Fighter *fighter);
void sonya_entry_retail_script_004e66d0(Fighter *fighter);
void sonya_entry_retail_script_004e66f0(Fighter *fighter);
void sonya_entry_retail_script_004e6710(Fighter *fighter);
void sonya_entry_retail_script_004e6798(Fighter *fighter);
void sonya_entry_retail_script_004e6800(Fighter *fighter);
void sonya_entry_retail_script_004e6860(Fighter *fighter);
void sonya_entry_retail_script_004e6878(Fighter *fighter);
void sonya_entry_retail_script_004e6950(Fighter *fighter);
void sonya_entry_retail_script_004e69b0(Fighter *fighter);
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
extern const MoveDef sonya_s_hp;
extern const MoveDef sonya_s_jab;
extern const MoveDef sonya_s_kick;
extern const MoveDef sonya_s_knockdown;
extern const MoveDef sonya_s_sweep;
extern const MoveDef sonya_s_throw;
extern const MoveDef sonya_s_uppercut;
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
void sonya_script_close_hp_tail(Fighter *fighter);
void sonya_script_combo_hp_tail(Fighter *fighter);
void sonya_script_crouch_lk_tail(Fighter *fighter);
void sonya_script_draw_equip(Fighter *fighter);
void sonya_script_first_release(Fighter *fighter);
void sonya_script_knee_tail(Fighter *fighter);
void sonya_script_leg_grab_tail(Fighter *fighter);
void sonya_script_leg_victim_end(Fighter *victim);
void sonya_script_lk_return(Fighter *fighter);
void sonya_script_reaction(Fighter *fighter, int reaction);
void sonya_script_ring_flash(Fighter *fighter, int task_type);
void sonya_script_ring_spawn(Fighter *fighter, int task_type);
void sonya_script_roundhouse_start(Fighter *fighter);
void sonya_script_roundhouse_tail(Fighter *fighter);
void sonya_script_second_branch(Fighter *fighter);
void sonya_script_second_cycle_end(Fighter *fighter);
void sonya_script_second_open(Fighter *fighter);
void sonya_script_second_pause(Fighter *fighter);
void sonya_script_second_repeat(Fighter *fighter);
void sonya_script_second_victim_end(Fighter *victim);
void sonya_script_spike_tail(Fighter *fighter);
void sonya_script_square_wave_tail(Fighter *fighter);
void sonya_script_sweep_tail(Fighter *fighter);
void sonya_script_throw_getup(Fighter *victim);
void sonya_script_weapon_hp_tail(Fighter *fighter);
void sonya_script_weapon_lp_tail(Fighter *fighter);
void sonya_script_weapon_victim_getup(Fighter *victim);
void sonya_second_fatality(Fighter *fighter);
void sonya_air_throw(Fighter *fighter);
void sonya_bicycle(Fighter *fighter);
void sonya_cartwheel(Fighter *fighter);
void sonya_crouch_lp(Fighter *fighter);
void sonya_hk(Fighter *fighter);
void sonya_hp(Fighter *fighter);
void sonya_jab(Fighter *fighter);
void sonya_leg_grab(Fighter *fighter);
void sonya_ring(Fighter *fighter);
void sonya_roundhouse(Fighter *fighter);
void sonya_spike_fatality(Fighter *fighter);
void sonya_square_wave(Fighter *fighter);
void sonya_sweep(Fighter *fighter);
void sonya_uppercut(Fighter *fighter);
void sonya_weapon(Fighter *fighter);
void sonya_special_cleanup(Fighter *fighter);
void sonya_spike_start(Fighter *fighter);
void sonya_square_victim(Fighter *victim);
void sonya_uppercut_script(Fighter *fighter);

#endif

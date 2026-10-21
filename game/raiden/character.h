#include "retail_native.h"
#ifndef MK4_RAIDEN_PRIVATE_H
#define MK4_RAIDEN_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define air_hp raiden_air_hp
#define arena_fatality raiden_arena_fatality
#define armed_block raiden_armed_block
#define armed_crouch raiden_armed_crouch
#define armed_idle raiden_armed_idle
#define armed_walk raiden_armed_walk
#define can_close_fatality retail_can_finish_close
#define can_ground_throw raiden_can_ground_throw
#define can_teleport raiden_can_teleport
#define cpu_air_hk raiden_cpu_air_hk
#define cpu_air_lk raiden_cpu_air_lk
#define cpu_close_hk raiden_cpu_close_hk
#define cpu_close_hp raiden_cpu_close_hp
#define enter_lk_throw raiden_enter_lk_throw
#define enter_sweep raiden_enter_sweep
#define enter_throw raiden_enter_throw
#define enter_uppercut raiden_enter_uppercut
#define enter_victory raiden_enter_victory
#define first_fatality raiden_first_fatality
#define ground_attack raiden_ground_attack
#define ground_native_next raiden_ground_native_next
#define ground_native_tail raiden_ground_native_tail
#define hp_script raiden_hp_script
#define lk_victim raiden_lk_victim
#define press_air_hk raiden_press_air_hk
#define press_air_hp raiden_press_air_hp
#define press_air_lk raiden_press_air_lk
#define press_air_lp raiden_press_air_lp
#define press_combo_hk raiden_press_combo_hk
#define press_combo_hp raiden_press_combo_hp
#define press_crouch_hk raiden_press_crouch_hk
#define press_crouch_lk raiden_press_crouch_lk
#define press_crouch_lp raiden_press_crouch_lp
#define press_jab raiden_press_jab
#define press_kick raiden_press_kick
#define press_knee_hp raiden_press_knee_hp
#define press_lk raiden_press_lk
#define press_lp raiden_press_lp
#define press_uppercut raiden_press_uppercut
#define s_hp raiden_s_hp
#define s_jab raiden_s_jab
#define s_kick raiden_s_kick
#define s_knockdown raiden_s_knockdown
#define s_sweep raiden_s_sweep
#define s_throw raiden_s_throw
#define s_uppercut raiden_s_uppercut
#define s_victory raiden_s_victory
#define scene_end_watch raiden_scene_end_watch
#define script_air_diagonal_tail raiden_script_air_diagonal_tail
#define script_air_hp_tail raiden_script_air_hp_tail
#define script_air_kick_tail raiden_script_air_kick_tail
#define script_air_land raiden_script_air_land
#define script_air_lk_tail raiden_script_air_lk_tail
#define script_air_lp_tail raiden_script_air_lp_tail
#define script_air_release raiden_script_air_release
#define script_close_hp_tail raiden_script_close_hp_tail
#define script_combo_hp_tail raiden_script_combo_hp_tail
#define script_crouch_lk_tail raiden_script_crouch_lk_tail
#define script_draw_equip raiden_script_draw_equip
#define script_first_end raiden_script_first_end
#define script_first_release raiden_script_first_release
#define script_knee_tail raiden_script_knee_tail
#define script_lightning_spawn raiden_script_lightning_spawn
#define script_lk_victim_end raiden_script_lk_victim_end
#define script_lp_end raiden_script_lp_end
#define script_lp_return raiden_script_lp_return
#define script_lp_select raiden_script_lp_select
#define script_reaction raiden_script_reaction
#define script_roundhouse_start raiden_script_roundhouse_start
#define script_roundhouse_tail raiden_script_roundhouse_tail
#define script_second_blend raiden_script_second_blend
#define script_second_effect raiden_script_second_effect
#define script_second_end raiden_script_second_end
#define script_second_particles raiden_script_second_particles
#define script_second_release raiden_script_second_release
#define script_second_victim_end raiden_script_second_victim_end
#define script_spike_tail raiden_script_spike_tail
#define script_staff_victim_tail raiden_script_staff_victim_tail
#define script_sweep_tail raiden_script_sweep_tail
#define script_throw_getup raiden_script_throw_getup
#define script_thrown_return raiden_script_thrown_return
#define script_torpedo_contact raiden_script_torpedo_contact
#define script_torpedo_miss raiden_script_torpedo_miss
#define script_torpedo_motion raiden_script_torpedo_motion
#define script_torpedo_query raiden_script_torpedo_query
#define script_torpedo_step raiden_script_torpedo_step
#define script_torpedo_victim_motion raiden_script_torpedo_victim_motion
#define script_victory raiden_script_victory
#define script_victory_effect raiden_script_victory_effect
#define script_weapon_victim_getup raiden_script_weapon_victim_getup
#define second_fatality raiden_second_fatality
#define special_cleanup raiden_special_cleanup
#define spike_start raiden_spike_start
#define uppercut_script raiden_uppercut_script

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

void raiden_air_hp(Fighter *fighter);
void raiden_arena_fatality(Fighter *fighter, int id);
void raiden_armed_block(Fighter *fighter);
void raiden_armed_crouch(Fighter *fighter);
void raiden_armed_idle(Fighter *fighter);
void raiden_armed_walk(Fighter *fighter);
bool raiden_can_ground_throw(const Fighter *fighter, bool low_kick);
bool raiden_can_teleport(const Fighter *fighter);
void raiden_cpu_air_hk(Fighter *fighter);
void raiden_cpu_air_lk(Fighter *fighter);
void raiden_cpu_close_hk(Fighter *fighter);
void raiden_cpu_close_hp(Fighter *fighter);
void raiden_enter_lk_throw(Fighter *fighter);
void raiden_enter_sweep(Fighter *fighter);
void raiden_enter_throw(Fighter *fighter);
void raiden_enter_uppercut(Fighter *fighter);
void raiden_enter_victory(Fighter *fighter);
void raiden_first_fatality(Fighter *fighter);
void raiden_ground_attack(Fighter *fighter, int index);
void raiden_ground_native_next(Fighter *fighter);
void raiden_ground_native_tail(Fighter *fighter);
void raiden_hp_script(Fighter *fighter);
void raiden_lk_victim(Fighter *victim);
void raiden_press_air_hk(Fighter *fighter);
void raiden_press_air_hp(Fighter *fighter);
void raiden_press_air_lk(Fighter *fighter);
void raiden_press_air_lp(Fighter *fighter);
void raiden_press_combo_hk(Fighter *fighter);
void raiden_press_combo_hp(Fighter *fighter);
void raiden_press_crouch_hk(Fighter *fighter);
void raiden_press_crouch_lk(Fighter *fighter);
void raiden_press_crouch_lp(Fighter *fighter);
void raiden_press_jab(Fighter *fighter);
void raiden_press_kick(Fighter *fighter);
void raiden_press_knee_hp(Fighter *fighter);
void raiden_press_lk(Fighter *fighter);
void raiden_press_lp(Fighter *fighter);
void raiden_press_uppercut(Fighter *fighter);
void raiden_crouch_lp(Fighter *fighter);
void raiden_hk(Fighter *fighter);
void raiden_hp(Fighter *fighter);
void raiden_jab(Fighter *fighter);
void raiden_kick(Fighter *fighter);
void raiden_lightning(Fighter *fighter);
void raiden_lk(Fighter *fighter);
void raiden_roundhouse(Fighter *fighter);
void raiden_spike_fatality(Fighter *fighter);
void raiden_sweep(Fighter *fighter);
void raiden_teleport(Fighter *fighter);
void raiden_torpedo(Fighter *fighter);
void raiden_uppercut(Fighter *fighter);
void raiden_weapon(Fighter *fighter);
void raiden_entry_retail_script_004de218(Fighter *fighter);
void raiden_entry_retail_script_004de640(Fighter *fighter);
void raiden_entry_retail_script_004de668(Fighter *fighter);
void raiden_entry_retail_script_004de6b8(Fighter *fighter);
void raiden_entry_retail_script_004de798(Fighter *fighter);
void raiden_entry_retail_script_004e0210(Fighter *fighter);
void raiden_entry_retail_script_004e0220(Fighter *fighter);
void raiden_entry_retail_script_004e56c8(Fighter *fighter);
void raiden_entry_retail_script_004e56d8(Fighter *fighter);
void raiden_entry_retail_script_004e56f0(Fighter *fighter);
void raiden_entry_retail_script_004e5740(Fighter *fighter);
void raiden_entry_retail_script_004e5760(Fighter *fighter);
void raiden_entry_retail_script_004e5770(Fighter *fighter);
void raiden_entry_retail_script_004e5788(Fighter *fighter);
void raiden_entry_retail_script_004e5ca0(Fighter *fighter);
void raiden_entry_retail_script_004e5d90(Fighter *fighter);
void raiden_entry_retail_script_004e5e28(Fighter *fighter);
void raiden_entry_retail_script_004e5e50(Fighter *fighter);
void raiden_entry_retail_script_004e6020(Fighter *fighter);
void raiden_entry_retail_script_004e6158(Fighter *fighter);
void raiden_entry_retail_script_004e6190(Fighter *fighter);
void raiden_entry_retail_script_004e61e0(Fighter *fighter);
void raiden_entry_retail_script_004e61f8(Fighter *fighter);
void raiden_entry_retail_script_004e62a0(Fighter *fighter);
void raiden_entry_retail_script_004e62c0(Fighter *fighter);
void raiden_entry_retail_script_004e62d8(Fighter *fighter);
void raiden_entry_retail_script_004e62f0(Fighter *fighter);
void raiden_entry_retail_script_004e63d8(Fighter *fighter);
void raiden_entry_retail_script_004e6500(Fighter *fighter);
void raiden_entry_retail_script_004e6658(Fighter *fighter);
void raiden_entry_retail_script_004e66d0(Fighter *fighter);
void raiden_entry_retail_script_004e66f0(Fighter *fighter);
void raiden_entry_retail_script_004e6710(Fighter *fighter);
void raiden_entry_retail_script_004e6798(Fighter *fighter);
void raiden_entry_retail_script_004e6800(Fighter *fighter);
void raiden_entry_retail_script_004e6890(Fighter *fighter);
void raiden_entry_retail_script_004e68a8(Fighter *fighter);
void raiden_entry_retail_script_004e6950(Fighter *fighter);
void raiden_entry_retail_script_004e6998(Fighter *fighter);
void raiden_entry_retail_script_004e69b0(Fighter *fighter);
void raiden_entry_retail_script_004e6a40(Fighter *fighter);
void raiden_entry_retail_script_004e8440(Fighter *fighter);
void raiden_entry_retail_script_004e8458(Fighter *fighter);
void raiden_entry_retail_script_004e8e18(Fighter *fighter);
void raiden_entry_retail_script_004e9910(Fighter *fighter);
void raiden_entry_retail_script_004e99c0(Fighter *fighter);
void raiden_entry_retail_script_004e9ad0(Fighter *fighter);
void raiden_entry_retail_script_004e9ae8(Fighter *fighter);
void raiden_entry_retail_script_004e9b40(Fighter *fighter);
void raiden_entry_retail_script_004e9bd0(Fighter *fighter);
void raiden_entry_retail_script_004e9d50(Fighter *fighter);
void raiden_entry_retail_script_004ea068(Fighter *fighter);
void raiden_entry_retail_script_004ea108(Fighter *fighter);
void raiden_entry_retail_script_004ed5f0(Fighter *fighter);
extern const MoveDef raiden_s_hp;
extern const MoveDef raiden_s_jab;
extern const MoveDef raiden_s_kick;
extern const MoveDef raiden_s_knockdown;
extern const MoveDef raiden_s_sweep;
extern const MoveDef raiden_s_throw;
extern const MoveDef raiden_s_uppercut;
extern const MoveDef raiden_s_victory;
void raiden_scene_end_watch(Fighter *fighter);
void raiden_script_air_diagonal_tail(Fighter *fighter);
void raiden_script_air_hp_tail(Fighter *fighter);
void raiden_script_air_kick_tail(Fighter *fighter);
void raiden_script_air_land(Fighter *fighter);
void raiden_script_air_lk_tail(Fighter *fighter);
void raiden_script_air_lp_tail(Fighter *fighter);
void raiden_script_air_release(Fighter *fighter);
void raiden_script_close_hp_tail(Fighter *fighter);
void raiden_script_combo_hp_tail(Fighter *fighter);
void raiden_script_crouch_lk_tail(Fighter *fighter);
void raiden_script_draw_equip(Fighter *fighter);
void raiden_script_first_end(Fighter *fighter);
void raiden_script_first_release(Fighter *fighter);
void raiden_script_knee_tail(Fighter *fighter);
void raiden_script_lightning_spawn(Fighter *fighter, int task_type);
void raiden_script_lk_victim_end(Fighter *victim);
void raiden_script_lp_end(Fighter *fighter);
void raiden_script_lp_return(Fighter *fighter);
void raiden_script_lp_select(Fighter *fighter);
void raiden_script_reaction(Fighter *fighter, int reaction);
void raiden_script_roundhouse_start(Fighter *fighter);
void raiden_script_roundhouse_tail(Fighter *fighter);
void raiden_script_second_blend(Fighter *fighter);
void raiden_script_second_effect(Fighter *fighter);
void raiden_script_second_end(Fighter *fighter);
void raiden_script_second_particles(Fighter *fighter);
void raiden_script_second_release(Fighter *fighter);
void raiden_script_second_victim_end(Fighter *fighter);
void raiden_script_spike_tail(Fighter *fighter);
void raiden_script_staff_victim_tail(Fighter *victim);
void raiden_script_sweep_tail(Fighter *fighter);
void raiden_script_throw_getup(Fighter *victim);
void raiden_script_thrown_return(Fighter *fighter);
void raiden_script_torpedo_contact(Fighter *fighter);
void raiden_script_torpedo_miss(Fighter *fighter);
void raiden_script_torpedo_motion(Fighter *fighter);
unsigned int raiden_script_torpedo_query(Fighter *fighter);
void raiden_script_torpedo_step(Fighter *fighter);
void raiden_script_torpedo_victim_motion(Fighter *victim);
void raiden_script_victory(Fighter *fighter);
void raiden_script_victory_effect(Fighter *fighter);
void raiden_script_weapon_victim_getup(Fighter *victim);
void raiden_second_fatality(Fighter *fighter);
void raiden_special_cleanup(Fighter *fighter);
void raiden_spike_start(Fighter *fighter);
void raiden_uppercut_script(Fighter *fighter);

#endif

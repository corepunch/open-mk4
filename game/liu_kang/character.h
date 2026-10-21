#include "retail_native.h"
#ifndef MK4_LIU_KANG_PRIVATE_H
#define MK4_LIU_KANG_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define bicycle_script liu_kang_bicycle_script
#define can_bicycle liu_kang_can_bicycle
#define can_close_fatality retail_can_finish_close
#define can_first_fatality liu_kang_can_first_fatality
#define can_ground_throw liu_kang_can_ground_throw
#define cpu_air_hk liu_kang_cpu_air_hk
#define cpu_air_hp liu_kang_cpu_air_hp
#define cpu_air_lk liu_kang_cpu_air_lk
#define cpu_close_hk liu_kang_cpu_close_hk
#define cpu_close_hp liu_kang_cpu_close_hp
#define enter_lk_throw liu_kang_enter_lk_throw
#define enter_throw liu_kang_enter_throw
#define fatal_pose liu_kang_fatal_pose
#define finish_scene liu_kang_finish_scene
#define fireball_mesh liu_kang_fireball_mesh
#define fireball_spawn liu_kang_fireball_spawn
#define flying_kick_script liu_kang_flying_kick_script
#define ground_attack liu_kang_ground_attack
#define ground_native_next liu_kang_ground_native_next
#define ground_native_tail liu_kang_ground_native_tail
#define liu_script_air_diagonal_tail liu_kang_liu_script_air_diagonal_tail
#define liu_script_air_hp_tail liu_kang_liu_script_air_hp_tail
#define liu_script_air_kick_tail liu_kang_liu_script_air_kick_tail
#define liu_script_air_lk_tail liu_kang_liu_script_air_lk_tail
#define liu_script_air_lp_tail liu_kang_liu_script_air_lp_tail
#define liu_script_air_return liu_kang_liu_script_air_return
#define liu_script_back_lp_tail liu_kang_liu_script_back_lp_tail
#define liu_script_bicycle_getup liu_kang_liu_script_bicycle_getup
#define liu_script_bicycle_motion liu_kang_liu_script_bicycle_motion
#define liu_script_close_hp_tail liu_kang_liu_script_close_hp_tail
#define liu_script_combo_hp_tail liu_kang_liu_script_combo_hp_tail
#define liu_script_crouch_lk_tail liu_kang_liu_script_crouch_lk_tail
#define liu_script_draw_equip liu_kang_liu_script_draw_equip
#define liu_script_fatal_align liu_kang_liu_script_fatal_align
#define liu_script_fatal_setup liu_kang_liu_script_fatal_setup
#define liu_script_fatal_spawn liu_kang_liu_script_fatal_spawn
#define liu_script_knee_tail liu_kang_liu_script_knee_tail
#define liu_script_lk_continue liu_kang_liu_script_lk_continue
#define liu_script_lk_damage liu_kang_liu_script_lk_damage
#define liu_script_lk_thrown_tail liu_kang_liu_script_lk_thrown_tail
#define liu_script_land liu_kang_liu_script_land
#define liu_script_lp_getup liu_kang_liu_script_lp_getup
#define liu_script_reaction liu_kang_liu_script_reaction
#define liu_script_scene_event liu_kang_liu_script_scene_event
#define liu_script_spawn_air liu_kang_liu_script_spawn_air
#define liu_script_spawn_high liu_kang_liu_script_spawn_high
#define liu_script_spawn_low liu_kang_liu_script_spawn_low
#define liu_script_spike_tail liu_kang_liu_script_spike_tail
#define liu_script_sword_blend liu_kang_liu_script_sword_blend
#define liu_script_sword_gate liu_kang_liu_script_sword_gate
#define lk_thrown_script liu_kang_lk_thrown_script
#define press_air_hk liu_kang_press_air_hk
#define press_air_hp liu_kang_press_air_hp
#define press_air_lk liu_kang_press_air_lk
#define press_air_lp liu_kang_press_air_lp
#define press_combo_hk liu_kang_press_combo_hk
#define press_combo_hp liu_kang_press_combo_hp
#define press_crouch_hk liu_kang_press_crouch_hk
#define press_crouch_lk liu_kang_press_crouch_lk
#define press_crouch_lp liu_kang_press_crouch_lp
#define press_jab liu_kang_press_jab
#define press_kick liu_kang_press_kick
#define press_knee_hp liu_kang_press_knee_hp
#define press_lk liu_kang_press_lk
#define press_lp liu_kang_press_lp
#define s_jab liu_kang_s_jab
#define s_kick liu_kang_s_kick
#define s_sweep liu_kang_s_sweep
#define s_throw liu_kang_s_throw
#define s_uppercut liu_kang_s_uppercut
#define start_fatality liu_kang_start_fatality
#define sword_attacks liu_kang_sword_attacks
#define throw_disarm liu_kang_throw_disarm
#define thrown_finish liu_kang_thrown_finish

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
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
#define SCENE(c, a, b) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, a, b, 1, 0, 0}

void liu_kang_bicycle_script(Fighter *fighter);
bool liu_kang_can_bicycle(const Fighter *fighter);
bool liu_kang_can_first_fatality(const Fighter *fighter);
bool liu_kang_can_ground_throw(const Fighter *fighter, bool low_kick);
void liu_kang_cpu_air_hk(Fighter *fighter);
void liu_kang_cpu_air_hp(Fighter *fighter);
void liu_kang_cpu_air_lk(Fighter *fighter);
void liu_kang_cpu_close_hk(Fighter *fighter);
void liu_kang_cpu_close_hp(Fighter *fighter);
void liu_kang_enter_lk_throw(Fighter *fighter);
void liu_kang_enter_throw(Fighter *fighter);
void liu_kang_fatal_pose(Fighter *fighter, int clip, int sample, bool owner);
void liu_kang_finish_scene(Fighter *fighter);
extern const MeshAnimation liu_kang_fireball_mesh;
void liu_kang_fireball_spawn(Fighter *fighter, int type);
void liu_kang_flying_kick_script(Fighter *fighter);
void liu_kang_ground_attack(Fighter *fighter, int index);
void liu_kang_ground_native_next(Fighter *fighter);
void liu_kang_ground_native_tail(Fighter *fighter);
void liu_kang_bicycle_kick(Fighter *fighter);
void liu_kang_crouch_lp(Fighter *fighter);
void liu_kang_first_fatality(Fighter *fighter);
void liu_kang_flying_kick(Fighter *fighter);
void liu_kang_high_fireball(Fighter *fighter);
void liu_kang_hk(Fighter *fighter);
void liu_kang_hp(Fighter *fighter);
void liu_kang_low_fireball(Fighter *fighter);
void liu_kang_second_fatality(Fighter *fighter);
void liu_kang_spike_fatality(Fighter *fighter);
void liu_kang_weapon(Fighter *fighter);
void liu_kang_liu_script_air_diagonal_tail(Fighter *fighter);
void liu_kang_liu_script_air_hp_tail(Fighter *fighter);
void liu_kang_liu_script_air_kick_tail(Fighter *fighter);
void liu_kang_liu_script_air_lk_tail(Fighter *fighter);
void liu_kang_liu_script_air_lp_tail(Fighter *fighter);
void liu_kang_liu_script_air_return(Fighter *fighter);
void liu_kang_liu_script_back_lp_tail(Fighter *fighter);
void liu_kang_liu_script_bicycle_getup(Fighter *victim);
void liu_kang_liu_script_bicycle_motion(Fighter *fighter);
void liu_kang_liu_script_close_hp_tail(Fighter *fighter);
void liu_kang_liu_script_combo_hp_tail(Fighter *fighter);
void liu_kang_liu_script_crouch_lk_tail(Fighter *fighter);
void liu_kang_liu_script_draw_equip(Fighter *fighter);
void liu_kang_liu_script_fatal_align(Fighter *fighter);
void liu_kang_liu_script_fatal_setup(Fighter *fighter);
void liu_kang_liu_script_fatal_spawn(Fighter *fighter, int task_type);
void liu_kang_liu_script_knee_tail(Fighter *fighter);
void liu_kang_liu_script_lk_continue(Fighter *fighter);
void liu_kang_liu_script_lk_damage(Fighter *fighter);
void liu_kang_liu_script_lk_thrown_tail(Fighter *victim);
void liu_kang_liu_script_land(Fighter *fighter);
void liu_kang_liu_script_lp_getup(Fighter *victim);
void liu_kang_liu_script_reaction(Fighter *fighter, int reaction);
void liu_kang_liu_script_scene_event(Fighter *fighter, int selector);
void liu_kang_liu_script_spawn_air(Fighter *fighter, int task_type);
void liu_kang_liu_script_spawn_high(Fighter *fighter, int task_type);
void liu_kang_liu_script_spawn_low(Fighter *fighter, int task_type);
void liu_kang_liu_script_spike_tail(Fighter *fighter);
void liu_kang_liu_script_sword_blend(Fighter *fighter);
unsigned int liu_kang_liu_script_sword_gate(Fighter *fighter);
void liu_kang_lk_thrown_script(Fighter *victim);
void liu_kang_press_air_hk(Fighter *fighter);
void liu_kang_press_air_hp(Fighter *fighter);
void liu_kang_press_air_lk(Fighter *fighter);
void liu_kang_press_air_lp(Fighter *fighter);
void liu_kang_press_combo_hk(Fighter *fighter);
void liu_kang_press_combo_hp(Fighter *fighter);
void liu_kang_press_crouch_hk(Fighter *fighter);
void liu_kang_press_crouch_lk(Fighter *fighter);
void liu_kang_press_crouch_lp(Fighter *fighter);
void liu_kang_press_jab(Fighter *fighter);
void liu_kang_press_kick(Fighter *fighter);
void liu_kang_press_knee_hp(Fighter *fighter);
void liu_kang_press_lk(Fighter *fighter);
void liu_kang_press_lp(Fighter *fighter);
void liu_kang_entry_retail_script_004de3b0(Fighter *fighter);
void liu_kang_entry_retail_script_004de410(Fighter *fighter);
void liu_kang_entry_retail_script_004de428(Fighter *fighter);
void liu_kang_entry_retail_script_004de468(Fighter *fighter);
void liu_kang_entry_retail_script_004de798(Fighter *fighter);
void liu_kang_entry_retail_script_004dfff8(Fighter *fighter);
void liu_kang_entry_retail_script_004e0020(Fighter *fighter);
void liu_kang_entry_retail_script_004e60b8(Fighter *fighter);
void liu_kang_entry_retail_script_004e6158(Fighter *fighter);
void liu_kang_entry_retail_script_004e6190(Fighter *fighter);
void liu_kang_entry_retail_script_004e61e0(Fighter *fighter);
void liu_kang_entry_retail_script_004e62c0(Fighter *fighter);
void liu_kang_entry_retail_script_004e62d8(Fighter *fighter);
void liu_kang_entry_retail_script_004e63d8(Fighter *fighter);
void liu_kang_entry_retail_script_004e6500(Fighter *fighter);
void liu_kang_entry_retail_script_004e6658(Fighter *fighter);
void liu_kang_entry_retail_script_004e66d0(Fighter *fighter);
void liu_kang_entry_retail_script_004e66f0(Fighter *fighter);
void liu_kang_entry_retail_script_004e6710(Fighter *fighter);
void liu_kang_entry_retail_script_004e6950(Fighter *fighter);
void liu_kang_entry_retail_script_004e69b0(Fighter *fighter);
void liu_kang_entry_retail_script_004e6d08(Fighter *fighter);
void liu_kang_entry_retail_script_004e6df0(Fighter *fighter);
void liu_kang_entry_retail_script_004e8610(Fighter *fighter);
void liu_kang_entry_retail_script_004e8678(Fighter *fighter);
void liu_kang_entry_retail_script_004e8688(Fighter *fighter);
void liu_kang_entry_retail_script_004e86a8(Fighter *fighter);
void liu_kang_entry_retail_script_004e86d0(Fighter *fighter);
void liu_kang_entry_retail_script_004e86e8(Fighter *fighter);
void liu_kang_entry_retail_script_004e8718(Fighter *fighter);
void liu_kang_entry_retail_script_004e9a90(Fighter *fighter);
void liu_kang_entry_retail_script_004e9ad0(Fighter *fighter);
void liu_kang_entry_retail_script_004e9ae8(Fighter *fighter);
void liu_kang_entry_retail_script_004e9b68(Fighter *fighter);
void liu_kang_entry_retail_script_004ec680(Fighter *fighter);
void liu_kang_entry_retail_script_004ec7f0(Fighter *fighter);
void liu_kang_entry_retail_script_004ed678(Fighter *fighter);
void liu_kang_entry_retail_script_004ed6c0(Fighter *fighter);
void liu_kang_entry_retail_script_004ed6f8(Fighter *fighter);
extern const MoveDef liu_kang_s_jab;
extern const MoveDef liu_kang_s_kick;
extern const MoveDef liu_kang_s_sweep;
extern const MoveDef liu_kang_s_throw;
extern const MoveDef liu_kang_s_uppercut;
void liu_kang_start_fatality(Fighter *fighter, int id);
extern const WeaponStrike liu_kang_sword_attacks[4];
void liu_kang_throw_disarm(Fighter *victim);
void liu_kang_thrown_finish(Fighter *victim);

void liu_kang_entry_retail_script_004e9de8(Fighter *fighter);
#endif

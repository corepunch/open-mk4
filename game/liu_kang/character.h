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
#define cpu_air_hk liu_kang_cpu_air_hk
#define cpu_air_hp liu_kang_cpu_air_hp
#define cpu_air_lk liu_kang_cpu_air_lk
#define cpu_close_hk liu_kang_cpu_close_hk
#define cpu_close_hp liu_kang_cpu_close_hp
#define enter_lk_throw retail_lk_throw
#define enter_throw retail_lp_throw
#define finish_scene liu_kang_finish_scene
#define fireball_mesh liu_kang_fireball_mesh
#define fireball_spawn liu_kang_fireball_spawn
#define flying_kick_script liu_kang_flying_kick_script
#define liu_script_air_return liu_kang_liu_script_air_return
#define liu_script_back_lp_tail liu_kang_liu_script_back_lp_tail
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
#define liu_script_sword_blend liu_kang_liu_script_sword_blend
#define liu_script_sword_gate liu_kang_liu_script_sword_gate
#define lk_thrown_script liu_kang_lk_thrown_script
#define press_combo_hk liu_kang_press_combo_hk
#define press_combo_hp liu_kang_press_combo_hp
#define press_knee_hp liu_kang_press_knee_hp
#define start_fatality liu_kang_start_fatality
#define sword_attacks liu_kang_sword_attacks
#define throw_disarm liu_kang_throw_disarm
#define thrown_finish liu_kang_thrown_finish

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
#define SCENE(c, a, b) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, a, b, 1, 0, 0}

void liu_kang_bicycle_script(Fighter *fighter);
bool liu_kang_can_bicycle(const Fighter *fighter);
bool liu_kang_can_first_fatality(const Fighter *fighter);
void liu_kang_cpu_close_hk(Fighter *fighter);
void liu_kang_cpu_close_hp(Fighter *fighter);
void liu_kang_finish_scene(Fighter *fighter);
extern const MeshAnimation liu_kang_fireball_mesh;
void liu_kang_fireball_spawn(Fighter *fighter, int type);
void liu_kang_flying_kick_script(Fighter *fighter);
void liu_kang_bicycle_kick(Fighter *fighter);
void liu_kang_first_fatality(Fighter *fighter);
void liu_kang_flying_kick(Fighter *fighter);
void liu_kang_high_fireball(Fighter *fighter);
void liu_kang_hk(Fighter *fighter);
void liu_kang_low_fireball(Fighter *fighter);
void liu_kang_second_fatality(Fighter *fighter);
void liu_kang_weapon(Fighter *fighter);
void liu_kang_liu_script_air_return(Fighter *fighter);
void liu_kang_liu_script_back_lp_tail(Fighter *fighter);
void liu_kang_liu_script_bicycle_motion(Fighter *fighter);
void liu_kang_liu_script_draw_equip(Fighter *fighter);
void liu_kang_liu_script_fatal_align(Fighter *fighter);
void liu_kang_liu_script_fatal_setup(Fighter *fighter);
void liu_kang_liu_script_fatal_spawn(Fighter *fighter, int task_type);
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
void liu_kang_liu_script_sword_blend(Fighter *fighter);
unsigned int liu_kang_liu_script_sword_gate(Fighter *fighter);
void liu_kang_lk_thrown_script(Fighter *victim);
void liu_kang_entry_retail_script_004de410(Fighter *fighter);
void liu_kang_entry_retail_script_004de428(Fighter *fighter);
void liu_kang_entry_retail_script_004de468(Fighter *fighter);
void liu_kang_entry_retail_script_004dfff8(Fighter *fighter);
void liu_kang_entry_retail_script_004e0020(Fighter *fighter);
void liu_kang_entry_retail_script_004e62c0(Fighter *fighter);
void liu_kang_entry_retail_script_004e62d8(Fighter *fighter);
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
void liu_kang_start_fatality(Fighter *fighter, int id);
extern const WeaponStrike liu_kang_sword_attacks[4];
void liu_kang_throw_disarm(Fighter *victim);
void liu_kang_thrown_finish(Fighter *victim);

void liu_kang_entry_retail_script_004e9de8(Fighter *fighter);
#endif

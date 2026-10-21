#include "retail_native.h"
#ifndef MK4_SUB_ZERO_PRIVATE_H
#define MK4_SUB_ZERO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define air_land sub_zero_air_land
#define breath_travel sub_zero_breath_travel
#define can_clone sub_zero_can_clone
#define can_close_fatality sub_zero_can_close_fatality
#define can_first_fatality sub_zero_can_first_fatality
#define can_ice sub_zero_can_ice
#define can_second_fatality retail_can_finish_range
#define can_slide sub_zero_can_slide
#define cpu_close_hk sub_zero_cpu_close_hk
#define cpu_close_hp sub_zero_cpu_close_hp
#define enter_lk_throw retail_lk_throw
#define enter_throw retail_lp_throw
#define finish_scene sub_zero_finish_scene
#define first_fatality_script sub_zero_first_fatality_script
#define ice_projectile sub_zero_ice_projectile
#define lk_thrown_script sub_zero_lk_thrown_script
#define lp_thrown_cleanup sub_zero_lp_thrown_cleanup
#define native_blends sub_zero_native_blends
#define owned_object sub_zero_owned_object
#define press_combo_hk sub_zero_press_combo_hk
#define press_combo_hp sub_zero_press_combo_hp
#define press_knee_hp sub_zero_press_knee_hp
#define s_lp_thrown sub_zero_s_lp_thrown
#define scene_blend_cleanup sub_zero_scene_blend_cleanup
#define script_breath_approach sub_zero_script_breath_approach
#define script_breath_freeze sub_zero_script_breath_freeze
#define script_breath_object sub_zero_script_breath_object
#define script_breath_shatter sub_zero_script_breath_shatter
#define script_breath_slide sub_zero_script_breath_slide
#define script_breath_victim_end sub_zero_script_breath_victim_end
#define script_close_hp_tail sub_zero_script_close_hp_tail
#define script_combo_hp_tail sub_zero_script_combo_hp_tail
#define script_crouch_lk_tail sub_zero_script_crouch_lk_tail
#define script_draw_equip sub_zero_script_draw_equip
#define script_first_begin sub_zero_script_first_begin
#define script_first_cues1 sub_zero_script_first_cues1
#define script_first_cues2 sub_zero_script_first_cues2
#define script_first_cues3 sub_zero_script_first_cues3
#define script_first_cues4 sub_zero_script_first_cues4
#define script_first_head sub_zero_script_first_head
#define script_first_pull sub_zero_script_first_pull
#define script_first_victim_blends sub_zero_script_first_victim_blends
#define script_first_victim_collapse sub_zero_script_first_victim_collapse
#define script_first_victim_end sub_zero_script_first_victim_end
#define script_lk_damage sub_zero_script_lk_damage
#define script_lk_owner_return sub_zero_script_lk_owner_return
#define script_lk_rebase sub_zero_script_lk_rebase
#define script_lk_victim_rise sub_zero_script_lk_victim_rise
#define script_lp_getup sub_zero_script_lp_getup
#define script_lp_owner_return sub_zero_script_lp_owner_return
#define script_reaction sub_zero_script_reaction
#define script_ready sub_zero_script_ready
#define script_slide_return sub_zero_script_slide_return
#define script_spawn_ice sub_zero_script_spawn_ice
#define script_thrown_return sub_zero_script_thrown_return
#define script_victory sub_zero_script_victory
#define second_fatality_script sub_zero_second_fatality_script
#define slide_script sub_zero_slide_script
#define start_fatality sub_zero_start_fatality
#define throw_disarm sub_zero_throw_disarm

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

enum {
  LP_THROW_RELEASE = 25,
  LK_THROW_DISARM = 39, /* Opening 10 + paired 29. */
  LK_THROW_VOICE = 44,  /* Another paired 5. */
  LK_THROW_DAMAGE = 52, /* Another paired 8. */
  LK_THROW_RELEASE = 57 /* Final paired 5 before the independent victim tail. */
};

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
#define BLEND(c, sample, step, first)                                                                                                                          \
  {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, sample, sample, 1, step, first}

void sub_zero_breath_travel(Fighter *fighter);
bool sub_zero_can_clone(const Fighter *fighter);
bool sub_zero_can_close_fatality(const Fighter *fighter);
bool sub_zero_can_first_fatality(const Fighter *fighter);
bool sub_zero_can_ice(const Fighter *fighter);
bool sub_zero_can_slide(const Fighter *fighter);
void sub_zero_cpu_close_hk(Fighter *fighter);
void sub_zero_cpu_close_hp(Fighter *fighter);
void sub_zero_finish_scene(Fighter *fighter);
void sub_zero_first_fatality_script(Fighter *fighter);
void sub_zero_ice_projectile(Projectile *p);
void sub_zero_lk_thrown_script(Fighter *victim);
void sub_zero_lp_thrown_cleanup(Fighter *victim);
void sub_zero_native_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end);
bool sub_zero_owned_object(const Fighter *fighter, int kind);
void sub_zero_entry_retail_script_004d2bf0(Fighter *fighter);
void sub_zero_entry_retail_script_004d2c10(Fighter *fighter);
void sub_zero_entry_retail_script_004d2c78(Fighter *fighter);
void sub_zero_entry_retail_script_004d2ca0(Fighter *fighter);
void sub_zero_entry_retail_script_004d2cc8(Fighter *fighter);
void sub_zero_entry_retail_script_004de4b8(Fighter *fighter);
void sub_zero_entry_retail_script_004de4e0(Fighter *fighter);
void sub_zero_entry_retail_script_004e0368(Fighter *fighter);
void sub_zero_entry_retail_script_004e0378(Fighter *fighter);
void sub_zero_entry_retail_script_004e0388(Fighter *fighter);
void sub_zero_entry_retail_script_004e0398(Fighter *fighter);
void sub_zero_entry_retail_script_004e03a8(Fighter *fighter);
void sub_zero_entry_retail_script_004e03b8(Fighter *fighter);
void sub_zero_entry_retail_script_004e03c8(Fighter *fighter);
void sub_zero_entry_retail_script_004e03d8(Fighter *fighter);
void sub_zero_entry_retail_script_004e03f0(Fighter *fighter);
void sub_zero_entry_retail_script_004e0420(Fighter *fighter);
void sub_zero_entry_retail_script_004e0430(Fighter *fighter);
void sub_zero_entry_retail_script_004e62c0(Fighter *fighter);
void sub_zero_entry_retail_script_004e62d8(Fighter *fighter);
void sub_zero_entry_retail_script_004e6a40(Fighter *fighter);
void sub_zero_entry_retail_script_004e84d8(Fighter *fighter);
void sub_zero_entry_retail_script_004e84f8(Fighter *fighter);
void sub_zero_entry_retail_script_004e8530(Fighter *fighter);
void sub_zero_entry_retail_script_004e9bd0(Fighter *fighter);
void sub_zero_entry_retail_script_004e9ee0(Fighter *fighter);
void sub_zero_entry_retail_script_004ed588(Fighter *fighter);
extern const MoveDef sub_zero_s_lp_thrown;
void sub_zero_scene_blend_cleanup(Fighter *fighter);
void sub_zero_script_breath_approach(Fighter *fighter);
void sub_zero_script_breath_freeze(Fighter *fighter, int type);
void sub_zero_script_breath_object(Fighter *fighter, int type);
void sub_zero_script_breath_shatter(Fighter *fighter);
void sub_zero_script_breath_slide(Fighter *fighter);
void sub_zero_script_breath_victim_end(Fighter *victim);
void sub_zero_script_draw_equip(Fighter *fighter);
void sub_zero_script_first_begin(Fighter *fighter);
void sub_zero_script_first_cues1(Fighter *fighter);
void sub_zero_script_first_cues2(Fighter *fighter);
void sub_zero_script_first_cues3(Fighter *fighter);
void sub_zero_script_first_cues4(Fighter *fighter);
void sub_zero_script_first_head(Fighter *fighter);
void sub_zero_script_first_pull(Fighter *fighter);
void sub_zero_script_first_victim_blends(Fighter *victim);
void sub_zero_script_first_victim_collapse(Fighter *victim);
void sub_zero_script_first_victim_end(Fighter *victim);
void sub_zero_script_lk_damage(Fighter *fighter);
void sub_zero_script_lk_owner_return(Fighter *fighter);
void sub_zero_script_lk_rebase(Fighter *fighter);
void sub_zero_script_lk_victim_rise(Fighter *victim);
void sub_zero_script_lp_getup(Fighter *victim);
void sub_zero_script_lp_owner_return(Fighter *fighter);
void sub_zero_script_reaction(Fighter *fighter, int reaction);
void sub_zero_script_ready(Fighter *fighter);
void sub_zero_script_slide_return(Fighter *fighter);
void sub_zero_script_spawn_ice(Fighter *fighter, int task_type);
void sub_zero_script_thrown_return(Fighter *victim);
void sub_zero_script_victory(Fighter *fighter);
void sub_zero_second_fatality_script(Fighter *fighter);
void sub_zero_slide_script(Fighter *fighter);
void sub_zero_start_fatality(Fighter *fighter, int id);
void sub_zero_clone(Fighter *fighter);
void sub_zero_first_fatality(Fighter *fighter);
void sub_zero_hk(Fighter *fighter);
void sub_zero_ice(Fighter *fighter);
void sub_zero_second_fatality(Fighter *fighter);
void sub_zero_slide(Fighter *fighter);
void sub_zero_weapon(Fighter *fighter);
void sub_zero_throw_disarm(Fighter *victim);

#endif

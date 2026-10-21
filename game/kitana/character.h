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
#define cpu_air_hk kitana_cpu_air_hk
#define cpu_air_lk kitana_cpu_air_lk
#define cpu_close_hk kitana_cpu_close_hk
#define cpu_close_hp kitana_cpu_close_hp
#define enter_lk_throw retail_lk_throw
#define enter_throw retail_lp_throw
#define enter_victory kitana_enter_victory
#define lk_victim kitana_lk_victim
#define press_combo_hk kitana_press_combo_hk
#define press_combo_hp kitana_press_combo_hp
#define press_knee_hp kitana_press_knee_hp
#define scene_end_watch kitana_scene_end_watch
#define script_cartwheel_tail kitana_script_cartwheel_tail
#define script_close_hp_tail kitana_script_close_hp_tail
#define script_combo_hp_tail kitana_script_combo_hp_tail
#define script_crouch_lk_tail kitana_script_crouch_lk_tail
#define script_draw_equip kitana_script_draw_equip
#define script_first_release kitana_script_first_release
#define script_leg_grab_tail kitana_script_leg_grab_tail
#define script_leg_victim_end kitana_script_leg_victim_end
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
#define script_throw_getup retail_getup_roll
#define script_weapon_hp_tail kitana_script_weapon_hp_tail
#define script_weapon_lp_tail kitana_script_weapon_lp_tail
#define script_weapon_victim_getup kitana_script_weapon_victim_getup
#define special_cleanup kitana_special_cleanup
#define square_victim kitana_square_victim

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

void kitana_bicycle(Fighter *fighter);
void kitana_cartwheel(Fighter *fighter);
void kitana_command_shadow_low(Fighter *fighter);
void kitana_command_shadow_up(Fighter *fighter);
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
void kitana_scene_end_watch(Fighter *fighter);
void kitana_script_cartwheel_tail(Fighter *fighter);
void kitana_script_draw_equip(Fighter *fighter);
void kitana_script_first_release(Fighter *fighter);
void kitana_script_leg_grab_tail(Fighter *fighter);
void kitana_script_leg_victim_end(Fighter *victim);
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
void kitana_script_weapon_hp_tail(Fighter *fighter);
void kitana_script_weapon_lp_tail(Fighter *fighter);
void kitana_script_weapon_victim_getup(Fighter *victim);
void kitana_special_cleanup(Fighter *fighter);
void kitana_square_victim(Fighter *victim);

#endif

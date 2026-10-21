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
#define can_close_fatality retail_can_finish_close
#define can_teleport raiden_can_teleport
#define cpu_air_hk raiden_cpu_air_hk
#define cpu_air_lk raiden_cpu_air_lk
#define cpu_close_hk raiden_cpu_close_hk
#define cpu_close_hp raiden_cpu_close_hp
#define enter_lk_throw retail_lk_throw
#define enter_throw raiden_enter_throw
#define enter_victory raiden_enter_victory
#define first_fatality raiden_first_fatality
#define lk_victim raiden_lk_victim
#define press_combo_hk raiden_press_combo_hk
#define press_combo_hp raiden_press_combo_hp
#define press_knee_hp raiden_press_knee_hp
#define scene_end_watch raiden_scene_end_watch
#define script_close_hp_tail raiden_script_close_hp_tail
#define script_combo_hp_tail raiden_script_combo_hp_tail
#define script_crouch_lk_tail raiden_script_crouch_lk_tail
#define script_draw_equip retail_weapon_draw_equip
#define script_first_end raiden_script_first_end
#define script_first_release raiden_script_first_release
#define script_lightning_spawn raiden_script_lightning_spawn
#define script_lp_end raiden_script_lp_end
#define script_lp_return raiden_script_lp_return
#define script_reaction raiden_script_reaction
#define script_second_blend raiden_script_second_blend
#define script_second_effect raiden_script_second_effect
#define script_second_end raiden_script_second_end
#define script_second_particles raiden_script_second_particles
#define script_second_release raiden_script_second_release
#define script_second_victim_end raiden_script_second_victim_end
#define script_staff_victim_tail raiden_script_staff_victim_tail
#define script_throw_getup retail_getup_flip
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

bool raiden_can_teleport(const Fighter *fighter);
void raiden_first_fatality(Fighter *fighter);
void raiden_lk_victim(Fighter *victim);
void raiden_lightning(Fighter *fighter);
void raiden_teleport(Fighter *fighter);
void raiden_torpedo(Fighter *fighter);
void raiden_scene_end_watch(Fighter *fighter);
void raiden_script_first_end(Fighter *fighter);
void raiden_script_first_release(Fighter *fighter);
void raiden_script_lightning_spawn(Fighter *fighter, int task_type);
void raiden_script_lp_end(Fighter *fighter);
void raiden_script_lp_return(Fighter *fighter);
void raiden_script_reaction(Fighter *fighter, int reaction);
void raiden_script_second_blend(Fighter *fighter);
void raiden_script_second_effect(Fighter *fighter);
void raiden_script_second_end(Fighter *fighter);
void raiden_script_second_particles(Fighter *fighter);
void raiden_script_second_release(Fighter *fighter);
void raiden_script_second_victim_end(Fighter *fighter);
void raiden_script_staff_victim_tail(Fighter *victim);
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

#endif

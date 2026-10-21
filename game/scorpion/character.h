#include "retail_native.h"
#ifndef MK4_SCORPION_PRIVATE_H
#define MK4_SCORPION_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define can_beast_fatality retail_can_finish_close
#define can_burn_fatality retail_can_finish_range
#define can_spear scorpion_can_spear
#define can_teleport scorpion_can_teleport
#define enter_lk_throw scorpion_enter_lk_throw
#define enter_throw scorpion_enter_throw
#define fire_script scorpion_fire_script
#define ground_attack scorpion_ground_attack
#define ground_native_next scorpion_ground_native_next
#define ground_native_tail scorpion_ground_native_tail
#define native_blend scorpion_native_blend
#define press_air_hk scorpion_press_air_hk
#define press_air_hp scorpion_press_air_hp
#define press_air_lk scorpion_press_air_lk
#define press_air_lp scorpion_press_air_lp
#define press_combo_hk scorpion_press_combo_hk
#define press_combo_hp scorpion_press_combo_hp
#define press_crouch_hk scorpion_press_crouch_hk
#define press_crouch_lk scorpion_press_crouch_lk
#define press_crouch_lp scorpion_press_crouch_lp
#define press_jab scorpion_press_jab
#define press_kick scorpion_press_kick
#define press_knee_hp scorpion_press_knee_hp
#define press_lk scorpion_press_lk
#define press_lp scorpion_press_lp
#define s_teleport_land scorpion_s_teleport_land
#define s_throw scorpion_s_throw
#define script_air_diagonal_tail scorpion_script_air_diagonal_tail
#define script_air_hp_tail scorpion_script_air_hp_tail
#define script_air_kick_tail scorpion_script_air_kick_tail
#define script_air_lk_tail scorpion_script_air_lk_tail
#define script_air_lp_tail scorpion_script_air_lp_tail
#define script_air_land scorpion_script_air_land
#define script_air_release scorpion_script_air_release
#define script_beast_attack scorpion_script_beast_attack
#define script_beast_begin scorpion_script_beast_begin
#define script_beast_gore scorpion_script_beast_gore
#define script_beast_park scorpion_script_beast_park
#define script_beast_victim_pieces scorpion_script_beast_victim_pieces
#define script_beast_voice scorpion_script_beast_voice
#define script_burn_begin scorpion_script_burn_begin
#define script_burn_repeat1 scorpion_script_burn_repeat1
#define script_burn_repeat2 scorpion_script_burn_repeat2
#define script_burn_tail scorpion_script_burn_tail
#define script_burn_victim_blend scorpion_script_burn_victim_blend
#define script_burn_victim_cue scorpion_script_burn_victim_cue
#define script_burn_victim_end scorpion_script_burn_victim_end
#define script_burn_victim_impact scorpion_script_burn_victim_impact
#define script_burn_victim_repeat scorpion_script_burn_victim_repeat
#define script_burn_victim_settle scorpion_script_burn_victim_settle
#define script_burn_victim_start scorpion_script_burn_victim_start
#define script_burn_victory scorpion_script_burn_victory
#define script_close_hp_tail scorpion_script_close_hp_tail
#define script_combo_hp_tail scorpion_script_combo_hp_tail
#define script_crouch_lk_tail scorpion_script_crouch_lk_tail
#define script_draw_equip scorpion_script_draw_equip
#define script_fire_decor scorpion_script_fire_decor
#define script_fire_flame scorpion_script_fire_flame
#define script_knee_tail scorpion_script_knee_tail
#define script_lk_damage scorpion_script_lk_damage
#define script_lk_rebase scorpion_script_lk_rebase
#define script_lp_return scorpion_script_lp_return
#define script_opponent_reaction scorpion_script_opponent_reaction
#define script_reaction scorpion_script_reaction
#define script_spawn_spear scorpion_script_spawn_spear
#define script_spear_done scorpion_script_spear_done
#define script_spear_pull scorpion_script_spear_pull
#define script_spear_return scorpion_script_spear_return
#define script_teleport_land scorpion_script_teleport_land
#define script_teleport_motion scorpion_script_teleport_motion
#define script_teleport_victim_tail scorpion_script_teleport_victim_tail
#define script_transform_effect scorpion_script_transform_effect
#define script_weapon_ready scorpion_script_weapon_ready
#define spear_stun_script scorpion_spear_stun_script
#define special_ready scorpion_special_ready
#define spin_script scorpion_spin_script
#define start_fatality scorpion_start_fatality
#define sword_spin_control scorpion_sword_spin_control
#define sword_spin_query scorpion_sword_spin_query
#define teleport_land_script scorpion_teleport_land_script
#define teleport_script scorpion_teleport_script

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
#define SWORD_MOVE(name, clip_id, action_id) static const MoveDef name = {.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = action_id}
#define BLEND(clip_id, cursor)                                                                                                                                 \
  {{.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, cursor, cursor, 1, 0x3333, 0x3333}

bool scorpion_can_spear(const Fighter *fighter);
bool scorpion_can_teleport(const Fighter *fighter);
void scorpion_enter_lk_throw(Fighter *fighter);
void scorpion_enter_throw(Fighter *fighter);
void scorpion_fire_script(Fighter *fighter);
void scorpion_ground_attack(Fighter *fighter, int index);
void scorpion_ground_native_next(Fighter *fighter);
void scorpion_ground_native_tail(Fighter *fighter);
void scorpion_native_blend(Fighter *fighter, const SceneClip *blend, FighterFn end);
void scorpion_press_air_hk(Fighter *fighter);
void scorpion_press_air_hp(Fighter *fighter);
void scorpion_press_air_lk(Fighter *fighter);
void scorpion_press_air_lp(Fighter *fighter);
void scorpion_press_combo_hk(Fighter *fighter);
void scorpion_press_combo_hp(Fighter *fighter);
void scorpion_press_crouch_hk(Fighter *fighter);
void scorpion_press_crouch_lk(Fighter *fighter);
void scorpion_press_crouch_lp(Fighter *fighter);
void scorpion_press_jab(Fighter *fighter);
void scorpion_press_kick(Fighter *fighter);
void scorpion_press_knee_hp(Fighter *fighter);
void scorpion_press_lk(Fighter *fighter);
void scorpion_press_lp(Fighter *fighter);
void scorpion_entry_retail_script_004d2e58(Fighter *fighter);
void scorpion_entry_retail_script_004d2e88(Fighter *fighter);
void scorpion_entry_retail_script_004d2eb0(Fighter *fighter);
void scorpion_entry_retail_script_004d2ef8(Fighter *fighter);
void scorpion_entry_retail_script_004d2f10(Fighter *fighter);
void scorpion_entry_retail_script_004de2f8(Fighter *fighter);
void scorpion_entry_retail_script_004de318(Fighter *fighter);
void scorpion_entry_retail_script_004de328(Fighter *fighter);
void scorpion_entry_retail_script_004de798(Fighter *fighter);
void scorpion_entry_retail_script_004e0278(Fighter *fighter);
void scorpion_entry_retail_script_004e0288(Fighter *fighter);
void scorpion_entry_retail_script_004e0298(Fighter *fighter);
void scorpion_entry_retail_script_004e02a8(Fighter *fighter);
void scorpion_entry_retail_script_004e02b8(Fighter *fighter);
void scorpion_entry_retail_script_004e02d0(Fighter *fighter);
void scorpion_entry_retail_script_004e0300(Fighter *fighter);
void scorpion_entry_retail_script_004e0310(Fighter *fighter);
void scorpion_entry_retail_script_004e0320(Fighter *fighter);
void scorpion_entry_retail_script_004e0338(Fighter *fighter);
void scorpion_entry_retail_script_004e0348(Fighter *fighter);
void scorpion_entry_retail_script_004e0358(Fighter *fighter);
void scorpion_entry_retail_script_004e5b58(Fighter *fighter);
void scorpion_entry_retail_script_004e5e28(Fighter *fighter);
void scorpion_entry_retail_script_004e5e50(Fighter *fighter);
void scorpion_entry_retail_script_004e60b8(Fighter *fighter);
void scorpion_entry_retail_script_004e6158(Fighter *fighter);
void scorpion_entry_retail_script_004e6190(Fighter *fighter);
void scorpion_entry_retail_script_004e61e0(Fighter *fighter);
void scorpion_entry_retail_script_004e61f8(Fighter *fighter);
void scorpion_entry_retail_script_004e62c0(Fighter *fighter);
void scorpion_entry_retail_script_004e62d8(Fighter *fighter);
void scorpion_entry_retail_script_004e63d8(Fighter *fighter);
void scorpion_entry_retail_script_004e6500(Fighter *fighter);
void scorpion_entry_retail_script_004e6658(Fighter *fighter);
void scorpion_entry_retail_script_004e66d0(Fighter *fighter);
void scorpion_entry_retail_script_004e66f0(Fighter *fighter);
void scorpion_entry_retail_script_004e6710(Fighter *fighter);
void scorpion_entry_retail_script_004e69b0(Fighter *fighter);
void scorpion_entry_retail_script_004e6d08(Fighter *fighter);
void scorpion_entry_retail_script_004e87b0(Fighter *fighter);
void scorpion_entry_retail_script_004e8818(Fighter *fighter);
void scorpion_entry_retail_script_004e8850(Fighter *fighter);
void scorpion_entry_retail_script_004e97f8(Fighter *fighter);
void scorpion_entry_retail_script_004e9910(Fighter *fighter);
void scorpion_entry_retail_script_004e9938(Fighter *fighter);
void scorpion_entry_retail_script_004e9970(Fighter *fighter);
void scorpion_entry_retail_script_004e9d00(Fighter *fighter);
void scorpion_entry_retail_script_004e9f20(Fighter *fighter);
void scorpion_entry_retail_script_004ec8b0(Fighter *fighter);
void scorpion_entry_retail_script_004ec918(Fighter *fighter);
void scorpion_entry_retail_script_004ed448(Fighter *fighter);
void scorpion_entry_retail_script_004ed4a8(Fighter *fighter);
extern const MoveDef scorpion_s_teleport_land;
extern const MoveDef scorpion_s_throw;
void scorpion_air_throw(Fighter *fighter);
void scorpion_beast_fatality(Fighter *fighter);
void scorpion_burn_fatality(Fighter *fighter);
void scorpion_fire(Fighter *fighter);
void scorpion_hk(Fighter *fighter);
void scorpion_hp(Fighter *fighter);
void scorpion_spear(Fighter *fighter);
void scorpion_teleport(Fighter *fighter);
void scorpion_weapon(Fighter *fighter);
void scorpion_script_air_diagonal_tail(Fighter *fighter);
void scorpion_script_air_hp_tail(Fighter *fighter);
void scorpion_script_air_kick_tail(Fighter *fighter);
void scorpion_script_air_lk_tail(Fighter *fighter);
void scorpion_script_air_lp_tail(Fighter *fighter);
void scorpion_script_air_land(Fighter *fighter);
void scorpion_script_air_release(Fighter *fighter);
void scorpion_script_beast_attack(Fighter *fighter);
void scorpion_script_beast_begin(Fighter *fighter);
void scorpion_script_beast_gore(Fighter *victim, int type);
void scorpion_script_beast_park(Fighter *fighter);
void scorpion_script_beast_victim_pieces(Fighter *victim);
void scorpion_script_beast_voice(Fighter *fighter);
void scorpion_script_burn_begin(Fighter *fighter);
void scorpion_script_burn_repeat1(Fighter *fighter);
void scorpion_script_burn_repeat2(Fighter *fighter);
void scorpion_script_burn_tail(Fighter *victim);
void scorpion_script_burn_victim_blend(Fighter *victim);
void scorpion_script_burn_victim_cue(Fighter *victim);
void scorpion_script_burn_victim_end(Fighter *victim);
void scorpion_script_burn_victim_impact(Fighter *victim);
void scorpion_script_burn_victim_repeat(Fighter *victim);
void scorpion_script_burn_victim_settle(Fighter *victim);
void scorpion_script_burn_victim_start(Fighter *fighter);
void scorpion_script_burn_victory(Fighter *fighter);
void scorpion_script_close_hp_tail(Fighter *fighter);
void scorpion_script_combo_hp_tail(Fighter *fighter);
void scorpion_script_draw_equip(Fighter *fighter);
void scorpion_script_fire_decor(Fighter *fighter);
void scorpion_script_fire_flame(Fighter *fighter);
void scorpion_script_knee_tail(Fighter *fighter);
void scorpion_script_lk_damage(Fighter *fighter);
void scorpion_script_lk_rebase(Fighter *fighter);
void scorpion_script_lp_return(Fighter *fighter);
void scorpion_script_opponent_reaction(Fighter *fighter, int reaction);
void scorpion_thrown_end_script(Fighter *victim);
void scorpion_script_reaction(Fighter *fighter, int reaction);
void scorpion_script_spawn_spear(Fighter *fighter, int type);
void scorpion_script_spear_done(Fighter *fighter);
void scorpion_script_spear_pull(Fighter *fighter);
void scorpion_script_spear_return(Fighter *fighter);
void scorpion_script_teleport_land(Fighter *fighter);
void scorpion_script_teleport_motion(Fighter *fighter);
void scorpion_script_teleport_victim_tail(Fighter *victim);
void scorpion_script_transform_effect(Fighter *fighter, int type);
void scorpion_script_weapon_ready(Fighter *fighter);
void scorpion_spear_stun_script(Fighter *fighter);
void scorpion_special_ready(Fighter *fighter);
void scorpion_spin_script(Fighter *fighter);
void scorpion_start_fatality(Fighter *fighter, int id);
void scorpion_sword_spin_control(Fighter *fighter);
void scorpion_sword_spin_query(Fighter *fighter);
void scorpion_teleport_land_script(Fighter *fighter);
void scorpion_teleport_script(Fighter *fighter);

#endif

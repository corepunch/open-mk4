#ifndef MK4_NATIVE_ACTIONS_H
#define MK4_NATIVE_ACTIONS_H
#include "game.h"
#include "script/bindings.h"

/* Verified local EXE entry points for shared script/native phase boundaries. */
enum {
  MK4_NATIVE_FIRE_CONTACT = 0x468640,
  MK4_NATIVE_FIRE_RECOVERY = 0x4688e0,
  MK4_NATIVE_THAW_LANDING = 0x486530
};

/* Retail Q16 damage multipliers, retaining the original truncated values. */
enum {
  MK4_DAMAGE_SCALE_30_PERCENT = 0x4ccc,
  MK4_DAMAGE_SCALE_40_PERCENT = 0x6666,
  MK4_DAMAGE_SCALE_50_PERCENT = 0x8000,
  MK4_DAMAGE_SCALE_60_PERCENT = 0x9999
};

void action_fireball(Fighter *fighter);
void action_air_fireball(Fighter *fighter);
void action_fireball_spawn(Fighter *fighter, int task_type);
void action_air_fireball_spawn(Fighter *fighter, int task_type);
void action_fireball_fall(Fighter *fighter);
void action_weapon_contact(Fighter *fighter, uint32_t address);
void action_weapon_equip(Fighter *fighter);
void action_weapon_draw(Fighter *fighter);
void action_weapon_draw_equip(Fighter *fighter);
void action_weapon_high_punch(Fighter *fighter);
void action_weapon_low_punch(Fighter *fighter);
void action_weapon_back_high_punch(Fighter *fighter);
void action_weapon_back_low_punch(Fighter *fighter);
void action_liu_kang_sword_high_punch(Fighter *fighter);
void action_liu_kang_sword_low_punch(Fighter *fighter);
void action_liu_kang_sword_back_high_punch(Fighter *fighter);
void action_liu_kang_sword_back_low_punch(Fighter *fighter);
void action_bow_high_punch(Fighter *fighter);
void action_bow_low_punch(Fighter *fighter);
void action_bow_back_strike(Fighter *fighter);
void action_bow_followup_strike(Fighter *fighter);
void action_sword_back_high_punch(Fighter *fighter);
void action_sword_back_low_punch(Fighter *fighter);
void action_sword_spin(Fighter *fighter);
void action_sonya_weapon_back_high_punch(Fighter *fighter);
void action_kai_weapon_back_strike(Fighter *fighter);
void action_cage_weapon_back_upper(Fighter *fighter);
void action_reptile_weapon_flurry(Fighter *fighter);
void action_reptile_weapon_back_low_punch(Fighter *fighter);
void action_axe_back_low_punch(Fighter *fighter);
void action_boomerang_low_punch(Fighter *fighter);
void action_getup_back(Fighter *fighter);
void action_getup_flip(Fighter *fighter);
void action_getup_roll(Fighter *fighter);
void action_getup_roll_flip(Fighter *fighter);
void action_flip_facing(Fighter *fighter);
void action_throw_ready(Fighter *fighter);
void action_lp_end(Fighter *fighter);
void action_crouch_return(Fighter *fighter);
void action_throw_turn(Fighter *fighter);
void action_victory(Fighter *fighter);
void action_victory_continue(Fighter *fighter);
void action_teleport(Fighter *fighter);
void action_teleport_vanish(Fighter *fighter);
void action_teleport_slam_fall(Fighter *fighter);
void action_teleport_slam_drop(Fighter *fighter);
void action_teleport_slam(Fighter *fighter);
void action_air_throw(Fighter *fighter);
void action_air_throw_lift(Fighter *fighter);
void action_air_throw_slam(Fighter *fighter);
void action_air_throw_flip(Fighter *fighter);
void action_air_throw_release(Fighter *fighter);
void action_air_throw_land(Fighter *fighter);
void action_air_lift_fall(Fighter *fighter);
void action_air_lift_release(Fighter *fighter);
void action_air_lift_getup(Fighter *fighter);
void action_air_flip_release(Fighter *fighter);
void action_air_flip_getup(Fighter *fighter);
void action_air_slam_fall(Fighter *fighter);
void action_air_slam_end(Fighter *fighter);
void action_air_victim_default(Fighter *fighter);
void action_air_victim_flip(Fighter *fighter);
void action_air_victim_lift(Fighter *fighter);
void action_slam_tail(Fighter *fighter);
void action_slam_reaction(Fighter *fighter);
void action_slam_released_fall(Fighter *fighter);
void action_air_diagonal_alt_tail(Fighter *fighter);
void action_air_diagonal_tail(Fighter *fighter);
void action_air_diagonal_wide_tail(Fighter *fighter);
void action_air_hp_tail(Fighter *fighter);
void action_air_land(Fighter *fighter);
void action_air_lk_alt_tail(Fighter *fighter);
void action_air_lk_tail(Fighter *fighter);
void action_air_lk_wide_tail(Fighter *fighter);
void action_air_lp_tail(Fighter *fighter);
void action_air_straight_land(Fighter *fighter);
void action_air_straight_tail(Fighter *fighter);
void action_air_straight_wide_tail(Fighter *fighter);
void action_air_bind_row(Fighter *fighter, uint32_t row);
void action_air_attack(Fighter *fighter, unsigned kind);
void action_air_high_punch(Fighter *fighter);
void action_air_low_punch(Fighter *fighter);
void action_air_kick(Fighter *fighter);
void action_air_diagonal_high_kick(Fighter *fighter);
void action_air_diagonal_low_kick(Fighter *fighter);
extern const ButtonTable action_close_buttons[];
extern const ButtonTable action_ground_buttons[];
extern const ButtonTable action_air_buttons[];
extern const Controls action_stances;
void action_normal_low_punch(Fighter *fighter);
void action_normal_low_kick(Fighter *fighter);
uint32_t action_button_target(const Mk4Image *image, uint32_t table, Button button);
void action_press_table(Fighter *fighter, Button button);
void action_press_native_button(Fighter *fighter, uint32_t target);
void action_combo_high_punch(Fighter *fighter);
void action_close_high_kick(Fighter *fighter);
void action_close_high_punch_tail(Fighter *fighter);
void action_combo_high_punch_tail(Fighter *fighter);
void action_close_high_kick_tail(Fighter *fighter);
void action_close_high_punch(Fighter *fighter);
void action_crouch_high_kick(Fighter *fighter);
void action_crouch_low_punch(Fighter *fighter);
void action_crouch_low_kick(Fighter *fighter);
void action_crouch_low_kick_tail(Fighter *fighter);
FighterFn action_throw_reaction(const Mk4Image *image, unsigned reaction);
void action_throw_victim_continue(Fighter *fighter);
void action_throw_victim_impact(Fighter *fighter);
void action_throw_grunt(void);
void action_impact_voice(Fighter *fighter);
void action_lp_align_and_release(Fighter *fighter);
void action_lk_throw(Fighter *fighter);
void action_throw_pair_start(Fighter *fighter);
void action_lk_return(Fighter *fighter);
void action_lk_pause_long(Fighter *fighter);
void action_lk_pause_short(Fighter *fighter);
void action_lk_jump_back(Fighter *fighter);
void action_lk_recoil(Fighter *fighter);
void action_lk_branch_continue(Fighter *fighter);
void action_lk_victim_return(Fighter *fighter);
void action_lk_rock_repeat_one(Fighter *fighter);
void action_lk_rock_repeat_two(Fighter *fighter);
void action_lk_rock_release(Fighter *fighter);
void action_lp_throw(Fighter *fighter);
void action_face_peer(Fighter *fighter);
void action_begin(Fighter *fighter, StateId state, int action, bool commands);
void action_sweep(Fighter *fighter);
void action_sweep_standard_tail(Fighter *fighter);
void action_sweep_advancing_tail(Fighter *fighter);
void action_jab(Fighter *fighter);
void action_high_punch(Fighter *fighter);
void action_high_kick(Fighter *fighter);
void action_low_kick(Fighter *fighter);
void action_kick_contact_rows(Fighter *fighter, uint32_t table);
void action_kick_contact_default(Fighter *fighter);
void action_kick_contact_wide(Fighter *fighter);
void action_kick_contact_narrow(Fighter *fighter);
void action_fan_start(Fighter *fighter);
void action_fan_finish(Fighter *fighter);
void action_fan_tail(Fighter *fighter);
void action_fan_cycle(Fighter *fighter);
void action_fan_victory(Fighter *fighter);
void action_fan_victim_end(Fighter *fighter);
bool action_can_finish_close(const Fighter *fighter);
bool action_can_finish_range(const Fighter *fighter);
bool action_has_gravity(const Fighter *fighter);
bool action_native_button_chord(const Fighter *fighter, uint32_t address);

void action_roundhouse(Fighter *fighter);
void action_roundhouse_start(Fighter *fighter);
void action_roundhouse_wide_start(Fighter *fighter);
void action_roundhouse_tail(Fighter *fighter);
void action_roundhouse_wide_tail(Fighter *fighter);
void action_uppercut(Fighter *fighter);

void action_finish_first(Fighter *fighter);
void action_finish_second(Fighter *fighter);
void action_spike_finish(Fighter *fighter);
void action_scene_pose(Fighter *fighter, int clip, int sample, bool owner);
void action_lp_select_short(Fighter *fighter);
void action_lp_select_long(Fighter *fighter);

void action_lp_victim(Fighter *fighter);

void action_script_release_reaction(Fighter *fighter, int reaction);
void action_disturb_resting_weapons(void);

FighterFn action_reaction_entry(const Mk4Image *image, unsigned reaction);
void action_stagger_reaction(Fighter *fighter);
void action_native_sound_list(uint32_t address);

void action_launch_reaction(Fighter *fighter);
void action_reaction_getup_back(Fighter *fighter);
void action_reaction_finish(Fighter *fighter);
void action_reaction_fall(Fighter *fighter);

void action_tumble_reaction(Fighter *fighter);

void action_controller_return(Fighter *fighter);

void action_task_pause(Fighter *fighter);
void action_task_retire(Fighter *fighter);
void action_scene_body_finish(Fighter *fighter);

void action_directional_motion(Fighter *fighter, q16 speed, q16 up, q16 gravity);
bool action_outer_edge(const Fighter *fighter);
void action_edge_push(Fighter *fighter);

void action_scene_hover(Fighter *fighter, int task_type);

void action_stagger_tail(Fighter *fighter);

void action_scene_join_victory(Fighter *fighter);

void action_script_reaction(Fighter *fighter);

void action_scaled_reaction(Fighter *fighter);

void action_branch_reaction(Fighter *fighter);
void action_getup_front(Fighter *fighter);

void action_weapon_reaction(Fighter *fighter);

void action_impact_reaction(Fighter *fighter);

void action_stagger_entry(Fighter *fighter);

void action_lk_victim(Fighter *fighter);

void action_scene_reaction(Fighter *fighter);

void action_stage_owner(Fighter *fighter);
void action_stage_victim(Fighter *fighter);
void action_stage_impale(Fighter *fighter);
void action_stage_complete(Fighter *fighter);

void action_snare_reaction(Fighter *fighter);
void action_script_reaction_dispatch(Fighter *fighter, int reaction);
void action_scene_victim_hold(Fighter *fighter);

void action_spear_arrival(Fighter *fighter);
void action_pair_release(Fighter *fighter);
void action_bicycle_capture(Fighter *fighter);
void action_bicycle_cycle(Fighter *fighter);
void action_fire_recovery_finish(Fighter *fighter);
void action_thaw_land(Fighter *fighter);
void action_uppercut_recover(Fighter *fighter);
void action_block_cycle(Fighter *fighter);
void action_bicycle_release(Fighter *fighter);
void action_torpedo_return(Fighter *fighter);
void action_record_reaction(Fighter *fighter);
void action_square_victim(Fighter *fighter);

uint32_t action_reaction_native(const Fighter *fighter);
FighterFn action_engine_reaction(uint32_t entry);

void action_aux_reaction(Fighter *fighter);
void action_shake_stun(Fighter *fighter);

void action_guard_fall(Fighter *fighter);
void action_scene_explode(Fighter *fighter);
void action_scene_park(Fighter *fighter);

#endif

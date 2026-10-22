#ifndef MK4_RETAIL_NATIVE_H
#define MK4_RETAIL_NATIVE_H
#include "game.h"

void retail_fireball(Fighter *fighter);
void retail_air_fireball(Fighter *fighter);
void retail_fireball_spawn(Fighter *fighter, int task_type);
void retail_air_fireball_spawn(Fighter *fighter, int task_type);
void retail_fireball_fall(Fighter *fighter);
void retail_weapon_contact(Fighter *fighter, uint32_t address);
void retail_weapon_equip(Fighter *fighter);
void retail_weapon_draw(Fighter *fighter);
void retail_weapon_draw_equip(Fighter *fighter);
void retail_weapon_high_punch(Fighter *fighter);
void retail_weapon_low_punch(Fighter *fighter);
void retail_weapon_back_high_punch(Fighter *fighter);
void retail_weapon_back_low_punch(Fighter *fighter);
void retail_weapon_native_4963e0(Fighter *fighter);
void retail_weapon_native_4964d0(Fighter *fighter);
void retail_weapon_native_496670(Fighter *fighter);
void retail_weapon_native_496920(Fighter *fighter);
void retail_weapon_native_495c30(Fighter *fighter);
void retail_weapon_native_495c60(Fighter *fighter);
void retail_weapon_native_495cc0(Fighter *fighter);
void retail_weapon_native_495cf0(Fighter *fighter);
void retail_weapon_native_496c90(Fighter *fighter);
void retail_weapon_native_496c50(Fighter *fighter);
void retail_weapon_native_480770(Fighter *fighter);
void retail_weapon_native_496720(Fighter *fighter);
void retail_weapon_native_495a90(Fighter *fighter);
void retail_weapon_native_495860(Fighter *fighter);
void retail_weapon_native_495d70(Fighter *fighter);
void retail_weapon_native_496140(Fighter *fighter);
void retail_weapon_native_496d10(Fighter *fighter);
void retail_weapon_native_495610(Fighter *fighter);
void retail_getup_back(Fighter *fighter);
void retail_getup_flip(Fighter *fighter);
void retail_getup_roll(Fighter *fighter);
void retail_getup_roll_flip(Fighter *fighter);
void retail_flip_facing(Fighter *fighter);
void retail_throw_ready(Fighter *fighter);
void retail_lp_end(Fighter *fighter);
void retail_crouch_return(Fighter *fighter);
void retail_throw_turn(Fighter *fighter);
void retail_victory(Fighter *fighter);
void retail_victory_continue(Fighter *fighter);
void retail_teleport(Fighter *fighter);
void retail_teleport_vanish(Fighter *fighter);
void retail_teleport_slam_fall(Fighter *fighter);
void retail_teleport_slam_drop(Fighter *fighter);
void retail_teleport_slam(Fighter *fighter);
void retail_air_throw(Fighter *fighter);
void retail_air_throw_lift(Fighter *fighter);
void retail_air_throw_slam(Fighter *fighter);
void retail_air_throw_flip(Fighter *fighter);
void retail_air_throw_release(Fighter *fighter);
void retail_air_throw_land(Fighter *fighter);
void retail_air_lift_fall(Fighter *fighter);
void retail_air_lift_release(Fighter *fighter);
void retail_air_lift_getup(Fighter *fighter);
void retail_air_flip_release(Fighter *fighter);
void retail_air_flip_getup(Fighter *fighter);
void retail_air_slam_fall(Fighter *fighter);
void retail_air_slam_end(Fighter *fighter);
void retail_air_victim_default(Fighter *fighter);
void retail_air_victim_flip(Fighter *fighter);
void retail_air_victim_lift(Fighter *fighter);
void retail_slam_tail(Fighter *fighter);
void retail_slam_reaction(Fighter *fighter);
void retail_slam_released_fall(Fighter *fighter);
void retail_air_diagonal_alt_tail(Fighter *fighter);
void retail_air_diagonal_tail(Fighter *fighter);
void retail_air_diagonal_wide_tail(Fighter *fighter);
void retail_air_hp_tail(Fighter *fighter);
void retail_air_land(Fighter *fighter);
void retail_air_lk_alt_tail(Fighter *fighter);
void retail_air_lk_tail(Fighter *fighter);
void retail_air_lk_wide_tail(Fighter *fighter);
void retail_air_lp_tail(Fighter *fighter);
void retail_air_straight_land(Fighter *fighter);
void retail_air_straight_tail(Fighter *fighter);
void retail_air_straight_wide_tail(Fighter *fighter);
void retail_air_bind_row(Fighter *fighter, uint32_t row);
void retail_air_attack(Fighter *fighter, unsigned kind);
void retail_air_high_punch(Fighter *fighter);
void retail_air_low_punch(Fighter *fighter);
void retail_air_kick(Fighter *fighter);
void retail_air_diagonal_high_kick(Fighter *fighter);
void retail_air_diagonal_low_kick(Fighter *fighter);
extern const ButtonTable retail_close_buttons[];
extern const ButtonTable retail_ground_buttons[];
extern const ButtonTable retail_air_buttons[];
extern const Controls retail_stances;
void retail_normal_low_punch(Fighter *fighter);
void retail_normal_low_kick(Fighter *fighter);
uint32_t retail_button_target(const Mk4Image *image, uint32_t table, Button button);
void retail_press_table(Fighter *fighter, Button button);
void retail_press_native_button(Fighter *fighter, uint32_t target);
void retail_combo_high_punch(Fighter *fighter);
void retail_close_high_kick(Fighter *fighter);
void retail_close_high_punch_tail(Fighter *fighter);
void retail_combo_high_punch_tail(Fighter *fighter);
void retail_close_high_kick_tail(Fighter *fighter);
void retail_close_high_punch(Fighter *fighter);
void retail_crouch_high_kick(Fighter *fighter);
void retail_crouch_low_punch(Fighter *fighter);
void retail_crouch_low_kick(Fighter *fighter);
void retail_crouch_low_kick_tail(Fighter *fighter);
FighterFn retail_throw_reaction(const Mk4Image *image, unsigned reaction);
void retail_throw_victim_continue(Fighter *fighter);
void retail_throw_victim_impact(Fighter *fighter);
void retail_throw_grunt(void);
void retail_impact_voice(Fighter *fighter);
void retail_lk_continue_840(Fighter *fighter);
void retail_lk_throw(Fighter *fighter);
void retail_throw_pair_start(Fighter *fighter);
void retail_lk_return(Fighter *fighter);
void retail_lk_pause_long(Fighter *fighter);
void retail_lk_pause_short(Fighter *fighter);
void retail_lk_jump_back(Fighter *fighter);
void retail_lk_recoil(Fighter *fighter);
void retail_lk_branch_continue(Fighter *fighter);
void retail_lk_victim_return(Fighter *fighter);
void retail_lk_script_170(Fighter *fighter);
void retail_lk_script_180(Fighter *fighter);
void retail_lk_script_190(Fighter *fighter);
void retail_lp_throw(Fighter *fighter);
void retail_face_peer(Fighter *fighter);
void retail_native_begin(Fighter *fighter, StateId state, int action, bool commands);
void retail_sweep(Fighter *fighter);
void retail_sweep_state(Fighter *fighter);
void retail_sweep_standard_tail(Fighter *fighter);
void retail_sweep_advancing_tail(Fighter *fighter);
void retail_jab(Fighter *fighter);
void retail_high_punch(Fighter *fighter);
void retail_high_kick(Fighter *fighter);
void retail_low_kick(Fighter *fighter);
void retail_kick_contact_rows(Fighter *fighter, uint32_t table);
void retail_kick_contact_default(Fighter *fighter);
void retail_kick_contact_wide(Fighter *fighter);
void retail_kick_contact_narrow(Fighter *fighter);
void retail_fan_start(Fighter *fighter);
void retail_fan_finish(Fighter *fighter);
void retail_fan_tail(Fighter *fighter);
void retail_fan_cycle(Fighter *fighter);
void retail_fan_victory(Fighter *fighter);
void retail_fan_victim_end(Fighter *fighter);
bool retail_native_vm(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument);
FighterFn retail_native_entry(unsigned character, uint32_t address);
bool retail_can_finish_close(const Fighter *fighter);
bool retail_can_finish_range(const Fighter *fighter);
bool retail_has_gravity(const Fighter *fighter);
CommandPredicate retail_native_predicate(uint32_t address);
bool retail_native_gate(const Fighter *fighter, uint32_t address);

void retail_press_sweep(Fighter *fighter);
void retail_roundhouse(Fighter *fighter);
void retail_press_roundhouse(Fighter *fighter);
void retail_roundhouse_start(Fighter *fighter);
void retail_roundhouse_wide_start(Fighter *fighter);
void retail_roundhouse_tail(Fighter *fighter);
void retail_roundhouse_wide_tail(Fighter *fighter);
void retail_uppercut(Fighter *fighter);
void retail_uppercut_state(Fighter *fighter);

void retail_finish_first(Fighter *fighter);
void retail_finish_second(Fighter *fighter);
void retail_spike_finish(Fighter *fighter);
void retail_scene_pose(Fighter *fighter, int clip, int sample, bool owner);
void retail_lp_select_short(Fighter *fighter);
void retail_lp_select_long(Fighter *fighter);

void retail_lp_victim(Fighter *fighter);

void retail_script_release_reaction(Fighter *fighter, int reaction);
void retail_disturb_resting_weapons(void);

FighterFn retail_reaction_entry(const Mk4Image *image, unsigned reaction);
void retail_stagger_reaction(Fighter *fighter);
void retail_native_sound_list(uint32_t address);

void retail_launch_reaction(Fighter *fighter);
void retail_reaction_getup_back(Fighter *fighter);
void retail_reaction_finish(Fighter *fighter);
void retail_reaction_fall(Fighter *fighter);

void retail_tumble_reaction(Fighter *fighter);

void retail_controller_return(Fighter *fighter);

void retail_task_pause(Fighter *fighter);
void retail_task_retire(Fighter *fighter);
void retail_scene_body_finish(Fighter *fighter);

void retail_directional_motion(Fighter *fighter, q16 speed, q16 up, q16 gravity);
bool retail_outer_edge(const Fighter *fighter);
void retail_edge_push(Fighter *fighter);

void retail_scene_hover(Fighter *fighter, int task_type);

void retail_stagger_tail(Fighter *fighter);

void retail_scene_join_victory(Fighter *fighter);

void retail_script_reaction(Fighter *fighter);

void retail_scaled_reaction(Fighter *fighter);

void retail_branch_reaction(Fighter *fighter);
void retail_getup_front(Fighter *fighter);

void retail_weapon_reaction(Fighter *fighter);

void retail_impact_reaction(Fighter *fighter);

void retail_stagger_entry(Fighter *fighter);

void retail_lk_victim(Fighter *fighter);

void retail_scene_reaction(Fighter *fighter);

void retail_stage_owner(Fighter *fighter);
void retail_stage_victim(Fighter *fighter);
void retail_stage_impale(Fighter *fighter);
void retail_stage_complete(Fighter *fighter);

void retail_snare_reaction(Fighter *fighter);
void retail_script_reaction_dispatch(Fighter *fighter, int reaction);
void retail_scene_victim_hold(Fighter *fighter);

void retail_spear_arrival(Fighter *fighter);
void retail_pair_release(Fighter *fighter);
void retail_bicycle_capture(Fighter *fighter);
void retail_bicycle_cycle(Fighter *fighter);
void retail_block_cycle(Fighter *fighter);
void retail_bicycle_release(Fighter *fighter);
void retail_torpedo_return(Fighter *fighter);
void retail_record_reaction(Fighter *fighter);
void retail_square_victim(Fighter *fighter);

uint32_t retail_reaction_native(const Fighter *fighter);
FighterFn retail_engine_reaction(uint32_t entry);

void retail_aux_reaction(Fighter *fighter);
void retail_shake_stun(Fighter *fighter);

void retail_guard_fall(Fighter *fighter);
void retail_scene_explode(Fighter *fighter);
void retail_scene_park(Fighter *fighter);

#endif

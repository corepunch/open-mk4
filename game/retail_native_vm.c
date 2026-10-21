/* Typed native ABI registrations, derived from the reviewed local-EXE
 * contracts. This is a transitional bridge to existing native services;
 * no retail script instructions or animation frame rows are emitted. */
#include "retail_native.h"
#include "retail_vm.h"
#include "script.h"

void cage_first_sever(Fighter *victim);
void cage_first_task_end(Fighter *actor);
void cage_first_torso_123(Fighter *torso);
void cage_first_torso_finish(Fighter *torso);
void cage_first_torso_half(Fighter *torso);
void cage_first_victim_131(Fighter *victim);
void cage_first_victim_148(Fighter *victim);
void cage_first_victim_166(Fighter *victim);
void cage_first_victim_185(Fighter *victim);
void cage_first_victim_206(Fighter *victim);
void cage_first_victim_59(Fighter *victim);
void cage_orb_spawn(Fighter *fighter, int task_type);
void cage_power_land(Fighter *fighter);
void cage_script_air_finish(Fighter *fighter);
void cage_script_close_hk_tail(Fighter *fighter);
void cage_script_close_hp_tail(Fighter *fighter);
void cage_script_combo_hp_tail(Fighter *fighter);
void cage_script_cpu_kick_tail(Fighter *fighter);
void cage_script_current_reaction(Fighter *fighter, int reaction);
void cage_script_draw_equip(Fighter *fighter);
void cage_script_fatality_1_signal(Fighter *fighter);
void cage_script_fatality_2_continue(Fighter *fighter);
void cage_script_finish(Fighter *fighter);
void cage_script_kick_finish(Fighter *fighter);
void cage_script_lk_throw_return(Fighter *fighter);
void cage_script_lp_return(Fighter *fighter);
void cage_script_opponent_reaction(Fighter *fighter, int reaction);
void cage_script_powered_punch_tail(Fighter *fighter);
void cage_script_roundhouse_finish(Fighter *fighter);
void cage_script_roundhouse_start(Fighter *fighter);
void cage_script_spike_tail(Fighter *fighter);
void cage_second_charge(Fighter *fighter, int task_type);
void cage_second_flash(Fighter *fighter, int task_type);
void cage_second_repeated_uppercuts(Fighter *fighter);
void cage_second_victim_finish(Fighter *fighter);
void cage_second_victim_pause(Fighter *fighter);
void cage_weapon_back_lp_contact(Fighter *fighter);
void cage_weapon_getup_back(Fighter *fighter);
void cage_weapon_getup_roll(Fighter *fighter);
void cage_weapon_upper_contact(Fighter *fighter);
void fighter_friction(Fighter *fighter);
void fujin_bow_finish(Fighter *fighter);
void fujin_dive_contact(Fighter *fighter);
void fujin_dive_land(Fighter *fighter);
void fujin_draw_equip(Fighter *fighter);
void fujin_first_cycle(Fighter *fighter);
void fujin_first_fire_end(Fighter *fighter);
void fujin_first_loop_start(Fighter *fighter);
void fujin_first_shoot(Fighter *fighter);
void fujin_grab_control_start(Fighter *fighter);
void fujin_script_air_finish(Fighter *fighter);
void fujin_script_close_hk_tail(Fighter *fighter);
void fujin_script_close_hp_tail(Fighter *fighter);
void fujin_script_combo_hp_tail(Fighter *fighter);
void fujin_script_current_reaction(Fighter *fighter, int reaction);
void fujin_script_finish(Fighter *fighter);
void fujin_script_kick_finish(Fighter *fighter);
void fujin_script_lk_return(Fighter *fighter);
void fujin_script_lp_extended_finish(Fighter *fighter);
void fujin_script_lp_select(Fighter *fighter);
void fujin_script_lp_short_finish(Fighter *fighter);
void fujin_script_opponent_reaction(Fighter *fighter, int reaction);
void fujin_script_roundhouse_finish(Fighter *fighter);
void fujin_script_roundhouse_start(Fighter *fighter);
void fujin_script_spike_tail(Fighter *fighter);
void fujin_second_blood(Fighter *fighter, int task_type);
void fujin_second_burst(Fighter *victim);
void fujin_second_event(Fighter *fighter, int selector);
void fujin_second_face(Fighter *fighter);
void fujin_second_finish(Fighter *victim);
void fujin_triple_contact(Fighter *fighter);
void fujin_triple_fast(Fighter *fighter);
void fujin_wind_crouch_return(Fighter *fighter);
void fujin_wind_getup(Fighter *fighter);
void fujin_wind_recovery(Fighter *fighter);
void goro_fireball_spawn(Fighter *fighter, int task_type);
void goro_quake_resting_weapons(void);
void goro_script_air_finish(Fighter *fighter);
void goro_script_air_getup(Fighter *victim);
void goro_script_air_land(Fighter *fighter);
void goro_script_air_release(Fighter *fighter);
void goro_script_close_hk_tail(Fighter *fighter);
void goro_script_close_hp_tail(Fighter *fighter);
void goro_script_combo_hp_tail(Fighter *fighter);
void goro_script_current_reaction(Fighter *fighter, int reaction);
void goro_script_kick_finish(Fighter *fighter);
void goro_script_kick_tail(Fighter *fighter);
void goro_script_leap_continue(Fighter *fighter);
void goro_script_lk_return(Fighter *fighter);
void goro_script_lp_finish(Fighter *fighter);
void goro_script_lp_pair_start(Fighter *fighter);
void goro_script_lp_phase_repeat(Fighter *fighter);
void goro_script_opponent_reaction(Fighter *fighter, int reaction);
void goro_script_punch_tail(Fighter *fighter);
void goro_script_quake_land(Fighter *fighter);
void goro_script_roundhouse_finish(Fighter *fighter);
void goro_script_roundhouse_start(Fighter *fighter);
void goro_script_spike_tail(Fighter *fighter);
void goro_script_stomp_return(Fighter *fighter);
void goro_script_stomp_victim_hold(Fighter *victim);
void goro_script_uppercut_victim_slide(Fighter *victim);
void goro_stomp_hold_separation(Fighter *fighter);
void goro_victim_finish_getup(Fighter *victim);
void goro_victim_getup(Fighter *victim);
void jarek_ball_land(Fighter *fighter);
void jarek_blade_spawn(Fighter *fighter, int task_type);
void jarek_first_heart(Fighter *fighter);
void jarek_first_victim_done(Fighter *victim);
void jarek_first_victim_end(Fighter *victim);
void jarek_first_victim_impact(Fighter *victim);
void jarek_first_victim_rip(Fighter *victim);
void jarek_quake_tail(Fighter *fighter);
void jarek_script_air_finish(Fighter *fighter);
void jarek_script_close_hk_tail(Fighter *fighter);
void jarek_script_close_hp_tail(Fighter *fighter);
void jarek_script_combo_hp_tail(Fighter *fighter);
void jarek_script_current_reaction(Fighter *fighter, int reaction);
void jarek_script_kick_finish(Fighter *fighter);
void jarek_script_lk_return(Fighter *fighter);
void jarek_script_low_kick_avoid(Fighter *fighter);
void jarek_script_low_kick_clear(Fighter *fighter);
void jarek_script_lp_end(Fighter *fighter);
void jarek_script_lp_return(Fighter *fighter);
void jarek_script_lp_select(Fighter *fighter);
void jarek_script_opponent_reaction(Fighter *fighter, int reaction);
void jarek_script_roundhouse_finish(Fighter *fighter);
void jarek_script_roundhouse_start(Fighter *fighter);
void jarek_script_spike_tail(Fighter *fighter);
void jarek_script_sweep_tail(Fighter *fighter);
void jarek_script_weapon_draw_finish(Fighter *fighter);
void jarek_second_glow(Fighter *fighter);
void jarek_second_handoff(Fighter *fighter);
void jarek_second_head_fall(Fighter *fighter, int task_type);
void jarek_second_victim_hold(Fighter *fighter);
void jarek_weapon_upper_contact(Fighter *fighter);
void jax_air_throw_fall(Fighter *fighter);
void jax_air_throw_getup(Fighter *victim);
void jax_air_throw_release(Fighter *fighter);
void jax_dash_punch_contact(Fighter *fighter);
void jax_dash_victim_getup(Fighter *victim);
void jax_fatality_1_first_arm(Fighter *victim);
void jax_fatality_1_grab(Fighter *victim);
void jax_fatality_1_second_arm(Fighter *victim);
void jax_fatality_1_victory(Fighter *owner);
void jax_fatality_2_approach(Fighter *fighter);
void jax_fatality_2_finish(Fighter *victim);
void jax_fatality_2_flash(Fighter *fighter, int task_type);
void jax_fatality_2_sparks(Fighter *fighter, int task_type);
void jax_fatality_2_victim_pause(Fighter *victim);
void jax_fatality_task_end(Fighter *victim);
void jax_lp_slam_first(Fighter *fighter);
void jax_lp_slam_second(Fighter *fighter);
void jax_lp_slam_third(Fighter *fighter);
void jax_lp_throw_return(Fighter *fighter);
void jax_missile_spawn(Fighter *fighter, int task_type);
void jax_script_air_finish(Fighter *fighter);
void jax_script_close_hk_tail(Fighter *fighter);
void jax_script_close_hp_tail(Fighter *fighter);
void jax_script_combo_hp_tail(Fighter *fighter);
void jax_script_finish(Fighter *fighter);
void jax_script_lk_return(Fighter *fighter);
void jax_script_opponent_reaction(Fighter *fighter, int reaction);
void jax_script_peer_reaction(Fighter *fighter, int reaction);
void jax_script_roundhouse_start(Fighter *fighter);
void jax_script_roundhouse_tail(Fighter *fighter);
void jax_script_spike_tail(Fighter *fighter);
void jax_script_weapon_draw_finish(Fighter *fighter);
void jax_throw_grunt(void);
void kai_aerial_punch_contact(Fighter *fighter);
void kai_first_owner_grab(Fighter *fighter);
void kai_first_owner_spin(Fighter *fighter);
void kai_first_owner_tear(Fighter *fighter);
void kai_first_task_end(Fighter *actor);
void kai_first_torso_203(Fighter *torso);
void kai_first_torso_impact(Fighter *torso);
void kai_first_victim_71(Fighter *victim);
void kai_first_victim_sever(Fighter *victim);
void kai_first_victim_turn(Fighter *victim);
void kai_ground_skull_spawn(Fighter *fighter);
void kai_handstand_crouch_return(Fighter *fighter);
void kai_handstand_rear_contact(Fighter *fighter);
void kai_handstand_spin_begin(Fighter *fighter);
void kai_handstand_victim_tail(Fighter *fighter);
void kai_lunge_kick_contact(Fighter *fighter);
void kai_rising_skull_finish(Fighter *fighter);
void kai_rising_skull_spawn(Fighter *fighter);
void kai_script_air_finish(Fighter *fighter);
void kai_script_close_hk_tail(Fighter *fighter);
void kai_script_close_hp_tail(Fighter *fighter);
void kai_script_combo_hp_tail(Fighter *fighter);
void kai_script_contact_b10_continuation(Fighter *fighter);
void kai_script_current_reaction(Fighter *fighter, int reaction);
void kai_script_draw_equip(Fighter *fighter);
void kai_script_finish(Fighter *fighter);
void kai_script_kick_finish(Fighter *fighter);
void kai_script_lk_return(Fighter *fighter);
void kai_script_lp_extended_finish(Fighter *fighter);
void kai_script_lp_select(Fighter *fighter);
void kai_script_lp_short_finish(Fighter *fighter);
void kai_script_opponent_reaction(Fighter *fighter, int reaction);
void kai_script_roundhouse_finish(Fighter *fighter);
void kai_script_roundhouse_start(Fighter *fighter);
void kai_script_spike_tail(Fighter *fighter);
void kai_script_stage_event(Fighter *fighter);
void kai_second_child_walk(Fighter *fighter);
void kai_second_event(Fighter *fighter, int selector);
void kai_second_hover(Fighter *fighter, int task_type);
void kai_second_skull(Fighter *fighter, int task_type);
void kai_second_victim_fall(Fighter *victim);
void kai_weapon_back_contact(Fighter *fighter);
void kai_weapon_face_peer(Fighter *fighter);
void kai_weapon_victim_getup(Fighter *fighter);
void kitana_script_air_diagonal_tail(Fighter *fighter);
void kitana_script_air_getup(Fighter *victim);
void kitana_script_air_hp_tail(Fighter *fighter);
void kitana_script_air_kick_tail(Fighter *fighter);
void kitana_script_air_lk_tail(Fighter *fighter);
void kitana_script_air_lp_tail(Fighter *fighter);
void kitana_script_air_release(Fighter *fighter);
void kitana_script_cartwheel_tail(Fighter *fighter);
void kitana_script_close_hp_tail(Fighter *fighter);
void kitana_script_combo_hp_tail(Fighter *fighter);
void kitana_script_crouch_lk_tail(Fighter *fighter);
void kitana_script_draw_equip(Fighter *fighter);
void kitana_script_first_release(Fighter *fighter);
void kitana_script_kick_tail(Fighter *fighter);
void kitana_script_knee_tail(Fighter *fighter);
void kitana_script_leg_grab_tail(Fighter *fighter);
void kitana_script_leg_victim_end(Fighter *victim);
void kitana_script_lk_return(Fighter *fighter);
void kitana_script_reaction(Fighter *fighter, int reaction);
void kitana_script_ring_flash(Fighter *fighter, int task_type);
void kitana_script_ring_spawn(Fighter *fighter, int task_type);
void kitana_script_roundhouse_start(Fighter *fighter);
void kitana_script_roundhouse_tail(Fighter *fighter);
void kitana_script_second_branch(Fighter *fighter);
void kitana_script_second_cycle_end(Fighter *fighter);
void kitana_script_second_open(Fighter *fighter);
void kitana_script_second_pause(Fighter *fighter);
void kitana_script_second_repeat(Fighter *fighter);
void kitana_script_second_victim_end(Fighter *victim);
void kitana_script_spike_tail(Fighter *fighter);
void kitana_script_square_wave_tail(Fighter *fighter);
void kitana_script_sweep_tail(Fighter *fighter);
void kitana_script_throw_getup(Fighter *victim);
void kitana_script_weapon_hp_tail(Fighter *fighter);
void kitana_script_weapon_lp_tail(Fighter *fighter);
void kitana_script_weapon_victim_getup(Fighter *victim);
void kitana_shadow_install_physics(Fighter *fighter);
void kitana_shadow_task_low(Fighter *fighter, int task_type);
void kitana_shadow_task_up(Fighter *fighter, int task_type);
void kitana_uppercut_script(Fighter *fighter);
void liu_kang_bicycle_script(Fighter *fighter);
void liu_kang_flying_kick_script(Fighter *fighter);
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
void liu_kang_liu_script_kick_tail(Fighter *fighter);
void liu_kang_liu_script_knee_tail(Fighter *fighter);
void liu_kang_liu_script_land(Fighter *fighter);
void liu_kang_liu_script_lk_continue(Fighter *fighter);
void liu_kang_liu_script_lk_damage(Fighter *fighter);
void liu_kang_liu_script_lk_thrown_tail(Fighter *victim);
void liu_kang_liu_script_lp_getup(Fighter *victim);
void liu_kang_liu_script_reaction(Fighter *fighter, int reaction);
void liu_kang_liu_script_roundhouse_start(Fighter *fighter);
void liu_kang_liu_script_roundhouse_tail(Fighter *fighter);
void liu_kang_liu_script_scene_event(Fighter *fighter, int selector);
void liu_kang_liu_script_spawn_air(Fighter *fighter, int task_type);
void liu_kang_liu_script_spawn_high(Fighter *fighter, int task_type);
void liu_kang_liu_script_spawn_low(Fighter *fighter, int task_type);
void liu_kang_liu_script_spike_tail(Fighter *fighter);
void liu_kang_liu_script_sweep_tail(Fighter *fighter);
void liu_kang_liu_script_sword_blend(Fighter *fighter);
unsigned liu_kang_liu_script_sword_gate(Fighter *fighter);
void liu_kang_uppercut_script(Fighter *fighter);
void mk4_mmove_air_getup(Fighter *victim);
void mk4_mmove_lk_owner_ready(Fighter *fighter);
void mk4_mmove_side_getup(Fighter *fighter);
void mk4_mmove_thrown_finish(Fighter *fighter);
void mk4_script_avoid_clear(Fighter *fighter);
void mk4_script_avoid_set(Fighter *fighter);
void mk4_script_disarm_self(Fighter *fighter);
void mk4_script_jab_contact(Fighter *fighter);
unsigned mk4_script_jab_gate(Fighter *fighter);
void mk4_script_landing_motion(Fighter *fighter);
void mk4_script_restore_motion(Fighter *fighter);
void mk4_script_save_motion(Fighter *fighter);
void mk4_script_scene_task(Fighter *fighter, int task_type);
void mk4_script_touchable(Fighter *fighter);
void native_air_straight_land(Fighter *fighter);
void native_standard_sweep(Fighter *fighter);
void native_uppercut_default(Fighter *fighter);
void noob_saibot_script_air_finish(Fighter *fighter);
void noob_saibot_script_air_throw_continue(Fighter *fighter);
void noob_saibot_script_air_throw_end(Fighter *fighter);
void noob_saibot_script_close_hk_tail(Fighter *fighter);
void noob_saibot_script_close_hp_tail(Fighter *fighter);
void noob_saibot_script_combo_hp_tail(Fighter *fighter);
void noob_saibot_script_finish(Fighter *fighter);
void noob_saibot_script_kick_finish(Fighter *fighter);
void noob_saibot_script_lk_return(Fighter *fighter);
void noob_saibot_script_lp_end(Fighter *fighter);
void noob_saibot_script_lp_reaction(Fighter *fighter, int reaction);
void noob_saibot_script_lp_return(Fighter *fighter);
void noob_saibot_script_lp_select(Fighter *fighter);
void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction);
void noob_saibot_script_roundhouse_finish(Fighter *fighter);
void noob_saibot_script_roundhouse_start(Fighter *fighter);
void noob_saibot_script_spike_tail(Fighter *fighter);
void noob_saibot_script_teleport_land(Fighter *fighter);
void noob_saibot_script_teleport_slam_drop(Fighter *fighter);
void noob_saibot_script_teleport_slam_fall(Fighter *fighter);
void noob_saibot_script_teleport_vanish(Fighter *fighter);
void noob_saibot_script_weapon_draw_finish(Fighter *fighter);
void noob_saibot_shadow_install_physics(Fighter *fighter);
void noob_saibot_shadow_task_low(Fighter *fighter, int task_type);
void noob_saibot_shadow_task_up(Fighter *fighter, int task_type);
void noob_saibot_star_spawn(Fighter *fighter, int task_type);
void quan_chi_fatality_beat(Fighter *fighter);
void quan_chi_fatality_cue_40(Fighter *fighter);
void quan_chi_fatality_hit_179(Fighter *fighter);
void quan_chi_fatality_hit_241(Fighter *fighter);
void quan_chi_fatality_leg(Fighter *fighter);
void quan_chi_script_air_finish(Fighter *fighter);
void quan_chi_script_air_getup(Fighter *victim);
void quan_chi_script_air_land(Fighter *fighter);
void quan_chi_script_air_release(Fighter *fighter);
void quan_chi_script_close_hk_tail(Fighter *fighter);
void quan_chi_script_close_hp_tail(Fighter *fighter);
void quan_chi_script_combo_hp_tail(Fighter *fighter);
void quan_chi_script_current_reaction(Fighter *fighter, int reaction);
void quan_chi_script_draw_equip(Fighter *fighter);
void quan_chi_script_finish(Fighter *fighter);
void quan_chi_script_flip_kick_tail(Fighter *fighter);
void quan_chi_script_getup_roll(Fighter *fighter);
void quan_chi_script_kick_finish(Fighter *fighter);
void quan_chi_script_lk_return(Fighter *fighter);
void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction);
void quan_chi_script_roundhouse_finish(Fighter *fighter);
void quan_chi_script_roundhouse_start(Fighter *fighter);
void quan_chi_script_skull_release(Fighter *fighter);
void quan_chi_script_spike_tail(Fighter *fighter);
void quan_chi_script_stomp_victim(Fighter *victim);
void quan_chi_script_sweep_finish(Fighter *fighter);
void quan_chi_script_teleport_land(Fighter *fighter);
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
void raiden_script_kick_tail(Fighter *fighter);
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
unsigned raiden_script_torpedo_query(Fighter *fighter);
void raiden_script_torpedo_step(Fighter *fighter);
void raiden_script_torpedo_victim_motion(Fighter *victim);
void raiden_script_victory_effect(Fighter *fighter);
void raiden_script_weapon_victim_getup(Fighter *victim);
void raiden_uppercut_script(Fighter *fighter);
void reiko_first_head_59(Fighter *piece);
void reiko_first_head_83(Fighter *piece);
void reiko_first_left_arm_56(Fighter *piece);
void reiko_first_left_arm_land(Fighter *piece);
void reiko_first_piece_end(Fighter *actor);
void reiko_first_right_arm_44(Fighter *piece);
void reiko_first_right_arm_land(Fighter *piece);
void reiko_first_tear(Fighter *victim);
void reiko_first_torso_29(Fighter *piece);
void reiko_first_torso_end(Fighter *piece);
void reiko_first_victim_118(Fighter *victim);
void reiko_first_victim_165(Fighter *victim);
void reiko_first_victory(Fighter *fighter);
void reiko_script_air_finish(Fighter *fighter);
void reiko_script_close_hk_tail(Fighter *fighter);
void reiko_script_close_hp_tail(Fighter *fighter);
void reiko_script_combo_hp_tail(Fighter *fighter);
void reiko_script_finish(Fighter *fighter);
void reiko_script_kick_finish(Fighter *fighter);
void reiko_script_lk_return(Fighter *fighter);
void reiko_script_lp_end(Fighter *fighter);
void reiko_script_lp_reaction(Fighter *fighter, int reaction);
void reiko_script_lp_return(Fighter *fighter);
void reiko_script_lp_select(Fighter *fighter);
void reiko_script_opponent_reaction(Fighter *fighter, int reaction);
void reiko_script_roundhouse_finish(Fighter *fighter);
void reiko_script_roundhouse_start(Fighter *fighter);
void reiko_script_spike_tail(Fighter *fighter);
void reiko_script_star_victim_tail(Fighter *fighter);
void reiko_script_teleport_land(Fighter *fighter);
void reiko_script_teleport_slam_drop(Fighter *fighter);
void reiko_script_teleport_slam_fall(Fighter *fighter);
void reiko_script_teleport_vanish(Fighter *fighter);
void reiko_script_weapon_draw_finish(Fighter *fighter);
void reiko_second_owner_park(Fighter *fighter);
void reiko_second_stars(Fighter *fighter, int task_type);
void reiko_second_victim_end(Fighter *victim);
void reiko_second_victim_fall(Fighter *victim);
void reiko_star_spawn(Fighter *fighter, int task_type);
void reptile_acid_spawn(Fighter *fighter, int task_type);
void reptile_acid_victim_tail(Fighter *victim);
void reptile_dash_victim_getup(Fighter *fighter);
void reptile_first_owner_bite(Fighter *fighter);
void reptile_first_owner_cycle(Fighter *fighter);
void reptile_first_owner_loop(Fighter *fighter);
void reptile_first_victim_bite(Fighter *victim);
void reptile_first_victim_collapse(Fighter *victim);
void reptile_first_victim_shake(Fighter *victim);
void reptile_script_acid_drool(Fighter *fighter);
void reptile_script_air_finish(Fighter *fighter);
void reptile_script_close_hk_tail(Fighter *fighter);
void reptile_script_close_hp_tail(Fighter *fighter);
void reptile_script_combo_hp_tail(Fighter *fighter);
void reptile_script_dash_contact(Fighter *fighter);
void reptile_script_draw_equip(Fighter *fighter);
void reptile_script_finish(Fighter *fighter);
void reptile_script_head_close(Fighter *fighter);
void reptile_script_head_open(Fighter *fighter);
void reptile_script_head_wide(Fighter *fighter);
void reptile_script_invisibility(Fighter *fighter);
void reptile_script_kick_tail(Fighter *fighter);
void reptile_script_lk_return(Fighter *fighter);
void reptile_script_lp_reaction(Fighter *fighter, int reaction);
void reptile_script_lp_return(Fighter *fighter);
void reptile_script_opponent_reaction(Fighter *fighter, int reaction);
void reptile_script_roll_tail(Fighter *fighter);
void reptile_script_roundhouse_finish(Fighter *fighter);
void reptile_script_roundhouse_start(Fighter *fighter);
void reptile_script_spike_tail(Fighter *fighter);
void reptile_second_acid(Fighter *fighter, int task_type);
void reptile_second_event(Fighter *fighter, int selector);
void reptile_second_face(Fighter *fighter);
void reptile_second_finish(Fighter *victim);
void reptile_second_hover(Fighter *fighter, int task_type);
void reptile_second_melt(Fighter *victim);
void reptile_weapon_face_peer(Fighter *fighter);
void reptile_weapon_flurry(Fighter *fighter);
void reptile_weapon_getup_back(Fighter *fighter);
void reptile_weapon_stagger_tail(Fighter *fighter);
void retail_fan_cycle(Fighter *fighter);
void retail_fan_tail(Fighter *fighter);
void retail_fan_victim_end(Fighter *fighter);
void retail_fan_victory(Fighter *fighter);
void scorpion_fire_script(Fighter *fighter);
void scorpion_script_air_diagonal_tail(Fighter *fighter);
void scorpion_script_air_hp_tail(Fighter *fighter);
void scorpion_script_air_kick_tail(Fighter *fighter);
void scorpion_script_air_land(Fighter *fighter);
void scorpion_script_air_lk_tail(Fighter *fighter);
void scorpion_script_air_lp_tail(Fighter *fighter);
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
void scorpion_script_crouch_lk_tail(Fighter *fighter);
void scorpion_script_draw_equip(Fighter *fighter);
void scorpion_script_fire_decor(Fighter *fighter);
void scorpion_script_fire_flame(Fighter *fighter);
void scorpion_script_kick_tail(Fighter *fighter);
void scorpion_script_knee_tail(Fighter *fighter);
void scorpion_script_lk_damage(Fighter *fighter);
void scorpion_script_lk_rebase(Fighter *fighter);
void scorpion_script_lk_return(Fighter *fighter);
void scorpion_script_lp_return(Fighter *fighter);
void scorpion_script_opponent_reaction(Fighter *fighter, int reaction);
void scorpion_script_reaction(Fighter *fighter, int reaction);
void scorpion_script_roundhouse_start(Fighter *fighter);
void scorpion_script_roundhouse_tail(Fighter *fighter);
void scorpion_script_spawn_spear(Fighter *fighter, int type);
void scorpion_script_spear_pull(Fighter *fighter);
void scorpion_script_spear_return(Fighter *fighter);
void scorpion_script_spike_tail(Fighter *fighter);
void scorpion_script_sweep_tail(Fighter *fighter);
void scorpion_script_teleport_land(Fighter *fighter);
void scorpion_script_teleport_motion(Fighter *fighter);
void scorpion_script_teleport_victim_tail(Fighter *victim);
void scorpion_script_transform_effect(Fighter *fighter, int type);
void scorpion_script_weapon_ready(Fighter *fighter);
void scorpion_spear_stun_script(Fighter *fighter);
void scorpion_spin_script(Fighter *fighter);
void scorpion_teleport_script(Fighter *fighter);
void scorpion_uppercut_script(Fighter *fighter);
void shinnok_fatality_reappear(Fighter *fighter);
void shinnok_fatality_victory(Fighter *fighter);
void shinnok_first_hand_squeeze(Fighter *hand);
void shinnok_first_hidden(Fighter *fighter);
void shinnok_first_squeeze_cycle(Fighter *fighter);
void shinnok_first_task_end(Fighter *actor);
void shinnok_first_vanished(Fighter *fighter);
void shinnok_first_victim_crush(Fighter *victim);
void shinnok_first_victim_held(Fighter *victim);
void shinnok_first_victim_squeeze(Fighter *victim);
void shinnok_script_air_finish(Fighter *fighter);
void shinnok_script_close_hk_tail(Fighter *fighter);
void shinnok_script_close_hp_tail(Fighter *fighter);
void shinnok_script_combo_hp_tail(Fighter *fighter);
void shinnok_script_current_reaction(Fighter *fighter, int reaction);
void shinnok_script_draw_equip(Fighter *fighter);
void shinnok_script_kick_finish(Fighter *fighter);
void shinnok_script_lp_return(Fighter *fighter);
void shinnok_script_opponent_reaction(Fighter *fighter, int reaction);
void shinnok_script_roundhouse_finish(Fighter *fighter);
void shinnok_script_roundhouse_start(Fighter *fighter);
void shinnok_script_spike_tail(Fighter *fighter);
void shinnok_script_sweep_finish(Fighter *fighter);
void shinnok_second_clap(Fighter *hand);
void shinnok_second_event(Fighter *fighter, int selector);
void shinnok_second_gore(Fighter *hand);
void shinnok_second_hands(Fighter *fighter);
void shinnok_second_vanish(Fighter *fighter);
void shinnok_second_vanished(Fighter *fighter);
void shinnok_weapon_back_getup(Fighter *fighter);
void shinnok_weapon_crouch_return(Fighter *fighter);
void shinnok_weapon_sweep_finish(Fighter *fighter);
void shinnok_weapon_sweep_getup(Fighter *fighter);
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
void sonya_script_kick_tail(Fighter *fighter);
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
void sonya_uppercut_script(Fighter *fighter);
void sub_zero_script_air_diagonal_tail(Fighter *fighter);
void sub_zero_script_air_getup(Fighter *victim);
void sub_zero_script_air_hp_tail(Fighter *fighter);
void sub_zero_script_air_kick_tail(Fighter *fighter);
void sub_zero_script_air_land(Fighter *fighter);
void sub_zero_script_air_lk_tail(Fighter *fighter);
void sub_zero_script_air_lp_tail(Fighter *fighter);
void sub_zero_script_air_release(Fighter *fighter);
void sub_zero_script_breath_approach(Fighter *fighter);
void sub_zero_script_breath_freeze(Fighter *fighter, int type);
void sub_zero_script_breath_object(Fighter *fighter, int type);
void sub_zero_script_breath_shatter(Fighter *fighter);
void sub_zero_script_breath_slide(Fighter *fighter);
void sub_zero_script_breath_victim_end(Fighter *victim);
void sub_zero_script_close_hp_tail(Fighter *fighter);
void sub_zero_script_combo_hp_tail(Fighter *fighter);
void sub_zero_script_crouch_lk_tail(Fighter *fighter);
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
void sub_zero_script_kick_tail(Fighter *fighter);
void sub_zero_script_knee_tail(Fighter *fighter);
void sub_zero_script_lk_damage(Fighter *fighter);
void sub_zero_script_lk_owner_return(Fighter *fighter);
void sub_zero_script_lk_rebase(Fighter *fighter);
void sub_zero_script_lk_victim_rise(Fighter *victim);
void sub_zero_script_lp_owner_return(Fighter *fighter);
void sub_zero_script_reaction(Fighter *fighter, int reaction);
void sub_zero_script_ready(Fighter *fighter);
void sub_zero_script_roundhouse_start(Fighter *fighter);
void sub_zero_script_roundhouse_tail(Fighter *fighter);
void sub_zero_script_slide_return(Fighter *fighter);
void sub_zero_script_spawn_ice(Fighter *fighter, int task_type);
void sub_zero_script_spike_tail(Fighter *fighter);
void sub_zero_script_sweep_tail(Fighter *fighter);
void sub_zero_script_victory(Fighter *fighter);
void sub_zero_slide_script(Fighter *fighter);
void sub_zero_uppercut_script(Fighter *fighter);
void tanya_air_fireball_fall(Fighter *fighter);
void tanya_air_fireball_spawn(Fighter *fighter, int task_type);
void tanya_blocked_recoil_land(Fighter *fighter);
void tanya_boomerang_release(Fighter *fighter);
void tanya_drill_kick_contact(Fighter *fighter);
void tanya_fireball_spawn(Fighter *fighter, int task_type);
void tanya_first_owner_53(Fighter *fighter);
void tanya_first_owner_drift(Fighter *fighter);
void tanya_first_owner_loop(Fighter *fighter);
void tanya_first_owner_replays(Fighter *fighter);
void tanya_first_victim_burst(Fighter *fighter);
void tanya_first_victim_repeat(Fighter *fighter);
void tanya_first_victim_swell(Fighter *fighter);
void tanya_script_air_finish(Fighter *fighter);
void tanya_script_close_hk_tail(Fighter *fighter);
void tanya_script_close_hp_tail(Fighter *fighter);
void tanya_script_combo_hp_tail(Fighter *fighter);
void tanya_script_current_reaction(Fighter *fighter, int reaction);
void tanya_script_draw_equip(Fighter *fighter);
void tanya_script_finish(Fighter *fighter);
void tanya_script_kick_finish(Fighter *fighter);
void tanya_script_lk_return(Fighter *fighter);
void tanya_script_lp_return(Fighter *fighter);
void tanya_script_opponent_reaction(Fighter *fighter, int reaction);
void tanya_script_roundhouse_finish(Fighter *fighter);
void tanya_script_roundhouse_start(Fighter *fighter);
void tanya_script_spike_tail(Fighter *fighter);
void tanya_second_event(Fighter *fighter, int selector);
void tanya_second_finish(Fighter *fighter);
void tanya_second_height(Fighter *fighter);
void tanya_second_owner_cue(Fighter *fighter, int task_type);
void tanya_second_victim_cue(Fighter *fighter, int task_type);
void tanya_second_victim_pause(Fighter *fighter);
void tanya_weapon_face_peer(Fighter *fighter);
void weapon_air_frames_native(Fighter *fighter, unsigned selector);

bool retail_native_vm(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  if (fighter->roster < 0 || fighter->roster >= MK4_ROSTER_COUNT) {
    return false;
  }
  unsigned context = fighter->retail_vm.native_owner_set ? fighter->retail_vm.native_owner : (unsigned)fighter->roster;
  if (context >= MK4_ROSTER_COUNT) {
    return false;
  }
  uint32_t owner = 1u << context;
  switch (kind) {
    case RETAIL_CALL:
      switch (address) {
        case 0x406ba0:
          if (owner & 0x800u) {
            kai_second_child_walk(fighter);
            return true;
          }
          return false;
        case 0x40eb80:
          if (owner & 0x400u) {
            reptile_script_head_close(fighter);
            return true;
          }
          return false;
        case 0x40ec40:
          if (owner & 0x400u) {
            reptile_script_head_open(fighter);
            return true;
          }
          return false;
        case 0x40ed20:
          if (owner & 0x400u) {
            reptile_script_head_wide(fighter);
            return true;
          }
          return false;
        case 0x40ee00:
          if (owner & 0x400u) {
            reptile_script_acid_drool(fighter);
            return true;
          }
          return false;
        case 0x410380:
          if (owner & 0x2u) {
            raiden_script_victory_effect(fighter);
            return true;
          }
          return false;
        case 0x416ae0:
          if (owner & 0x1u) {
            scorpion_script_fire_decor(fighter);
            return true;
          }
          return false;
        case 0x416d40:
          if (owner & 0x1u) {
            scorpion_script_fire_flame(fighter);
            return true;
          }
          return false;
        case 0x41db70:
          if (owner & 0x40u) {
            shinnok_second_gore(fighter);
            return true;
          }
          return false;
        case 0x41dba0:
          if (owner & 0x40u) {
            shinnok_second_gore(fighter);
            return true;
          }
          return false;
        case 0x41dbd0:
          if (owner & 0x40u) {
            shinnok_second_gore(fighter);
            return true;
          }
          return false;
        case 0x41de30:
          if (owner & 0x1000u) {
            jarek_second_glow(fighter);
            return true;
          }
          return false;
        case 0x427b70:
          if (owner & 0x2u) {
            raiden_script_second_particles(fighter);
            return true;
          }
          return false;
        case 0x42b680:
          if (owner & 0x8000u) {
            goro_script_lp_pair_start(fighter);
            return true;
          }
          return false;
        case 0x43a5c0:
          if (owner & 0x8000u) {
            goro_quake_resting_weapons();
            return true;
          }
          return false;
        case 0x46a950:
          if (owner & 0x1000u) {
            jarek_script_low_kick_clear(fighter);
            return true;
          }
          if (owner & 0x3efffu) {
            mk4_script_avoid_clear(fighter);
            return true;
          }
          return false;
        case 0x46a970:
          if (owner & 0x1000u) {
            jarek_script_low_kick_avoid(fighter);
            return true;
          }
          if (owner & 0x3efffu) {
            mk4_script_avoid_set(fighter);
            return true;
          }
          return false;
        case 0x46ddd0:
          if (owner & 0x3ffffu) {
            mk4_script_jab_contact(fighter);
            return true;
          }
          return false;
        case 0x46de60:
          if (owner & 0x3ffffu) {
            fighter->retail_vm.result = mk4_script_jab_gate(fighter);
            return true;
          }
          return false;
        case 0x47d720:
          if (owner & 0x2u) {
            raiden_script_torpedo_step(fighter);
            return true;
          }
          return false;
        case 0x48a1f0:
          if (owner & 0x1u) {
            scorpion_script_beast_voice(fighter);
            return true;
          }
          return false;
        case 0x48a220:
          if (owner & 0x2000u) {
            jax_throw_grunt();
            return true;
          }
          return false;
        case 0x48f6e0:
          if (owner & 0x8u) {
            mk4_script_save_motion(fighter);
            return true;
          }
          return false;
        case 0x48f750:
          if (owner & 0xau) {
            mk4_script_restore_motion(fighter);
            return true;
          }
          return false;
        case 0x48f9f0:
          if (owner & 0x8u) {
            liu_kang_liu_script_fatal_align(fighter);
            return true;
          }
          return false;
        case 0x490290:
          if (owner & 0x10004u) {
            mk4_script_touchable(fighter);
            return true;
          }
          return false;
        case 0x494520:
          if (owner & 0x10004u) {
            mk4_script_disarm_self(fighter);
            return true;
          }
          return false;
        default:
          return false;
      }
    case RETAIL_TAIL:
      switch (address) {
        case 0x41b080:
          if (owner & 0x10u) {
            sub_zero_script_breath_victim_end(fighter);
            return true;
          }
          return false;
        case 0x41b440:
          if (owner & 0x10u) {
            sub_zero_script_breath_approach(fighter);
            return true;
          }
          return false;
        case 0x41b450:
          if (owner & 0x10u) {
            sub_zero_script_breath_slide(fighter);
            return true;
          }
          return false;
        case 0x41b570:
          if (owner & 0x10u) {
            sub_zero_script_breath_shatter(fighter);
            return true;
          }
          return false;
        case 0x41c4b0:
          if (owner & 0x80u) {
            reiko_second_victim_fall(fighter);
            return true;
          }
          return false;
        case 0x41c4e0:
          if (owner & 0x80u) {
            reiko_second_victim_end(fighter);
            return true;
          }
          return false;
        case 0x41dfc0:
          if (owner & 0x1000u) {
            jarek_second_handoff(fighter);
            return true;
          }
          return false;
        case 0x41e0a0:
          if (owner & 0x1000u) {
            jarek_second_victim_hold(fighter);
            return true;
          }
          return false;
        case 0x41ea50:
          if (owner & 0x1u) {
            scorpion_script_beast_begin(fighter);
            return true;
          }
          return false;
        case 0x41ebd0:
          if (owner & 0x1u) {
            scorpion_script_beast_attack(fighter);
            return true;
          }
          return false;
        case 0x41ec70:
          if (owner & 0x1u) {
            scorpion_script_beast_victim_pieces(fighter);
            return true;
          }
          return false;
        case 0x41f1b0:
          if (owner & 0x80u) {
            reiko_second_owner_park(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_beast_park(fighter);
            return true;
          }
          return false;
        case 0x42bdf0:
          if (owner & 0x2ffe0u) {
            retail_vm_enter(fighter, 0x4de318, fighter->retail_vm.finish);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_lk_rebase(fighter);
            return true;
          }
          return false;
        case 0x42be00:
          if (owner & 0x2ffe0u) {
            retail_vm_enter(fighter, 0x4de328, fighter->retail_vm.finish);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_lk_damage(fighter);
            return true;
          }
          return false;
        case 0x42be10:
          if (owner & 0x4000u) {
            cage_script_lk_throw_return(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x40u) {
            mk4_mmove_lk_owner_ready(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_lk_return(fighter);
            return true;
          }
          return false;
        case 0x42bf00:
          if (owner & 0x10000u) {
            kitana_script_lk_return(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_lk_return(fighter);
            return true;
          }
          return false;
        case 0x42c0b0:
          if (owner & 0x8u) {
            liu_kang_liu_script_lk_continue(fighter);
            return true;
          }
          return false;
        case 0x42c0d0:
          if (owner & 0x8u) {
            liu_kang_liu_script_lk_damage(fighter);
            return true;
          }
          return false;
        case 0x42c170:
          if (owner & 0x8u) {
            retail_vm_enter(fighter, 0x4de478, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x42c200:
          if (owner & 0x10u) {
            sub_zero_script_lk_rebase(fighter);
            return true;
          }
          return false;
        case 0x42c220:
          if (owner & 0x10u) {
            sub_zero_script_lk_damage(fighter);
            return true;
          }
          return false;
        case 0x42c300:
          if (owner & 0x10u) {
            sub_zero_script_lk_owner_return(fighter);
            return true;
          }
          return false;
        case 0x42c770:
          if (owner & 0x8000u) {
            retail_vm_enter(fighter, 0x4de7e8, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x42c7b0:
          if (owner & 0x8000u) {
            goro_script_lp_phase_repeat(fighter);
            return true;
          }
          return false;
        case 0x42c7f0:
          if (owner & 0x2000u) {
            retail_vm_enter(fighter, 0x4de4f0, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x42c820:
          if (owner & 0x2000u) {
            jax_lp_slam_first(fighter);
            return true;
          }
          return false;
        case 0x42c8c0:
          if (owner & 0x2000u) {
            jax_lp_slam_second(fighter);
            return true;
          }
          return false;
        case 0x42c8f0:
          if (owner & 0x2000u) {
            jax_lp_slam_third(fighter);
            return true;
          }
          return false;
        case 0x42c9f0:
          if (owner & 0x2000u) {
            jax_lp_throw_return(fighter);
            return true;
          }
          return false;
        case 0x42cb80:
          if (owner & 0x20u) {
            fujin_script_lp_select(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_lp_select(fighter);
            return true;
          }
          return false;
        case 0x42cbc0:
          if (owner & 0x20u) {
            fujin_script_lp_extended_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_lp_extended_finish(fighter);
            return true;
          }
          return false;
        case 0x42cc00:
          if (owner & 0x1000u) {
            jarek_script_lp_select(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_lp_select(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_lp_select(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_lp_select(fighter);
            return true;
          }
          return false;
        case 0x42cc40:
          if (owner & 0x1000u) {
            jarek_script_lp_end(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_lp_end(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_lp_end(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_lp_end(fighter);
            return true;
          }
          return false;
        case 0x42ce60:
          if (owner & 0x4000u) {
            cage_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_lp_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_lp_return(fighter);
            return true;
          }
          return false;
        case 0x42ce80:
          if (owner & 0x20u) {
            fujin_script_lp_short_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_lp_short_finish(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_lp_return(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_lp_owner_return(fighter);
            return true;
          }
          return false;
        case 0x43afc0:
          if (owner & 0x8000u) {
            goro_script_punch_tail(fighter);
            return true;
          }
          return false;
        case 0x43b1a0:
          if (owner & 0x8000u) {
            goro_script_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x43b370:
          if (owner & 0x8000u) {
            goro_script_leap_continue(fighter);
            return true;
          }
          return false;
        case 0x43b590:
          if (owner & 0x8000u) {
            goro_script_stomp_return(fighter);
            return true;
          }
          return false;
        case 0x43b8c0:
          if (owner & 0x8000u) {
            goro_script_quake_land(fighter);
            return true;
          }
          return false;
        case 0x43c500:
          if (owner & 0x8u) {
            liu_kang_liu_script_fatal_setup(fighter);
            return true;
          }
          return false;
        case 0x43c820:
          if (owner & 0x40u) {
            shinnok_second_vanish(fighter);
            return true;
          }
          return false;
        case 0x43ca30:
          if (owner & 0x40u) {
            shinnok_second_clap(fighter);
            return true;
          }
          return false;
        case 0x43d460:
          if (owner & 0x3ffffu) {
            retail_fan_tail(fighter);
            return true;
          }
          return false;
        case 0x43d510:
          if (owner & 0x3ffffu) {
            retail_fan_cycle(fighter);
            return true;
          }
          return false;
        case 0x43d530:
          if (owner & 0x3ffffu) {
            retail_fan_cycle(fighter);
            return true;
          }
          return false;
        case 0x43d6f0:
          if (owner & 0x3ffffu) {
            retail_fan_victory(fighter);
            return true;
          }
          return false;
        case 0x43db50:
          if (owner & 0x3ffffu) {
            retail_fan_victim_end(fighter);
            return true;
          }
          return false;
        case 0x43f510:
          if (owner & 0x10000u) {
            kitana_script_first_release(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_first_release(fighter);
            return true;
          }
          return false;
        case 0x441140:
          if (owner & 0x2u) {
            raiden_script_first_release(fighter);
            return true;
          }
          return false;
        case 0x441180:
          if (owner & 0x2u) {
            raiden_script_first_end(fighter);
            return true;
          }
          return false;
        case 0x443930:
          if (owner & 0x1u) {
            scorpion_script_burn_begin(fighter);
            return true;
          }
          return false;
        case 0x443a30:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_start(fighter);
            return true;
          }
          return false;
        case 0x443a70:
          if (owner & 0x1u) {
            scorpion_script_burn_repeat1(fighter);
            return true;
          }
          return false;
        case 0x443b40:
          if (owner & 0x1u) {
            scorpion_script_burn_repeat2(fighter);
            return true;
          }
          return false;
        case 0x443c10:
          if (owner & 0x1u) {
            scorpion_script_burn_victory(fighter);
            return true;
          }
          return false;
        case 0x444000:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_cue(fighter);
            return true;
          }
          return false;
        case 0x444030:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_repeat(fighter);
            return true;
          }
          return false;
        case 0x444060:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_blend(fighter);
            return true;
          }
          return false;
        case 0x444100:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_impact(fighter);
            return true;
          }
          return false;
        case 0x444150:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_settle(fighter);
            return true;
          }
          return false;
        case 0x444190:
          if (owner & 0x1u) {
            scorpion_script_burn_victim_end(fighter);
            return true;
          }
          return false;
        case 0x445150:
          if (owner & 0x10u) {
            sub_zero_script_first_begin(fighter);
            return true;
          }
          return false;
        case 0x445220:
          if (owner & 0x10u) {
            sub_zero_script_first_pull(fighter);
            return true;
          }
          return false;
        case 0x445240:
          if (owner & 0x10u) {
            sub_zero_script_first_cues1(fighter);
            return true;
          }
          return false;
        case 0x445280:
          if (owner & 0x10u) {
            sub_zero_script_first_cues2(fighter);
            return true;
          }
          return false;
        case 0x4452b0:
          if (owner & 0x10u) {
            sub_zero_script_first_cues3(fighter);
            return true;
          }
          return false;
        case 0x4452e0:
          if (owner & 0x10u) {
            sub_zero_script_first_cues4(fighter);
            return true;
          }
          return false;
        case 0x445310:
          if (owner & 0x10u) {
            sub_zero_script_first_head(fighter);
            return true;
          }
          return false;
        case 0x445510:
          if (owner & 0x10u) {
            sub_zero_script_first_victim_blends(fighter);
            return true;
          }
          return false;
        case 0x4456c0:
          if (owner & 0x10u) {
            sub_zero_script_first_victim_collapse(fighter);
            return true;
          }
          return false;
        case 0x445700:
          if (owner & 0x10u) {
            sub_zero_script_first_victim_end(fighter);
            return true;
          }
          return false;
        case 0x446f80:
          if (owner & 0x1000u) {
            jarek_first_heart(fighter);
            return true;
          }
          return false;
        case 0x447310:
          if (owner & 0x1000u) {
            jarek_first_victim_rip(fighter);
            return true;
          }
          return false;
        case 0x4473b0:
          if (owner & 0x1000u) {
            jarek_first_victim_impact(fighter);
            return true;
          }
          return false;
        case 0x447410:
          if (owner & 0x1000u) {
            jarek_first_victim_end(fighter);
            return true;
          }
          return false;
        case 0x448480:
          if (owner & 0x4000u) {
            cage_script_fatality_1_signal(fighter);
            return true;
          }
          return false;
        case 0x4487b0:
          if (owner & 0x4000u) {
            cage_first_victim_59(fighter);
            return true;
          }
          return false;
        case 0x4487f0:
          if (owner & 0x4000u) {
            cage_first_victim_131(fighter);
            return true;
          }
          return false;
        case 0x448830:
          if (owner & 0x4000u) {
            cage_first_victim_148(fighter);
            return true;
          }
          return false;
        case 0x448870:
          if (owner & 0x4000u) {
            cage_first_victim_166(fighter);
            return true;
          }
          return false;
        case 0x4488b0:
          if (owner & 0x4000u) {
            cage_first_victim_185(fighter);
            return true;
          }
          return false;
        case 0x4488f0:
          if (owner & 0x4000u) {
            cage_first_victim_206(fighter);
            return true;
          }
          return false;
        case 0x448930:
          if (owner & 0x4000u) {
            cage_first_sever(fighter);
            return true;
          }
          return false;
        case 0x448c80:
          if (owner & 0x4000u) {
            cage_first_torso_half(fighter);
            return true;
          }
          return false;
        case 0x448d20:
          if (owner & 0x4000u) {
            cage_first_torso_123(fighter);
            return true;
          }
          return false;
        case 0x448d70:
          if (owner & 0x4000u) {
            cage_first_torso_finish(fighter);
            return true;
          }
          return false;
        case 0x449620:
          if (owner & 0x2000u) {
            jax_fatality_1_victory(fighter);
            return true;
          }
          return false;
        case 0x449890:
          if (owner & 0x2000u) {
            jax_fatality_1_grab(fighter);
            return true;
          }
          return false;
        case 0x4498d0:
          if (owner & 0x2000u) {
            jax_fatality_1_first_arm(fighter);
            return true;
          }
          return false;
        case 0x449940:
          if (owner & 0x2000u) {
            jax_fatality_1_second_arm(fighter);
            return true;
          }
          return false;
        case 0x44aeb0:
          if (owner & 0x800u) {
            kai_first_owner_grab(fighter);
            return true;
          }
          return false;
        case 0x44af20:
          if (owner & 0x800u) {
            kai_first_owner_spin(fighter);
            return true;
          }
          return false;
        case 0x44af30:
          if (owner & 0x800u) {
            kai_first_owner_tear(fighter);
            return true;
          }
          return false;
        case 0x44b260:
          if (owner & 0x800u) {
            kai_first_victim_71(fighter);
            return true;
          }
          return false;
        case 0x44b290:
          if (owner & 0x800u) {
            kai_first_victim_turn(fighter);
            return true;
          }
          return false;
        case 0x44b2c0:
          if (owner & 0x800u) {
            kai_first_victim_sever(fighter);
            return true;
          }
          return false;
        case 0x44b600:
          if (owner & 0x800u) {
            kai_first_torso_203(fighter);
            return true;
          }
          return false;
        case 0x44b650:
          if (owner & 0x800u) {
            kai_first_torso_impact(fighter);
            return true;
          }
          return false;
        case 0x44be10:
          if (owner & 0x100u) {
            quan_chi_fatality_cue_40(fighter);
            return true;
          }
          return false;
        case 0x44be40:
          if (owner & 0x100u) {
            quan_chi_fatality_leg(fighter);
            return true;
          }
          return false;
        case 0x44be60:
          if (owner & 0x100u) {
            quan_chi_fatality_hit_179(fighter);
            return true;
          }
          return false;
        case 0x44beb0:
          if (owner & 0x100u) {
            quan_chi_fatality_hit_241(fighter);
            return true;
          }
          return false;
        case 0x44bf00:
          if (owner & 0x100u) {
            retail_vm_enter(fighter, 0x4e1548, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x44bf10:
          if (owner & 0x100u) {
            quan_chi_fatality_beat(fighter);
            return true;
          }
          return false;
        case 0x44d4a0:
          if (owner & 0x200u) {
            tanya_first_owner_53(fighter);
            return true;
          }
          return false;
        case 0x44d4e0:
          if (owner & 0x200u) {
            tanya_first_owner_drift(fighter);
            return true;
          }
          return false;
        case 0x44d510:
          if (owner & 0x200u) {
            tanya_first_owner_replays(fighter);
            return true;
          }
          return false;
        case 0x44d530:
          if (owner & 0x200u) {
            tanya_first_owner_loop(fighter);
            return true;
          }
          return false;
        case 0x44d6e0:
          if (owner & 0x200u) {
            tanya_first_victim_swell(fighter);
            return true;
          }
          return false;
        case 0x44d750:
          if (owner & 0x200u) {
            tanya_first_victim_repeat(fighter);
            return true;
          }
          return false;
        case 0x44d7a0:
          if (owner & 0x200u) {
            tanya_first_victim_burst(fighter);
            return true;
          }
          return false;
        case 0x44f5e0:
          if (owner & 0x20u) {
            fujin_first_loop_start(fighter);
            return true;
          }
          return false;
        case 0x44f620:
          if (owner & 0x20u) {
            fujin_first_cycle(fighter);
            return true;
          }
          return false;
        case 0x44f6f0:
          if (owner & 0x20u) {
            fujin_first_shoot(fighter);
            return true;
          }
          return false;
        case 0x4528a0:
          if (owner & 0x80u) {
            reiko_first_victory(fighter);
            return true;
          }
          return false;
        case 0x452ac0:
          if (owner & 0x80u) {
            reiko_first_tear(fighter);
            return true;
          }
          return false;
        case 0x452bb0:
          if (owner & 0x80u) {
            reiko_first_victim_118(fighter);
            return true;
          }
          return false;
        case 0x452c10:
          if (owner & 0x80u) {
            reiko_first_victim_165(fighter);
            return true;
          }
          return false;
        case 0x452c50:
          if (owner & 0x80u) {
            reiko_first_piece_end(fighter);
            return true;
          }
          return false;
        case 0x452e50:
          if (owner & 0x80u) {
            reiko_first_torso_29(fighter);
            return true;
          }
          return false;
        case 0x452e90:
          if (owner & 0x80u) {
            reiko_first_torso_end(fighter);
            return true;
          }
          return false;
        case 0x452f00:
          if (owner & 0x80u) {
            reiko_first_head_59(fighter);
            return true;
          }
          return false;
        case 0x452f40:
          if (owner & 0x80u) {
            reiko_first_head_83(fighter);
            return true;
          }
          return false;
        case 0x452f90:
          if (owner & 0x80u) {
            reiko_first_piece_end(fighter);
            return true;
          }
          return false;
        case 0x452fe0:
          if (owner & 0x80u) {
            reiko_first_left_arm_56(fighter);
            return true;
          }
          return false;
        case 0x453020:
          if (owner & 0x80u) {
            reiko_first_left_arm_land(fighter);
            return true;
          }
          return false;
        case 0x453040:
          if (owner & 0x80u) {
            reiko_first_piece_end(fighter);
            return true;
          }
          return false;
        case 0x453090:
          if (owner & 0x80u) {
            reiko_first_right_arm_44(fighter);
            return true;
          }
          return false;
        case 0x4530d0:
          if (owner & 0x80u) {
            reiko_first_right_arm_land(fighter);
            return true;
          }
          return false;
        case 0x4530f0:
          if (owner & 0x80u) {
            reiko_first_piece_end(fighter);
            return true;
          }
          return false;
        case 0x453ae0:
          if (owner & 0x400u) {
            reptile_first_owner_cycle(fighter);
            return true;
          }
          return false;
        case 0x453b20:
          if (owner & 0x400u) {
            reptile_first_owner_bite(fighter);
            return true;
          }
          return false;
        case 0x453ba0:
          if (owner & 0x400u) {
            reptile_first_owner_loop(fighter);
            return true;
          }
          return false;
        case 0x454130:
          if (owner & 0x400u) {
            reptile_first_victim_bite(fighter);
            return true;
          }
          return false;
        case 0x4541b0:
          if (owner & 0x400u) {
            reptile_first_victim_shake(fighter);
            return true;
          }
          return false;
        case 0x4541e0:
          if (owner & 0x400u) {
            reptile_first_victim_collapse(fighter);
            return true;
          }
          return false;
        case 0x455110:
          if (owner & 0x40u) {
            shinnok_first_vanished(fighter);
            return true;
          }
          return false;
        case 0x455140:
          if (owner & 0x40u) {
            shinnok_first_hidden(fighter);
            return true;
          }
          return false;
        case 0x4551b0:
          if (owner & 0x40u) {
            shinnok_fatality_reappear(fighter);
            return true;
          }
          return false;
        case 0x455230:
          if (owner & 0x40u) {
            shinnok_fatality_victory(fighter);
            return true;
          }
          return false;
        case 0x455330:
          if (owner & 0x40u) {
            shinnok_second_vanished(fighter);
            return true;
          }
          return false;
        case 0x455360:
          if (owner & 0x40u) {
            shinnok_second_hands(fighter);
            return true;
          }
          return false;
        case 0x4558b0:
          if (owner & 0x40u) {
            shinnok_first_hand_squeeze(fighter);
            return true;
          }
          return false;
        case 0x4558d0:
          if (owner & 0x40u) {
            shinnok_first_squeeze_cycle(fighter);
            return true;
          }
          return false;
        case 0x455ae0:
          if (owner & 0x40u) {
            shinnok_first_victim_held(fighter);
            return true;
          }
          return false;
        case 0x455b60:
          if (owner & 0x40u) {
            shinnok_first_victim_squeeze(fighter);
            return true;
          }
          return false;
        case 0x455b80:
          if (owner & 0x40u) {
            shinnok_first_squeeze_cycle(fighter);
            return true;
          }
          return false;
        case 0x455ba0:
          if (owner & 0x40u) {
            shinnok_first_victim_crush(fighter);
            return true;
          }
          return false;
        case 0x45fc40:
          if (owner & 0x20u) {
            fujin_wind_crouch_return(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_handstand_crouch_return(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_weapon_crouch_return(fighter);
            return true;
          }
          return false;
        case 0x464960:
          if (owner & 0x2u) {
            raiden_script_second_blend(fighter);
            return true;
          }
          return false;
        case 0x464a60:
          if (owner & 0x2u) {
            raiden_script_second_effect(fighter);
            return true;
          }
          return false;
        case 0x464a90:
          if (owner & 0x2u) {
            retail_vm_enter(fighter, 0x4e5728, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x464aa0:
          if (owner & 0x2u) {
            raiden_script_second_release(fighter);
            return true;
          }
          return false;
        case 0x464bb0:
          if (owner & 0x2u) {
            raiden_script_second_end(fighter);
            return true;
          }
          return false;
        case 0x464ee0:
          if (owner & 0x2u) {
            raiden_script_second_victim_end(fighter);
            return true;
          }
          return false;
        case 0x4650b0:
          if (owner & 0x200u) {
            tanya_second_height(fighter);
            return true;
          }
          return false;
        case 0x465130:
          if (owner & 0x200u) {
            tanya_second_finish(fighter);
            return true;
          }
          return false;
        case 0x465250:
          if (owner & 0x200u) {
            retail_vm_enter(fighter, 0x4e5860, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x4658a0:
          if (owner & 0x4000u) {
            cage_script_fatality_2_continue(fighter);
            return true;
          }
          return false;
        case 0x4658f0:
          if (owner & 0x4000u) {
            cage_second_repeated_uppercuts(fighter);
            return true;
          }
          return false;
        case 0x465b10:
          if (owner & 0x4000u) {
            cage_second_victim_finish(fighter);
            return true;
          }
          return false;
        case 0x465e10:
          if (owner & 0x10000u) {
            kitana_script_second_open(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_open(fighter);
            return true;
          }
          return false;
        case 0x465e70:
          if (owner & 0x10000u) {
            kitana_script_second_repeat(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_repeat(fighter);
            return true;
          }
          return false;
        case 0x465ea0:
          if (owner & 0x10000u) {
            kitana_script_second_branch(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_branch(fighter);
            return true;
          }
          return false;
        case 0x465ed0:
          if (owner & 0x10000u) {
            kitana_script_second_pause(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_pause(fighter);
            return true;
          }
          return false;
        case 0x466000:
          if (owner & 0x10000u) {
            kitana_script_second_cycle_end(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_cycle_end(fighter);
            return true;
          }
          return false;
        case 0x466190:
          if (owner & 0x10000u) {
            kitana_script_second_victim_end(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_second_victim_end(fighter);
            return true;
          }
          return false;
        case 0x466a90:
          if (owner & 0x2000u) {
            jax_fatality_2_approach(fighter);
            return true;
          }
          return false;
        case 0x466bb0:
          if (owner & 0x2000u) {
            jax_fatality_2_finish(fighter);
            return true;
          }
          return false;
        case 0x466df0:
          if (owner & 0x4000u) {
            cage_second_victim_pause(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_fatality_2_victim_pause(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_second_victim_pause(fighter);
            return true;
          }
          return false;
        case 0x467fc0:
          if (owner & 0x4000u) {
            cage_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_spike_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_spike_tail(fighter);
            return true;
          }
          return false;
        case 0x468640:
          if (owner & 0x1u) {
            scorpion_fire_script(fighter);
            return true;
          }
          return false;
        case 0x4698b0:
          if (owner & 0x400u) {
            reptile_script_invisibility(fighter);
            return true;
          }
          return false;
        case 0x46a500:
          if (owner & 0x800u) {
            kai_second_victim_fall(fighter);
            return true;
          }
          return false;
        case 0x46a650:
          if (owner & 0x20u) {
            fujin_wind_recovery(fighter);
            return true;
          }
          return false;
        case 0x46a6b0:
          if (owner & 0x20u) {
            fujin_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_kick_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x46a6e0:
          if (owner & 0x11024u) {
            retail_vm_reverse_return(fighter);
            return true;
          }
          return false;
        case 0x46a760:
          if (owner & 0x8000u) {
            goro_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_kick_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_kick_tail(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            retail_vm_enter(fighter, 0x4e5c70, fighter->retail_vm.finish);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_kick_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_kick_finish(fighter);
            return true;
          }
          return false;
        case 0x46a790:
          if (owner & 0x4000u) {
            cage_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_kick_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_kick_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x46a850:
          if (owner & 0x11224u) {
            retail_vm_reverse_return(fighter);
            return true;
          }
          return false;
        case 0x46a8d0:
          if (owner & 0x2a4c9u) {
            retail_vm_reverse_return(fighter);
            return true;
          }
          return false;
        case 0x46a9f0:
          if (owner & 0x2000u) {
            jax_air_throw_fall(fighter);
            return true;
          }
          return false;
        case 0x46aa40:
          if (owner & 0x2000u) {
            jax_air_throw_release(fighter);
            return true;
          }
          return false;
        case 0x46aae0:
          if (owner & 0x8000u) {
            goro_script_air_release(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_release(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_release(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_release(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_release(fighter);
            return true;
          }
          return false;
        case 0x46ab20:
          if (owner & 0x8000u) {
            goro_script_air_land(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_land(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_land(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_land(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_land(fighter);
            return true;
          }
          return false;
        case 0x46ac10:
          if (owner & 0x20000u) {
            noob_saibot_script_air_throw_continue(fighter);
            return true;
          }
          return false;
        case 0x46ac90:
          if (owner & 0x20000u) {
            retail_vm_enter(fighter, 0x4e5eb0, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x46aca0:
          if (owner & 0x20000u) {
            noob_saibot_script_air_throw_end(fighter);
            return true;
          }
          return false;
        case 0x46aed0:
          if (owner & 0x10000u) {
            kitana_script_air_release(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_release(fighter);
            return true;
          }
          return false;
        case 0x46b740:
          if (owner & 0x10000u) {
            kitana_script_crouch_lk_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_crouch_lk_tail(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_crouch_lk_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_crouch_lk_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_crouch_lk_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_crouch_lk_tail(fighter);
            return true;
          }
          return false;
        case 0x46bb60:
          if (owner & 0x4000u) {
            cage_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_close_hk_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_knee_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_close_hk_tail(fighter);
            return true;
          }
          return false;
        case 0x46bf10:
          if (owner & 0x4000u) {
            cage_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_close_hp_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_close_hp_tail(fighter);
            return true;
          }
          return false;
        case 0x46c2c0:
          if (owner & 0x4000u) {
            cage_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_combo_hp_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_combo_hp_tail(fighter);
            return true;
          }
          return false;
        case 0x46c7f0:
          if (owner & 0x4000u) {
            cage_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_air_kick_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_kick_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_kick_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x46c820:
          if (owner & 0xd540u) {
            native_air_straight_land(fighter);
            return true;
          }
          return false;
        case 0x46c970:
          if (owner & 0x20u) {
            fujin_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_air_kick_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_kick_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_air_finish(fighter);
            return true;
          }
          return false;
        case 0x46cde0:
          if (owner & 0x10000u) {
            kitana_uppercut_script(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_uppercut_script(fighter);
            return true;
          }
          if (owner & 0x2fee0u) {
            native_uppercut_default(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_uppercut_script(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_uppercut_script(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_uppercut_script(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_uppercut_script(fighter);
            return true;
          }
          return false;
        case 0x46d260:
          if (owner & 0x4000u) {
            cage_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_draw_equip(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_weapon_draw_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_weapon_draw_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_weapon_draw_finish(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_weapon_draw_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_draw_equip(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_draw_equip(fighter);
            return true;
          }
          return false;
        case 0x46dd00:
          if (owner & 0x3ffffu) {
            retail_vm_enter(fighter, 0x4e6330, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x46dd10:
          if (owner & 0x3ffffu) {
            retail_vm_enter(fighter, 0x4e6360, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x46dd20:
          if (owner & 0x3ffffu) {
            retail_vm_enter(fighter, 0x4e63b0, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x46df90:
          if (owner & 0x1000u) {
            jarek_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_air_lk_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_lk_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_air_finish(fighter);
            return true;
          }
          return false;
        case 0x46e070:
          if (owner & 0x2000u) {
            jax_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_lk_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_lk_tail(fighter);
            return true;
          }
          return false;
        case 0x46e150:
          if (owner & 0x4000u) {
            cage_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_air_lk_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_lk_tail(fighter);
            return true;
          }
          return false;
        case 0x46e3d0:
          if (owner & 0x1000u) {
            jarek_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_air_diagonal_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_diagonal_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_air_finish(fighter);
            return true;
          }
          return false;
        case 0x46e4d0:
          if (owner & 0x8000u) {
            goro_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_diagonal_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_diagonal_tail(fighter);
            return true;
          }
          return false;
        case 0x46e5c0:
          if (owner & 0x4000u) {
            cage_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_air_diagonal_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_diagonal_tail(fighter);
            return true;
          }
          return false;
        case 0x46e730:
          if (owner & 0x4000u) {
            cage_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_hp_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_air_finish(fighter);
            return true;
          }
          return false;
        case 0x46eab0:
          if (owner & 0x37fffu) {
            weapon_air_frames_native(fighter, fighter->retail_vm.selector);
            return true;
          }
          return false;
        case 0x46ec10:
          if (owner & 0x37fffu) {
            weapon_air_frames_native(fighter, fighter->retail_vm.selector);
            return true;
          }
          return false;
        case 0x46eea0:
          if (owner & 0x4000u) {
            cage_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_air_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_lp_tail(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_air_finish(fighter);
            return true;
          }
          return false;
        case 0x46efd0:
          if (owner & 0x20u) {
            fujin_script_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_finish(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_finish(fighter);
            return true;
          }
          return false;
        case 0x46f080:
          if (owner & 0x4000u) {
            cage_script_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            native_standard_sweep(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_sweep_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_sweep_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_sweep_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_sweep_tail(fighter);
            return true;
          }
          return false;
        case 0x46f220:
          if (owner & 0x4000u) {
            cage_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_roundhouse_start(fighter);
            return true;
          }
          return false;
        case 0x46f270:
          if (owner & 0x4000u) {
            cage_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_roundhouse_tail(fighter);
            return true;
          }
          return false;
        case 0x46f390:
          if (owner & 0x20u) {
            fujin_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_roundhouse_start(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_roundhouse_start(fighter);
            return true;
          }
          return false;
        case 0x46f3e0:
          if (owner & 0x20u) {
            fujin_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_roundhouse_tail(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_roundhouse_finish(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_roundhouse_finish(fighter);
            return true;
          }
          return false;
        case 0x470420:
          if (owner & 0x37decu) {
            mk4_mmove_thrown_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_thrown_return(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_weapon_ready(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_ready(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_lp_return(fighter);
            return true;
          }
          return false;
        case 0x4712e0:
          if (owner & 0x1000u) {
            retail_vm_enter(fighter, 0x4e6a40, fighter->retail_vm.finish);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_victory(fighter);
            return true;
          }
          return false;
        case 0x478e20:
          if (owner & 0x800u) {
            kai_lunge_kick_contact(fighter);
            return true;
          }
          return false;
        case 0x479b70:
          if (owner & 0x4000u) {
            cage_script_cpu_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x479f00:
          if (owner & 0x4000u) {
            cage_script_powered_punch_tail(fighter);
            return true;
          }
          return false;
        case 0x47a3b0:
          if (owner & 0x100u) {
            quan_chi_script_flip_kick_tail(fighter);
            return true;
          }
          return false;
        case 0x47a6d0:
          if (owner & 0x400u) {
            reptile_script_dash_contact(fighter);
            return true;
          }
          return false;
        case 0x47ab50:
          if (owner & 0x2000u) {
            jax_dash_punch_contact(fighter);
            return true;
          }
          return false;
        case 0x47b010:
          if (owner & 0x400u) {
            reptile_script_roll_tail(fighter);
            return true;
          }
          return false;
        case 0x47b600:
          if (owner & 0x1000u) {
            jarek_quake_tail(fighter);
            return true;
          }
          return false;
        case 0x47b6c0:
          if (owner & 0x10000u) {
            kitana_script_cartwheel_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_cartwheel_tail(fighter);
            return true;
          }
          return false;
        case 0x47ba20:
          if (owner & 0x200u) {
            tanya_drill_kick_contact(fighter);
            return true;
          }
          return false;
        case 0x47bd40:
          if (owner & 0x800u) {
            kai_aerial_punch_contact(fighter);
            return true;
          }
          return false;
        case 0x47bf90:
          if (owner & 0x20u) {
            fujin_dive_contact(fighter);
            return true;
          }
          return false;
        case 0x47bfd0:
          if (owner & 0x20u) {
            fujin_dive_land(fighter);
            return true;
          }
          return false;
        case 0x47c0a0:
          if (owner & 0x20000u) {
            noob_saibot_script_teleport_vanish(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_teleport_vanish(fighter);
            return true;
          }
          return false;
        case 0x47c360:
          if (owner & 0x20000u) {
            noob_saibot_script_teleport_slam_fall(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_teleport_slam_fall(fighter);
            return true;
          }
          return false;
        case 0x47c390:
          if (owner & 0x20000u) {
            noob_saibot_script_teleport_slam_drop(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_teleport_slam_drop(fighter);
            return true;
          }
          return false;
        case 0x47c580:
          if (owner & 0x800u) {
            kai_script_stage_event(fighter);
            return true;
          }
          return false;
        case 0x47c880:
          if (owner & 0x800u) {
            kai_handstand_spin_begin(fighter);
            return true;
          }
          return false;
        case 0x47cc10:
          if (owner & 0x800u) {
            retail_vm_enter(fighter, 0x4e8358, fighter->retail_vm.finish);
            return true;
          }
          return false;
        case 0x47cc50:
          if (owner & 0x800u) {
            kai_script_contact_b10_continuation(fighter);
            return true;
          }
          return false;
        case 0x47ccc0:
          if (owner & 0x800u) {
            kai_handstand_rear_contact(fighter);
            return true;
          }
          return false;
        case 0x47d680:
          if (owner & 0x2u) {
            raiden_script_torpedo_miss(fighter);
            return true;
          }
          return false;
        case 0x47d7f0:
          if (owner & 0x2u) {
            raiden_script_torpedo_contact(fighter);
            return true;
          }
          return false;
        case 0x47dae0:
          if (owner & 0x4000u) {
            cage_power_land(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_ball_land(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_blocked_recoil_land(fighter);
            return true;
          }
          return false;
        case 0x47dc40:
          if (owner & 0x10u) {
            sub_zero_slide_script(fighter);
            return true;
          }
          return false;
        case 0x47de00:
          if (owner & 0x10u) {
            sub_zero_script_slide_return(fighter);
            return true;
          }
          return false;
        case 0x47e0a0:
          if (owner & 0x10000u) {
            kitana_script_leg_grab_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_leg_grab_tail(fighter);
            return true;
          }
          return false;
        case 0x47e1d0:
          if (owner & 0x10000u) {
            kitana_script_square_wave_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_square_wave_tail(fighter);
            return true;
          }
          return false;
        case 0x47eb30:
          if (owner & 0x8u) {
            liu_kang_bicycle_script(fighter);
            return true;
          }
          return false;
        case 0x47ed40:
          if (owner & 0x8u) {
            liu_kang_liu_script_land(fighter);
            return true;
          }
          return false;
        case 0x47ee00:
          if (owner & 0x8u) {
            liu_kang_flying_kick_script(fighter);
            return true;
          }
          return false;
        case 0x47ef10:
          if (owner & 0x8u) {
            liu_kang_liu_script_land(fighter);
            return true;
          }
          return false;
        case 0x47f060:
          if (owner & 0x20u) {
            fujin_triple_fast(fighter);
            return true;
          }
          return false;
        case 0x47f070:
          if (owner & 0x20u) {
            fujin_triple_contact(fighter);
            return true;
          }
          return false;
        case 0x47fcf0:
          if (owner & 0x1u) {
            scorpion_teleport_script(fighter);
            return true;
          }
          return false;
        case 0x47ffc0:
          if (owner & 0x20000u) {
            noob_saibot_script_teleport_land(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_teleport_land(fighter);
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_teleport_land(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_teleport_land(fighter);
            return true;
          }
          return false;
        case 0x4803a0:
          if (owner & 0x20u) {
            fujin_grab_control_start(fighter);
            return true;
          }
          return false;
        case 0x4807b0:
          if (owner & 0x1u) {
            scorpion_spin_script(fighter);
            return true;
          }
          return false;
        case 0x481590:
          if (owner & 0x400u) {
            reptile_second_melt(fighter);
            return true;
          }
          return false;
        case 0x4816f0:
          if (owner & 0x20u) {
            fujin_second_burst(fighter);
            return true;
          }
          return false;
        case 0x4817c0:
          if (owner & 0x20u) {
            fujin_second_finish(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_second_finish(fighter);
            return true;
          }
          return false;
        case 0x482280:
          if (owner & 0x400u) {
            reptile_weapon_stagger_tail(fighter);
            return true;
          }
          return false;
        case 0x4824f0:
          if (owner & 0x8000u) {
            goro_script_uppercut_victim_slide(fighter);
            return true;
          }
          return false;
        case 0x482540:
          if (owner & 0x8000u) {
            goro_victim_getup(fighter);
            return true;
          }
          return false;
        case 0x482790:
          if (owner & 0x4000u) {
            cage_weapon_getup_back(fighter);
            return true;
          }
          if (owner & 0x8000u) {
            goro_victim_finish_getup(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_weapon_victim_getup(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_weapon_victim_getup(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_weapon_victim_getup(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_weapon_getup_back(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_weapon_victim_getup(fighter);
            return true;
          }
          return false;
        case 0x483090:
          if (owner & 0x40u) {
            shinnok_weapon_sweep_getup(fighter);
            return true;
          }
          return false;
        case 0x4830a0:
          if (owner & 0x40u) {
            shinnok_weapon_sweep_finish(fighter);
            return true;
          }
          return false;
        case 0x483df0:
          if (owner & 0x80u) {
            reiko_script_star_victim_tail(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_acid_victim_tail(fighter);
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_burn_tail(fighter);
            return true;
          }
          return false;
        case 0x484330:
          if (owner & 0x8000u) {
            goro_script_stomp_victim_hold(fighter);
            return true;
          }
          return false;
        case 0x484420:
          if (owner & 0x100u) {
            quan_chi_script_stomp_victim(fighter);
            return true;
          }
          return false;
        case 0x4847e0:
          if (owner & 0x10000u) {
            kitana_script_air_getup(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_air_getup(fighter);
            return true;
          }
          return false;
        case 0x484a30:
          if (owner & 0x2000u) {
            jax_air_throw_getup(fighter);
            return true;
          }
          return false;
        case 0x484b60:
          if (owner & 0x8000u) {
            goro_script_air_getup(fighter);
            return true;
          }
          if (owner & 0x3u) {
            mk4_mmove_air_getup(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_air_getup(fighter);
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_air_getup(fighter);
            return true;
          }
          return false;
        case 0x484c00:
          if (owner & 0x1u) {
            scorpion_script_teleport_victim_tail(fighter);
            return true;
          }
          return false;
        case 0x485510:
          if (owner & 0x2u) {
            raiden_script_lk_victim_end(fighter);
            return true;
          }
          return false;
        case 0x4858d0:
          if (owner & 0x10000u) {
            kitana_script_leg_victim_end(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_leg_victim_end(fighter);
            return true;
          }
          return false;
        case 0x485930:
          if (owner & 0x8u) {
            liu_kang_liu_script_lk_thrown_tail(fighter);
            return true;
          }
          return false;
        case 0x485ad0:
          if (owner & 0x20080u) {
            mk4_mmove_thrown_finish(fighter);
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_throw_getup(fighter);
            return true;
          }
          return false;
        case 0x485b10:
          if (owner & 0x8u) {
            liu_kang_liu_script_lp_getup(fighter);
            return true;
          }
          return false;
        case 0x485b50:
          if (owner & 0x10000u) {
            kitana_script_throw_getup(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_throw_getup(fighter);
            return true;
          }
          return false;
        case 0x486000:
          if (owner & 0x1u) {
            scorpion_script_spear_pull(fighter);
            return true;
          }
          return false;
        case 0x487220:
          if (owner & 0x10u) {
            sub_zero_script_lk_victim_rise(fighter);
            return true;
          }
          return false;
        case 0x487420:
          if (owner & 0x1u) {
            scorpion_spear_stun_script(fighter);
            return true;
          }
          return false;
        case 0x488590:
          if (owner & 0x2u) {
            raiden_script_staff_victim_tail(fighter);
            return true;
          }
          return false;
        case 0x488ac0:
          if (owner & 0x800u) {
            kai_handstand_victim_tail(fighter);
            return true;
          }
          return false;
        case 0x488b40:
          if (owner & 0x8u) {
            liu_kang_liu_script_bicycle_getup(fighter);
            return true;
          }
          if (owner & 0x3fda6u) {
            mk4_mmove_side_getup(fighter);
            return true;
          }
          return false;
        case 0x488b80:
          if (owner & 0x400u) {
            reptile_dash_victim_getup(fighter);
            return true;
          }
          return false;
        case 0x488ba0:
          if (owner & 0x20u) {
            fujin_wind_getup(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_weapon_back_getup(fighter);
            return true;
          }
          return false;
        case 0x488c00:
          if (owner & 0x4000u) {
            cage_weapon_getup_roll(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_dash_victim_getup(fighter);
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_getup_roll(fighter);
            return true;
          }
          return false;
        case 0x488c10:
          if (owner & 0x10000u) {
            kitana_script_weapon_victim_getup(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_weapon_victim_getup(fighter);
            return true;
          }
          return false;
        case 0x4958b0:
          if (owner & 0x4000u) {
            cage_weapon_upper_contact(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_weapon_upper_contact(fighter);
            return true;
          }
          return false;
        case 0x495ae0:
          if (owner & 0x800u) {
            kai_weapon_back_contact(fighter);
            return true;
          }
          return false;
        case 0x495db0:
          if (owner & 0x400u) {
            reptile_weapon_flurry(fighter);
            return true;
          }
          return false;
        case 0x496430:
          if (owner & 0x8u) {
            liu_kang_liu_script_sword_blend(fighter);
            return true;
          }
          return false;
        case 0x496770:
          if (owner & 0x10000u) {
            kitana_script_weapon_hp_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_weapon_hp_tail(fighter);
            return true;
          }
          return false;
        case 0x496970:
          if (owner & 0x4000u) {
            cage_weapon_back_lp_contact(fighter);
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_weapon_lp_tail(fighter);
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_back_lp_tail(fighter);
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_weapon_lp_tail(fighter);
            return true;
          }
          return false;
        case 0x497b60:
          if (owner & 0x200u) {
            tanya_boomerang_release(fighter);
            return true;
          }
          return false;
        case 0x498eb0:
          if (owner & 0x10000u) {
            kitana_shadow_install_physics(fighter);
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_shadow_install_physics(fighter);
            return true;
          }
          if (owner & 0x200u) {
            tanya_air_fireball_fall(fighter);
            return true;
          }
          return false;
        case 0x499000:
          if (owner & 0x800u) {
            kai_ground_skull_spawn(fighter);
            return true;
          }
          return false;
        case 0x499450:
          if (owner & 0x800u) {
            kai_rising_skull_spawn(fighter);
            return true;
          }
          return false;
        case 0x499470:
          if (owner & 0x800u) {
            kai_rising_skull_finish(fighter);
            return true;
          }
          return false;
        case 0x499780:
          if (owner & 0x100u) {
            quan_chi_script_skull_release(fighter);
            return true;
          }
          return false;
        case 0x49a480:
          if (owner & 0x20u) {
            fujin_bow_finish(fighter);
            return true;
          }
          return false;
        case 0x49a680:
          if (owner & 0x1u) {
            scorpion_script_spear_return(fighter);
            return true;
          }
          return false;
        case 0x49c010:
          if (owner & 0x8u) {
            liu_kang_liu_script_air_return(fighter);
            return true;
          }
          return false;
        case 0x49cb60:
          if (owner & 0x4000u) {
            cage_first_task_end(fighter);
            return true;
          }
          if (owner & 0x20u) {
            fujin_first_fire_end(fighter);
            return true;
          }
          if (owner & 0x1000u) {
            jarek_first_victim_done(fighter);
            return true;
          }
          if (owner & 0x2000u) {
            jax_fatality_task_end(fighter);
            return true;
          }
          if (owner & 0x800u) {
            kai_first_task_end(fighter);
            return true;
          }
          if (owner & 0x40u) {
            shinnok_first_task_end(fighter);
            return true;
          }
          return false;
        default:
          return false;
      }
    case RETAIL_SPAWN:
      switch (address) {
        case 0x412d80:
          if (owner & 0x2000u) {
            if (argument != 619u) {
              return false;
            }
            jax_fatality_2_sparks(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x412ec0:
          if (owner & 0x2000u) {
            if (argument != 619u) {
              return false;
            }
            jax_fatality_2_flash(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x413970:
          if (owner & 0x4000u) {
            if (argument != 619u) {
              return false;
            }
            cage_second_charge(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x413a10:
          if (owner & 0x4000u) {
            if (argument != 619u) {
              return false;
            }
            cage_second_flash(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41b860:
          if (owner & 0x10u) {
            if (argument != 0u) {
              return false;
            }
            sub_zero_script_breath_freeze(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41bbb0:
          if (owner & 0x10u) {
            if (argument != 0u) {
              return false;
            }
            sub_zero_script_breath_object(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41d260:
          if (owner & 0x80u) {
            if (argument != 0u) {
              return false;
            }
            reiko_second_stars(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41e0e0:
          if (owner & 0x1000u) {
            if (argument != 0u) {
              return false;
            }
            jarek_second_head_fall(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41e490:
          if (owner & 0x400u) {
            if (argument != 0u) {
              return false;
            }
            reptile_second_acid(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41ed80:
          if (owner & 0x1u) {
            if (argument != 0u) {
              return false;
            }
            scorpion_script_beast_gore(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x41ef20:
          if (owner & 0x20u) {
            if (argument != 38u) {
              return false;
            }
            fujin_second_blood(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x43c140:
          if (owner & 0x800u) {
            if (argument != 38u) {
              return false;
            }
            kai_second_skull(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x43c540:
          if (owner & 0x8u) {
            if (argument != 38u) {
              return false;
            }
            liu_kang_liu_script_fatal_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x450d40:
          if (owner & 0x1u) {
            if (argument != 0u) {
              return false;
            }
            scorpion_script_transform_effect(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x464bd0:
          if (owner & 0x12006u) {
            if (argument != 619u) {
              return false;
            }
            mk4_script_scene_task(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x465270:
          if (owner & 0x200u) {
            if (argument != 619u) {
              return false;
            }
            tanya_second_owner_cue(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x465390:
          if (owner & 0x200u) {
            if (argument != 619u) {
              return false;
            }
            tanya_second_victim_cue(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x46a280:
          if (owner & 0x800u) {
            if (argument != 38u) {
              return false;
            }
            kai_second_hover(fighter, (int)argument);
            return true;
          }
          if (owner & 0x400u) {
            if (argument != 38u) {
              return false;
            }
            reptile_second_hover(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x496e00:
          if (owner & 0x20000u) {
            if (argument != 82u) {
              return false;
            }
            noob_saibot_star_spawn(fighter, (int)argument);
            return true;
          }
          if (owner & 0x80u) {
            if (argument != 82u) {
              return false;
            }
            reiko_star_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x496fc0:
          if (owner & 0x2000u) {
            if (argument != 80u) {
              return false;
            }
            jax_missile_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x4970d0:
          if (owner & 0x4000u) {
            if (argument != 79u) {
              return false;
            }
            cage_orb_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x497580:
          if (owner & 0x1000u) {
            if (argument != 78u) {
              return false;
            }
            jarek_blade_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x498920:
          if (owner & 0x400u) {
            if (argument != 76u) {
              return false;
            }
            reptile_acid_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x498bb0:
          if (owner & 0x10000u) {
            if (argument != 75u) {
              return false;
            }
            kitana_shadow_task_low(fighter, (int)argument);
            return true;
          }
          if (owner & 0x20000u) {
            if (argument != 75u) {
              return false;
            }
            noob_saibot_shadow_task_low(fighter, (int)argument);
            return true;
          }
          if (owner & 0x200u) {
            if (argument != 75u) {
              return false;
            }
            tanya_fireball_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x498ed0:
          if (owner & 0x10000u) {
            if (argument != 75u) {
              return false;
            }
            kitana_shadow_task_up(fighter, (int)argument);
            return true;
          }
          if (owner & 0x20000u) {
            if (argument != 75u) {
              return false;
            }
            noob_saibot_shadow_task_up(fighter, (int)argument);
            return true;
          }
          if (owner & 0x200u) {
            if (argument != 75u) {
              return false;
            }
            tanya_air_fireball_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49a6a0:
          if (owner & 0x1u) {
            if (argument != 67u) {
              return false;
            }
            scorpion_script_spawn_spear(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49b9e0:
          if (owner & 0x10000u) {
            if (argument != 38u) {
              return false;
            }
            kitana_script_ring_flash(fighter, (int)argument);
            return true;
          }
          if (owner & 0x4u) {
            if (argument != 38u) {
              return false;
            }
            sonya_script_ring_flash(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49ba50:
          if (owner & 0x10000u) {
            if (argument != 68u) {
              return false;
            }
            kitana_script_ring_spawn(fighter, (int)argument);
            return true;
          }
          if (owner & 0x4u) {
            if (argument != 68u) {
              return false;
            }
            sonya_script_ring_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49bcb0:
          if (owner & 0x10u) {
            if (argument != 66u) {
              return false;
            }
            sub_zero_script_spawn_ice(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49be10:
          if (owner & 0x2u) {
            if (argument != 65u) {
              return false;
            }
            raiden_script_lightning_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49c0d0:
          if (owner & 0x8u) {
            if (argument != 64u) {
              return false;
            }
            liu_kang_liu_script_spawn_air(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49c110:
          if (owner & 0x8u) {
            if (argument != 64u) {
              return false;
            }
            liu_kang_liu_script_spawn_low(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49c160:
          if (owner & 0x8u) {
            if (argument != 64u) {
              return false;
            }
            liu_kang_liu_script_spawn_high(fighter, (int)argument);
            return true;
          }
          return false;
        case 0x49ca90:
          if (owner & 0x8000u) {
            if (argument != 83u) {
              return false;
            }
            goro_fireball_spawn(fighter, (int)argument);
            return true;
          }
          return false;
        default:
          return false;
      }
    case RETAIL_HANDLER:
      switch (address) {
        case 0x45ac30:
          if (owner & 0x20u) {
            fujin_second_event(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x800u) {
            kai_second_event(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_scene_event(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x400u) {
            reptile_second_event(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x40u) {
            shinnok_second_event(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x200u) {
            tanya_second_event(fighter, (int)(argument & 255u));
            return true;
          }
          return false;
        case 0x45b1f0:
          if (owner & 0x4000u) {
            cage_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_peer_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x800u) {
            kai_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x8u) {
            liu_kang_liu_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_lp_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_lp_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_lp_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x10u) {
            sub_zero_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_current_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          return false;
        case 0x45b230:
          if (owner & 0x4000u) {
            cage_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x20u) {
            fujin_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x8000u) {
            goro_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x1000u) {
            jarek_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x2000u) {
            jax_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x800u) {
            kai_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x10000u) {
            kitana_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x20000u) {
            noob_saibot_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x100u) {
            quan_chi_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x2u) {
            raiden_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x80u) {
            reiko_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x400u) {
            reptile_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x1u) {
            scorpion_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x40u) {
            shinnok_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x4u) {
            sonya_script_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          if (owner & 0x200u) {
            tanya_script_opponent_reaction(fighter, (int)(argument & 255u));
            return true;
          }
          return false;
        case 0x45b380:
          if (owner & 0x20u) {
            fujin_second_face(fighter);
            return true;
          }
          if (owner & 0x400u) {
            reptile_second_face(fighter);
            return true;
          }
          return false;
        default:
          return false;
      }
    case RETAIL_PREDICATE:
      switch (address) {
        case 0x46db00:
          if (owner & 0x8u) {
            fighter->retail_vm.result = liu_kang_liu_script_sword_gate(fighter);
            return true;
          }
          return false;
        case 0x46de60:
          if (owner & 0x3ffffu) {
            fighter->retail_vm.result = mk4_script_jab_gate(fighter);
            return true;
          }
          return false;
        case 0x48d450:
          if (owner & 0x2u) {
            fighter->retail_vm.result = raiden_script_torpedo_query(fighter);
            return true;
          }
          return false;
        default:
          return false;
      }
    case RETAIL_PHYSICS:
      switch (address) {
        case 0x4707e0:
          if (owner & 0x20492u) {
            fighter->body.physics = mk4_script_landing_motion;
            return true;
          }
          return false;
        case 0x47d6e0:
          if (owner & 0x2u) {
            fighter->body.physics = raiden_script_torpedo_motion;
            return true;
          }
          return false;
        case 0x480040:
          if (owner & 0x1u) {
            fighter->body.physics = scorpion_script_teleport_motion;
            return true;
          }
          return false;
        case 0x480e90:
          if (owner & 0x8u) {
            fighter->body.physics = liu_kang_liu_script_bicycle_motion;
            return true;
          }
          return false;
        case 0x480f40:
          if (owner & 0x8000u) {
            fighter->body.physics = goro_stomp_hold_separation;
            return true;
          }
          return false;
        case 0x4889b0:
          if (owner & 0x2u) {
            fighter->body.physics = raiden_script_torpedo_victim_motion;
            return true;
          }
          return false;
        case 0x490bd0:
          if (owner & 0x800u) {
            fighter->body.physics = kai_weapon_face_peer;
            return true;
          }
          if (owner & 0x400u) {
            fighter->body.physics = reptile_weapon_face_peer;
            return true;
          }
          if (owner & 0x200u) {
            fighter->body.physics = tanya_weapon_face_peer;
            return true;
          }
          return false;
        case 0x4913a0:
          if (owner & 0x3fffdu) {
            fighter->body.physics = fighter_friction;
            return true;
          }
          return false;
        default:
          return false;
      }
  }
  return false;
}

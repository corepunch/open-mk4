/* Typed native ABI registrations, derived from the reviewed local-EXE
 * contracts. Each native address selects one shared engine service;
 * no retail script instructions or animation frame rows are emitted. */
#include "retail_native.h"
#include "retail_vm.h"
#include "script.h"
#include "native_services.h"

unsigned liu_kang_liu_script_sword_gate(Fighter *fighter);
unsigned mk4_script_jab_gate(Fighter *fighter);
unsigned raiden_script_torpedo_query(Fighter *fighter);

bool retail_native_vm(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  if (fighter->roster < 0 || fighter->roster >= MK4_ROSTER_COUNT) {
    return false;
  }
  switch (kind) {
    case RETAIL_CALL:
      switch (address) {
        case 0x490350:
          retail_flip_facing(fighter);
          return true;
        case 0x48a350:
          retail_impact_voice(fighter);
          return true;
        case 0x406ba0:
          kai_second_child_walk(fighter);
          return true;
        case 0x40eb80:
          reptile_script_head_close(fighter);
          return true;
        case 0x40ec40:
          reptile_script_head_open(fighter);
          return true;
        case 0x40ed20:
          reptile_script_head_wide(fighter);
          return true;
        case 0x40ee00:
          reptile_script_acid_drool(fighter);
          return true;
        case 0x410380:
          raiden_script_victory_effect(fighter);
          return true;
        case 0x416ae0:
          scorpion_script_fire_decor(fighter);
          return true;
        case 0x416d40:
          scorpion_script_fire_flame(fighter);
          return true;
        case 0x41db70:
          shinnok_second_gore(fighter);
          return true;
        case 0x41dba0:
          shinnok_second_gore(fighter);
          return true;
        case 0x41dbd0:
          shinnok_second_gore(fighter);
          return true;
        case 0x41de30:
          jarek_second_glow(fighter);
          return true;
        case 0x427b70:
          raiden_script_second_particles(fighter);
          return true;
        case 0x42b680:
          retail_throw_pair_start(fighter);
          return true;
        case 0x43a5c0:
          retail_disturb_resting_weapons();
          return true;
        case 0x46a950:
          mk4_script_avoid_clear(fighter);
          return true;
        case 0x46a970:
          mk4_script_avoid_set(fighter);
          return true;
        case 0x46ddd0:
          mk4_script_jab_contact(fighter);
          return true;
        case 0x46de60:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case 0x47d720:
          raiden_script_torpedo_step(fighter);
          return true;
        case 0x48a1f0:
          scorpion_script_beast_voice(fighter);
          return true;
        case 0x48a220:
          retail_throw_grunt();
          return true;
        case 0x48f6e0:
          mk4_script_save_motion(fighter);
          return true;
        case 0x48f750:
          mk4_script_restore_motion(fighter);
          return true;
        case 0x48f9f0:
          liu_kang_liu_script_fatal_align(fighter);
          return true;
        case 0x490290:
          mk4_script_touchable(fighter);
          return true;
        case 0x494520:
          mk4_script_disarm_self(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_TAIL:
      switch (address) {
        case 0x46f650:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case 0x42c7d0:
          retail_lk_continue_840(fighter);
          return true;
        case 0x41b080:
          sub_zero_script_breath_victim_end(fighter);
          return true;
        case 0x41b440:
          sub_zero_script_breath_approach(fighter);
          return true;
        case 0x41b450:
          sub_zero_script_breath_slide(fighter);
          return true;
        case 0x41b570:
          sub_zero_script_breath_shatter(fighter);
          return true;
        case 0x41c4b0:
          reiko_second_victim_fall(fighter);
          return true;
        case 0x41c4e0:
          reiko_second_victim_end(fighter);
          return true;
        case 0x41dfc0:
          jarek_second_handoff(fighter);
          return true;
        case 0x41e0a0:
          jarek_second_victim_hold(fighter);
          return true;
        case 0x41ea50:
          scorpion_script_beast_begin(fighter);
          return true;
        case 0x41ebd0:
          scorpion_script_beast_attack(fighter);
          return true;
        case 0x41ec70:
          scorpion_script_beast_victim_pieces(fighter);
          return true;
        case 0x41f1b0:
          retail_task_pause(fighter);
          return true;
        case 0x42bdf0:
          retail_vm_enter(fighter, 0x4de318, fighter->retail_vm.finish);
          return true;
        case 0x42be00:
          retail_vm_enter(fighter, 0x4de328, fighter->retail_vm.finish);
          return true;
        case 0x42b730:
        case 0x42b7a0:
          retail_lk_return(fighter);
          return true;
        case 0x42bb80:
          retail_lk_pause_long(fighter);
          return true;
        case 0x42bd30:
          retail_lk_pause_short(fighter);
          return true;
        case 0x42bb00:
          retail_lk_jump_back(fighter);
          return true;
        case 0x42b840:
          retail_lk_recoil(fighter);
          return true;
        case 0x42b800:
          retail_lk_branch_continue(fighter);
          return true;
        case 0x42bc90:
          retail_lk_victim_return(fighter);
          return true;
        case 0x42bc10:
          retail_lk_script_170(fighter);
          return true;
        case 0x42bc20:
          retail_lk_script_180(fighter);
          return true;
        case 0x42bc30:
          retail_lk_script_190(fighter);
          return true;
        case 0x42be10:
          mk4_mmove_lk_owner_ready(fighter);
          return true;
        case 0x42bf00:
          mk4_mmove_lk_blend_return(fighter);
          return true;
        case 0x42c0b0:
          liu_kang_liu_script_lk_continue(fighter);
          return true;
        case 0x42c0d0:
          liu_kang_liu_script_lk_damage(fighter);
          return true;
        case 0x42c170:
          retail_vm_enter(fighter, 0x4de478, fighter->retail_vm.finish);
          return true;
        case 0x42c200:
          sub_zero_script_lk_rebase(fighter);
          return true;
        case 0x42c220:
          sub_zero_script_lk_damage(fighter);
          return true;
        case 0x42c300:
          sub_zero_script_lk_owner_return(fighter);
          return true;
        case 0x42c770:
          retail_vm_enter(fighter, 0x4de7e8, fighter->retail_vm.finish);
          return true;
        case 0x42c7b0:
          goro_script_lp_phase_repeat(fighter);
          return true;
        case 0x42c7f0:
          retail_vm_enter(fighter, 0x4de4f0, fighter->retail_vm.finish);
          return true;
        case 0x42c820:
          jax_lp_slam_first(fighter);
          return true;
        case 0x42c8c0:
          jax_lp_slam_second(fighter);
          return true;
        case 0x42c8f0:
          jax_lp_slam_third(fighter);
          return true;
        case 0x42c9f0:
          jax_lp_throw_return(fighter);
          return true;
        case 0x42cb80:
          retail_lp_select_short(fighter);
          return true;
        case 0x42cbc0:
          retail_throw_ready(fighter);
          return true;
        case 0x42cc00:
          retail_lp_select_long(fighter);
          return true;
        case 0x42cc40:
          retail_lp_end(fighter);
          return true;
        case 0x42ce60:
          retail_throw_ready(fighter);
          return true;
        case 0x42ce80:
          retail_throw_ready(fighter);
          return true;
        case 0x43afc0:
          goro_script_punch_tail(fighter);
          return true;
        case 0x43b1a0:
          goro_script_kick_tail(fighter);
          return true;
        case 0x43b370:
          goro_script_leap_continue(fighter);
          return true;
        case 0x43b590:
          goro_script_stomp_return(fighter);
          return true;
        case 0x43b8c0:
          goro_script_quake_land(fighter);
          return true;
        case 0x43c500:
          liu_kang_liu_script_fatal_setup(fighter);
          return true;
        case 0x43c820:
          shinnok_second_vanish(fighter);
          return true;
        case 0x43ca30:
          shinnok_second_clap(fighter);
          return true;
        case 0x43d460:
          retail_fan_tail(fighter);
          return true;
        case 0x43d510:
          retail_fan_cycle(fighter);
          return true;
        case 0x43d530:
          retail_fan_cycle(fighter);
          return true;
        case 0x43d6f0:
          retail_fan_victory(fighter);
          return true;
        case 0x43db50:
          retail_fan_victim_end(fighter);
          return true;
        case 0x43f510:
          sonya_script_first_release(fighter);
          return true;
        case 0x441140:
          raiden_script_first_release(fighter);
          return true;
        case 0x441180:
          raiden_script_first_end(fighter);
          return true;
        case 0x443930:
          scorpion_script_burn_begin(fighter);
          return true;
        case 0x443a30:
          scorpion_script_burn_victim_start(fighter);
          return true;
        case 0x443a70:
          scorpion_script_burn_repeat1(fighter);
          return true;
        case 0x443b40:
          scorpion_script_burn_repeat2(fighter);
          return true;
        case 0x443c10:
          scorpion_script_burn_victory(fighter);
          return true;
        case 0x444000:
          scorpion_script_burn_victim_cue(fighter);
          return true;
        case 0x444030:
          scorpion_script_burn_victim_repeat(fighter);
          return true;
        case 0x444060:
          scorpion_script_burn_victim_blend(fighter);
          return true;
        case 0x444100:
          scorpion_script_burn_victim_impact(fighter);
          return true;
        case 0x444150:
          scorpion_script_burn_victim_settle(fighter);
          return true;
        case 0x444190:
          scorpion_script_burn_victim_end(fighter);
          return true;
        case 0x445150:
          sub_zero_script_first_begin(fighter);
          return true;
        case 0x445220:
          sub_zero_script_first_pull(fighter);
          return true;
        case 0x445240:
          sub_zero_script_first_cues1(fighter);
          return true;
        case 0x445280:
          sub_zero_script_first_cues2(fighter);
          return true;
        case 0x4452b0:
          sub_zero_script_first_cues3(fighter);
          return true;
        case 0x4452e0:
          sub_zero_script_first_cues4(fighter);
          return true;
        case 0x445310:
          sub_zero_script_first_head(fighter);
          return true;
        case 0x445510:
          sub_zero_script_first_victim_blends(fighter);
          return true;
        case 0x4456c0:
          sub_zero_script_first_victim_collapse(fighter);
          return true;
        case 0x445700:
          sub_zero_script_first_victim_end(fighter);
          return true;
        case 0x446f80:
          jarek_first_heart(fighter);
          return true;
        case 0x447310:
          jarek_first_victim_rip(fighter);
          return true;
        case 0x4473b0:
          jarek_first_victim_impact(fighter);
          return true;
        case 0x447410:
          jarek_first_victim_end(fighter);
          return true;
        case 0x448480:
          cage_script_fatality_1_signal(fighter);
          return true;
        case 0x4487b0:
          cage_first_victim_59(fighter);
          return true;
        case 0x4487f0:
          cage_first_victim_131(fighter);
          return true;
        case 0x448830:
          cage_first_victim_148(fighter);
          return true;
        case 0x448870:
          cage_first_victim_166(fighter);
          return true;
        case 0x4488b0:
          cage_first_victim_185(fighter);
          return true;
        case 0x4488f0:
          cage_first_victim_206(fighter);
          return true;
        case 0x448930:
          cage_first_sever(fighter);
          return true;
        case 0x448c80:
          cage_first_torso_half(fighter);
          return true;
        case 0x448d20:
          cage_first_torso_123(fighter);
          return true;
        case 0x448d70:
          cage_first_torso_finish(fighter);
          return true;
        case 0x449620:
          jax_fatality_1_victory(fighter);
          return true;
        case 0x449890:
          jax_fatality_1_grab(fighter);
          return true;
        case 0x4498d0:
          jax_fatality_1_first_arm(fighter);
          return true;
        case 0x449940:
          jax_fatality_1_second_arm(fighter);
          return true;
        case 0x44aeb0:
          kai_first_owner_grab(fighter);
          return true;
        case 0x44af20:
          kai_first_owner_spin(fighter);
          return true;
        case 0x44af30:
          kai_first_owner_tear(fighter);
          return true;
        case 0x44b260:
          kai_first_victim_71(fighter);
          return true;
        case 0x44b290:
          kai_first_victim_turn(fighter);
          return true;
        case 0x44b2c0:
          kai_first_victim_sever(fighter);
          return true;
        case 0x44b600:
          kai_first_torso_203(fighter);
          return true;
        case 0x44b650:
          kai_first_torso_impact(fighter);
          return true;
        case 0x44be10:
          quan_chi_fatality_cue_40(fighter);
          return true;
        case 0x44be40:
          quan_chi_fatality_leg(fighter);
          return true;
        case 0x44be60:
          quan_chi_fatality_hit_179(fighter);
          return true;
        case 0x44beb0:
          quan_chi_fatality_hit_241(fighter);
          return true;
        case 0x44bf00:
          retail_vm_enter(fighter, 0x4e1548, fighter->retail_vm.finish);
          return true;
        case 0x44bf10:
          quan_chi_fatality_beat(fighter);
          return true;
        case 0x44d4a0:
          tanya_first_owner_53(fighter);
          return true;
        case 0x44d4e0:
          tanya_first_owner_drift(fighter);
          return true;
        case 0x44d510:
          tanya_first_owner_replays(fighter);
          return true;
        case 0x44d530:
          tanya_first_owner_loop(fighter);
          return true;
        case 0x44d6e0:
          tanya_first_victim_swell(fighter);
          return true;
        case 0x44d750:
          tanya_first_victim_repeat(fighter);
          return true;
        case 0x44d7a0:
          tanya_first_victim_burst(fighter);
          return true;
        case 0x44f5e0:
          fujin_first_loop_start(fighter);
          return true;
        case 0x44f620:
          fujin_first_cycle(fighter);
          return true;
        case 0x44f6f0:
          fujin_first_shoot(fighter);
          return true;
        case 0x4528a0:
          reiko_first_victory(fighter);
          return true;
        case 0x452ac0:
          reiko_first_tear(fighter);
          return true;
        case 0x452bb0:
          reiko_first_victim_118(fighter);
          return true;
        case 0x452c10:
          reiko_first_victim_165(fighter);
          return true;
        case 0x452c50:
          reiko_first_piece_end(fighter);
          return true;
        case 0x452e50:
          reiko_first_torso_29(fighter);
          return true;
        case 0x452e90:
          reiko_first_torso_end(fighter);
          return true;
        case 0x452f00:
          reiko_first_head_59(fighter);
          return true;
        case 0x452f40:
          reiko_first_head_83(fighter);
          return true;
        case 0x452f90:
          reiko_first_piece_end(fighter);
          return true;
        case 0x452fe0:
          reiko_first_left_arm_56(fighter);
          return true;
        case 0x453020:
          reiko_first_left_arm_land(fighter);
          return true;
        case 0x453040:
          reiko_first_piece_end(fighter);
          return true;
        case 0x453090:
          reiko_first_right_arm_44(fighter);
          return true;
        case 0x4530d0:
          reiko_first_right_arm_land(fighter);
          return true;
        case 0x4530f0:
          reiko_first_piece_end(fighter);
          return true;
        case 0x453ae0:
          reptile_first_owner_cycle(fighter);
          return true;
        case 0x453b20:
          reptile_first_owner_bite(fighter);
          return true;
        case 0x453ba0:
          reptile_first_owner_loop(fighter);
          return true;
        case 0x454130:
          reptile_first_victim_bite(fighter);
          return true;
        case 0x4541b0:
          reptile_first_victim_shake(fighter);
          return true;
        case 0x4541e0:
          reptile_first_victim_collapse(fighter);
          return true;
        case 0x455110:
          shinnok_first_vanished(fighter);
          return true;
        case 0x455140:
          shinnok_first_hidden(fighter);
          return true;
        case 0x4551b0:
          shinnok_fatality_reappear(fighter);
          return true;
        case 0x455230:
          shinnok_fatality_victory(fighter);
          return true;
        case 0x455330:
          shinnok_second_vanished(fighter);
          return true;
        case 0x455360:
          shinnok_second_hands(fighter);
          return true;
        case 0x4558b0:
          shinnok_first_hand_squeeze(fighter);
          return true;
        case 0x4558d0:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case 0x455ae0:
          shinnok_first_victim_held(fighter);
          return true;
        case 0x455b60:
          shinnok_first_victim_squeeze(fighter);
          return true;
        case 0x455b80:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case 0x455ba0:
          shinnok_first_victim_crush(fighter);
          return true;
        case 0x45fc40:
          retail_crouch_return(fighter);
          return true;
        case 0x464960:
          raiden_script_second_blend(fighter);
          return true;
        case 0x464a60:
          raiden_script_second_effect(fighter);
          return true;
        case 0x464a90:
          retail_vm_enter(fighter, 0x4e5728, fighter->retail_vm.finish);
          return true;
        case 0x464aa0:
          raiden_script_second_release(fighter);
          return true;
        case 0x464bb0:
          raiden_script_second_end(fighter);
          return true;
        case 0x464ee0:
          raiden_script_second_victim_end(fighter);
          return true;
        case 0x4650b0:
          tanya_second_height(fighter);
          return true;
        case 0x465130:
          tanya_second_finish(fighter);
          return true;
        case 0x465250:
          retail_vm_enter(fighter, 0x4e5860, fighter->retail_vm.finish);
          return true;
        case 0x4658a0:
          cage_script_fatality_2_continue(fighter);
          return true;
        case 0x4658f0:
          cage_second_repeated_uppercuts(fighter);
          return true;
        case 0x465b10:
          cage_second_victim_finish(fighter);
          return true;
        case 0x465e10:
          sonya_script_second_open(fighter);
          return true;
        case 0x465e70:
          sonya_script_second_repeat(fighter);
          return true;
        case 0x465ea0:
          sonya_script_second_branch(fighter);
          return true;
        case 0x465ed0:
          sonya_script_second_pause(fighter);
          return true;
        case 0x466000:
          sonya_script_second_cycle_end(fighter);
          return true;
        case 0x466190:
          sonya_script_second_victim_end(fighter);
          return true;
        case 0x466a90:
          jax_fatality_2_approach(fighter);
          return true;
        case 0x466bb0:
          jax_fatality_2_finish(fighter);
          return true;
        case 0x466df0:
          retail_task_pause(fighter);
          return true;
        case 0x467fc0:
          retail_stage_owner(fighter);
          return true;
        case 0x468640:
          scorpion_fire_script(fighter);
          return true;
        case 0x4698b0:
          reptile_script_invisibility(fighter);
          return true;
        case 0x46a500:
          kai_second_victim_fall(fighter);
          return true;
        case 0x46a650:
          fujin_wind_recovery(fighter);
          return true;
        case 0x46a6b0:
          retail_kick_contact_default(fighter);
          return true;
        case 0x46a6e0:
          retail_vm_reverse_return(fighter);
          return true;
        case 0x46a760:
          retail_kick_contact_wide(fighter);
          return true;
        case 0x46a790:
          retail_kick_contact_narrow(fighter);
          return true;
        case 0x46a850:
          retail_vm_reverse_return(fighter);
          return true;
        case 0x46a8d0:
          retail_vm_reverse_return(fighter);
          return true;
        case 0x46a9f0:
          retail_air_lift_fall(fighter);
          return true;
        case 0x46aa40:
          retail_air_lift_release(fighter);
          return true;
        case 0x46aae0:
          retail_air_throw_release(fighter);
          return true;
        case 0x46ab20:
          retail_air_throw_land(fighter);
          return true;
        case 0x46ac10:
          retail_air_slam_fall(fighter);
          return true;
        case 0x46ac90:
          retail_vm_enter(fighter, 0x4e5eb0, fighter->retail_vm.finish);
          return true;
        case 0x46aca0:
          retail_air_slam_end(fighter);
          return true;
        case 0x46aed0:
          retail_air_flip_release(fighter);
          return true;
        case 0x46b740:
          retail_crouch_low_kick_tail(fighter);
          return true;
        case 0x46bb60:
          retail_close_high_kick_tail(fighter);
          return true;
        case 0x46bf10:
          retail_close_high_punch_tail(fighter);
          return true;
        case 0x46c2c0:
          retail_combo_high_punch_tail(fighter);
          return true;
        case 0x46c7f0:
          retail_air_straight_tail(fighter);
          return true;
        case 0x46e240:
          retail_air_land(fighter);
          return true;
        case 0x46dff0:
          retail_air_land(fighter);
          return true;
        case 0x46c8f0:
          retail_air_land(fighter);
          return true;
        case 0x46c820:
          retail_air_straight_land(fighter);
          return true;
        case 0x46c970:
          retail_air_straight_wide_tail(fighter);
          return true;
        case 0x46cde0:
          native_uppercut_default(fighter);
          return true;
        case 0x46d260:
          retail_weapon_draw_equip(fighter);
          return true;
        case 0x46dd00:
          retail_vm_enter(fighter, 0x4e6330, fighter->retail_vm.finish);
          return true;
        case 0x46dd10:
          retail_vm_enter(fighter, 0x4e6360, fighter->retail_vm.finish);
          return true;
        case 0x46dd20:
          retail_vm_enter(fighter, 0x4e63b0, fighter->retail_vm.finish);
          return true;
        case 0x46df90:
          retail_air_lk_tail(fighter);
          return true;
        case 0x46e070:
          retail_air_lk_alt_tail(fighter);
          return true;
        case 0x46e150:
          retail_air_lk_wide_tail(fighter);
          return true;
        case 0x46e3d0:
          retail_air_diagonal_tail(fighter);
          return true;
        case 0x46e4d0:
          retail_air_diagonal_alt_tail(fighter);
          return true;
        case 0x46e5c0:
          retail_air_diagonal_wide_tail(fighter);
          return true;
        case 0x46e730:
          retail_air_hp_tail(fighter);
          return true;
        case 0x46eab0:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case 0x46ec10:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case 0x46eea0:
          retail_air_lp_tail(fighter);
          return true;
        case 0x46efd0:
          retail_sweep_advancing_tail(fighter);
          return true;
        case 0x46f080:
          retail_sweep_standard_tail(fighter);
          return true;
        case 0x46f220:
          retail_roundhouse_start(fighter);
          return true;
        case 0x46f270:
          retail_roundhouse_tail(fighter);
          return true;
        case 0x46f390:
          retail_roundhouse_wide_start(fighter);
          return true;
        case 0x46f3e0:
          retail_roundhouse_wide_tail(fighter);
          return true;
        case 0x470420:
          retail_controller_return(fighter);
          return true;
        case 0x4712e0:
          retail_victory_continue(fighter);
          return true;
        case 0x478e20:
          kai_lunge_kick_contact(fighter);
          return true;
        case 0x479b70:
          cage_script_cpu_kick_tail(fighter);
          return true;
        case 0x479f00:
          cage_script_powered_punch_tail(fighter);
          return true;
        case 0x47a3b0:
          quan_chi_script_flip_kick_tail(fighter);
          return true;
        case 0x47a6d0:
          reptile_script_dash_contact(fighter);
          return true;
        case 0x47ab50:
          jax_dash_punch_contact(fighter);
          return true;
        case 0x47b010:
          reptile_script_roll_tail(fighter);
          return true;
        case 0x47b600:
          jarek_quake_tail(fighter);
          return true;
        case 0x47b6c0:
          sonya_script_cartwheel_tail(fighter);
          return true;
        case 0x47ba20:
          tanya_drill_kick_contact(fighter);
          return true;
        case 0x47bd40:
          kai_aerial_punch_contact(fighter);
          return true;
        case 0x47bf90:
          fujin_dive_contact(fighter);
          return true;
        case 0x47bfd0:
          fujin_dive_land(fighter);
          return true;
        case 0x47c0a0:
          retail_teleport_vanish(fighter);
          return true;
        case 0x47c360:
          retail_teleport_slam_fall(fighter);
          return true;
        case 0x47c390:
          retail_teleport_slam_drop(fighter);
          return true;
        case 0x47c580:
          kai_script_stage_event(fighter);
          return true;
        case 0x47c880:
          kai_handstand_spin_begin(fighter);
          return true;
        case 0x47cc10:
          retail_vm_enter(fighter, 0x4e8358, fighter->retail_vm.finish);
          return true;
        case 0x47cc50:
          kai_script_contact_b10_continuation(fighter);
          return true;
        case 0x47ccc0:
          kai_handstand_rear_contact(fighter);
          return true;
        case 0x47d680:
          raiden_script_torpedo_miss(fighter);
          return true;
        case 0x47d7f0:
          raiden_script_torpedo_contact(fighter);
          return true;
        case 0x47dae0:
          retail_recoil_land(fighter);
          return true;
        case 0x47dc40:
          sub_zero_slide_script(fighter);
          return true;
        case 0x47de00:
          sub_zero_script_slide_return(fighter);
          return true;
        case 0x47e0a0:
          sonya_script_leg_grab_tail(fighter);
          return true;
        case 0x47e1d0:
          sonya_script_square_wave_tail(fighter);
          return true;
        case 0x47eb30:
          liu_kang_bicycle_script(fighter);
          return true;
        case 0x47ed40:
          liu_kang_liu_script_land(fighter);
          return true;
        case 0x47ee00:
          liu_kang_flying_kick_script(fighter);
          return true;
        case 0x47ef10:
          liu_kang_liu_script_land(fighter);
          return true;
        case 0x47f060:
          fujin_triple_fast(fighter);
          return true;
        case 0x47f070:
          fujin_triple_contact(fighter);
          return true;
        case 0x47fcf0:
          scorpion_teleport_script(fighter);
          return true;
        case 0x47ffc0:
          mk4_land(fighter);
          return true;
        case 0x4803a0:
          fujin_grab_control_start(fighter);
          return true;
        case 0x4807b0:
          scorpion_spin_script(fighter);
          return true;
        case 0x481590:
          reptile_second_melt(fighter);
          return true;
        case 0x4816f0:
          fujin_second_burst(fighter);
          return true;
        case 0x4817c0:
          retail_scene_body_finish(fighter);
          return true;
        case 0x482280:
          reptile_weapon_stagger_tail(fighter);
          return true;
        case 0x4824f0:
          goro_script_uppercut_victim_slide(fighter);
          return true;
        case 0x482540:
          goro_victim_getup(fighter);
          return true;
        case 0x4829e0:
          retail_vm_enter(fighter, 0x4e91c0, mk4_script_thrown_ready);
          return true;
        case 0x4829f0:
          retail_vm_enter(fighter, 0x4e91d0, mk4_script_thrown_ready);
          return true;
        case 0x482a00:
          retail_vm_enter(fighter, 0x4e91e0, mk4_script_thrown_ready);
          return true;
        case 0x482790:
          retail_reaction_getup_back(fighter);
          return true;
        case 0x4835f0:
          retail_reaction_fall(fighter);
          return true;
        case 0x483610:
          retail_reaction_finish(fighter);
          return true;
        case 0x483090:
          shinnok_weapon_sweep_getup(fighter);
          return true;
        case 0x4830a0:
          shinnok_weapon_sweep_finish(fighter);
          return true;
        case 0x483720:
          retail_stage_impale(fighter);
          return true;
        case 0x4837c0:
          retail_stage_complete(fighter);
          return true;
        case 0x483df0:
          retail_stagger_tail(fighter);
          return true;
        case 0x484330:
          goro_script_stomp_victim_hold(fighter);
          return true;
        case 0x484420:
          quan_chi_script_stomp_victim(fighter);
          return true;
        case 0x4847e0:
          retail_air_flip_getup(fighter);
          return true;
        case 0x484a30:
          retail_air_lift_getup(fighter);
          return true;
        case 0x484b60:
          mk4_mmove_air_getup(fighter);
          return true;
        case 0x484c00:
          scorpion_script_teleport_victim_tail(fighter);
          return true;
        case 0x4854c0:
          retail_throw_victim_continue(fighter);
          return true;
        case 0x4854d0:
          retail_throw_victim_impact(fighter);
          return true;
        case 0x485510:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case 0x4858d0:
          sonya_script_leg_victim_end(fighter);
          return true;
        case 0x485930:
          liu_kang_liu_script_lk_thrown_tail(fighter);
          return true;
        case 0x485ad0:
          retail_getup_flip(fighter);
          return true;
        case 0x485b10:
          retail_getup_roll_flip(fighter);
          return true;
        case 0x485b50:
          /* 490350 twice, through488bd0, leaves the original facing. */
          retail_getup_roll(fighter);
          return true;
        case 0x486530:
          retail_thaw_land(fighter);
          return true;
        case 0x486000:
          retail_shake_stun(fighter);
          return true;
        case 0x487220:
          sub_zero_script_lk_victim_rise(fighter);
          return true;
        case 0x481fb0:
          retail_vm_return(fighter);
          return true;
        case 0x487980:
          retail_pair_release(fighter);
          return true;
        case 0x488890:
          retail_bicycle_cycle(fighter);
          return true;
        case 0x483930:
          retail_uppercut_recover(fighter);
          return true;
        case 0x486f30:
          retail_block_cycle(fighter);
          return true;
        case 0x460250:
          retail_controller_return(fighter);
          return true;
        case 0x487420:
          scorpion_spear_stun_script(fighter);
          return true;
        case 0x488590:
          raiden_script_staff_victim_tail(fighter);
          return true;
        case 0x488ac0:
          kai_handstand_victim_tail(fighter);
          return true;
        case 0x488b40:
          mk4_mmove_side_getup(fighter);
          return true;
        case 0x485c40:
        case 0x488b80:
          retail_getup_flip(fighter);
          return true;
        case 0x488ba0:
          retail_getup_back(fighter);
          return true;
        case 0x488c00:
          retail_getup_roll(fighter);
          return true;
        case 0x488c10:
          retail_getup_front(fighter);
          return true;
        case 0x4958b0:
          retail_weapon_contact(fighter, address);
          return true;
        case 0x495ae0:
          retail_weapon_contact(fighter, address);
          return true;
        case 0x495db0:
          reptile_weapon_flurry(fighter);
          return true;
        case 0x496430:
          liu_kang_liu_script_sword_blend(fighter);
          return true;
        case 0x496770:
          retail_weapon_contact(fighter, address);
          return true;
        case 0x496970:
          retail_weapon_contact(fighter, address);
          return true;
        case 0x497b60:
          tanya_boomerang_release(fighter);
          return true;
        case 0x498eb0:
          retail_fireball_fall(fighter);
          return true;
        case 0x499000:
          kai_ground_skull_spawn(fighter);
          return true;
        case 0x499450:
          kai_rising_skull_spawn(fighter);
          return true;
        case 0x499470:
          kai_rising_skull_finish(fighter);
          return true;
        case 0x499780:
          quan_chi_script_skull_release(fighter);
          return true;
        case 0x49a480:
          fujin_bow_finish(fighter);
          return true;
        case 0x49a680:
          scorpion_script_spear_return(fighter);
          return true;
        case 0x49c010:
          liu_kang_liu_script_air_return(fighter);
          return true;
        case 0x49cb60:
          retail_task_retire(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_SPAWN:
      switch (address) {
        case 0x412d80:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_sparks(fighter, (int)argument);
          return true;
        case 0x412ec0:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_flash(fighter, (int)argument);
          return true;
        case 0x413970:
          if (argument != 619u) {
            return false;
          }
          cage_second_charge(fighter, (int)argument);
          return true;
        case 0x413a10:
          if (argument != 619u) {
            return false;
          }
          cage_second_flash(fighter, (int)argument);
          return true;
        case 0x41b860:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_freeze(fighter, (int)argument);
          return true;
        case 0x41bbb0:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_object(fighter, (int)argument);
          return true;
        case 0x41d260:
          if (argument != 0u) {
            return false;
          }
          reiko_second_stars(fighter, (int)argument);
          return true;
        case 0x41e0e0:
          if (argument != 0u) {
            return false;
          }
          jarek_second_head_fall(fighter, (int)argument);
          return true;
        case 0x41e490:
          if (argument != 0u) {
            return false;
          }
          reptile_second_acid(fighter, (int)argument);
          return true;
        case 0x41ed80:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_beast_gore(fighter, (int)argument);
          return true;
        case 0x41ef20:
          if (argument != 38u) {
            return false;
          }
          fujin_second_blood(fighter, (int)argument);
          return true;
        case 0x43c140:
          if (argument != 38u) {
            return false;
          }
          kai_second_skull(fighter, (int)argument);
          return true;
        case 0x43c540:
          if (argument != 38u) {
            return false;
          }
          liu_kang_liu_script_fatal_spawn(fighter, (int)argument);
          return true;
        case 0x450d40:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_transform_effect(fighter, (int)argument);
          return true;
        case 0x464bd0:
          if (argument != 619u) {
            return false;
          }
          mk4_script_scene_task(fighter, (int)argument);
          return true;
        case 0x465270:
          if (argument != 619u) {
            return false;
          }
          tanya_second_owner_cue(fighter, (int)argument);
          return true;
        case 0x465390:
          if (argument != 619u) {
            return false;
          }
          tanya_second_victim_cue(fighter, (int)argument);
          return true;
        case 0x46a280:
          if (argument != 38u) {
            return false;
          }
          retail_scene_hover(fighter, (int)argument);
          return true;
        case 0x496e00:
          if (argument != 82u) {
            return false;
          }
          reiko_star_spawn(fighter, (int)argument);
          return true;
        case 0x496fc0:
          if (argument != 80u) {
            return false;
          }
          jax_missile_spawn(fighter, (int)argument);
          return true;
        case 0x4970d0:
          if (argument != 79u) {
            return false;
          }
          cage_orb_spawn(fighter, (int)argument);
          return true;
        case 0x497580:
          if (argument != 78u) {
            return false;
          }
          jarek_blade_spawn(fighter, (int)argument);
          return true;
        case 0x498920:
          if (argument != 76u) {
            return false;
          }
          reptile_acid_spawn(fighter, (int)argument);
          return true;
        case 0x498bb0:
          if (argument != 75u) {
            return false;
          }
          retail_fireball_spawn(fighter, (int)argument);
          return true;
        case 0x498ed0:
          if (argument != 75u) {
            return false;
          }
          retail_air_fireball_spawn(fighter, (int)argument);
          return true;
        case 0x49a6a0:
          if (argument != 67u) {
            return false;
          }
          scorpion_script_spawn_spear(fighter, (int)argument);
          return true;
        case 0x49b9e0:
          if (argument != 38u) {
            return false;
          }
          sonya_script_ring_flash(fighter, (int)argument);
          return true;
        case 0x49ba50:
          if (argument != 68u) {
            return false;
          }
          sonya_script_ring_spawn(fighter, (int)argument);
          return true;
        case 0x49bcb0:
          if (argument != 66u) {
            return false;
          }
          sub_zero_script_spawn_ice(fighter, (int)argument);
          return true;
        case 0x49be10:
          if (argument != 65u) {
            return false;
          }
          raiden_script_lightning_spawn(fighter, (int)argument);
          return true;
        case 0x49c0d0:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_air(fighter, (int)argument);
          return true;
        case 0x49c110:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_low(fighter, (int)argument);
          return true;
        case 0x49c160:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_high(fighter, (int)argument);
          return true;
        case 0x49ca90:
          if (argument != 83u) {
            return false;
          }
          goro_fireball_spawn(fighter, (int)argument);
          return true;
        default:
          return false;
      }
    case RETAIL_HANDLER:
      switch (address) {
        case 0x45ac30:
          mk4_script_context(fighter, (int)(argument & 255u));
          return true;
        case 0x45b1f0:
          retail_script_reaction_dispatch(fighter, (int)(argument & 255u));
          return true;
        case 0x45b230:
          if (retail_reaction_entry(fighter->retail_vm.image, argument & 255u) == NULL) {
            return false;
          }
          retail_script_release_reaction(fighter, (int)(argument & 255u));
          return true;
        case 0x45b380:
          retail_face_peer(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_PREDICATE:
      switch (address) {
        case 0x46db00:
          fighter->retail_vm.result = liu_kang_liu_script_sword_gate(fighter);
          return true;
        case 0x46de60:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case 0x48d450:
          fighter->retail_vm.result = raiden_script_torpedo_query(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_PHYSICS:
      switch (address) {
        case 0x4707e0:
          fighter->body.physics = mk4_script_landing_motion;
          return true;
        case 0x47d6e0:
          fighter->body.physics = raiden_script_torpedo_motion;
          return true;
        case 0x480040:
          fighter->body.physics = scorpion_script_teleport_motion;
          return true;
        case 0x480e90:
          fighter->body.physics = liu_kang_liu_script_bicycle_motion;
          return true;
        case 0x480f40:
          fighter->body.physics = goro_stomp_hold_separation;
          return true;
        case 0x4889b0:
          fighter->body.physics = raiden_script_torpedo_victim_motion;
          return true;
        case 0x490bd0:
          fighter->body.physics = retail_face_peer;
          return true;
        case 0x4913a0:
          fighter->body.physics = fighter_friction;
          return true;
        default:
          return false;
      }
  }
  return false;
}

#include "script/native_ids.h"
/* Typed native ABI registrations, derived from the reviewed local-EXE
 * contracts. Each native address selects one shared engine service;
 * no retail script instructions or animation frame rows are emitted. */
#include "native/actions.h"
#include "script/vm.h"
#include "native/helpers.h"
#include "native/capabilities.h"

unsigned liu_kang_liu_script_sword_gate(Fighter *fighter);
unsigned mk4_script_jab_gate(Fighter *fighter);
unsigned raiden_script_torpedo_query(Fighter *fighter);

bool script_native_dispatch(Fighter *fighter, uint32_t address, ScriptNativeKind kind, unsigned argument) {
  if (fighter->roster < 0 || fighter->roster >= MK4_ROSTER_COUNT) {
    return false;
  }
  switch (kind) {
    case MK4_CALL:
      switch (address) {
        case native_flip_facing:
          action_flip_facing(fighter);
          return true;
        case native_impact_voice:
          action_impact_voice(fighter);
          return true;
        case native_kai_second_child_walk:
          kai_second_child_walk(fighter);
          return true;
        case native_reptile_script_head_close:
          reptile_script_head_close(fighter);
          return true;
        case native_reptile_script_head_open:
          reptile_script_head_open(fighter);
          return true;
        case native_reptile_script_head_wide:
          reptile_script_head_wide(fighter);
          return true;
        case native_reptile_script_acid_drool:
          reptile_script_acid_drool(fighter);
          return true;
        case native_raiden_script_victory_effect:
          raiden_script_victory_effect(fighter);
          return true;
        case native_scorpion_script_fire_decor:
          scorpion_script_fire_decor(fighter);
          return true;
        case native_scorpion_script_fire_flame:
          scorpion_script_fire_flame(fighter);
          return true;
        case native_shinnok_second_gore_19_pieces:
          shinnok_second_gore(fighter);
          return true;
        case native_shinnok_second_gore_8_pieces:
          shinnok_second_gore(fighter);
          return true;
        case native_shinnok_second_gore_7_pieces:
          shinnok_second_gore(fighter);
          return true;
        case native_jarek_second_glow:
          jarek_second_glow(fighter);
          return true;
        case native_raiden_script_second_particles:
          raiden_script_second_particles(fighter);
          return true;
        case native_throw_pair_start:
          action_throw_pair_start(fighter);
          return true;
        case native_disturb_resting_weapons:
          action_disturb_resting_weapons();
          return true;
        case native_avoid_clear:
          mk4_script_avoid_clear(fighter);
          return true;
        case native_avoid_set:
          mk4_script_avoid_set(fighter);
          return true;
        case native_jab_contact:
          mk4_script_jab_contact(fighter);
          return true;
        case native_jab_repeat_predicate:
          fighter->execution.script.result = mk4_script_jab_gate(fighter);
          return true;
        case native_raiden_script_torpedo_step:
          raiden_script_torpedo_step(fighter);
          return true;
        case native_scorpion_script_beast_voice:
          scorpion_script_beast_voice(fighter);
          return true;
        case native_throw_grunt:
          action_throw_grunt();
          return true;
        case native_save_motion:
          mk4_script_save_motion(fighter);
          return true;
        case native_restore_motion:
          mk4_script_restore_motion(fighter);
          return true;
        case native_liu_kang_liu_script_fatal_align:
          liu_kang_liu_script_fatal_align(fighter);
          return true;
        case native_touchable:
          mk4_script_touchable(fighter);
          return true;
        case native_disarm_self:
          mk4_script_disarm_self(fighter);
          return true;
        default:
          return false;
      }
    case MK4_TAIL:
      switch (address) {
        case native_jax_ground_pound_controller_return:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case native_lp_align_and_release:
          action_lp_align_and_release(fighter);
          return true;
        case native_sub_zero_script_breath_victim_end:
          sub_zero_script_breath_victim_end(fighter);
          return true;
        case native_sub_zero_script_breath_approach:
          sub_zero_script_breath_approach(fighter);
          return true;
        case native_sub_zero_script_breath_slide:
          sub_zero_script_breath_slide(fighter);
          return true;
        case native_sub_zero_script_breath_shatter:
          sub_zero_script_breath_shatter(fighter);
          return true;
        case native_reiko_second_victim_fall:
          reiko_second_victim_fall(fighter);
          return true;
        case native_reiko_second_victim_end:
          reiko_second_victim_end(fighter);
          return true;
        case native_jarek_second_handoff:
          jarek_second_handoff(fighter);
          return true;
        case native_jarek_second_victim_hold:
          jarek_second_victim_hold(fighter);
          return true;
        case native_scorpion_script_beast_begin:
          scorpion_script_beast_begin(fighter);
          return true;
        case native_scorpion_script_beast_attack:
          scorpion_script_beast_attack(fighter);
          return true;
        case native_scorpion_script_beast_victim_pieces:
          scorpion_script_beast_victim_pieces(fighter);
          return true;
        case native_task_park_60_tick_loop:
          action_task_pause(fighter);
          return true;
        case native_cage_lk_throw_paired_continuation:
          script_vm_enter(fighter, fn_cage_lk_throw_damage, fighter->execution.script.finish);
          return true;
        case native_cage_lk_throw_release:
          script_vm_enter(fighter, fn_cage_lk_throw_release, fighter->execution.script.finish);
          return true;
        case native_cage_lk_throw_owner_return:
        case native_jarek_lk_throw_owner_return:
          action_lk_return(fighter);
          return true;
        case native_lk_pause_long:
          action_lk_pause_long(fighter);
          return true;
        case native_lk_pause_short:
          action_lk_pause_short(fighter);
          return true;
        case native_lk_jump_back:
          action_lk_jump_back(fighter);
          return true;
        case native_lk_recoil:
          action_lk_recoil(fighter);
          return true;
        case native_lk_branch_continue:
          action_lk_branch_continue(fighter);
          return true;
        case native_lk_victim_return:
          action_lk_victim_return(fighter);
          return true;
        case native_lk_throw_rock_repeat_one:
          action_lk_rock_repeat_one(fighter);
          return true;
        case native_lk_throw_rock_repeat_two:
          action_lk_rock_repeat_two(fighter);
          return true;
        case native_lk_throw_rock_release:
          action_lk_rock_release(fighter);
          return true;
        case native_mmove_lk_owner_ready:
          mk4_mmove_lk_owner_ready(fighter);
          return true;
        case native_mmove_lk_blend_return:
          mk4_mmove_lk_blend_return(fighter);
          return true;
        case native_liu_kang_liu_script_lk_continue:
          liu_kang_liu_script_lk_continue(fighter);
          return true;
        case native_liu_kang_liu_script_lk_damage:
          liu_kang_liu_script_lk_damage(fighter);
          return true;
        case native_liu_kang_lk_throw_retreat:
          script_vm_enter(fighter, fn_liu_kang_lk_throw_retreat, fighter->execution.script.finish);
          return true;
        case native_sub_zero_script_lk_rebase:
          sub_zero_script_lk_rebase(fighter);
          return true;
        case native_sub_zero_script_lk_damage:
          sub_zero_script_lk_damage(fighter);
          return true;
        case native_sub_zero_script_lk_owner_return:
          sub_zero_script_lk_owner_return(fighter);
          return true;
        case native_goro_lp_throw_five_phase_sequence:
          script_vm_enter(fighter, fn_goro_lp_throw_repeated_damage, fighter->execution.script.finish);
          return true;
        case native_goro_script_lp_phase_repeat:
          goro_script_lp_phase_repeat(fighter);
          return true;
        case native_jax_lp_throw_throw_continuation:
          script_vm_enter(fighter, fn_jax_lp_throw_first_slam, fighter->execution.script.finish);
          return true;
        case native_jax_lp_slam_first:
          jax_lp_slam_first(fighter);
          return true;
        case native_jax_lp_slam_second:
          jax_lp_slam_second(fighter);
          return true;
        case native_jax_lp_slam_third:
          jax_lp_slam_third(fighter);
          return true;
        case native_jax_lp_throw_return:
          jax_lp_throw_return(fighter);
          return true;
        case native_lp_select_short:
          action_lp_select_short(fighter);
          return true;
        case native_lp_throw_owner_ready:
          action_throw_ready(fighter);
          return true;
        case native_lp_select_long:
          action_lp_select_long(fighter);
          return true;
        case native_lp_end:
          action_lp_end(fighter);
          return true;
        case native_lp_throw_victim_adjust_return:
          action_throw_ready(fighter);
          return true;
        case native_sub_zero_lp_throw_owner_return:
          action_throw_ready(fighter);
          return true;
        case native_goro_script_punch_tail:
          goro_script_punch_tail(fighter);
          return true;
        case native_goro_script_kick_tail:
          goro_script_kick_tail(fighter);
          return true;
        case native_goro_script_leap_continue:
          goro_script_leap_continue(fighter);
          return true;
        case native_goro_script_stomp_return:
          goro_script_stomp_return(fighter);
          return true;
        case native_goro_script_quake_land:
          goro_script_quake_land(fighter);
          return true;
        case native_liu_kang_liu_script_fatal_setup:
          liu_kang_liu_script_fatal_setup(fighter);
          return true;
        case native_shinnok_second_vanish:
          shinnok_second_vanish(fighter);
          return true;
        case native_shinnok_second_clap:
          shinnok_second_clap(fighter);
          return true;
        case native_fan_tail:
          action_fan_tail(fighter);
          return true;
        case native_fan_male_victim_owner_cycle:
          action_fan_cycle(fighter);
          return true;
        case native_fan_female_victim_owner_cycle:
          action_fan_cycle(fighter);
          return true;
        case native_fan_victory:
          action_fan_victory(fighter);
          return true;
        case native_fan_victim_end:
          action_fan_victim_end(fighter);
          return true;
        case native_sonya_script_first_release:
          sonya_script_first_release(fighter);
          return true;
        case native_raiden_script_first_release:
          raiden_script_first_release(fighter);
          return true;
        case native_raiden_script_first_end:
          raiden_script_first_end(fighter);
          return true;
        case native_scorpion_script_burn_begin:
          scorpion_script_burn_begin(fighter);
          return true;
        case native_scorpion_script_burn_victim_start:
          scorpion_script_burn_victim_start(fighter);
          return true;
        case native_scorpion_script_burn_repeat1:
          scorpion_script_burn_repeat1(fighter);
          return true;
        case native_scorpion_script_burn_repeat2:
          scorpion_script_burn_repeat2(fighter);
          return true;
        case native_scorpion_script_burn_victory:
          scorpion_script_burn_victory(fighter);
          return true;
        case native_scorpion_script_burn_victim_cue:
          scorpion_script_burn_victim_cue(fighter);
          return true;
        case native_scorpion_script_burn_victim_repeat:
          scorpion_script_burn_victim_repeat(fighter);
          return true;
        case native_scorpion_script_burn_victim_blend:
          scorpion_script_burn_victim_blend(fighter);
          return true;
        case native_scorpion_script_burn_victim_impact:
          scorpion_script_burn_victim_impact(fighter);
          return true;
        case native_scorpion_script_burn_victim_settle:
          scorpion_script_burn_victim_settle(fighter);
          return true;
        case native_scorpion_script_burn_victim_end:
          scorpion_script_burn_victim_end(fighter);
          return true;
        case native_sub_zero_script_first_begin:
          sub_zero_script_first_begin(fighter);
          return true;
        case native_sub_zero_script_first_pull:
          sub_zero_script_first_pull(fighter);
          return true;
        case native_sub_zero_script_first_cues1:
          sub_zero_script_first_cues1(fighter);
          return true;
        case native_sub_zero_script_first_cues2:
          sub_zero_script_first_cues2(fighter);
          return true;
        case native_sub_zero_script_first_cues3:
          sub_zero_script_first_cues3(fighter);
          return true;
        case native_sub_zero_script_first_cues4:
          sub_zero_script_first_cues4(fighter);
          return true;
        case native_sub_zero_script_first_head:
          sub_zero_script_first_head(fighter);
          return true;
        case native_sub_zero_script_first_victim_blends:
          sub_zero_script_first_victim_blends(fighter);
          return true;
        case native_sub_zero_script_first_victim_collapse:
          sub_zero_script_first_victim_collapse(fighter);
          return true;
        case native_sub_zero_script_first_victim_end:
          sub_zero_script_first_victim_end(fighter);
          return true;
        case native_jarek_first_heart:
          jarek_first_heart(fighter);
          return true;
        case native_jarek_first_victim_rip:
          jarek_first_victim_rip(fighter);
          return true;
        case native_jarek_first_victim_impact:
          jarek_first_victim_impact(fighter);
          return true;
        case native_jarek_first_victim_end:
          jarek_first_victim_end(fighter);
          return true;
        case native_cage_script_fatality_1_signal:
          cage_script_fatality_1_signal(fighter);
          return true;
        case native_cage_first_victim_59:
          cage_first_victim_59(fighter);
          return true;
        case native_cage_first_victim_131:
          cage_first_victim_131(fighter);
          return true;
        case native_cage_first_victim_148:
          cage_first_victim_148(fighter);
          return true;
        case native_cage_first_victim_166:
          cage_first_victim_166(fighter);
          return true;
        case native_cage_first_victim_185:
          cage_first_victim_185(fighter);
          return true;
        case native_cage_first_victim_206:
          cage_first_victim_206(fighter);
          return true;
        case native_cage_first_sever:
          cage_first_sever(fighter);
          return true;
        case native_cage_first_torso_half:
          cage_first_torso_half(fighter);
          return true;
        case native_cage_first_torso_123:
          cage_first_torso_123(fighter);
          return true;
        case native_cage_first_torso_finish:
          cage_first_torso_finish(fighter);
          return true;
        case native_jax_fatality_1_victory:
          jax_fatality_1_victory(fighter);
          return true;
        case native_jax_fatality_1_grab:
          jax_fatality_1_grab(fighter);
          return true;
        case native_jax_fatality_1_first_arm:
          jax_fatality_1_first_arm(fighter);
          return true;
        case native_jax_fatality_1_second_arm:
          jax_fatality_1_second_arm(fighter);
          return true;
        case native_kai_first_owner_grab:
          kai_first_owner_grab(fighter);
          return true;
        case native_kai_first_owner_spin:
          kai_first_owner_spin(fighter);
          return true;
        case native_kai_first_owner_tear:
          kai_first_owner_tear(fighter);
          return true;
        case native_kai_first_victim_71:
          kai_first_victim_71(fighter);
          return true;
        case native_kai_first_victim_turn:
          kai_first_victim_turn(fighter);
          return true;
        case native_kai_first_victim_sever:
          kai_first_victim_sever(fighter);
          return true;
        case native_kai_first_torso_203:
          kai_first_torso_203(fighter);
          return true;
        case native_kai_first_torso_impact:
          kai_first_torso_impact(fighter);
          return true;
        case native_quan_chi_fatality_cue_40:
          quan_chi_fatality_cue_40(fighter);
          return true;
        case native_quan_chi_fatality_leg:
          quan_chi_fatality_leg(fighter);
          return true;
        case native_quan_chi_fatality_hit_179:
          quan_chi_fatality_hit_179(fighter);
          return true;
        case native_quan_chi_fatality_hit_241:
          quan_chi_fatality_hit_241(fighter);
          return true;
        case native_quan_chi_fatality_1_owner_beat:
          script_vm_enter(fighter, fn_quan_chi_fatality_1_owner_beat, fighter->execution.script.finish);
          return true;
        case native_quan_chi_fatality_beat:
          quan_chi_fatality_beat(fighter);
          return true;
        case native_tanya_first_owner_53:
          tanya_first_owner_53(fighter);
          return true;
        case native_tanya_first_owner_drift:
          tanya_first_owner_drift(fighter);
          return true;
        case native_tanya_first_owner_replays:
          tanya_first_owner_replays(fighter);
          return true;
        case native_tanya_first_owner_loop:
          tanya_first_owner_loop(fighter);
          return true;
        case native_tanya_first_victim_swell:
          tanya_first_victim_swell(fighter);
          return true;
        case native_tanya_first_victim_repeat:
          tanya_first_victim_repeat(fighter);
          return true;
        case native_tanya_first_victim_burst:
          tanya_first_victim_burst(fighter);
          return true;
        case native_fujin_first_loop_start:
          fujin_first_loop_start(fighter);
          return true;
        case native_fujin_first_cycle:
          fujin_first_cycle(fighter);
          return true;
        case native_fujin_first_shoot:
          fujin_first_shoot(fighter);
          return true;
        case native_reiko_first_victory:
          reiko_first_victory(fighter);
          return true;
        case native_reiko_first_tear:
          reiko_first_tear(fighter);
          return true;
        case native_reiko_first_victim_118:
          reiko_first_victim_118(fighter);
          return true;
        case native_reiko_first_victim_165:
          reiko_first_victim_165(fighter);
          return true;
        case native_reiko_leg_tear_victim_finish:
          reiko_first_piece_end(fighter);
          return true;
        case native_reiko_first_torso_29:
          reiko_first_torso_29(fighter);
          return true;
        case native_reiko_first_torso_end:
          reiko_first_torso_end(fighter);
          return true;
        case native_reiko_first_head_59:
          reiko_first_head_59(fighter);
          return true;
        case native_reiko_first_head_83:
          reiko_first_head_83(fighter);
          return true;
        case native_reiko_leg_tear_head_finish:
          reiko_first_piece_end(fighter);
          return true;
        case native_reiko_first_left_arm_56:
          reiko_first_left_arm_56(fighter);
          return true;
        case native_reiko_first_left_arm_land:
          reiko_first_left_arm_land(fighter);
          return true;
        case native_reiko_leg_tear_left_arm_finish:
          reiko_first_piece_end(fighter);
          return true;
        case native_reiko_first_right_arm_44:
          reiko_first_right_arm_44(fighter);
          return true;
        case native_reiko_first_right_arm_land:
          reiko_first_right_arm_land(fighter);
          return true;
        case native_reiko_leg_tear_right_arm_finish:
          reiko_first_piece_end(fighter);
          return true;
        case native_reptile_first_owner_cycle:
          reptile_first_owner_cycle(fighter);
          return true;
        case native_reptile_first_owner_bite:
          reptile_first_owner_bite(fighter);
          return true;
        case native_reptile_first_owner_loop:
          reptile_first_owner_loop(fighter);
          return true;
        case native_reptile_first_victim_bite:
          reptile_first_victim_bite(fighter);
          return true;
        case native_reptile_first_victim_shake:
          reptile_first_victim_shake(fighter);
          return true;
        case native_reptile_first_victim_collapse:
          reptile_first_victim_collapse(fighter);
          return true;
        case native_shinnok_first_vanished:
          shinnok_first_vanished(fighter);
          return true;
        case native_shinnok_first_hidden:
          shinnok_first_hidden(fighter);
          return true;
        case native_shinnok_fatality_reappear:
          shinnok_fatality_reappear(fighter);
          return true;
        case native_shinnok_fatality_victory:
          shinnok_fatality_victory(fighter);
          return true;
        case native_shinnok_second_vanished:
          shinnok_second_vanished(fighter);
          return true;
        case native_shinnok_second_hands:
          shinnok_second_hands(fighter);
          return true;
        case native_shinnok_first_hand_squeeze:
          shinnok_first_hand_squeeze(fighter);
          return true;
        case native_shinnok_hand_squeeze_cycle:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case native_shinnok_first_victim_held:
          shinnok_first_victim_held(fighter);
          return true;
        case native_shinnok_first_victim_squeeze:
          shinnok_first_victim_squeeze(fighter);
          return true;
        case native_shinnok_victim_squeeze_cycle:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case native_shinnok_first_victim_crush:
          shinnok_first_victim_crush(fighter);
          return true;
        case native_crouch_return:
          action_crouch_return(fighter);
          return true;
        case native_raiden_script_second_blend:
          raiden_script_second_blend(fighter);
          return true;
        case native_raiden_script_second_effect:
          raiden_script_second_effect(fighter);
          return true;
        case native_raiden_fatality_2_held_pose:
          script_vm_enter(fighter, fn_raiden_fatality_2_held_pose, fighter->execution.script.finish);
          return true;
        case native_raiden_script_second_release:
          raiden_script_second_release(fighter);
          return true;
        case native_raiden_script_second_end:
          raiden_script_second_end(fighter);
          return true;
        case native_raiden_script_second_victim_end:
          raiden_script_second_victim_end(fighter);
          return true;
        case native_tanya_second_height:
          tanya_second_height(fighter);
          return true;
        case native_tanya_second_finish:
          tanya_second_finish(fighter);
          return true;
        case native_tanya_fatality_2_victim_collapse:
          script_vm_enter(fighter, fn_tanya_fatality_2_victim_collapse, fighter->execution.script.finish);
          return true;
        case native_cage_script_fatality_2_continue:
          cage_script_fatality_2_continue(fighter);
          return true;
        case native_cage_second_repeated_uppercuts:
          cage_second_repeated_uppercuts(fighter);
          return true;
        case native_cage_second_victim_finish:
          cage_second_victim_finish(fighter);
          return true;
        case native_sonya_script_second_open:
          sonya_script_second_open(fighter);
          return true;
        case native_sonya_script_second_repeat:
          sonya_script_second_repeat(fighter);
          return true;
        case native_sonya_script_second_branch:
          sonya_script_second_branch(fighter);
          return true;
        case native_sonya_script_second_pause:
          sonya_script_second_pause(fighter);
          return true;
        case native_sonya_script_second_cycle_end:
          sonya_script_second_cycle_end(fighter);
          return true;
        case native_sonya_script_second_victim_end:
          sonya_script_second_victim_end(fighter);
          return true;
        case native_jax_fatality_2_approach:
          jax_fatality_2_approach(fighter);
          return true;
        case native_jax_fatality_2_finish:
          jax_fatality_2_finish(fighter);
          return true;
        case native_jax_fatality_2_victim_pause:
          action_task_pause(fighter);
          return true;
        case native_stage_owner:
          action_stage_owner(fighter);
          return true;
        case native_fire_contact:
          scorpion_fire_script(fighter);
          return true;
        case native_fire_recovery:
          action_fire_recovery_finish(fighter);
          return true;
        case native_reptile_script_invisibility:
          reptile_script_invisibility(fighter);
          return true;
        case native_kai_second_victim_fall:
          kai_second_victim_fall(fighter);
          return true;
        case native_fujin_wind_recovery:
          fujin_wind_recovery(fighter);
          return true;
        case native_kick_contact_default:
          action_kick_contact_default(fighter);
          return true;
        case native_high_kick_reverse_return:
          script_vm_reverse_return(fighter);
          return true;
        case native_kick_contact_wide:
          action_kick_contact_wide(fighter);
          return true;
        case native_kick_contact_narrow:
          action_kick_contact_narrow(fighter);
          return true;
        case native_low_kick_reverse_return:
          script_vm_reverse_return(fighter);
          return true;
        case native_low_kick_narrow_reverse_return:
          script_vm_reverse_return(fighter);
          return true;
        case native_air_lift_fall:
          action_air_lift_fall(fighter);
          return true;
        case native_air_lift_release:
          action_air_lift_release(fighter);
          return true;
        case native_air_throw_release:
          action_air_throw_release(fighter);
          return true;
        case native_air_throw_land:
          action_air_throw_land(fighter);
          return true;
        case native_air_slam_fall:
          action_air_slam_fall(fighter);
          return true;
        case native_noob_saibot_air_throw_landing_link:
          script_vm_enter(fighter, fn_quan_chi_air_throw_landing, fighter->execution.script.finish);
          return true;
        case native_air_slam_end:
          action_air_slam_end(fighter);
          return true;
        case native_air_flip_release:
          action_air_flip_release(fighter);
          return true;
        case native_crouch_low_kick_tail:
          action_crouch_low_kick_tail(fighter);
          return true;
        case native_close_high_kick_tail:
          action_close_high_kick_tail(fighter);
          return true;
        case native_close_high_punch_tail:
          action_close_high_punch_tail(fighter);
          return true;
        case native_combo_high_punch_tail:
          action_combo_high_punch_tail(fighter);
          return true;
        case native_air_straight_tail:
          action_air_straight_tail(fighter);
          return true;
        case native_air_attack_land_or_finish:
          action_air_land(fighter);
          return true;
        case native_air_low_kick_land:
          action_air_land(fighter);
          return true;
        case native_air_straight_kick_land:
          action_air_land(fighter);
          return true;
        case native_air_straight_land:
          action_air_straight_land(fighter);
          return true;
        case native_air_straight_wide_tail:
          action_air_straight_wide_tail(fighter);
          return true;
        case native_uppercut_default:
          action_uppercut_default(fighter);
          return true;
        case native_weapon_draw_equip:
          action_weapon_draw_equip(fighter);
          return true;
        case native_cage_jab_second_punch:
          script_vm_enter(fighter, fn_cage_jab_second_punch, fighter->execution.script.finish);
          return true;
        case native_cage_jab_repeat:
          script_vm_enter(fighter, fn_cage_jab_repeat, fighter->execution.script.finish);
          return true;
        case native_cage_jab_recovery:
          script_vm_enter(fighter, fn_cage_jab_recovery, fighter->execution.script.finish);
          return true;
        case native_air_lk_tail:
          action_air_lk_tail(fighter);
          return true;
        case native_air_lk_alt_tail:
          action_air_lk_alt_tail(fighter);
          return true;
        case native_air_lk_wide_tail:
          action_air_lk_wide_tail(fighter);
          return true;
        case native_air_diagonal_tail:
          action_air_diagonal_tail(fighter);
          return true;
        case native_air_diagonal_alt_tail:
          action_air_diagonal_alt_tail(fighter);
          return true;
        case native_air_diagonal_wide_tail:
          action_air_diagonal_wide_tail(fighter);
          return true;
        case native_air_hp_tail:
          action_air_hp_tail(fighter);
          return true;
        case native_weapon_air_high_punch_open:
          weapon_air_frames_native(fighter, fighter->execution.script.selector);
          return true;
        case native_weapon_air_low_punch_open:
          weapon_air_frames_native(fighter, fighter->execution.script.selector);
          return true;
        case native_air_lp_tail:
          action_air_lp_tail(fighter);
          return true;
        case native_sweep_advancing_tail:
          action_sweep_advancing_tail(fighter);
          return true;
        case native_sweep_standard_tail:
          action_sweep_standard_tail(fighter);
          return true;
        case native_roundhouse_start:
          action_roundhouse_start(fighter);
          return true;
        case native_roundhouse_tail:
          action_roundhouse_tail(fighter);
          return true;
        case native_roundhouse_wide_start:
          action_roundhouse_wide_start(fighter);
          return true;
        case native_roundhouse_wide_tail:
          action_roundhouse_wide_tail(fighter);
          return true;
        case native_return_to_player_or_cpu_control:
          action_controller_return(fighter);
          return true;
        case native_victory_continue:
          action_victory_continue(fighter);
          return true;
        case native_kai_lunge_kick_contact:
          kai_lunge_kick_contact(fighter);
          return true;
        case native_cage_script_cpu_kick_tail:
          cage_script_cpu_kick_tail(fighter);
          return true;
        case native_cage_script_powered_punch_tail:
          cage_script_powered_punch_tail(fighter);
          return true;
        case native_quan_chi_script_flip_kick_tail:
          quan_chi_script_flip_kick_tail(fighter);
          return true;
        case native_reptile_script_dash_contact:
          reptile_script_dash_contact(fighter);
          return true;
        case native_jax_dash_punch_contact:
          jax_dash_punch_contact(fighter);
          return true;
        case native_reptile_script_roll_tail:
          reptile_script_roll_tail(fighter);
          return true;
        case native_jarek_quake_tail:
          jarek_quake_tail(fighter);
          return true;
        case native_sonya_script_cartwheel_tail:
          sonya_script_cartwheel_tail(fighter);
          return true;
        case native_tanya_drill_kick_contact:
          tanya_drill_kick_contact(fighter);
          return true;
        case native_kai_aerial_punch_contact:
          kai_aerial_punch_contact(fighter);
          return true;
        case native_fujin_dive_contact:
          fujin_dive_contact(fighter);
          return true;
        case native_fujin_dive_land:
          fujin_dive_land(fighter);
          return true;
        case native_teleport_vanish:
          action_teleport_vanish(fighter);
          return true;
        case native_teleport_slam_fall:
          action_teleport_slam_fall(fighter);
          return true;
        case native_teleport_slam_drop:
          action_teleport_slam_drop(fighter);
          return true;
        case native_kai_script_stage_event:
          kai_script_stage_event(fighter);
          return true;
        case native_kai_handstand_spin_begin:
          kai_handstand_spin_begin(fighter);
          return true;
        case native_handstand_front:
          script_vm_enter(fighter, fn_handstand_front, fighter->execution.script.finish);
          return true;
        case native_kai_script_contact_b10_continuation:
          kai_script_contact_b10_continuation(fighter);
          return true;
        case native_kai_handstand_rear_contact:
          kai_handstand_rear_contact(fighter);
          return true;
        case native_raiden_script_torpedo_miss:
          raiden_script_torpedo_miss(fighter);
          return true;
        case native_raiden_script_torpedo_contact:
          raiden_script_torpedo_contact(fighter);
          return true;
        case native_recoil_land:
          action_recoil_land(fighter);
          return true;
        case native_sub_zero_slide_script:
          sub_zero_slide_script(fighter);
          return true;
        case native_sub_zero_script_slide_return:
          sub_zero_script_slide_return(fighter);
          return true;
        case native_sonya_script_leg_grab_tail:
          sonya_script_leg_grab_tail(fighter);
          return true;
        case native_sonya_script_square_wave_tail:
          sonya_script_square_wave_tail(fighter);
          return true;
        case native_liu_kang_bicycle_script:
          liu_kang_bicycle_script(fighter);
          return true;
        case native_liu_kang_bicycle_land:
          liu_kang_liu_script_land(fighter);
          return true;
        case native_liu_kang_flying_kick_script:
          liu_kang_flying_kick_script(fighter);
          return true;
        case native_liu_kang_flying_kick_land:
          liu_kang_liu_script_land(fighter);
          return true;
        case native_fujin_triple_fast:
          fujin_triple_fast(fighter);
          return true;
        case native_fujin_triple_contact:
          fujin_triple_contact(fighter);
          return true;
        case native_scorpion_teleport_script:
          scorpion_teleport_script(fighter);
          return true;
        case native_land:
          mk4_land(fighter);
          return true;
        case native_fujin_grab_control_start:
          fujin_grab_control_start(fighter);
          return true;
        case native_scorpion_spin_script:
          scorpion_spin_script(fighter);
          return true;
        case native_reptile_second_melt:
          reptile_second_melt(fighter);
          return true;
        case native_fujin_second_burst:
          fujin_second_burst(fighter);
          return true;
        case native_scene_body_finish:
          action_scene_body_finish(fighter);
          return true;
        case native_reptile_weapon_stagger_tail:
          reptile_weapon_stagger_tail(fighter);
          return true;
        case native_goro_script_uppercut_victim_slide:
          goro_script_uppercut_victim_slide(fighter);
          return true;
        case native_goro_victim_getup:
          goro_victim_getup(fighter);
          return true;
        case native_reaction_clip_129_startup:
          script_vm_enter(fighter, fn_reaction_clip_129_startup, mk4_script_thrown_ready);
          return true;
        case native_reaction_clip_129_wait_landing:
          script_vm_enter(fighter, fn_reaction_clip_129_wait_landing, mk4_script_thrown_ready);
          return true;
        case native_reaction_clip_129_getup:
          script_vm_enter(fighter, fn_reaction_clip_129_wait_landing_tail_1_operand_1, mk4_script_thrown_ready);
          return true;
        case native_reaction_getup_back:
          action_reaction_getup_back(fighter);
          return true;
        case native_reaction_fall:
          action_reaction_fall(fighter);
          return true;
        case native_reaction_finish:
          action_reaction_finish(fighter);
          return true;
        case native_shinnok_weapon_sweep_getup:
          shinnok_weapon_sweep_getup(fighter);
          return true;
        case native_shinnok_weapon_sweep_finish:
          shinnok_weapon_sweep_finish(fighter);
          return true;
        case native_stage_impale:
          action_stage_impale(fighter);
          return true;
        case native_stage_complete:
          action_stage_complete(fighter);
          return true;
        case native_stagger_tail:
          action_stagger_tail(fighter);
          return true;
        case native_goro_script_stomp_victim_hold:
          goro_script_stomp_victim_hold(fighter);
          return true;
        case native_quan_chi_script_stomp_victim:
          quan_chi_script_stomp_victim(fighter);
          return true;
        case native_air_flip_getup:
          action_air_flip_getup(fighter);
          return true;
        case native_air_lift_getup:
          action_air_lift_getup(fighter);
          return true;
        case native_mmove_air_getup:
          mk4_mmove_air_getup(fighter);
          return true;
        case native_scorpion_script_teleport_victim_tail:
          scorpion_script_teleport_victim_tail(fighter);
          return true;
        case native_throw_victim_continue:
          action_throw_victim_continue(fighter);
          return true;
        case native_throw_victim_impact:
          action_throw_victim_impact(fighter);
          return true;
        case native_throw_victim_stop_and_return:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case native_sonya_script_leg_victim_end:
          sonya_script_leg_victim_end(fighter);
          return true;
        case native_liu_kang_liu_script_lk_thrown_tail:
          liu_kang_liu_script_lk_thrown_tail(fighter);
          return true;
        case native_lp_sliding_impact_flip_getup:
          action_getup_flip(fighter);
          return true;
        case native_getup_roll_flip:
          action_getup_roll_flip(fighter);
          return true;
        case native_lp_two_speed_slide_double_flip_roll_getup:
          /* 490350 twice, through488bd0, leaves the original facing. */
          action_getup_roll(fighter);
          return true;
        case native_thaw_landing:
          action_thaw_land(fighter);
          return true;
        case native_shake_stun:
          action_shake_stun(fighter);
          return true;
        case native_sub_zero_script_lk_victim_rise:
          sub_zero_script_lk_victim_rise(fighter);
          return true;
        case native_vm_return:
          script_vm_return(fighter);
          return true;
        case native_pair_release:
          action_pair_release(fighter);
          return true;
        case native_bicycle_cycle:
          action_bicycle_cycle(fighter);
          return true;
        case native_uppercut_recover:
          action_uppercut_recover(fighter);
          return true;
        case native_block_cycle:
          action_block_cycle(fighter);
          return true;
        case native_standing_block_hold:
          action_controller_return(fighter);
          return true;
        case native_scorpion_spear_stun_script:
          scorpion_spear_stun_script(fighter);
          return true;
        case native_raiden_script_staff_victim_tail:
          raiden_script_staff_victim_tail(fighter);
          return true;
        case native_kai_handstand_victim_tail:
          kai_handstand_victim_tail(fighter);
          return true;
        case native_mmove_side_getup:
          mk4_mmove_side_getup(fighter);
          return true;
        case native_lp_table_victim_stop_and_flip_getup:
        case native_sub_zero_lp_throw_victim_getup:
          action_getup_flip(fighter);
          return true;
        case native_getup_back:
          action_getup_back(fighter);
          return true;
        case native_jax_dash_punch_roll_getup:
          action_getup_roll(fighter);
          return true;
        case native_getup_front:
          action_getup_front(fighter);
          return true;
        case native_cage_weapon_upper_contact:
          action_weapon_contact(fighter, address);
          return true;
        case native_kai_weapon_back_strike_contact:
          action_weapon_contact(fighter, address);
          return true;
        case native_reptile_weapon_flurry:
          reptile_weapon_flurry(fighter);
          return true;
        case native_liu_kang_liu_script_sword_blend:
          liu_kang_liu_script_sword_blend(fighter);
          return true;
        case native_liu_kang_sword_back_high_punch_contact:
          action_weapon_contact(fighter, address);
          return true;
        case native_cage_weapon_back_lp_contact:
          action_weapon_contact(fighter, address);
          return true;
        case native_tanya_boomerang_release:
          tanya_boomerang_release(fighter);
          return true;
        case native_fireball_fall:
          action_fireball_fall(fighter);
          return true;
        case native_kai_ground_skull_spawn:
          kai_ground_skull_spawn(fighter);
          return true;
        case native_kai_rising_skull_spawn:
          kai_rising_skull_spawn(fighter);
          return true;
        case native_kai_rising_skull_finish:
          kai_rising_skull_finish(fighter);
          return true;
        case native_quan_chi_script_skull_release:
          quan_chi_script_skull_release(fighter);
          return true;
        case native_fujin_bow_finish:
          fujin_bow_finish(fighter);
          return true;
        case native_scorpion_script_spear_return:
          scorpion_script_spear_return(fighter);
          return true;
        case native_liu_kang_liu_script_air_return:
          liu_kang_liu_script_air_return(fighter);
          return true;
        case native_task_retire:
          action_task_retire(fighter);
          return true;
        default:
          return false;
      }
    case MK4_SPAWN:
      switch (address) {
        case native_jax_fatality_2_sparks:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_sparks(fighter, (int)argument);
          return true;
        case native_jax_fatality_2_flash:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_flash(fighter, (int)argument);
          return true;
        case native_cage_second_charge:
          if (argument != 619u) {
            return false;
          }
          cage_second_charge(fighter, (int)argument);
          return true;
        case native_cage_second_flash:
          if (argument != 619u) {
            return false;
          }
          cage_second_flash(fighter, (int)argument);
          return true;
        case native_breath_shatter_decoration:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_freeze(fighter, (int)argument);
          return true;
        case native_breath_scene_decoration:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_object(fighter, (int)argument);
          return true;
        case native_reiko_second_stars:
          if (argument != 0u) {
            return false;
          }
          reiko_second_stars(fighter, (int)argument);
          return true;
        case native_jarek_second_head_fall:
          if (argument != 0u) {
            return false;
          }
          jarek_second_head_fall(fighter, (int)argument);
          return true;
        case native_reptile_second_acid:
          if (argument != 0u) {
            return false;
          }
          reptile_second_acid(fighter, (int)argument);
          return true;
        case native_scorpion_script_beast_gore:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_beast_gore(fighter, (int)argument);
          return true;
        case native_fujin_second_blood:
          if (argument != 38u) {
            return false;
          }
          fujin_second_blood(fighter, (int)argument);
          return true;
        case native_kai_second_skull:
          if (argument != 38u) {
            return false;
          }
          kai_second_skull(fighter, (int)argument);
          return true;
        case native_liu_finisher_projectile:
          if (argument != 38u) {
            return false;
          }
          liu_kang_liu_script_fatal_spawn(fighter, (int)argument);
          return true;
        case native_scorpion_script_transform_effect:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_transform_effect(fighter, (int)argument);
          return true;
        case native_scene_keepalive_task:
          if (argument != 619u) {
            return false;
          }
          mk4_script_scene_task(fighter, (int)argument);
          return true;
        case native_tanya_second_owner_cue:
          if (argument != 619u) {
            return false;
          }
          tanya_second_owner_cue(fighter, (int)argument);
          return true;
        case native_tanya_second_victim_cue:
          if (argument != 619u) {
            return false;
          }
          tanya_second_victim_cue(fighter, (int)argument);
          return true;
        case native_scene_hover:
          if (argument != 38u) {
            return false;
          }
          action_scene_hover(fighter, (int)argument);
          return true;
        case native_reiko_star_spawn:
          if (argument != 82u) {
            return false;
          }
          reiko_star_spawn(fighter, (int)argument);
          return true;
        case native_jax_missile_spawn:
          if (argument != 80u) {
            return false;
          }
          jax_missile_spawn(fighter, (int)argument);
          return true;
        case native_cage_orb_spawn:
          if (argument != 79u) {
            return false;
          }
          cage_orb_spawn(fighter, (int)argument);
          return true;
        case native_jarek_blade_spawn:
          if (argument != 78u) {
            return false;
          }
          jarek_blade_spawn(fighter, (int)argument);
          return true;
        case native_reptile_acid_spawn:
          if (argument != 76u) {
            return false;
          }
          reptile_acid_spawn(fighter, (int)argument);
          return true;
        case native_fireball_spawn:
          if (argument != 75u) {
            return false;
          }
          action_fireball_spawn(fighter, (int)argument);
          return true;
        case native_air_fireball_spawn:
          if (argument != 75u) {
            return false;
          }
          action_air_fireball_spawn(fighter, (int)argument);
          return true;
        case native_spear_task:
          if (argument != 67u) {
            return false;
          }
          scorpion_script_spawn_spear(fighter, (int)argument);
          return true;
        case native_sonya_script_ring_flash:
          if (argument != 38u) {
            return false;
          }
          sonya_script_ring_flash(fighter, (int)argument);
          return true;
        case native_sonya_ring_object:
          if (argument != 68u) {
            return false;
          }
          sonya_script_ring_spawn(fighter, (int)argument);
          return true;
        case native_ice_projectile_task:
          if (argument != 66u) {
            return false;
          }
          sub_zero_script_spawn_ice(fighter, (int)argument);
          return true;
        case native_raiden_lightning_object:
          if (argument != 65u) {
            return false;
          }
          raiden_script_lightning_spawn(fighter, (int)argument);
          return true;
        case native_liu_air_fireball_object:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_air(fighter, (int)argument);
          return true;
        case native_liu_low_fireball_object:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_low(fighter, (int)argument);
          return true;
        case native_liu_high_fireball_object:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_high(fighter, (int)argument);
          return true;
        case native_goro_fireball_spawn:
          if (argument != 83u) {
            return false;
          }
          goro_fireball_spawn(fighter, (int)argument);
          return true;
        default:
          return false;
      }
    case MK4_HANDLER:
      switch (address) {
        case native_context:
          mk4_script_context(fighter, (int)(argument & 255u));
          return true;
        case native_script_reaction_dispatch:
          action_script_reaction_dispatch(fighter, (int)(argument & 255u));
          return true;
        case native_dispatch_opponent_reaction:
          if (action_reaction_entry(fighter->execution.script.image, argument & 255u) == NULL) {
            return false;
          }
          action_script_release_reaction(fighter, (int)(argument & 255u));
          return true;
        case native_face_peer_and_resume_script:
          action_face_peer(fighter);
          return true;
        default:
          return false;
      }
    case MK4_PREDICATE:
      switch (address) {
        case native_liu_kang_liu_script_sword_gate:
          fighter->execution.script.result = liu_kang_liu_script_sword_gate(fighter);
          return true;
        case native_jab_repeat_predicate:
          fighter->execution.script.result = mk4_script_jab_gate(fighter);
          return true;
        case native_contact_query_d:
          fighter->execution.script.result = raiden_script_torpedo_query(fighter);
          return true;
        default:
          return false;
      }
    case MK4_PHYSICS:
      switch (address) {
        case native_landing_motion:
          fighter->body.physics = mk4_script_landing_motion;
          return true;
        case native_raiden_script_torpedo_motion:
          fighter->body.physics = raiden_script_torpedo_motion;
          return true;
        case native_teleport_group_callback:
          fighter->body.physics = scorpion_script_teleport_motion;
          return true;
        case native_liu_kang_liu_script_bicycle_motion:
          fighter->body.physics = liu_kang_liu_script_bicycle_motion;
          return true;
        case native_goro_stomp_hold_separation:
          fighter->body.physics = goro_stomp_hold_separation;
          return true;
        case native_raiden_script_torpedo_victim_motion:
          fighter->body.physics = raiden_script_torpedo_victim_motion;
          return true;
        case native_face_peer_orient_group:
          fighter->body.physics = action_face_peer;
          return true;
        case native_velocity_friction:
          fighter->body.physics = fighter_friction;
          return true;
        default:
          return false;
      }
  }
  return false;
}

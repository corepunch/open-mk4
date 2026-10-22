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
        case RETAIL_NATIVE_FLIP_FACING_490350:
          retail_flip_facing(fighter);
          return true;
        case RETAIL_NATIVE_IMPACT_VOICE_48A350:
          retail_impact_voice(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SECOND_CHILD_WALK_406BA0:
          kai_second_child_walk(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE_40EB80:
          reptile_script_head_close(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_OPEN_40EC40:
          reptile_script_head_open(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_WIDE_40ED20:
          reptile_script_head_wide(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_ACID_DROOL_40EE00:
          reptile_script_acid_drool(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT_410380:
          raiden_script_victory_effect(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_FIRE_DECOR_416AE0:
          scorpion_script_fire_decor(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_FIRE_FLAME_416D40:
          scorpion_script_fire_flame(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_GORE_41DB70:
          shinnok_second_gore(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_GORE_41DBA0:
          shinnok_second_gore(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_GORE_41DBD0:
          shinnok_second_gore(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_GLOW_41DE30:
          jarek_second_glow(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES_427B70:
          raiden_script_second_particles(fighter);
          return true;
        case RETAIL_NATIVE_THROW_PAIR_START_42B680:
          retail_throw_pair_start(fighter);
          return true;
        case RETAIL_NATIVE_DISTURB_RESTING_WEAPONS_43A5C0:
          retail_disturb_resting_weapons();
          return true;
        case RETAIL_NATIVE_AVOID_CLEAR_46A950:
          mk4_script_avoid_clear(fighter);
          return true;
        case RETAIL_NATIVE_AVOID_SET_46A970:
          mk4_script_avoid_set(fighter);
          return true;
        case RETAIL_NATIVE_JAB_CONTACT_46DDD0:
          mk4_script_jab_contact(fighter);
          return true;
        case RETAIL_NATIVE_JAB_REPEAT_PREDICATE_46DE60:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP_47D720:
          raiden_script_torpedo_step(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_VOICE_48A1F0:
          scorpion_script_beast_voice(fighter);
          return true;
        case RETAIL_NATIVE_THROW_GRUNT_48A220:
          retail_throw_grunt();
          return true;
        case RETAIL_NATIVE_SAVE_MOTION_48F6E0:
          mk4_script_save_motion(fighter);
          return true;
        case RETAIL_NATIVE_RESTORE_MOTION_48F750:
          mk4_script_restore_motion(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN_48F9F0:
          liu_kang_liu_script_fatal_align(fighter);
          return true;
        case RETAIL_NATIVE_TOUCHABLE_490290:
          mk4_script_touchable(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_494520:
          mk4_script_disarm_self(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_TAIL:
      switch (address) {
        case RETAIL_NATIVE_MMOVE_THROWN_FINISH_46F650:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case RETAIL_NATIVE_LK_CONTINUE_840_42C7D0:
          retail_lk_continue_840(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END_41B080:
          sub_zero_script_breath_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH_41B440:
          sub_zero_script_breath_approach(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE_41B450:
          sub_zero_script_breath_slide(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER_41B570:
          sub_zero_script_breath_shatter(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_SECOND_VICTIM_FALL_41C4B0:
          reiko_second_victim_fall(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_SECOND_VICTIM_END_41C4E0:
          reiko_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_HANDOFF_41DFC0:
          jarek_second_handoff(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_VICTIM_HOLD_41E0A0:
          jarek_second_victim_hold(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN_41EA50:
          scorpion_script_beast_begin(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK_41EBD0:
          scorpion_script_beast_attack(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES_41EC70:
          scorpion_script_beast_victim_pieces(fighter);
          return true;
        case RETAIL_NATIVE_TASK_PAUSE_41F1B0:
          retail_task_pause(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_42BDF0:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4DE318, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_42BE00:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4DE328, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_LK_RETURN_42B730:
        case RETAIL_NATIVE_LK_RETURN_42B7A0:
          retail_lk_return(fighter);
          return true;
        case RETAIL_NATIVE_LK_PAUSE_LONG_42BB80:
          retail_lk_pause_long(fighter);
          return true;
        case RETAIL_NATIVE_LK_PAUSE_SHORT_42BD30:
          retail_lk_pause_short(fighter);
          return true;
        case RETAIL_NATIVE_LK_JUMP_BACK_42BB00:
          retail_lk_jump_back(fighter);
          return true;
        case RETAIL_NATIVE_LK_RECOIL_42B840:
          retail_lk_recoil(fighter);
          return true;
        case RETAIL_NATIVE_LK_BRANCH_CONTINUE_42B800:
          retail_lk_branch_continue(fighter);
          return true;
        case RETAIL_NATIVE_LK_VICTIM_RETURN_42BC90:
          retail_lk_victim_return(fighter);
          return true;
        case RETAIL_NATIVE_LK_SCRIPT_170_42BC10:
          retail_lk_script_170(fighter);
          return true;
        case RETAIL_NATIVE_LK_SCRIPT_180_42BC20:
          retail_lk_script_180(fighter);
          return true;
        case RETAIL_NATIVE_LK_SCRIPT_190_42BC30:
          retail_lk_script_190(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_LK_OWNER_READY_42BE10:
          mk4_mmove_lk_owner_ready(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_LK_BLEND_RETURN_42BF00:
          mk4_mmove_lk_blend_return(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE_42C0B0:
          liu_kang_liu_script_lk_continue(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE_42C0D0:
          liu_kang_liu_script_lk_damage(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_42C170:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4DE478, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE_42C200:
          sub_zero_script_lk_rebase(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE_42C220:
          sub_zero_script_lk_damage(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN_42C300:
          sub_zero_script_lk_owner_return(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_42C770:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_SCRIPT_LP_PHASE_REPEAT_4DE7E8, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT_42C7B0:
          goro_script_lp_phase_repeat(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_42C7F0:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4DE4F0, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_FIRST_42C820:
          jax_lp_slam_first(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_SECOND_42C8C0:
          jax_lp_slam_second(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_THIRD_42C8F0:
          jax_lp_slam_third(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_THROW_RETURN_42C9F0:
          jax_lp_throw_return(fighter);
          return true;
        case RETAIL_NATIVE_LP_SELECT_SHORT_42CB80:
          retail_lp_select_short(fighter);
          return true;
        case RETAIL_NATIVE_THROW_READY_42CBC0:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_LP_SELECT_LONG_42CC00:
          retail_lp_select_long(fighter);
          return true;
        case RETAIL_NATIVE_LP_END_42CC40:
          retail_lp_end(fighter);
          return true;
        case RETAIL_NATIVE_THROW_READY_42CE60:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_THROW_READY_42CE80:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_PUNCH_TAIL_43AFC0:
          goro_script_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_KICK_TAIL_43B1A0:
          goro_script_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_LEAP_CONTINUE_43B370:
          goro_script_leap_continue(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_STOMP_RETURN_43B590:
          goro_script_stomp_return(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_QUAKE_LAND_43B8C0:
          goro_script_quake_land(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP_43C500:
          liu_kang_liu_script_fatal_setup(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_VANISH_43C820:
          shinnok_second_vanish(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_CLAP_43CA30:
          shinnok_second_clap(fighter);
          return true;
        case RETAIL_NATIVE_FAN_TAIL_43D460:
          retail_fan_tail(fighter);
          return true;
        case RETAIL_NATIVE_FAN_CYCLE_43D510:
          retail_fan_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FAN_CYCLE_43D530:
          retail_fan_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FAN_VICTORY_43D6F0:
          retail_fan_victory(fighter);
          return true;
        case RETAIL_NATIVE_FAN_VICTIM_END_43DB50:
          retail_fan_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_FIRST_RELEASE_43F510:
          sonya_script_first_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE_441140:
          raiden_script_first_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_FIRST_END_441180:
          raiden_script_first_end(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_BEGIN_443930:
          scorpion_script_burn_begin(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START_443A30:
          scorpion_script_burn_victim_start(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1_443A70:
          scorpion_script_burn_repeat1(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2_443B40:
          scorpion_script_burn_repeat2(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTORY_443C10:
          scorpion_script_burn_victory(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE_444000:
          scorpion_script_burn_victim_cue(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT_444030:
          scorpion_script_burn_victim_repeat(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND_444060:
          scorpion_script_burn_victim_blend(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT_444100:
          scorpion_script_burn_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE_444150:
          scorpion_script_burn_victim_settle(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END_444190:
          scorpion_script_burn_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN_445150:
          sub_zero_script_first_begin(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL_445220:
          sub_zero_script_first_pull(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1_445240:
          sub_zero_script_first_cues1(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2_445280:
          sub_zero_script_first_cues2(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3_4452B0:
          sub_zero_script_first_cues3(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4_4452E0:
          sub_zero_script_first_cues4(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD_445310:
          sub_zero_script_first_head(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS_445510:
          sub_zero_script_first_victim_blends(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE_4456C0:
          sub_zero_script_first_victim_collapse(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END_445700:
          sub_zero_script_first_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_HEART_446F80:
          jarek_first_heart(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_RIP_447310:
          jarek_first_victim_rip(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_IMPACT_4473B0:
          jarek_first_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_END_447410:
          jarek_first_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL_448480:
          cage_script_fatality_1_signal(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_59_4487B0:
          cage_first_victim_59(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_131_4487F0:
          cage_first_victim_131(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_148_448830:
          cage_first_victim_148(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_166_448870:
          cage_first_victim_166(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_185_4488B0:
          cage_first_victim_185(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_206_4488F0:
          cage_first_victim_206(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_SEVER_448930:
          cage_first_sever(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_HALF_448C80:
          cage_first_torso_half(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_123_448D20:
          cage_first_torso_123(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_FINISH_448D70:
          cage_first_torso_finish(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_VICTORY_449620:
          jax_fatality_1_victory(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_GRAB_449890:
          jax_fatality_1_grab(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_FIRST_ARM_4498D0:
          jax_fatality_1_first_arm(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_SECOND_ARM_449940:
          jax_fatality_1_second_arm(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_GRAB_44AEB0:
          kai_first_owner_grab(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_SPIN_44AF20:
          kai_first_owner_spin(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_TEAR_44AF30:
          kai_first_owner_tear(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_71_44B260:
          kai_first_victim_71(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_TURN_44B290:
          kai_first_victim_turn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_SEVER_44B2C0:
          kai_first_victim_sever(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_TORSO_203_44B600:
          kai_first_torso_203(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_TORSO_IMPACT_44B650:
          kai_first_torso_impact(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_CUE_40_44BE10:
          quan_chi_fatality_cue_40(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_LEG_44BE40:
          quan_chi_fatality_leg(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_HIT_179_44BE60:
          quan_chi_fatality_hit_179(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_HIT_241_44BEB0:
          quan_chi_fatality_hit_241(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_44BF00:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E1548, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_BEAT_44BF10:
          quan_chi_fatality_beat(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_53_44D4A0:
          tanya_first_owner_53(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_DRIFT_44D4E0:
          tanya_first_owner_drift(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_REPLAYS_44D510:
          tanya_first_owner_replays(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_LOOP_44D530:
          tanya_first_owner_loop(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_SWELL_44D6E0:
          tanya_first_victim_swell(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_REPEAT_44D750:
          tanya_first_victim_repeat(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_BURST_44D7A0:
          tanya_first_victim_burst(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_LOOP_START_44F5E0:
          fujin_first_loop_start(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_CYCLE_44F620:
          fujin_first_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_SHOOT_44F6F0:
          fujin_first_shoot(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTORY_4528A0:
          reiko_first_victory(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TEAR_452AC0:
          reiko_first_tear(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTIM_118_452BB0:
          reiko_first_victim_118(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTIM_165_452C10:
          reiko_first_victim_165(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_452C50:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TORSO_29_452E50:
          reiko_first_torso_29(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TORSO_END_452E90:
          reiko_first_torso_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_HEAD_59_452F00:
          reiko_first_head_59(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_HEAD_83_452F40:
          reiko_first_head_83(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_452F90:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_LEFT_ARM_56_452FE0:
          reiko_first_left_arm_56(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_LEFT_ARM_LAND_453020:
          reiko_first_left_arm_land(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_453040:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_RIGHT_ARM_44_453090:
          reiko_first_right_arm_44(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_RIGHT_ARM_LAND_4530D0:
          reiko_first_right_arm_land(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_4530F0:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_CYCLE_453AE0:
          reptile_first_owner_cycle(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_BITE_453B20:
          reptile_first_owner_bite(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_LOOP_453BA0:
          reptile_first_owner_loop(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_BITE_454130:
          reptile_first_victim_bite(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_SHAKE_4541B0:
          reptile_first_victim_shake(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE_4541E0:
          reptile_first_victim_collapse(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VANISHED_455110:
          shinnok_first_vanished(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_HIDDEN_455140:
          shinnok_first_hidden(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FATALITY_REAPPEAR_4551B0:
          shinnok_fatality_reappear(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FATALITY_VICTORY_455230:
          shinnok_fatality_victory(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_VANISHED_455330:
          shinnok_second_vanished(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_HANDS_455360:
          shinnok_second_hands(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE_4558B0:
          shinnok_first_hand_squeeze(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_SQUEEZE_CYCLE_4558D0:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_HELD_455AE0:
          shinnok_first_victim_held(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE_455B60:
          shinnok_first_victim_squeeze(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_SQUEEZE_CYCLE_455B80:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH_455BA0:
          shinnok_first_victim_crush(fighter);
          return true;
        case RETAIL_NATIVE_CROUCH_RETURN_45FC40:
          retail_crouch_return(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND_464960:
          raiden_script_second_blend(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT_464A60:
          raiden_script_second_effect(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_464A90:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E5728, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE_464AA0:
          raiden_script_second_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_END_464BB0:
          raiden_script_second_end(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END_464EE0:
          raiden_script_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_HEIGHT_4650B0:
          tanya_second_height(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_FINISH_465130:
          tanya_second_finish(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_465250:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E5860, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE_4658A0:
          cage_script_fatality_2_continue(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS_4658F0:
          cage_second_repeated_uppercuts(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_VICTIM_FINISH_465B10:
          cage_second_victim_finish(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_OPEN_465E10:
          sonya_script_second_open(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_REPEAT_465E70:
          sonya_script_second_repeat(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_BRANCH_465EA0:
          sonya_script_second_branch(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_PAUSE_465ED0:
          sonya_script_second_pause(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END_466000:
          sonya_script_second_cycle_end(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END_466190:
          sonya_script_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_APPROACH_466A90:
          jax_fatality_2_approach(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_FINISH_466BB0:
          jax_fatality_2_finish(fighter);
          return true;
        case RETAIL_NATIVE_TASK_PAUSE_466DF0:
          retail_task_pause(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_OWNER_467FC0:
          retail_stage_owner(fighter);
          return true;
        case RETAIL_NATIVE_FIRE_CONTACT:
          scorpion_fire_script(fighter);
          return true;
        case RETAIL_NATIVE_FIRE_RECOVERY:
          retail_fire_recovery_finish(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_INVISIBILITY_4698B0:
          reptile_script_invisibility(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SECOND_VICTIM_FALL_46A500:
          kai_second_victim_fall(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_WIND_RECOVERY_46A650:
          fujin_wind_recovery(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_DEFAULT_46A6B0:
          retail_kick_contact_default(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A6E0:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_WIDE_46A760:
          retail_kick_contact_wide(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_NARROW_46A790:
          retail_kick_contact_narrow(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A850:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A8D0:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_FALL_46A9F0:
          retail_air_lift_fall(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_RELEASE_46AA40:
          retail_air_lift_release(fighter);
          return true;
        case RETAIL_NATIVE_AIR_THROW_RELEASE_46AAE0:
          retail_air_throw_release(fighter);
          return true;
        case RETAIL_NATIVE_AIR_THROW_LAND_46AB20:
          retail_air_throw_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_SLAM_FALL_46AC10:
          retail_air_slam_fall(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_46AC90:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E5EB0, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_AIR_SLAM_END_46ACA0:
          retail_air_slam_end(fighter);
          return true;
        case RETAIL_NATIVE_AIR_FLIP_RELEASE_46AED0:
          retail_air_flip_release(fighter);
          return true;
        case RETAIL_NATIVE_CROUCH_LOW_KICK_TAIL_46B740:
          retail_crouch_low_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CLOSE_HIGH_KICK_TAIL_46BB60:
          retail_close_high_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CLOSE_HIGH_PUNCH_TAIL_46BF10:
          retail_close_high_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_COMBO_HIGH_PUNCH_TAIL_46C2C0:
          retail_combo_high_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_STRAIGHT_TAIL_46C7F0:
          retail_air_straight_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LAND_46E240:
          retail_air_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LAND_46DFF0:
          retail_air_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LAND_46C8F0:
          retail_air_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_STRAIGHT_LAND_46C820:
          retail_air_straight_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_STRAIGHT_WIDE_TAIL_46C970:
          retail_air_straight_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_UPPERCUT_DEFAULT_46CDE0:
          native_uppercut_default(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_DRAW_EQUIP_46D260:
          retail_weapon_draw_equip(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_46DD00:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E6330, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_46DD10:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E6360, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_46DD20:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E63B0, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_AIR_LK_TAIL_46DF90:
          retail_air_lk_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LK_ALT_TAIL_46E070:
          retail_air_lk_alt_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LK_WIDE_TAIL_46E150:
          retail_air_lk_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_TAIL_46E3D0:
          retail_air_diagonal_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_ALT_TAIL_46E4D0:
          retail_air_diagonal_alt_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_WIDE_TAIL_46E5C0:
          retail_air_diagonal_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_HP_TAIL_46E730:
          retail_air_hp_tail(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_AIR_FRAMES_NATIVE_46EAB0:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case RETAIL_NATIVE_WEAPON_AIR_FRAMES_NATIVE_46EC10:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case RETAIL_NATIVE_AIR_LP_TAIL_46EEA0:
          retail_air_lp_tail(fighter);
          return true;
        case RETAIL_NATIVE_SWEEP_ADVANCING_TAIL_46EFD0:
          retail_sweep_advancing_tail(fighter);
          return true;
        case RETAIL_NATIVE_SWEEP_STANDARD_TAIL_46F080:
          retail_sweep_standard_tail(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_START_46F220:
          retail_roundhouse_start(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_TAIL_46F270:
          retail_roundhouse_tail(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_WIDE_START_46F390:
          retail_roundhouse_wide_start(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_WIDE_TAIL_46F3E0:
          retail_roundhouse_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_CONTROLLER_RETURN_470420:
          retail_controller_return(fighter);
          return true;
        case RETAIL_NATIVE_VICTORY_CONTINUE_4712E0:
          retail_victory_continue(fighter);
          return true;
        case RETAIL_NATIVE_KAI_LUNGE_KICK_CONTACT_478E20:
          kai_lunge_kick_contact(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL_479B70:
          cage_script_cpu_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL_479F00:
          cage_script_powered_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_FLIP_KICK_TAIL_47A3B0:
          quan_chi_script_flip_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_DASH_CONTACT_47A6D0:
          reptile_script_dash_contact(fighter);
          return true;
        case RETAIL_NATIVE_JAX_DASH_PUNCH_CONTACT_47AB50:
          jax_dash_punch_contact(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_ROLL_TAIL_47B010:
          reptile_script_roll_tail(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_QUAKE_TAIL_47B600:
          jarek_quake_tail(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL_47B6C0:
          sonya_script_cartwheel_tail(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_DRILL_KICK_CONTACT_47BA20:
          tanya_drill_kick_contact(fighter);
          return true;
        case RETAIL_NATIVE_KAI_AERIAL_PUNCH_CONTACT_47BD40:
          kai_aerial_punch_contact(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_DIVE_CONTACT_47BF90:
          fujin_dive_contact(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_DIVE_LAND_47BFD0:
          fujin_dive_land(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_VANISH_47C0A0:
          retail_teleport_vanish(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_SLAM_FALL_47C360:
          retail_teleport_slam_fall(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_SLAM_DROP_47C390:
          retail_teleport_slam_drop(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SCRIPT_STAGE_EVENT_47C580:
          kai_script_stage_event(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_SPIN_BEGIN_47C880:
          kai_handstand_spin_begin(fighter);
          return true;
        case RETAIL_NATIVE_HANDSTAND_FRONT_47CC10:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_HANDSTAND_FRONT_4E8358, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_KAI_SCRIPT_CONTACT_B10_CONTINUATION_47CC50:
          kai_script_contact_b10_continuation(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_REAR_CONTACT_47CCC0:
          kai_handstand_rear_contact(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS_47D680:
          raiden_script_torpedo_miss(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT_47D7F0:
          raiden_script_torpedo_contact(fighter);
          return true;
        case RETAIL_NATIVE_RECOIL_LAND_47DAE0:
          retail_recoil_land(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SLIDE_SCRIPT_47DC40:
          sub_zero_slide_script(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN_47DE00:
          sub_zero_script_slide_return(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL_47E0A0:
          sonya_script_leg_grab_tail(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL_47E1D0:
          sonya_script_square_wave_tail(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_BICYCLE_SCRIPT_47EB30:
          liu_kang_bicycle_script(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LAND_47ED40:
          liu_kang_liu_script_land(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT_47EE00:
          liu_kang_flying_kick_script(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LAND_47EF10:
          liu_kang_liu_script_land(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_TRIPLE_FAST_47F060:
          fujin_triple_fast(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_TRIPLE_CONTACT_47F070:
          fujin_triple_contact(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_TELEPORT_SCRIPT_47FCF0:
          scorpion_teleport_script(fighter);
          return true;
        case RETAIL_NATIVE_LAND_47FFC0:
          mk4_land(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_GRAB_CONTROL_START_4803A0:
          fujin_grab_control_start(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SPIN_SCRIPT_4807B0:
          scorpion_spin_script(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SECOND_MELT_481590:
          reptile_second_melt(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_SECOND_BURST_4816F0:
          fujin_second_burst(fighter);
          return true;
        case RETAIL_NATIVE_SCENE_BODY_FINISH_4817C0:
          retail_scene_body_finish(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_WEAPON_STAGGER_TAIL_482280:
          reptile_weapon_stagger_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE_4824F0:
          goro_script_uppercut_victim_slide(fighter);
          return true;
        case RETAIL_NATIVE_GORO_VICTIM_GETUP_482540:
          goro_victim_getup(fighter);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_4829E0:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E91C0, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_4829F0:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E91D0, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_482A00:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_NATIVE_VM_4E91E0, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_REACTION_GETUP_BACK_482790:
          retail_reaction_getup_back(fighter);
          return true;
        case RETAIL_NATIVE_REACTION_FALL_4835F0:
          retail_reaction_fall(fighter);
          return true;
        case RETAIL_NATIVE_REACTION_FINISH_483610:
          retail_reaction_finish(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP_483090:
          shinnok_weapon_sweep_getup(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH_4830A0:
          shinnok_weapon_sweep_finish(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_IMPALE_483720:
          retail_stage_impale(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_COMPLETE_4837C0:
          retail_stage_complete(fighter);
          return true;
        case RETAIL_NATIVE_STAGGER_TAIL_483DF0:
          retail_stagger_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD_484330:
          goro_script_stomp_victim_hold(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_STOMP_VICTIM_484420:
          quan_chi_script_stomp_victim(fighter);
          return true;
        case RETAIL_NATIVE_AIR_FLIP_GETUP_4847E0:
          retail_air_flip_getup(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_GETUP_484A30:
          retail_air_lift_getup(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_AIR_GETUP_484B60:
          mk4_mmove_air_getup(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL_484C00:
          scorpion_script_teleport_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_THROW_VICTIM_CONTINUE_4854C0:
          retail_throw_victim_continue(fighter);
          return true;
        case RETAIL_NATIVE_THROW_VICTIM_IMPACT_4854D0:
          retail_throw_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_THROWN_FINISH_485510:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END_4858D0:
          sonya_script_leg_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL_485930:
          liu_kang_liu_script_lk_thrown_tail(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FLIP_485AD0:
          retail_getup_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_ROLL_FLIP_485B10:
          retail_getup_roll_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_ROLL_485B50:
          /* 490350 twice, through488bd0, leaves the original facing. */
          retail_getup_roll(fighter);
          return true;
        case RETAIL_NATIVE_THAW_LANDING:
          retail_thaw_land(fighter);
          return true;
        case RETAIL_NATIVE_SHAKE_STUN_486000:
          retail_shake_stun(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_VICTIM_RISE_487220:
          sub_zero_script_lk_victim_rise(fighter);
          return true;
        case RETAIL_NATIVE_VM_RETURN_481FB0:
          retail_vm_return(fighter);
          return true;
        case RETAIL_NATIVE_PAIR_RELEASE_487980:
          retail_pair_release(fighter);
          return true;
        case RETAIL_NATIVE_BICYCLE_CYCLE_488890:
          retail_bicycle_cycle(fighter);
          return true;
        case RETAIL_NATIVE_UPPERCUT_RECOVER_483930:
          retail_uppercut_recover(fighter);
          return true;
        case RETAIL_NATIVE_BLOCK_CYCLE_486F30:
          retail_block_cycle(fighter);
          return true;
        case RETAIL_NATIVE_CONTROLLER_RETURN_460250:
          retail_controller_return(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SPEAR_STUN_SCRIPT_487420:
          scorpion_spear_stun_script(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL_488590:
          raiden_script_staff_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_VICTIM_TAIL_488AC0:
          kai_handstand_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_SIDE_GETUP_488B40:
          mk4_mmove_side_getup(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FLIP_485C40:
        case RETAIL_NATIVE_GETUP_FLIP_488B80:
          retail_getup_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_BACK_488BA0:
          retail_getup_back(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_ROLL_488C00:
          retail_getup_roll(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FRONT_488C10:
          retail_getup_front(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_4958B0:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_495AE0:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_REPTILE_WEAPON_FLURRY_495DB0:
          reptile_weapon_flurry(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND_496430:
          liu_kang_liu_script_sword_blend(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_496770:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_496970:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_TANYA_BOOMERANG_RELEASE_497B60:
          tanya_boomerang_release(fighter);
          return true;
        case RETAIL_NATIVE_FIREBALL_FALL_498EB0:
          retail_fireball_fall(fighter);
          return true;
        case RETAIL_NATIVE_KAI_GROUND_SKULL_SPAWN_499000:
          kai_ground_skull_spawn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_RISING_SKULL_SPAWN_499450:
          kai_rising_skull_spawn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_RISING_SKULL_FINISH_499470:
          kai_rising_skull_finish(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE_499780:
          quan_chi_script_skull_release(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_BOW_FINISH_49A480:
          fujin_bow_finish(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN_49A680:
          scorpion_script_spear_return(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN_49C010:
          liu_kang_liu_script_air_return(fighter);
          return true;
        case RETAIL_NATIVE_TASK_RETIRE_49CB60:
          retail_task_retire(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_SPAWN:
      switch (address) {
        case RETAIL_NATIVE_NATIVE_VM_412D80:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_sparks(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_412EC0:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_413970:
          if (argument != 619u) {
            return false;
          }
          cage_second_charge(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_413A10:
          if (argument != 619u) {
            return false;
          }
          cage_second_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_BREATH_SHATTER_DECORATION_41B860:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_freeze(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_BREATH_SCENE_DECORATION_41BBB0:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_object(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_41D260:
          if (argument != 0u) {
            return false;
          }
          reiko_second_stars(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_41E0E0:
          if (argument != 0u) {
            return false;
          }
          jarek_second_head_fall(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_41E490:
          if (argument != 0u) {
            return false;
          }
          reptile_second_acid(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_41ED80:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_beast_gore(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_41EF20:
          if (argument != 38u) {
            return false;
          }
          fujin_second_blood(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_43C140:
          if (argument != 38u) {
            return false;
          }
          kai_second_skull(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_FINISHER_PROJECTILE_43C540:
          if (argument != 38u) {
            return false;
          }
          liu_kang_liu_script_fatal_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_450D40:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_transform_effect(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SCENE_KEEPALIVE_TASK_464BD0:
          if (argument != 619u) {
            return false;
          }
          mk4_script_scene_task(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_465270:
          if (argument != 619u) {
            return false;
          }
          tanya_second_owner_cue(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_465390:
          if (argument != 619u) {
            return false;
          }
          tanya_second_victim_cue(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_46A280:
          if (argument != 38u) {
            return false;
          }
          retail_scene_hover(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_496E00:
          if (argument != 82u) {
            return false;
          }
          reiko_star_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_496FC0:
          if (argument != 80u) {
            return false;
          }
          jax_missile_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_4970D0:
          if (argument != 79u) {
            return false;
          }
          cage_orb_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_497580:
          if (argument != 78u) {
            return false;
          }
          jarek_blade_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_FUNC_TABLE_LOOKUP_CALL_G_TABLE_00498980_498920:
          if (argument != 76u) {
            return false;
          }
          reptile_acid_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_498BB0:
          if (argument != 75u) {
            return false;
          }
          retail_fireball_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_498ED0:
          if (argument != 75u) {
            return false;
          }
          retail_air_fireball_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SPEAR_TASK_49A6A0:
          if (argument != 67u) {
            return false;
          }
          scorpion_script_spawn_spear(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_49B9E0:
          if (argument != 38u) {
            return false;
          }
          sonya_script_ring_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SONYA_RING_OBJECT_49BA50:
          if (argument != 68u) {
            return false;
          }
          sonya_script_ring_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_ICE_PROJECTILE_TASK_49BCB0:
          if (argument != 66u) {
            return false;
          }
          sub_zero_script_spawn_ice(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_RAIDEN_LIGHTNING_OBJECT_49BE10:
          if (argument != 65u) {
            return false;
          }
          raiden_script_lightning_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_AIR_FIREBALL_OBJECT_49C0D0:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_air(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_LOW_FIREBALL_OBJECT_49C110:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_low(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_HIGH_FIREBALL_OBJECT_49C160:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_high(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_NATIVE_VM_49CA90:
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
        case RETAIL_NATIVE_CONTEXT_45AC30:
          mk4_script_context(fighter, (int)(argument & 255u));
          return true;
        case RETAIL_NATIVE_SCRIPT_REACTION_DISPATCH_45B1F0:
          retail_script_reaction_dispatch(fighter, (int)(argument & 255u));
          return true;
        case RETAIL_NATIVE_DISPATCH_OPPONENT_REACTION_45B230:
          if (retail_reaction_entry(fighter->retail_vm.image, argument & 255u) == NULL) {
            return false;
          }
          retail_script_release_reaction(fighter, (int)(argument & 255u));
          return true;
        case RETAIL_NATIVE_FACE_PEER_45B380:
          retail_face_peer(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_PREDICATE:
      switch (address) {
        case RETAIL_NATIVE_NATIVE_VM_46DB00:
          fighter->retail_vm.result = liu_kang_liu_script_sword_gate(fighter);
          return true;
        case RETAIL_NATIVE_JAB_REPEAT_PREDICATE_46DE60:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case RETAIL_NATIVE_CONTACT_QUERY_D_48D450:
          fighter->retail_vm.result = raiden_script_torpedo_query(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_PHYSICS:
      switch (address) {
        case RETAIL_NATIVE_NATIVE_VM_4707E0:
          fighter->body.physics = mk4_script_landing_motion;
          return true;
        case RETAIL_NATIVE_NATIVE_VM_47D6E0:
          fighter->body.physics = raiden_script_torpedo_motion;
          return true;
        case RETAIL_NATIVE_TELEPORT_GROUP_CALLBACK_480040:
          fighter->body.physics = scorpion_script_teleport_motion;
          return true;
        case RETAIL_NATIVE_NATIVE_VM_480E90:
          fighter->body.physics = liu_kang_liu_script_bicycle_motion;
          return true;
        case RETAIL_NATIVE_NATIVE_VM_480F40:
          fighter->body.physics = goro_stomp_hold_separation;
          return true;
        case RETAIL_NATIVE_NATIVE_VM_4889B0:
          fighter->body.physics = raiden_script_torpedo_victim_motion;
          return true;
        case RETAIL_NATIVE_NATIVE_VM_490BD0:
          fighter->body.physics = retail_face_peer;
          return true;
        case RETAIL_NATIVE_VELOCITY_FRICTION_4913A0:
          fighter->body.physics = fighter_friction;
          return true;
        default:
          return false;
      }
  }
  return false;
}

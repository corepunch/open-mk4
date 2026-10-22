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
        case RETAIL_NATIVE_FLIP_FACING:
          retail_flip_facing(fighter);
          return true;
        case RETAIL_NATIVE_IMPACT_VOICE:
          retail_impact_voice(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SECOND_CHILD_WALK:
          kai_second_child_walk(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE:
          reptile_script_head_close(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_OPEN:
          reptile_script_head_open(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_HEAD_WIDE:
          reptile_script_head_wide(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_ACID_DROOL:
          reptile_script_acid_drool(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT:
          raiden_script_victory_effect(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_FIRE_DECOR:
          scorpion_script_fire_decor(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_FIRE_FLAME:
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
        case RETAIL_NATIVE_JAREK_SECOND_GLOW:
          jarek_second_glow(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES:
          raiden_script_second_particles(fighter);
          return true;
        case RETAIL_NATIVE_THROW_PAIR_START:
          retail_throw_pair_start(fighter);
          return true;
        case RETAIL_NATIVE_DISTURB_RESTING_WEAPONS:
          retail_disturb_resting_weapons();
          return true;
        case RETAIL_NATIVE_AVOID_CLEAR:
          mk4_script_avoid_clear(fighter);
          return true;
        case RETAIL_NATIVE_AVOID_SET:
          mk4_script_avoid_set(fighter);
          return true;
        case RETAIL_NATIVE_JAB_CONTACT:
          mk4_script_jab_contact(fighter);
          return true;
        case RETAIL_NATIVE_JAB_REPEAT_PREDICATE:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP:
          raiden_script_torpedo_step(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_VOICE:
          scorpion_script_beast_voice(fighter);
          return true;
        case RETAIL_NATIVE_THROW_GRUNT:
          retail_throw_grunt();
          return true;
        case RETAIL_NATIVE_SAVE_MOTION:
          mk4_script_save_motion(fighter);
          return true;
        case RETAIL_NATIVE_RESTORE_MOTION:
          mk4_script_restore_motion(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN:
          liu_kang_liu_script_fatal_align(fighter);
          return true;
        case RETAIL_NATIVE_TOUCHABLE:
          mk4_script_touchable(fighter);
          return true;
        case RETAIL_NATIVE_DISARM_SELF:
          mk4_script_disarm_self(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_TAIL:
      switch (address) {
        case RETAIL_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case RETAIL_NATIVE_LP_ALIGN_AND_RELEASE:
          retail_lp_align_and_release(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END:
          sub_zero_script_breath_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH:
          sub_zero_script_breath_approach(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE:
          sub_zero_script_breath_slide(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER:
          sub_zero_script_breath_shatter(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_SECOND_VICTIM_FALL:
          reiko_second_victim_fall(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_SECOND_VICTIM_END:
          reiko_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_HANDOFF:
          jarek_second_handoff(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_VICTIM_HOLD:
          jarek_second_victim_hold(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN:
          scorpion_script_beast_begin(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK:
          scorpion_script_beast_attack(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES:
          scorpion_script_beast_victim_pieces(fighter);
          return true;
        case RETAIL_NATIVE_TASK_PAUSE_41F1B0:
          retail_task_pause(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_LK_THROW_PAIRED_CONTINUATION:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_LK_THROW_DAMAGE, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_CAGE_LK_THROW_RELEASE:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_LK_THROW_RELEASE, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_LK_RETURN_42B730:
        case RETAIL_NATIVE_LK_RETURN_42B7A0:
          retail_lk_return(fighter);
          return true;
        case RETAIL_NATIVE_LK_PAUSE_LONG:
          retail_lk_pause_long(fighter);
          return true;
        case RETAIL_NATIVE_LK_PAUSE_SHORT:
          retail_lk_pause_short(fighter);
          return true;
        case RETAIL_NATIVE_LK_JUMP_BACK:
          retail_lk_jump_back(fighter);
          return true;
        case RETAIL_NATIVE_LK_RECOIL:
          retail_lk_recoil(fighter);
          return true;
        case RETAIL_NATIVE_LK_BRANCH_CONTINUE:
          retail_lk_branch_continue(fighter);
          return true;
        case RETAIL_NATIVE_LK_VICTIM_RETURN:
          retail_lk_victim_return(fighter);
          return true;
        case RETAIL_NATIVE_LK_THROW_ROCK_REPEAT_ONE:
          retail_lk_rock_repeat_one(fighter);
          return true;
        case RETAIL_NATIVE_LK_THROW_ROCK_REPEAT_TWO:
          retail_lk_rock_repeat_two(fighter);
          return true;
        case RETAIL_NATIVE_LK_THROW_ROCK_RELEASE:
          retail_lk_rock_release(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_LK_OWNER_READY:
          mk4_mmove_lk_owner_ready(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_LK_BLEND_RETURN:
          mk4_mmove_lk_blend_return(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE:
          liu_kang_liu_script_lk_continue(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE:
          liu_kang_liu_script_lk_damage(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LK_THROW_RETREAT:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LIU_KANG_LK_THROW_RETREAT, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE:
          sub_zero_script_lk_rebase(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE:
          sub_zero_script_lk_damage(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN:
          sub_zero_script_lk_owner_return(fighter);
          return true;
        case RETAIL_NATIVE_GORO_LP_THROW_FIVE_PHASE_SEQUENCE:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GORO_LP_THROW_REPEATED_DAMAGE, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT:
          goro_script_lp_phase_repeat(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_THROW_THROW_CONTINUATION:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_LP_THROW_FIRST_SLAM, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_FIRST:
          jax_lp_slam_first(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_SECOND:
          jax_lp_slam_second(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_SLAM_THIRD:
          jax_lp_slam_third(fighter);
          return true;
        case RETAIL_NATIVE_JAX_LP_THROW_RETURN:
          jax_lp_throw_return(fighter);
          return true;
        case RETAIL_NATIVE_LP_SELECT_SHORT:
          retail_lp_select_short(fighter);
          return true;
        case RETAIL_NATIVE_THROW_READY_42CBC0:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_LP_SELECT_LONG:
          retail_lp_select_long(fighter);
          return true;
        case RETAIL_NATIVE_LP_END:
          retail_lp_end(fighter);
          return true;
        case RETAIL_NATIVE_THROW_READY_42CE60:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_LP_THROW_OWNER_RETURN:
          retail_throw_ready(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_PUNCH_TAIL:
          goro_script_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_KICK_TAIL:
          goro_script_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_LEAP_CONTINUE:
          goro_script_leap_continue(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_STOMP_RETURN:
          goro_script_stomp_return(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_QUAKE_LAND:
          goro_script_quake_land(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP:
          liu_kang_liu_script_fatal_setup(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_VANISH:
          shinnok_second_vanish(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_CLAP:
          shinnok_second_clap(fighter);
          return true;
        case RETAIL_NATIVE_FAN_TAIL:
          retail_fan_tail(fighter);
          return true;
        case RETAIL_NATIVE_FAN_CYCLE_43D510:
          retail_fan_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FAN_CYCLE_43D530:
          retail_fan_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FAN_VICTORY:
          retail_fan_victory(fighter);
          return true;
        case RETAIL_NATIVE_FAN_VICTIM_END:
          retail_fan_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_FIRST_RELEASE:
          sonya_script_first_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE:
          raiden_script_first_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_FIRST_END:
          raiden_script_first_end(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_BEGIN:
          scorpion_script_burn_begin(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START:
          scorpion_script_burn_victim_start(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1:
          scorpion_script_burn_repeat1(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2:
          scorpion_script_burn_repeat2(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTORY:
          scorpion_script_burn_victory(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE:
          scorpion_script_burn_victim_cue(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT:
          scorpion_script_burn_victim_repeat(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND:
          scorpion_script_burn_victim_blend(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT:
          scorpion_script_burn_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE:
          scorpion_script_burn_victim_settle(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END:
          scorpion_script_burn_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN:
          sub_zero_script_first_begin(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL:
          sub_zero_script_first_pull(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1:
          sub_zero_script_first_cues1(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2:
          sub_zero_script_first_cues2(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3:
          sub_zero_script_first_cues3(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4:
          sub_zero_script_first_cues4(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD:
          sub_zero_script_first_head(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS:
          sub_zero_script_first_victim_blends(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE:
          sub_zero_script_first_victim_collapse(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END:
          sub_zero_script_first_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_HEART:
          jarek_first_heart(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_RIP:
          jarek_first_victim_rip(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_IMPACT:
          jarek_first_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_FIRST_VICTIM_END:
          jarek_first_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL:
          cage_script_fatality_1_signal(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_59:
          cage_first_victim_59(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_131:
          cage_first_victim_131(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_148:
          cage_first_victim_148(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_166:
          cage_first_victim_166(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_185:
          cage_first_victim_185(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_VICTIM_206:
          cage_first_victim_206(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_SEVER:
          cage_first_sever(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_HALF:
          cage_first_torso_half(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_123:
          cage_first_torso_123(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_FIRST_TORSO_FINISH:
          cage_first_torso_finish(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_VICTORY:
          jax_fatality_1_victory(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_GRAB:
          jax_fatality_1_grab(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_FIRST_ARM:
          jax_fatality_1_first_arm(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_1_SECOND_ARM:
          jax_fatality_1_second_arm(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_GRAB:
          kai_first_owner_grab(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_SPIN:
          kai_first_owner_spin(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_OWNER_TEAR:
          kai_first_owner_tear(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_71:
          kai_first_victim_71(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_TURN:
          kai_first_victim_turn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_VICTIM_SEVER:
          kai_first_victim_sever(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_TORSO_203:
          kai_first_torso_203(fighter);
          return true;
        case RETAIL_NATIVE_KAI_FIRST_TORSO_IMPACT:
          kai_first_torso_impact(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_CUE_40:
          quan_chi_fatality_cue_40(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_LEG:
          quan_chi_fatality_leg(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_HIT_179:
          quan_chi_fatality_hit_179(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_HIT_241:
          quan_chi_fatality_hit_241(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_1_OWNER_BEAT:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_QUAN_CHI_FATALITY_1_OWNER_BEAT, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_FATALITY_BEAT:
          quan_chi_fatality_beat(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_53:
          tanya_first_owner_53(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_DRIFT:
          tanya_first_owner_drift(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_REPLAYS:
          tanya_first_owner_replays(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_OWNER_LOOP:
          tanya_first_owner_loop(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_SWELL:
          tanya_first_victim_swell(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_REPEAT:
          tanya_first_victim_repeat(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FIRST_VICTIM_BURST:
          tanya_first_victim_burst(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_LOOP_START:
          fujin_first_loop_start(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_CYCLE:
          fujin_first_cycle(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_FIRST_SHOOT:
          fujin_first_shoot(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTORY:
          reiko_first_victory(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TEAR:
          reiko_first_tear(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTIM_118:
          reiko_first_victim_118(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_VICTIM_165:
          reiko_first_victim_165(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_452C50:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TORSO_29:
          reiko_first_torso_29(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_TORSO_END:
          reiko_first_torso_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_HEAD_59:
          reiko_first_head_59(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_HEAD_83:
          reiko_first_head_83(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_452F90:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_LEFT_ARM_56:
          reiko_first_left_arm_56(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_LEFT_ARM_LAND:
          reiko_first_left_arm_land(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_453040:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_RIGHT_ARM_44:
          reiko_first_right_arm_44(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_RIGHT_ARM_LAND:
          reiko_first_right_arm_land(fighter);
          return true;
        case RETAIL_NATIVE_REIKO_FIRST_PIECE_END_4530F0:
          reiko_first_piece_end(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_CYCLE:
          reptile_first_owner_cycle(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_BITE:
          reptile_first_owner_bite(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_OWNER_LOOP:
          reptile_first_owner_loop(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_BITE:
          reptile_first_victim_bite(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_SHAKE:
          reptile_first_victim_shake(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE:
          reptile_first_victim_collapse(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VANISHED:
          shinnok_first_vanished(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_HIDDEN:
          shinnok_first_hidden(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FATALITY_REAPPEAR:
          shinnok_fatality_reappear(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FATALITY_VICTORY:
          shinnok_fatality_victory(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_VANISHED:
          shinnok_second_vanished(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_SECOND_HANDS:
          shinnok_second_hands(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE:
          shinnok_first_hand_squeeze(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_SQUEEZE_CYCLE_4558D0:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_HELD:
          shinnok_first_victim_held(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE:
          shinnok_first_victim_squeeze(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_SQUEEZE_CYCLE_455B80:
          shinnok_first_squeeze_cycle(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH:
          shinnok_first_victim_crush(fighter);
          return true;
        case RETAIL_NATIVE_CROUCH_RETURN:
          retail_crouch_return(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND:
          raiden_script_second_blend(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT:
          raiden_script_second_effect(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_FATALITY_2_HELD_POSE:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_RAIDEN_FATALITY_2_HELD_POSE, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE:
          raiden_script_second_release(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_END:
          raiden_script_second_end(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END:
          raiden_script_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_HEIGHT:
          tanya_second_height(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_FINISH:
          tanya_second_finish(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_TANYA_FATALITY_2_VICTIM_COLLAPSE, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE:
          cage_script_fatality_2_continue(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS:
          cage_second_repeated_uppercuts(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_VICTIM_FINISH:
          cage_second_victim_finish(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_OPEN:
          sonya_script_second_open(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_REPEAT:
          sonya_script_second_repeat(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_BRANCH:
          sonya_script_second_branch(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_PAUSE:
          sonya_script_second_pause(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END:
          sonya_script_second_cycle_end(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END:
          sonya_script_second_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_APPROACH:
          jax_fatality_2_approach(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_FINISH:
          jax_fatality_2_finish(fighter);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_VICTIM_PAUSE:
          retail_task_pause(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_OWNER:
          retail_stage_owner(fighter);
          return true;
        case RETAIL_NATIVE_FIRE_CONTACT:
          scorpion_fire_script(fighter);
          return true;
        case RETAIL_NATIVE_FIRE_RECOVERY:
          retail_fire_recovery_finish(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_INVISIBILITY:
          reptile_script_invisibility(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SECOND_VICTIM_FALL:
          kai_second_victim_fall(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_WIND_RECOVERY:
          fujin_wind_recovery(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_DEFAULT:
          retail_kick_contact_default(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A6E0:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_WIDE:
          retail_kick_contact_wide(fighter);
          return true;
        case RETAIL_NATIVE_KICK_CONTACT_NARROW:
          retail_kick_contact_narrow(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A850:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_VM_REVERSE_RETURN_46A8D0:
          retail_vm_reverse_return(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_FALL:
          retail_air_lift_fall(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_RELEASE:
          retail_air_lift_release(fighter);
          return true;
        case RETAIL_NATIVE_AIR_THROW_RELEASE:
          retail_air_throw_release(fighter);
          return true;
        case RETAIL_NATIVE_AIR_THROW_LAND:
          retail_air_throw_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_SLAM_FALL:
          retail_air_slam_fall(fighter);
          return true;
        case RETAIL_NATIVE_NOOB_SAIBOT_AIR_THROW_LANDING_LINK:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_QUAN_CHI_AIR_THROW_LANDING, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_AIR_SLAM_END:
          retail_air_slam_end(fighter);
          return true;
        case RETAIL_NATIVE_AIR_FLIP_RELEASE:
          retail_air_flip_release(fighter);
          return true;
        case RETAIL_NATIVE_CROUCH_LOW_KICK_TAIL:
          retail_crouch_low_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CLOSE_HIGH_KICK_TAIL:
          retail_close_high_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CLOSE_HIGH_PUNCH_TAIL:
          retail_close_high_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_COMBO_HIGH_PUNCH_TAIL:
          retail_combo_high_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_STRAIGHT_TAIL:
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
        case RETAIL_NATIVE_AIR_STRAIGHT_LAND:
          retail_air_straight_land(fighter);
          return true;
        case RETAIL_NATIVE_AIR_STRAIGHT_WIDE_TAIL:
          retail_air_straight_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_UPPERCUT_DEFAULT:
          native_uppercut_default(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_DRAW_EQUIP:
          retail_weapon_draw_equip(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_JAB_SECOND_PUNCH:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_JAB_SECOND_PUNCH, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_CAGE_JAB_REPEAT:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_JAB_REPEAT, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_CAGE_JAB_RECOVERY:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_JAB_RECOVERY, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_AIR_LK_TAIL:
          retail_air_lk_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LK_ALT_TAIL:
          retail_air_lk_alt_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LK_WIDE_TAIL:
          retail_air_lk_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_TAIL:
          retail_air_diagonal_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_ALT_TAIL:
          retail_air_diagonal_alt_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_DIAGONAL_WIDE_TAIL:
          retail_air_diagonal_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_AIR_HP_TAIL:
          retail_air_hp_tail(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_AIR_FRAMES_NATIVE_46EAB0:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case RETAIL_NATIVE_WEAPON_AIR_FRAMES_NATIVE_46EC10:
          weapon_air_frames_native(fighter, fighter->retail_vm.selector);
          return true;
        case RETAIL_NATIVE_AIR_LP_TAIL:
          retail_air_lp_tail(fighter);
          return true;
        case RETAIL_NATIVE_SWEEP_ADVANCING_TAIL:
          retail_sweep_advancing_tail(fighter);
          return true;
        case RETAIL_NATIVE_SWEEP_STANDARD_TAIL:
          retail_sweep_standard_tail(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_START:
          retail_roundhouse_start(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_TAIL:
          retail_roundhouse_tail(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_WIDE_START:
          retail_roundhouse_wide_start(fighter);
          return true;
        case RETAIL_NATIVE_ROUNDHOUSE_WIDE_TAIL:
          retail_roundhouse_wide_tail(fighter);
          return true;
        case RETAIL_NATIVE_CONTROLLER_RETURN_470420:
          retail_controller_return(fighter);
          return true;
        case RETAIL_NATIVE_VICTORY_CONTINUE:
          retail_victory_continue(fighter);
          return true;
        case RETAIL_NATIVE_KAI_LUNGE_KICK_CONTACT:
          kai_lunge_kick_contact(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL:
          cage_script_cpu_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL:
          cage_script_powered_punch_tail(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_FLIP_KICK_TAIL:
          quan_chi_script_flip_kick_tail(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_DASH_CONTACT:
          reptile_script_dash_contact(fighter);
          return true;
        case RETAIL_NATIVE_JAX_DASH_PUNCH_CONTACT:
          jax_dash_punch_contact(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SCRIPT_ROLL_TAIL:
          reptile_script_roll_tail(fighter);
          return true;
        case RETAIL_NATIVE_JAREK_QUAKE_TAIL:
          jarek_quake_tail(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL:
          sonya_script_cartwheel_tail(fighter);
          return true;
        case RETAIL_NATIVE_TANYA_DRILL_KICK_CONTACT:
          tanya_drill_kick_contact(fighter);
          return true;
        case RETAIL_NATIVE_KAI_AERIAL_PUNCH_CONTACT:
          kai_aerial_punch_contact(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_DIVE_CONTACT:
          fujin_dive_contact(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_DIVE_LAND:
          fujin_dive_land(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_VANISH:
          retail_teleport_vanish(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_SLAM_FALL:
          retail_teleport_slam_fall(fighter);
          return true;
        case RETAIL_NATIVE_TELEPORT_SLAM_DROP:
          retail_teleport_slam_drop(fighter);
          return true;
        case RETAIL_NATIVE_KAI_SCRIPT_STAGE_EVENT:
          kai_script_stage_event(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_SPIN_BEGIN:
          kai_handstand_spin_begin(fighter);
          return true;
        case RETAIL_NATIVE_HANDSTAND_FRONT:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_HANDSTAND_FRONT, fighter->retail_vm.finish);
          return true;
        case RETAIL_NATIVE_KAI_SCRIPT_CONTACT_B10_CONTINUATION:
          kai_script_contact_b10_continuation(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_REAR_CONTACT:
          kai_handstand_rear_contact(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS:
          raiden_script_torpedo_miss(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT:
          raiden_script_torpedo_contact(fighter);
          return true;
        case RETAIL_NATIVE_RECOIL_LAND:
          retail_recoil_land(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SLIDE_SCRIPT:
          sub_zero_slide_script(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN:
          sub_zero_script_slide_return(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL:
          sonya_script_leg_grab_tail(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL:
          sonya_script_square_wave_tail(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_BICYCLE_SCRIPT:
          liu_kang_bicycle_script(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LAND_47ED40:
          liu_kang_liu_script_land(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT:
          liu_kang_flying_kick_script(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LAND_47EF10:
          liu_kang_liu_script_land(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_TRIPLE_FAST:
          fujin_triple_fast(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_TRIPLE_CONTACT:
          fujin_triple_contact(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_TELEPORT_SCRIPT:
          scorpion_teleport_script(fighter);
          return true;
        case RETAIL_NATIVE_LAND:
          mk4_land(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_GRAB_CONTROL_START:
          fujin_grab_control_start(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SPIN_SCRIPT:
          scorpion_spin_script(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_SECOND_MELT:
          reptile_second_melt(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_SECOND_BURST:
          fujin_second_burst(fighter);
          return true;
        case RETAIL_NATIVE_SCENE_BODY_FINISH:
          retail_scene_body_finish(fighter);
          return true;
        case RETAIL_NATIVE_REPTILE_WEAPON_STAGGER_TAIL:
          reptile_weapon_stagger_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE:
          goro_script_uppercut_victim_slide(fighter);
          return true;
        case RETAIL_NATIVE_GORO_VICTIM_GETUP:
          goro_victim_getup(fighter);
          return true;
        case RETAIL_NATIVE_REACTION_CLIP_129_STARTUP:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_CLIP_129_STARTUP, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_REACTION_CLIP_129_WAIT_LANDING:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_CLIP_129_WAIT_LANDING, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_REACTION_CLIP_129_GETUP:
          retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_CLIP_129_GETUP, mk4_script_thrown_ready);
          return true;
        case RETAIL_NATIVE_REACTION_GETUP_BACK:
          retail_reaction_getup_back(fighter);
          return true;
        case RETAIL_NATIVE_REACTION_FALL:
          retail_reaction_fall(fighter);
          return true;
        case RETAIL_NATIVE_REACTION_FINISH:
          retail_reaction_finish(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP:
          shinnok_weapon_sweep_getup(fighter);
          return true;
        case RETAIL_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH:
          shinnok_weapon_sweep_finish(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_IMPALE:
          retail_stage_impale(fighter);
          return true;
        case RETAIL_NATIVE_STAGE_COMPLETE:
          retail_stage_complete(fighter);
          return true;
        case RETAIL_NATIVE_STAGGER_TAIL:
          retail_stagger_tail(fighter);
          return true;
        case RETAIL_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD:
          goro_script_stomp_victim_hold(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_STOMP_VICTIM:
          quan_chi_script_stomp_victim(fighter);
          return true;
        case RETAIL_NATIVE_AIR_FLIP_GETUP:
          retail_air_flip_getup(fighter);
          return true;
        case RETAIL_NATIVE_AIR_LIFT_GETUP:
          retail_air_lift_getup(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_AIR_GETUP:
          mk4_mmove_air_getup(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL:
          scorpion_script_teleport_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_THROW_VICTIM_CONTINUE:
          retail_throw_victim_continue(fighter);
          return true;
        case RETAIL_NATIVE_THROW_VICTIM_IMPACT:
          retail_throw_victim_impact(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_THROWN_FINISH_485510:
          mk4_mmove_thrown_finish(fighter);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END:
          sonya_script_leg_victim_end(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL:
          liu_kang_liu_script_lk_thrown_tail(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FLIP_485AD0:
          retail_getup_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_ROLL_FLIP:
          retail_getup_roll_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_ROLL_485B50:
          /* 490350 twice, through488bd0, leaves the original facing. */
          retail_getup_roll(fighter);
          return true;
        case RETAIL_NATIVE_THAW_LANDING:
          retail_thaw_land(fighter);
          return true;
        case RETAIL_NATIVE_SHAKE_STUN:
          retail_shake_stun(fighter);
          return true;
        case RETAIL_NATIVE_SUB_ZERO_SCRIPT_LK_VICTIM_RISE:
          sub_zero_script_lk_victim_rise(fighter);
          return true;
        case RETAIL_NATIVE_VM_RETURN:
          retail_vm_return(fighter);
          return true;
        case RETAIL_NATIVE_PAIR_RELEASE:
          retail_pair_release(fighter);
          return true;
        case RETAIL_NATIVE_BICYCLE_CYCLE:
          retail_bicycle_cycle(fighter);
          return true;
        case RETAIL_NATIVE_UPPERCUT_RECOVER:
          retail_uppercut_recover(fighter);
          return true;
        case RETAIL_NATIVE_BLOCK_CYCLE:
          retail_block_cycle(fighter);
          return true;
        case RETAIL_NATIVE_CONTROLLER_RETURN_460250:
          retail_controller_return(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SPEAR_STUN_SCRIPT:
          scorpion_spear_stun_script(fighter);
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL:
          raiden_script_staff_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_KAI_HANDSTAND_VICTIM_TAIL:
          kai_handstand_victim_tail(fighter);
          return true;
        case RETAIL_NATIVE_MMOVE_SIDE_GETUP:
          mk4_mmove_side_getup(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FLIP_485C40:
        case RETAIL_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP:
          retail_getup_flip(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_BACK:
          retail_getup_back(fighter);
          return true;
        case RETAIL_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP:
          retail_getup_roll(fighter);
          return true;
        case RETAIL_NATIVE_GETUP_FRONT:
          retail_getup_front(fighter);
          return true;
        case RETAIL_NATIVE_CAGE_WEAPON_UPPER_CONTACT:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_495AE0:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_REPTILE_WEAPON_FLURRY:
          reptile_weapon_flurry(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND:
          liu_kang_liu_script_sword_blend(fighter);
          return true;
        case RETAIL_NATIVE_WEAPON_CONTACT_496770:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_CAGE_WEAPON_BACK_LP_CONTACT:
          retail_weapon_contact(fighter, address);
          return true;
        case RETAIL_NATIVE_TANYA_BOOMERANG_RELEASE:
          tanya_boomerang_release(fighter);
          return true;
        case RETAIL_NATIVE_FIREBALL_FALL:
          retail_fireball_fall(fighter);
          return true;
        case RETAIL_NATIVE_KAI_GROUND_SKULL_SPAWN:
          kai_ground_skull_spawn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_RISING_SKULL_SPAWN:
          kai_rising_skull_spawn(fighter);
          return true;
        case RETAIL_NATIVE_KAI_RISING_SKULL_FINISH:
          kai_rising_skull_finish(fighter);
          return true;
        case RETAIL_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE:
          quan_chi_script_skull_release(fighter);
          return true;
        case RETAIL_NATIVE_FUJIN_BOW_FINISH:
          fujin_bow_finish(fighter);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN:
          scorpion_script_spear_return(fighter);
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN:
          liu_kang_liu_script_air_return(fighter);
          return true;
        case RETAIL_NATIVE_TASK_RETIRE:
          retail_task_retire(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_SPAWN:
      switch (address) {
        case RETAIL_NATIVE_JAX_FATALITY_2_SPARKS:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_sparks(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_JAX_FATALITY_2_FLASH:
          if (argument != 619u) {
            return false;
          }
          jax_fatality_2_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_CHARGE:
          if (argument != 619u) {
            return false;
          }
          cage_second_charge(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_CAGE_SECOND_FLASH:
          if (argument != 619u) {
            return false;
          }
          cage_second_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_BREATH_SHATTER_DECORATION:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_freeze(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_BREATH_SCENE_DECORATION:
          if (argument != 0u) {
            return false;
          }
          sub_zero_script_breath_object(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_REIKO_SECOND_STARS:
          if (argument != 0u) {
            return false;
          }
          reiko_second_stars(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_JAREK_SECOND_HEAD_FALL:
          if (argument != 0u) {
            return false;
          }
          jarek_second_head_fall(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_REPTILE_SECOND_ACID:
          if (argument != 0u) {
            return false;
          }
          reptile_second_acid(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_BEAST_GORE:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_beast_gore(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_FUJIN_SECOND_BLOOD:
          if (argument != 38u) {
            return false;
          }
          fujin_second_blood(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_KAI_SECOND_SKULL:
          if (argument != 38u) {
            return false;
          }
          kai_second_skull(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_FINISHER_PROJECTILE:
          if (argument != 38u) {
            return false;
          }
          liu_kang_liu_script_fatal_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SCORPION_SCRIPT_TRANSFORM_EFFECT:
          if (argument != 0u) {
            return false;
          }
          scorpion_script_transform_effect(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SCENE_KEEPALIVE_TASK:
          if (argument != 619u) {
            return false;
          }
          mk4_script_scene_task(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_OWNER_CUE:
          if (argument != 619u) {
            return false;
          }
          tanya_second_owner_cue(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_TANYA_SECOND_VICTIM_CUE:
          if (argument != 619u) {
            return false;
          }
          tanya_second_victim_cue(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SCENE_HOVER:
          if (argument != 38u) {
            return false;
          }
          retail_scene_hover(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_REIKO_STAR_SPAWN:
          if (argument != 82u) {
            return false;
          }
          reiko_star_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_JAX_MISSILE_SPAWN:
          if (argument != 80u) {
            return false;
          }
          jax_missile_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_CAGE_ORB_SPAWN:
          if (argument != 79u) {
            return false;
          }
          cage_orb_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_JAREK_BLADE_SPAWN:
          if (argument != 78u) {
            return false;
          }
          jarek_blade_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_REPTILE_ACID_SPAWN:
          if (argument != 76u) {
            return false;
          }
          reptile_acid_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_FIREBALL_SPAWN:
          if (argument != 75u) {
            return false;
          }
          retail_fireball_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_AIR_FIREBALL_SPAWN:
          if (argument != 75u) {
            return false;
          }
          retail_air_fireball_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SPEAR_TASK:
          if (argument != 67u) {
            return false;
          }
          scorpion_script_spawn_spear(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SONYA_SCRIPT_RING_FLASH:
          if (argument != 38u) {
            return false;
          }
          sonya_script_ring_flash(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_SONYA_RING_OBJECT:
          if (argument != 68u) {
            return false;
          }
          sonya_script_ring_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_ICE_PROJECTILE_TASK:
          if (argument != 66u) {
            return false;
          }
          sub_zero_script_spawn_ice(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_RAIDEN_LIGHTNING_OBJECT:
          if (argument != 65u) {
            return false;
          }
          raiden_script_lightning_spawn(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_AIR_FIREBALL_OBJECT:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_air(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_LOW_FIREBALL_OBJECT:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_low(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_LIU_HIGH_FIREBALL_OBJECT:
          if (argument != 64u) {
            return false;
          }
          liu_kang_liu_script_spawn_high(fighter, (int)argument);
          return true;
        case RETAIL_NATIVE_GORO_FIREBALL_SPAWN:
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
        case RETAIL_NATIVE_CONTEXT:
          mk4_script_context(fighter, (int)(argument & 255u));
          return true;
        case RETAIL_NATIVE_SCRIPT_REACTION_DISPATCH:
          retail_script_reaction_dispatch(fighter, (int)(argument & 255u));
          return true;
        case RETAIL_NATIVE_DISPATCH_OPPONENT_REACTION:
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
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE:
          fighter->retail_vm.result = liu_kang_liu_script_sword_gate(fighter);
          return true;
        case RETAIL_NATIVE_JAB_REPEAT_PREDICATE:
          fighter->retail_vm.result = mk4_script_jab_gate(fighter);
          return true;
        case RETAIL_NATIVE_CONTACT_QUERY_D:
          fighter->retail_vm.result = raiden_script_torpedo_query(fighter);
          return true;
        default:
          return false;
      }
    case RETAIL_PHYSICS:
      switch (address) {
        case RETAIL_NATIVE_LANDING_MOTION:
          fighter->body.physics = mk4_script_landing_motion;
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_MOTION:
          fighter->body.physics = raiden_script_torpedo_motion;
          return true;
        case RETAIL_NATIVE_TELEPORT_GROUP_CALLBACK:
          fighter->body.physics = scorpion_script_teleport_motion;
          return true;
        case RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION:
          fighter->body.physics = liu_kang_liu_script_bicycle_motion;
          return true;
        case RETAIL_NATIVE_GORO_STOMP_HOLD_SEPARATION:
          fighter->body.physics = goro_stomp_hold_separation;
          return true;
        case RETAIL_NATIVE_RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION:
          fighter->body.physics = raiden_script_torpedo_victim_motion;
          return true;
        case RETAIL_NATIVE_FACE_PEER_490BD0:
          fighter->body.physics = retail_face_peer;
          return true;
        case RETAIL_NATIVE_VELOCITY_FRICTION:
          fighter->body.physics = fighter_friction;
          return true;
        default:
          return false;
      }
  }
  return false;
}

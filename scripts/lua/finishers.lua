-- Finishers.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004dfed0 in MK4/MK-4.EXE.
function S.fujin_second_fatality()
  mk4.wait(60)
  mk4.sound_indexed(60)
  mk4.native_release(55)
  mk4.prefixed_finish(547)
  mk4.bind_cursor(0, 548)
  mk4.wait(8)
  mk4.scene_active()
  mk4.spawn(0x26, native.FUJIN_SECOND_BLOOD)
  mk4.native_return(181)
  mk4.park()
  return S.reptile_fatality_2()
end

-- Retail script 0x004dff08 in MK4/MK-4.EXE.
function S.reptile_fatality_2()
  mk4.native_release(53)
  mk4.spawn(0x26, native.SCENE_HOVER)
  mk4.bind_cursor(0, 493)
  mk4.wait(150)
  mk4.scene_active()
  mk4.native_return(180)
  mk4.spawn(0x0, native.REPTILE_SECOND_ACID)
  mk4.sound_indexed(61)
  mk4.prefixed_finish(544)
  mk4.prefixed_finish(545)
  mk4.prefixed_finish(545)
  mk4.prefixed_finish(545)
  mk4.prefixed_finish(545)
  mk4.prefixed_finish(545)
  mk4.prefixed_finish(545)
  mk4.clip_operation(544)
  mk4.bind_cursor(0, 493)
  mk4.park()
  return S.kai_fatality_2()
end

-- Retail script 0x004dff88 in MK4/MK-4.EXE.
function S.kai_fatality_2()
  mk4.bind_cursor(0, 493)
  mk4.scene_event(45)
  mk4.wait(40)
  mk4.spawn(0x26, native.SCENE_HOVER)
  mk4.native_release(46)
  mk4.wait(60)
  mk4.sound_indexed(58)
  mk4.scene_active()
  mk4.wait(30)
  mk4.call(native.KAI_SECOND_CHILD_WALK)
  mk4.prefixed_finish(486)
  mk4.sound_c(51)
  mk4.spawn(0x26, native.KAI_SECOND_SKULL)
  mk4.bind_cursor(0, 487)
  mk4.park()
end

-- Retail script 0x004dfff8 in MK4/MK-4.EXE.
function S.liu_kang_start_fatality()
  mk4.scene_event(42)
  mk4.wait(15)
  mk4.scene_event(43)
  mk4.wait(20)
  mk4.scene_event(44)
  mk4.wait(60)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_FATAL_SETUP)
end

-- Retail script 0x004e0020 in MK4/MK-4.EXE.
function S.liu_kang_fireball_fatality_paired_opening()
  mk4.call(native.LIU_KANG_LIU_SCRIPT_FATAL_ALIGN)
  mk4.sound_b(10)
  mk4.open_peer_clip(539)
  mk4.open_clip(0, 537)
  mk4.play_pair(37)
  mk4.scene_active()
  mk4.native_return(176)
  mk4.forward()
  mk4.wait(18)
  mk4.sound_a(33)
  mk4.sound_indexed(57)
  mk4.prefixed_finish(538)
  mk4.spawn(0x26, native.LIU_FINISHER_PROJECTILE)
  mk4.playback_pair(0)
  mk4.park()
end

-- Retail script 0x004e0088 in MK4/MK-4.EXE.
function S.shinnok_fatality_2()
  mk4.scene_event(48)
  mk4.wait(15)
  mk4.scene_event(49)
  mk4.wait(20)
  mk4.scene_event(50)
  mk4.wait(20)
  mk4.native_release(51)
  mk4.wait(20)
  mk4.native_return(179)
  mk4.tail(native.SHINNOK_SECOND_VANISH)
end

-- Retail script 0x004e00b8 in MK4/MK-4.EXE.
function S.shinnok_fatality_2_first_hand()
  mk4.prefixed_to(120, 541)
  mk4.wait(120)
  mk4.call(native.SHINNOK_SECOND_GORE_19_PIECES)
  mk4.call(native.SHINNOK_SECOND_GORE_7_PIECES)
  mk4.forward()
  mk4.park()
end

-- Retail script 0x004e00e0 in MK4/MK-4.EXE.
function S.shinnok_fatality_2_second_hand()
  mk4.prefixed_to(120, 541)
  mk4.spawn_effect(6)
  mk4.sound_c(105)
  mk4.tail(native.SHINNOK_SECOND_CLAP)
  return S.shinnok_second_clap()
end

-- Retail script 0x004e00f8 in MK4/MK-4.EXE.
function S.shinnok_second_clap()
  mk4.wait(120)
  mk4.call(native.SHINNOK_SECOND_GORE_19_PIECES)
  mk4.call(native.SHINNOK_SECOND_GORE_8_PIECES)
  mk4.forward()
  mk4.park()
end

-- Retail script 0x004e0118 in MK4/MK-4.EXE.
function S.jax_fan_fatality_performer()
  mk4.prefixed_to(69, 415)
  mk4.tail(native.FAN_TAIL)
  return S.jarek_fan_fatality_male_victim_owner_cycle()
end

-- Retail script 0x004e0128 in MK4/MK-4.EXE.
function S.jarek_fan_fatality_male_victim_owner_cycle()
  mk4.forward_to(8)
  mk4.cursor_set(0)
  mk4.tail(native.FAN_MALE_VICTIM_OWNER_CYCLE)
  return S.jax_fan_fatality_owner_cycle()
end

-- Retail script 0x004e0138 in MK4/MK-4.EXE.
function S.jax_fan_fatality_owner_cycle()
  mk4.forward_to(77)
  mk4.cursor_set(69)
  mk4.tail(native.FAN_FEMALE_VICTIM_OWNER_CYCLE)
  return S.jax_fan_fatality_owner_release()
end

-- Retail script 0x004e0148 in MK4/MK-4.EXE.
function S.jax_fan_fatality_owner_release()
  mk4.cursor_set(78)
  mk4.forward_to(107)
  mk4.tail(native.FAN_VICTORY)
  return S.fan_victory()
end

-- Retail script 0x004e0158 in MK4/MK-4.EXE.
function S.fan_victory()
  mk4.forward()
  mk4.prefixed_finish(412)
  return S.fan_victory_cursor_set_1()
end

-- Retail script 0x004e0164 in MK4/MK-4.EXE.
function S.fan_victory_cursor_set_1()
  mk4.cursor_set(18)
  mk4.forward()
  return S.fan_victory_cursor_set_1()
end

-- Retail script 0x004e0188 in MK4/MK-4.EXE.
function S.jax_fan_fatality_victim()
  mk4.prefixed_to(69, 416)
  return S.jax_fan_fatality_victim_forward_to_1()
end

-- Retail script 0x004e0190 in MK4/MK-4.EXE.
function S.jax_fan_fatality_victim_forward_to_1()
  mk4.forward_to(77)
  mk4.cursor_set(69)
  return S.jax_fan_fatality_victim_forward_to_1()
end

-- Retail script 0x004e01a0 in MK4/MK-4.EXE.
function S.jax_fan_fatality_victim_release()
  mk4.cursor_set(78)
  mk4.forward_to(107)
  mk4.tail(native.FAN_VICTIM_END)
end

-- Retail script 0x004e01c0 in MK4/MK-4.EXE.
function S.sonya_first_fatality()
  mk4.prefixed_to(42, 388)
  mk4.tail(native.SONYA_SCRIPT_FIRST_RELEASE)
  return S.sonya_fatality_1_owner_loop()
end

-- Retail script 0x004e01d0 in MK4/MK-4.EXE.
function S.sonya_fatality_1_owner_loop()
  mk4.forward()
  mk4.cursor_set(84)
  return S.sonya_fatality_1_owner_loop()
end

-- Retail script 0x004e0210 in MK4/MK-4.EXE.
function S.raiden_first_fatality()
  mk4.prefixed_finish(392)
  mk4.tail(native.RAIDEN_SCRIPT_FIRST_RELEASE)
  return S.raiden_fatality_1_owner_release()
end

-- Retail script 0x004e0220 in MK4/MK-4.EXE.
function S.raiden_fatality_1_owner_release()
  mk4.playback_pair(20)
  mk4.forward()
  mk4.tail(native.RAIDEN_SCRIPT_FIRST_END)
end

-- Retail script 0x004e0278 in MK4/MK-4.EXE.
function S.burn_start()
  mk4.prefixed_to(43, 390)
  mk4.tail(native.SCORPION_SCRIPT_BURN_BEGIN)
  return S.scorpion_burn_begin()
end

-- Retail script 0x004e0288 in MK4/MK-4.EXE.
function S.scorpion_burn_begin()
  mk4.forward_to(105)
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_START)
end

-- Retail script 0x004e0298 in MK4/MK-4.EXE.
function S.scorpion_fatality_1_owner_flame()
  mk4.forward_to(123)
  mk4.tail(native.SCORPION_SCRIPT_BURN_REPEAT1)
end

-- Retail script 0x004e02a8 in MK4/MK-4.EXE.
function S.scorpion_burn_repeat1()
  mk4.cursor_set(105)
  mk4.forward_to(123)
  mk4.tail(native.SCORPION_SCRIPT_BURN_REPEAT2)
  return S.scorpion_burn_repeat2()
end

-- Retail script 0x004e02b8 in MK4/MK-4.EXE.
function S.scorpion_burn_repeat2()
  mk4.cursor_set(105)
  mk4.forward_to(123)
  mk4.forward()
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTORY)
end

-- Retail script 0x004e02d0 in MK4/MK-4.EXE.
function S.scorpion_burn_victory()
  mk4.prefixed_finish(115)
  mk4.prefixed_finish(116)
  mk4.prefixed_finish(117)
  mk4.bind_cursor(0, 118)
  mk4.park()
end

-- Retail script 0x004e0300 in MK4/MK-4.EXE.
function S.scorpion_fatality_1_victim()
  mk4.prefixed_to(12, 391)
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_CUE)
  return S.scorpion_burn_victim_cue()
end

-- Retail script 0x004e0310 in MK4/MK-4.EXE.
function S.scorpion_burn_victim_cue()
  mk4.forward_to(14)
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_REPEAT)
end

-- Retail script 0x004e0320 in MK4/MK-4.EXE.
function S.scorpion_burn_victim_repeat()
  mk4.forward()
  mk4.cursor_set(63)
  mk4.forward()
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_BLEND)
end

-- Retail script 0x004e0338 in MK4/MK-4.EXE.
function S.scorpion_burn_victim_blend()
  mk4.prefixed_to(19, 206)
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_IMPACT)
  return S.scorpion_burn_victim_impact()
end

-- Retail script 0x004e0348 in MK4/MK-4.EXE.
function S.scorpion_burn_victim_impact()
  mk4.forward_to(89)
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_SETTLE)
end

-- Retail script 0x004e0358 in MK4/MK-4.EXE.
function S.scorpion_burn_victim_settle()
  mk4.forward()
  mk4.tail(native.SCORPION_SCRIPT_BURN_VICTIM_END)
end

-- Retail script 0x004e0368 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_setup()
  mk4.wait(50)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_BEGIN)
end

-- Retail script 0x004e0378 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_first_pull()
  mk4.prefixed_to(17, 385)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_PULL)
  return S.sub_zero_fatality_1_pull_forward()
end

-- Retail script 0x004e0388 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_pull_forward()
  mk4.forward_to(42)
  mk4.cursor_set(41)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_CUES1)
  return S.sub_zero_fatality_1_pull_reverse()
end

-- Retail script 0x004e0398 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_pull_reverse()
  mk4.playback_pair(31)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_CUES2)
end

-- Retail script 0x004e03a8 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_second_pull()
  mk4.forward_to(42)
  mk4.cursor_set(41)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_CUES3)
  return S.sub_zero_fatality_1_second_reverse()
end

-- Retail script 0x004e03b8 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_second_reverse()
  mk4.playback_pair(31)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_CUES4)
end

-- Retail script 0x004e03c8 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_head_release()
  mk4.forward_to(44)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_HEAD)
end

-- Retail script 0x004e03d8 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_owner_loop()
  mk4.forward_to(60)
  return S.sub_zero_fatality_1_owner_loop_forward_1()
end

-- Retail script 0x004e03dc in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_owner_loop_forward_1()
  mk4.forward()
  mk4.cursor_set(96)
  return S.sub_zero_fatality_1_owner_loop_forward_1()
end

-- Retail script 0x004e03f0 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_victim()
  mk4.prefixed_to(15, 387)
  mk4.cursor_set(14)
  mk4.playback_pair(4)
  mk4.forward_to(15)
  mk4.cursor_set(14)
  mk4.playback_pair(4)
  mk4.forward_to(17)
  mk4.forward_to(48)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS)
end

-- Retail script 0x004e0420 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_victim_finish()
  mk4.forward_to(63)
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE)
end

-- Retail script 0x004e0430 in MK4/MK-4.EXE.
function S.sub_zero_fatality_1_victim_cleanup()
  mk4.forward()
  mk4.tail(native.SUB_ZERO_SCRIPT_FIRST_VICTIM_END)
end

-- Retail script 0x004e1220 in MK4/MK-4.EXE.
function S.jarek_fatality_1_female_victim_owner()
  mk4.prefixed_to(43, 403)
  return S.jarek_fatality_1_male_victim_owner_tail_1()
end

-- Retail script 0x004e1230 in MK4/MK-4.EXE.
function S.jarek_fatality_1_male_victim_owner()
  mk4.prefixed_to(43, 402)
  return S.jarek_fatality_1_male_victim_owner_tail_1()
end

-- Retail script 0x004e1238 in MK4/MK-4.EXE.
function S.jarek_fatality_1_male_victim_owner_tail_1()
  mk4.tail(native.JAREK_FIRST_HEART)
end

-- Retail script 0x004e1248 in MK4/MK-4.EXE.
function S.jarek_first_heart()
  mk4.forward()
  mk4.bind_cursor(0, 405)
  mk4.park()
end

-- Retail script 0x004e1260 in MK4/MK-4.EXE.
function S.victim_reach()
  mk4.prefixed_to(43, 404)
  mk4.tail(native.JAREK_FIRST_VICTIM_RIP)
  return S.jarek_first_victim_rip()
end

-- Retail script 0x004e1270 in MK4/MK-4.EXE.
function S.jarek_first_victim_rip()
  mk4.forward_to(147)
  mk4.tail(native.JAREK_FIRST_VICTIM_IMPACT)
end

-- Retail script 0x004e1280 in MK4/MK-4.EXE.
function S.jarek_first_victim_impact()
  mk4.forward_to(194)
  mk4.tail(native.JAREK_FIRST_VICTIM_END)
end

-- Retail script 0x004e1290 in MK4/MK-4.EXE.
function S.jarek_first_victim_end()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e12a0 in MK4/MK-4.EXE.
function S.cage_fatality_1_scene_setup()
  mk4.forward()
  mk4.tail(native.CAGE_SCRIPT_FATALITY_1_SIGNAL)
end

-- Retail script 0x004e12b0 in MK4/MK-4.EXE.
function S.cage_fatality_1_scene_loop()
  mk4.prefixed_finish(443)
  return S.cage_fatality_1_scene_loop()
end

-- Retail script 0x004e12c0 in MK4/MK-4.EXE.
function S.cage_torso_rip_male_victim()
  mk4.prefixed_to(58, 410)
  return S.cage_torso_rip_female_victim_tail_1()
end

-- Retail script 0x004e12d0 in MK4/MK-4.EXE.
function S.cage_torso_rip_female_victim()
  mk4.prefixed_to(58, 411)
  return S.cage_torso_rip_female_victim_tail_1()
end

-- Retail script 0x004e12d8 in MK4/MK-4.EXE.
function S.cage_torso_rip_female_victim_tail_1()
  mk4.tail(native.CAGE_FIRST_VICTIM_59)
  return S.cage_first_victim_59()
end

-- Retail script 0x004e12e0 in MK4/MK-4.EXE.
function S.cage_first_victim_59()
  mk4.forward_to(130)
  mk4.tail(native.CAGE_FIRST_VICTIM_131)
end

-- Retail script 0x004e12f0 in MK4/MK-4.EXE.
function S.cage_first_victim_131()
  mk4.forward_to(147)
  mk4.tail(native.CAGE_FIRST_VICTIM_148)
end

-- Retail script 0x004e1300 in MK4/MK-4.EXE.
function S.cage_first_victim_148()
  mk4.forward_to(165)
  mk4.tail(native.CAGE_FIRST_VICTIM_166)
end

-- Retail script 0x004e1310 in MK4/MK-4.EXE.
function S.cage_first_victim_166()
  mk4.forward_to(184)
  mk4.tail(native.CAGE_FIRST_VICTIM_185)
end

-- Retail script 0x004e1320 in MK4/MK-4.EXE.
function S.cage_first_victim_185()
  mk4.forward_to(205)
  mk4.tail(native.CAGE_FIRST_VICTIM_206)
end

-- Retail script 0x004e1330 in MK4/MK-4.EXE.
function S.cage_first_victim_206()
  mk4.forward_to(224)
  mk4.tail(native.CAGE_FIRST_SEVER)
end

-- Retail script 0x004e1340 in MK4/MK-4.EXE.
function S.cage_torso_rip_lower_finish()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e1350 in MK4/MK-4.EXE.
function S.cage_first_sever()
  mk4.prefixed_to(58, 408)
  mk4.tail(native.CAGE_FIRST_TORSO_HALF)
  return S.cage_first_torso_half()
end

-- Retail script 0x004e1360 in MK4/MK-4.EXE.
function S.cage_first_torso_half()
  mk4.forward_to(122)
  mk4.tail(native.CAGE_FIRST_TORSO_123)
end

-- Retail script 0x004e1370 in MK4/MK-4.EXE.
function S.cage_first_torso_123()
  mk4.forward_to(138)
  mk4.tail(native.CAGE_FIRST_TORSO_FINISH)
end

-- Retail script 0x004e1380 in MK4/MK-4.EXE.
function S.cage_first_torso_finish()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e1390 in MK4/MK-4.EXE.
function S.jax_fatality_1_owner_opening()
  mk4.prefixed_finish(384)
  mk4.tail(native.JAX_FATALITY_1_VICTORY)
  return S.jax_fatality_1_owner_loop()
end

-- Retail script 0x004e13a0 in MK4/MK-4.EXE.
function S.jax_fatality_1_owner_loop()
  mk4.prefixed_finish(443)
  return S.jax_fatality_1_owner_loop()
end

-- Retail script 0x004e13b0 in MK4/MK-4.EXE.
function S.arm_tear_victim_clip_204()
  mk4.prefixed_to(49, 204)
  return S.arm_tear_victim_clip_400_tail_1()
end

-- Retail script 0x004e13c0 in MK4/MK-4.EXE.
function S.arm_tear_victim_clip_400()
  mk4.prefixed_to(49, 400)
  return S.arm_tear_victim_clip_400_tail_1()
end

-- Retail script 0x004e13c8 in MK4/MK-4.EXE.
function S.arm_tear_victim_clip_400_tail_1()
  mk4.tail(native.JAX_FATALITY_1_GRAB)
  return S.jax_fatality_1_grab()
end

-- Retail script 0x004e13d0 in MK4/MK-4.EXE.
function S.jax_fatality_1_grab()
  mk4.forward_to(78)
  mk4.tail(native.JAX_FATALITY_1_FIRST_ARM)
end

-- Retail script 0x004e13e0 in MK4/MK-4.EXE.
function S.jax_fatality_1_first_arm()
  mk4.forward_to(128)
  mk4.tail(native.JAX_FATALITY_1_SECOND_ARM)
end

-- Retail script 0x004e13f0 in MK4/MK-4.EXE.
function S.jax_fatality_1_second_arm()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e1430 in MK4/MK-4.EXE.
function S.kai_fatality_1_owner_opening()
  mk4.prefixed_to(45, 440)
  mk4.tail(native.KAI_FIRST_OWNER_GRAB)
  return S.kai_first_owner_grab()
end

-- Retail script 0x004e1440 in MK4/MK-4.EXE.
function S.kai_first_owner_grab()
  mk4.forward_to(123)
  mk4.tail(native.KAI_FIRST_OWNER_SPIN)
end

-- Retail script 0x004e1450 in MK4/MK-4.EXE.
function S.kai_first_owner_spin()
  mk4.prefixed_finish(445)
  mk4.prefixed_finish(445)
  mk4.tail(native.KAI_FIRST_OWNER_TEAR)
  return S.kai_fatality_1_owner_victory()
end

-- Retail script 0x004e1468 in MK4/MK-4.EXE.
function S.kai_fatality_1_owner_victory()
  mk4.forward()
  return S.kai_fatality_1_owner_victory_prefixed_finish_1()
end

-- Retail script 0x004e146c in MK4/MK-4.EXE.
function S.kai_fatality_1_owner_victory_prefixed_finish_1()
  mk4.prefixed_finish(443)
  return S.kai_fatality_1_owner_victory_prefixed_finish_1()
end

-- Retail script 0x004e1480 in MK4/MK-4.EXE.
function S.kai_fatality_1_victim_opening()
  mk4.prefixed_to(70, 441)
  mk4.tail(native.KAI_FIRST_VICTIM_71)
  return S.kai_first_victim_71()
end

-- Retail script 0x004e1490 in MK4/MK-4.EXE.
function S.kai_first_victim_71()
  mk4.forward_to(123)
  mk4.prefixed_finish(444)
  mk4.tail(native.KAI_FIRST_VICTIM_TURN)
end

-- Retail script 0x004e14a8 in MK4/MK-4.EXE.
function S.kai_first_victim_turn()
  mk4.prefixed_finish(444)
  mk4.tail(native.KAI_FIRST_VICTIM_SEVER)
  return S.kai_fatality_1_victim_finish()
end

-- Retail script 0x004e14b8 in MK4/MK-4.EXE.
function S.kai_fatality_1_victim_finish()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e14c8 in MK4/MK-4.EXE.
function S.kai_fatality_1_torso_opening()
  mk4.prefixed_to(202, 442)
  mk4.tail(native.KAI_FIRST_TORSO_203)
  return S.kai_first_torso_203()
end

-- Retail script 0x004e14d8 in MK4/MK-4.EXE.
function S.kai_first_torso_203()
  mk4.forward_to(242)
  mk4.tail(native.KAI_FIRST_TORSO_IMPACT)
end

-- Retail script 0x004e14e8 in MK4/MK-4.EXE.
function S.kai_first_torso_impact()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e14f8 in MK4/MK-4.EXE.
function S.quan_chi_fatality_1_owner_opening()
  mk4.prefixed_to(40, 420)
  mk4.tail(native.QUAN_CHI_FATALITY_CUE_40)
  return S.quan_chi_fatality_cue_40()
end

-- Retail script 0x004e1508 in MK4/MK-4.EXE.
function S.quan_chi_fatality_cue_40()
  mk4.forward_to(117)
  mk4.tail(native.QUAN_CHI_FATALITY_LEG)
end

-- Retail script 0x004e1518 in MK4/MK-4.EXE.
function S.quan_chi_fatality_leg()
  mk4.forward_to(179)
  mk4.tail(native.QUAN_CHI_FATALITY_HIT_179)
end

-- Retail script 0x004e1528 in MK4/MK-4.EXE.
function S.quan_chi_fatality_hit_179()
  mk4.forward_to(241)
  mk4.tail(native.QUAN_CHI_FATALITY_HIT_241)
end

-- Retail script 0x004e1538 in MK4/MK-4.EXE.
function S.quan_chi_fatality_hit_241()
  mk4.forward()
  mk4.tail(native.QUAN_CHI_FATALITY_1_OWNER_BEAT)
end

-- Retail script 0x004e1548 in MK4/MK-4.EXE.
function S.quan_chi_fatality_1_owner_beat()
  mk4.prefixed_to(18, 421)
  mk4.tail(native.QUAN_CHI_FATALITY_BEAT)
  return S.quan_chi_fatality_beat()
end

-- Retail script 0x004e1558 in MK4/MK-4.EXE.
function S.quan_chi_fatality_beat()
  mk4.forward()
  return S.quan_chi_fatality_1_owner_beat()
end

-- Retail script 0x004e1568 in MK4/MK-4.EXE.
function S.victim_stream()
  mk4.prefixed_finish(424)
  return S.victim_stream_prefixed_finish_1()
end

-- Retail script 0x004e1570 in MK4/MK-4.EXE.
function S.victim_stream_prefixed_finish_1()
  mk4.prefixed_finish(417)
  return S.victim_stream_prefixed_finish_1()
end

-- Retail script 0x004e1580 in MK4/MK-4.EXE.
function S.leg_entry()
  mk4.prefixed_finish(422)
  return S.leg_entry_prefixed_finish_1()
end

-- Retail script 0x004e1588 in MK4/MK-4.EXE.
function S.leg_entry_prefixed_finish_1()
  mk4.prefixed_finish(423)
  return S.leg_entry_prefixed_finish_1()
end

-- Retail script 0x004e1598 in MK4/MK-4.EXE.
function S.owner_kiss_start()
  mk4.prefixed_to(52, 395)
  mk4.tail(native.TANYA_FIRST_OWNER_53)
  return S.tanya_first_owner_53()
end

-- Retail script 0x004e15a8 in MK4/MK-4.EXE.
function S.tanya_first_owner_53()
  mk4.forward_to(104)
  mk4.tail(native.TANYA_FIRST_OWNER_DRIFT)
end

-- Retail script 0x004e15b8 in MK4/MK-4.EXE.
function S.tanya_first_owner_drift()
  mk4.forward()
  mk4.prefixed_finish(396)
  mk4.tail(native.TANYA_FIRST_OWNER_REPLAYS)
end

-- Retail script 0x004e15d0 in MK4/MK-4.EXE.
function S.tanya_first_owner_loop()
  mk4.prefixed_finish()
  mk4.tail(native.TANYA_FIRST_OWNER_LOOP)
end

-- Retail script 0x004e15e0 in MK4/MK-4.EXE.
function S.victim_scene()
  mk4.prefixed_finish(397)
  mk4.tail(native.TANYA_FIRST_VICTIM_SWELL)
  return S.tanya_fatality_1_victim_pulse()
end

-- Retail script 0x004e15f0 in MK4/MK-4.EXE.
function S.tanya_fatality_1_victim_pulse()
  mk4.playback_pair(188)
  mk4.forward()
  mk4.tail(native.TANYA_FIRST_VICTIM_REPEAT)
  return S.tanya_fatality_1_victim_shake()
end

-- Retail script 0x004e1600 in MK4/MK-4.EXE.
function S.tanya_fatality_1_victim_shake()
  mk4.prefixed_to(9, 409)
  mk4.forward_to(11)
  mk4.playback_pair(9)
  mk4.forward_to(11)
  mk4.playback_pair(9)
  mk4.forward_to(11)
  mk4.playback_pair(9)
  mk4.forward_to(11)
  mk4.playback_pair(9)
  mk4.forward_to(21)
  mk4.forward_to(23)
  mk4.playback_pair(21)
  mk4.forward_to(23)
  mk4.playback_pair(21)
  mk4.forward_to(23)
  mk4.playback_pair(21)
  mk4.forward_to(23)
  mk4.playback_pair(21)
  mk4.forward_to(38)
  mk4.forward_to(40)
  mk4.playback_pair(38)
  mk4.forward_to(40)
  mk4.playback_pair(38)
  mk4.forward_to(40)
  mk4.playback_pair(38)
  mk4.forward_to(40)
  mk4.playback_pair(38)
  mk4.forward_to(55)
  mk4.forward_to(57)
  mk4.playback_pair(55)
  mk4.forward_to(57)
  mk4.playback_pair(55)
  mk4.forward_to(57)
  mk4.playback_pair(55)
  mk4.forward_to(57)
  mk4.playback_pair(55)
  mk4.forward_to(64)
  mk4.forward_to(66)
  mk4.playback_pair(64)
  mk4.forward_to(66)
  mk4.playback_pair(64)
  mk4.forward_to(66)
  mk4.playback_pair(64)
  mk4.forward_to(72)
  mk4.forward_to(74)
  mk4.playback_pair(72)
  mk4.forward_to(74)
  mk4.playback_pair(72)
  mk4.tail(native.TANYA_FIRST_VICTIM_BURST)
end

-- Retail script 0x004e16d0 in MK4/MK-4.EXE.
function S.tanya_first_victim_burst()
  mk4.forward()
  mk4.playback_pair(72)
  return S.tanya_first_victim_burst()
end

-- Retail script 0x004e1f58 in MK4/MK-4.EXE.
function S.fujin_first_fatality()
  mk4.half_restart(398)
  mk4.tail(native.FUJIN_FIRST_LOOP_START)
  return S.wind_finish_cycle_script()
end

-- Retail script 0x004e1f68 in MK4/MK-4.EXE.
function S.wind_finish_cycle_script()
  mk4.prefixed_finish(399)
  mk4.tail(native.FUJIN_FIRST_CYCLE)
end

-- Retail script 0x004e1f80 in MK4/MK-4.EXE.
function S.wind_finish_exit_script()
  mk4.prefixed_finish(210)
  mk4.prefixed_finish(212)
  mk4.tail(native.FUJIN_FIRST_SHOOT)
  return S.fujin_fatality_1_crossbow_shot()
end

-- Retail script 0x004e1f98 in MK4/MK-4.EXE.
function S.fujin_fatality_1_crossbow_shot()
  mk4.prefixed_finish(213)
  mk4.tail(native.TASK_RETIRE)
  return S.dragon_victim_script()
end

-- Retail script 0x004e1fa8 in MK4/MK-4.EXE.
function S.dragon_victim_script()
  mk4.prefixed_finish(419)
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e2cf0 in MK4/MK-4.EXE.
function S.owner_tear()
  mk4.half_count(32, 425)
  mk4.forward_to(43)
  mk4.playback_pair(41)
  mk4.forward_to(43)
  mk4.playback_pair(41)
  mk4.forward()
  mk4.tail(native.REIKO_FIRST_VICTORY)
end

-- Retail script 0x004e2d18 in MK4/MK-4.EXE.
function S.owner_victory_loop()
  mk4.prefixed_finish(443)
  return S.owner_victory_loop()
end

-- Retail script 0x004e2d28 in MK4/MK-4.EXE.
function S.victim_place()
  mk4.prefixed_to(37, 428)
  mk4.tail(native.REIKO_FIRST_TEAR)
  return S.reiko_first_tear()
end

-- Retail script 0x004e2d38 in MK4/MK-4.EXE.
function S.reiko_first_tear()
  mk4.cursor_set(38)
  mk4.forward_to(117)
  mk4.tail(native.REIKO_FIRST_VICTIM_118)
  return S.reiko_first_victim_118()
end

-- Retail script 0x004e2d48 in MK4/MK-4.EXE.
function S.reiko_first_victim_118()
  mk4.forward_to(165)
  mk4.tail(native.REIKO_FIRST_VICTIM_165)
end

-- Retail script 0x004e2d58 in MK4/MK-4.EXE.
function S.reiko_first_victim_165()
  mk4.forward()
  mk4.tail(native.REIKO_LEG_TEAR_VICTIM_FINISH)
end

-- Retail script 0x004e2d68 in MK4/MK-4.EXE.
function S.torso_start()
  mk4.prefixed_to(29, 429)
  mk4.tail(native.REIKO_FIRST_TORSO_29)
  return S.reiko_first_torso_29()
end

-- Retail script 0x004e2d78 in MK4/MK-4.EXE.
function S.reiko_first_torso_29()
  mk4.forward()
  mk4.tail(native.REIKO_FIRST_TORSO_END)
end

-- Retail script 0x004e2d88 in MK4/MK-4.EXE.
function S.head_start()
  mk4.prefixed_to(41, 430)
  mk4.cursor_set(21)
  mk4.forward_to(41)
  mk4.cursor_set(21)
  mk4.forward_to(41)
  mk4.cursor_set(21)
  mk4.forward_to(41)
  mk4.cursor_set(21)
  mk4.forward_to(41)
  mk4.cursor_set(21)
  mk4.forward_to(58)
  mk4.tail(native.REIKO_FIRST_HEAD_59)
  return S.reiko_first_head_59()
end

-- Retail script 0x004e2dc0 in MK4/MK-4.EXE.
function S.reiko_first_head_59()
  mk4.forward_to(82)
  mk4.tail(native.REIKO_FIRST_HEAD_83)
end

-- Retail script 0x004e2dd0 in MK4/MK-4.EXE.
function S.reiko_first_head_83()
  mk4.forward()
  mk4.tail(native.REIKO_LEG_TEAR_HEAD_FINISH)
end

-- Retail script 0x004e2de0 in MK4/MK-4.EXE.
function S.left_arm_start()
  mk4.prefixed_to(25, 427)
  mk4.cursor_set(4)
  mk4.forward_to(55)
  mk4.tail(native.REIKO_FIRST_LEFT_ARM_56)
  return S.reiko_first_left_arm_56()
end

-- Retail script 0x004e2df8 in MK4/MK-4.EXE.
function S.reiko_first_left_arm_56()
  mk4.forward_to(66)
  mk4.tail(native.REIKO_FIRST_LEFT_ARM_LAND)
end

-- Retail script 0x004e2e08 in MK4/MK-4.EXE.
function S.reiko_first_left_arm_land()
  mk4.forward()
  mk4.tail(native.REIKO_LEG_TEAR_LEFT_ARM_FINISH)
end

-- Retail script 0x004e2e18 in MK4/MK-4.EXE.
function S.right_arm_start()
  mk4.prefixed_to(29, 426)
  mk4.cursor_set(4)
  mk4.forward_to(29)
  mk4.cursor_set(4)
  mk4.forward_to(29)
  mk4.cursor_set(4)
  mk4.forward_to(43)
  mk4.tail(native.REIKO_FIRST_RIGHT_ARM_44)
  return S.reiko_first_right_arm_44()
end

-- Retail script 0x004e2e40 in MK4/MK-4.EXE.
function S.reiko_first_right_arm_44()
  mk4.forward_to(68)
  mk4.tail(native.REIKO_FIRST_RIGHT_ARM_LAND)
end

-- Retail script 0x004e2e50 in MK4/MK-4.EXE.
function S.reiko_first_right_arm_land()
  mk4.forward()
  mk4.tail(native.REIKO_LEG_TEAR_RIGHT_ARM_FINISH)
end

-- Retail script 0x004e2e60 in MK4/MK-4.EXE.
function S.owner_entry()
  mk4.prefixed_to(10, 432)
  mk4.tail(native.REPTILE_FIRST_OWNER_CYCLE)
  return S.reptile_first_owner_cycle()
end

-- Retail script 0x004e2e70 in MK4/MK-4.EXE.
function S.reptile_first_owner_cycle()
  mk4.forward_to(138)
  mk4.cursor_set(99)
  mk4.forward_to(138)
  mk4.cursor_set(99)
  mk4.forward_to(138)
  mk4.cursor_set(99)
  mk4.forward_to(168)
  mk4.tail(native.REPTILE_FIRST_OWNER_BITE)
end

-- Retail script 0x004e2e98 in MK4/MK-4.EXE.
function S.reptile_first_owner_bite()
  mk4.forward()
  mk4.tail(native.REPTILE_FIRST_OWNER_LOOP)
end

-- Retail script 0x004e2ea8 in MK4/MK-4.EXE.
function S.reptile_first_owner_loop()
  return S.reptile_first_owner_bite()
end

-- Retail script 0x004e2eb0 in MK4/MK-4.EXE.
function S.victim_entry()
  mk4.prefixed_to(85, 431)
  mk4.tail(native.REPTILE_FIRST_VICTIM_BITE)
  return S.reptile_first_victim_bite()
end

-- Retail script 0x004e2ec0 in MK4/MK-4.EXE.
function S.reptile_first_victim_bite()
  mk4.forward_to(124)
  mk4.tail(native.REPTILE_FIRST_VICTIM_SHAKE)
end

-- Retail script 0x004e2ed0 in MK4/MK-4.EXE.
function S.reptile_first_victim_shake()
  mk4.cursor_set(85)
  mk4.forward_to(124)
  mk4.cursor_set(85)
  mk4.forward_to(124)
  mk4.cursor_set(85)
  mk4.tail(native.REPTILE_FIRST_VICTIM_COLLAPSE)
end

-- Retail script 0x004e2ef0 in MK4/MK-4.EXE.
function S.shinnok_fatality_1_owner_vanish()
  mk4.prefixed_to(32, 12)
  mk4.tail(native.SHINNOK_FIRST_VANISHED)
  return S.shinnok_first_vanished()
end

-- Retail script 0x004e2f00 in MK4/MK-4.EXE.
function S.shinnok_first_vanished()
  mk4.forward()
  mk4.tail(native.SHINNOK_FIRST_HIDDEN)
end

-- Retail script 0x004e2f10 in MK4/MK-4.EXE.
function S.shinnok_fatality_return()
  mk4.prefixed_to(18, 436)
  mk4.tail(native.SHINNOK_FATALITY_REAPPEAR)
  return S.shinnok_fatality_reappear()
end

-- Retail script 0x004e2f20 in MK4/MK-4.EXE.
function S.shinnok_fatality_reappear()
  mk4.forward()
  mk4.tail(native.SHINNOK_FATALITY_VICTORY)
end

-- Retail script 0x004e2f30 in MK4/MK-4.EXE.
function S.victory_loop()
  mk4.prefixed_finish(443)
  return S.victory_loop()
end

-- Retail script 0x004e2f40 in MK4/MK-4.EXE.
function S.hand_entry()
  mk4.prefixed_to(164, 434)
  mk4.cursor_set(159)
  mk4.tail(native.SHINNOK_FIRST_HAND_SQUEEZE)
end

-- Retail script 0x004e2f58 in MK4/MK-4.EXE.
function S.shinnok_fatality_1_hand_squeeze()
  mk4.forward_to(164)
  mk4.cursor_set(159)
  mk4.tail(native.SHINNOK_HAND_SQUEEZE_CYCLE)
  return S.shinnok_fatality_1_hand_release()
end

-- Retail script 0x004e2f68 in MK4/MK-4.EXE.
function S.shinnok_fatality_1_hand_release()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e2f78 in MK4/MK-4.EXE.
function S.victim_lift()
  mk4.prefixed_to(64, 435)
  mk4.tail(native.SHINNOK_FIRST_VICTIM_HELD)
  return S.shinnok_first_victim_held()
end

-- Retail script 0x004e2f88 in MK4/MK-4.EXE.
function S.shinnok_first_victim_held()
  mk4.cursor_set(59)
  mk4.tail(native.SHINNOK_FIRST_VICTIM_SQUEEZE)
end

-- Retail script 0x004e2f98 in MK4/MK-4.EXE.
function S.shinnok_fatality_1_victim_squeeze()
  mk4.forward_to(64)
  mk4.cursor_set(59)
  mk4.tail(native.SHINNOK_VICTIM_SQUEEZE_CYCLE)
  return S.shinnok_fatality_1_victim_crush()
end

-- Retail script 0x004e2fa8 in MK4/MK-4.EXE.
function S.shinnok_fatality_1_victim_crush()
  mk4.forward_to(138)
  mk4.tail(native.SHINNOK_FIRST_VICTIM_CRUSH)
end

-- Retail script 0x004e2fb8 in MK4/MK-4.EXE.
function S.shinnok_first_victim_crush()
  mk4.forward()
  mk4.tail(native.TASK_RETIRE)
end

-- Retail script 0x004e2fc8 in MK4/MK-4.EXE.
function S.shinnok_second_vanish()
  mk4.prefixed_to(32, 12)
  mk4.tail(native.SHINNOK_SECOND_VANISHED)
  return S.shinnok_second_vanished()
end

-- Retail script 0x004e2fd8 in MK4/MK-4.EXE.
function S.shinnok_second_vanished()
  mk4.forward()
  mk4.tail(native.SHINNOK_SECOND_HANDS)
end

-- Retail script 0x004e56c8 in MK4/MK-4.EXE.
function S.raiden_second_fatality()
  mk4.native_release(75)
  mk4.tail(native.RAIDEN_SCRIPT_SECOND_BLEND)
end

-- Retail script 0x004e56d8 in MK4/MK-4.EXE.
function S.raiden_fatality_2_first_playback()
  mk4.call(native.RAIDEN_SCRIPT_SECOND_PARTICLES)
  mk4.playback_pair(94)
  mk4.forward()
  mk4.tail(native.RAIDEN_SCRIPT_SECOND_EFFECT)
  return S.raiden_fatality_2_repeated_playback()
end

-- Retail script 0x004e56f0 in MK4/MK-4.EXE.
function S.raiden_fatality_2_repeated_playback()
  mk4.native_release(76)
  mk4.playback_pair(94)
  mk4.forward()
  mk4.playback_pair(94)
  mk4.forward()
  mk4.playback_pair(80)
  mk4.scene_active()
  mk4.blend(570)
  mk4.native_return(187)
  mk4.clip(570)
  mk4.tail(native.RAIDEN_FATALITY_2_HELD_POSE)
  return S.raiden_fatality_2_held_pose()
end

-- Retail script 0x004e5728 in MK4/MK-4.EXE.
function S.raiden_fatality_2_held_pose()
  mk4.bind_cursor(0, 571)
  mk4.wait(211)
  mk4.native_release(77)
  mk4.tail(native.RAIDEN_SCRIPT_SECOND_RELEASE)
  return S.raiden_fatality_2_ending()
end

-- Retail script 0x004e5740 in MK4/MK-4.EXE.
function S.raiden_fatality_2_ending()
  mk4.spawn(0x26b, native.SCENE_KEEPALIVE_TASK)
  mk4.clip(572)
  mk4.tail(native.RAIDEN_SCRIPT_SECOND_END)
end

-- Retail script 0x004e5760 in MK4/MK-4.EXE.
function S.raiden_fatality_2_owner_hold()
  mk4.clip(573)
  mk4.park()
end

-- Retail script 0x004e5770 in MK4/MK-4.EXE.
function S.raiden_fatality_2_victim()
  mk4.clip(574)
  mk4.bind_cursor(0, 575)
  mk4.tail(native.RAIDEN_SCRIPT_SECOND_VICTIM_END)
  return S.raiden_fatality_2_victim_hold()
end

-- Retail script 0x004e5788 in MK4/MK-4.EXE.
function S.raiden_fatality_2_victim_hold()
  mk4.wait(211)
  mk4.clip(576)
  mk4.park()
  return S.tanya_fatality_2()
end

-- Retail script 0x004e5798 in MK4/MK-4.EXE.
function S.tanya_fatality_2()
  mk4.native_release(78)
  mk4.wait(30)
  mk4.wait(30)
  mk4.scene_active()
  mk4.sound_indexed(18)
  mk4.native_return(186)
  mk4.clip(564)
  mk4.tail(native.TANYA_SECOND_HEIGHT)
  return S.tanya_second_height()
end

-- Retail script 0x004e57c0 in MK4/MK-4.EXE.
function S.tanya_second_height()
  mk4.spawn(0x26b, native.TANYA_SECOND_OWNER_CUE)
  mk4.clip(565)
  mk4.scene_event(69)
  mk4.wait(10)
  mk4.spawn(0x26b, native.TANYA_SECOND_OWNER_CUE)
  mk4.clip(565)
  mk4.scene_event(70)
  mk4.wait(10)
  mk4.spawn(0x26b, native.TANYA_SECOND_OWNER_CUE)
  mk4.clip(565)
  mk4.clip(566)
  mk4.tail(native.TANYA_SECOND_FINISH)
end

-- Retail script 0x004e5820 in MK4/MK-4.EXE.
function S.tanya_fatality_2_victim_scene()
  mk4.clip(567)
  mk4.clip(568)
  mk4.wait(10)
  mk4.clip(568)
  mk4.wait(10)
  mk4.clip(568)
  mk4.spawn(0x26b, native.TANYA_SECOND_VICTIM_CUE)
  mk4.tail(native.TANYA_FATALITY_2_VICTIM_COLLAPSE)
end

-- Retail script 0x004e5860 in MK4/MK-4.EXE.
function S.tanya_fatality_2_victim_collapse()
  mk4.native_release(71)
  mk4.clip(569)
  mk4.park()
  return S.cage_fatality_2_scene_entry()
end

-- Retail script 0x004e5870 in MK4/MK-4.EXE.
function S.cage_fatality_2_scene_entry()
  mk4.native_release(68)
  mk4.wait(30)
  mk4.tail(native.CAGE_SCRIPT_FATALITY_2_CONTINUE)
  return S.cage_fatality_2_uppercut()
end

-- Retail script 0x004e5880 in MK4/MK-4.EXE.
function S.cage_fatality_2_uppercut()
  mk4.wait(5)
  mk4.scene_active()
  mk4.sound_c(2)
  mk4.wait(6)
  mk4.spawn(0x26b, native.CAGE_SECOND_CHARGE)
  mk4.wait(6)
  mk4.play_clip(1, 293)
  mk4.half_count(4)
  mk4.spawn_effect(6)
  mk4.sound_context(2)
  mk4.spawn(0x26b, native.CAGE_SECOND_FLASH)
  mk4.half_count(1)
  mk4.sound_c(93)
  mk4.sound_b(1)
  mk4.native_return(185)
  mk4.tail(native.CAGE_SECOND_REPEATED_UPPERCUTS)
  return S.cage_second_repeated_uppercuts()
end

-- Retail script 0x004e58d8 in MK4/MK-4.EXE.
function S.cage_second_repeated_uppercuts()
  mk4.prefixed_to(60, 111)
  mk4.native_release(80)
  mk4.forward()
  mk4.cursor_set(42)
  mk4.forward()
  return S.raiden_victory_clip_111_cursor_set_1()
end

-- Retail script 0x004e5900 in MK4/MK-4.EXE.
function S.cage_second_victim_hit()
  mk4.clip(130)
  mk4.blend(206)
  mk4.play_clip(20, 206)
  mk4.sound_special()
  mk4.forward_to(90)
  mk4.sound_special()
  mk4.tail(native.CAGE_SECOND_VICTIM_FINISH)
end

-- Retail script 0x004e5930 in MK4/MK-4.EXE.
function S.sonya_second_fatality()
  mk4.native_release(79)
  mk4.wait(40)
  mk4.tail(native.SONYA_SCRIPT_SECOND_OPEN)
  return S.sonya_fatality_2_opening()
end

-- Retail script 0x004e5940 in MK4/MK-4.EXE.
function S.sonya_fatality_2_opening()
  mk4.prefixed_finish(558)
  mk4.native_return(184)
  mk4.scene_active()
  mk4.clip(559)
  mk4.tail(native.SONYA_SCRIPT_SECOND_REPEAT)
  return S.sonya_fatality_2_cycle()
end

-- Retail script 0x004e5960 in MK4/MK-4.EXE.
function S.sonya_fatality_2_cycle()
  mk4.spawn(0x26b, native.SCENE_KEEPALIVE_TASK)
  mk4.play_clip(24, 560)
  mk4.wait(6)
  mk4.tail(native.SONYA_SCRIPT_SECOND_BRANCH)
  return S.sonya_fatality_2_first_cycle()
end

-- Retail script 0x004e5980 in MK4/MK-4.EXE.
function S.sonya_fatality_2_first_cycle()
  mk4.playback_tail(0)
  mk4.tail(native.SONYA_SCRIPT_SECOND_PAUSE)
end

-- Retail script 0x004e5990 in MK4/MK-4.EXE.
function S.sonya_fatality_2_second_cycle()
  mk4.playback_tail(0)
  mk4.wait(15)
  mk4.clip(560)
  mk4.tail(native.SONYA_SCRIPT_SECOND_CYCLE_END)
  return S.sonya_fatality_2_owner_ending()
end

-- Retail script 0x004e59a8 in MK4/MK-4.EXE.
function S.sonya_fatality_2_owner_ending()
  mk4.forward()
  mk4.prefixed_finish(114)
  return S.sonya_fatality_2_owner_ending_playback_pair_1()
end

-- Retail script 0x004e59b4 in MK4/MK-4.EXE.
function S.sonya_fatality_2_owner_ending_playback_pair_1()
  mk4.playback_pair(69)
  mk4.forward_to(99)
  return S.sonya_fatality_2_owner_ending_playback_pair_1()
end

-- Retail script 0x004e59c8 in MK4/MK-4.EXE.
function S.sonya_fatality_2_victim()
  mk4.clip(561)
  mk4.tail(native.SONYA_SCRIPT_SECOND_VICTIM_END)
end

-- Retail script 0x004e5a20 in MK4/MK-4.EXE.
function S.sonya_fatality_2_victim_end()
  mk4.forward()
  mk4.park()
end

-- Retail script 0x004e5a80 in MK4/MK-4.EXE.
function S.jax_fatality_2()
  mk4.native_release(67)
  mk4.bind_cursor(0, 284)
  mk4.wait(20)
  mk4.tail(native.JAX_FATALITY_2_APPROACH)
  return S.jax_fatality_2_approach()
end

-- Retail script 0x004e5a98 in MK4/MK-4.EXE.
function S.jax_fatality_2_approach()
  mk4.spawn(0x26b, native.JAX_FATALITY_2_SPARKS)
  mk4.native_release(67)
  mk4.sound_c(70)
  mk4.wait(15)
  mk4.scene_active()
  mk4.sound_c(70)
  mk4.wait(15)
  mk4.sound_c(2)
  mk4.prefixed_finish(555)
  mk4.spawn(0x26b, native.JAX_FATALITY_2_FLASH)
  mk4.spawn_effect(6)
  mk4.sound_c(105)
  mk4.sound_c(44)
  mk4.native_return(183)
  mk4.wait(15)
  mk4.clip(556)
  mk4.native_release(80)
  mk4.prefixed_finish(114)
  return S.jax_fatality_2_approach_playback_pair_1()
end

-- Retail script 0x004e5afc in MK4/MK-4.EXE.
function S.jax_fatality_2_approach_playback_pair_1()
  mk4.playback_pair(69)
  mk4.forward_to(99)
  return S.jax_fatality_2_approach_playback_pair_1()
end

-- Retail script 0x004e5b10 in MK4/MK-4.EXE.
function S.jax_fatality_2_head()
  mk4.sound_b(8)
  mk4.clip(557)
  mk4.spawn(0x26b, native.SCENE_KEEPALIVE_TASK)
  mk4.clip(206)
  mk4.tail(native.JAX_FATALITY_2_FINISH)
  return S.cage_fatality_2_victim_start()
end

-- Retail script 0x004e5b38 in MK4/MK-4.EXE.
function S.cage_fatality_2_victim_start()
  mk4.forward()
  mk4.tail(native.JAX_FATALITY_2_VICTIM_PAUSE)
end

return S

-- Throws.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004ddff8 in MK4/MK-4.EXE.
function S.jarek_lk_throw_startup()
  mk4.native_release(32)
  mk4.sound_indexed(7)
  mk4.open_peer_clip(164)
  mk4.open_clip(0, 30)
  mk4.playback_single(22)
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x1999)
  mk4.playback_single(61)
  mk4.native_step(153)
  mk4.tail(native.JAREK_LK_THROW_OWNER_RETURN)
  return S.tanya_lk_throw_startup()
end

-- Retail script 0x004de038 in MK4/MK-4.EXE.
function S.tanya_lk_throw_startup()
  mk4.native_release(24)
  mk4.sound_indexed(29)
  mk4.open_peer_clip(162)
  mk4.open_clip(0, 42)
  mk4.playback_single(30)
  mk4.tail(native.LK_BRANCH_CONTINUE)
end

-- Retail script 0x004de060 in MK4/MK-4.EXE.
function S.lk_branch_continue()
  mk4.playback_single(47)
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x1999)
  mk4.playback_single(127)
  mk4.native_step(135)
  mk4.tail(native.LK_RECOIL)
  return S.shinnok_lk_throw_startup()
end

-- Retail script 0x004de088 in MK4/MK-4.EXE.
function S.shinnok_lk_throw_startup()
  mk4.sound_indexed(55)
  mk4.native_release(21)
  mk4.open_peer_clip(146)
  mk4.open_clip(0, 31)
  mk4.playback_single(59)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x1999)
  mk4.playback_single(127)
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x23d7)
  mk4.playback_single(157)
  mk4.native_step(134)
  mk4.tail(native.LK_JUMP_BACK)
end

-- Retail script 0x004de0e8 in MK4/MK-4.EXE.
function S.reptile_lk_throw_startup()
  mk4.native_release(18)
  mk4.sound_indexed(27)
  mk4.open_peer_clip(147)
  mk4.open_clip(0, 32)
  mk4.playback_single(85)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x35c2)
  mk4.playback_single(105)
  mk4.native_release(19)
  mk4.playback_single(123)
  mk4.native_step(133)
  mk4.tail(native.LK_PAUSE_LONG)
end

-- Retail script 0x004de138 in MK4/MK-4.EXE.
function S.reiko_lk_throw_startup()
  mk4.open_peer_clip(148)
  mk4.open_clip(0, 40)
  mk4.playback_single(38)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.damage(0x35c2)
  mk4.tail(native.LK_THROW_ROCK_REPEAT_ONE)
end

-- Retail script 0x004de170 in MK4/MK-4.EXE.
function S.lk_throw_rock_repeat_one()
  mk4.playback_single(43)
  mk4.playback_pair_reverse(42)
  mk4.tail(native.LK_THROW_ROCK_REPEAT_TWO)
  return S.lk_throw_rock_repeat_two()
end

-- Retail script 0x004de180 in MK4/MK-4.EXE.
function S.lk_throw_rock_repeat_two()
  mk4.playback_single(43)
  mk4.playback_pair_reverse(42)
  mk4.tail(native.LK_THROW_ROCK_RELEASE)
  return S.lk_throw_rock_release()
end

-- Retail script 0x004de190 in MK4/MK-4.EXE.
function S.lk_throw_rock_release()
  mk4.playback_single(43)
  mk4.playback_pair_reverse(42)
  mk4.playback_single(43)
  mk4.playback_pair_reverse(42)
  mk4.playback_single(59)
  mk4.native_step(132)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004de1b0 in MK4/MK-4.EXE.
function S.quan_chi_lk_throw_startup()
  mk4.native_release(16)
  mk4.open_peer_clip(160)
  mk4.open_clip(0, 38)
  mk4.playback_single(40)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.damage(0x35c2)
  mk4.playback_single(99)
  mk4.native_release(17)
  mk4.native_step(131)
  mk4.native_word(0x8000)
  mk4.motion(0xfffff334, 0xffffe148, 0x28f)
  mk4.tail(native.LK_VICTIM_RETURN)
end

-- Retail script 0x004de218 in MK4/MK-4.EXE.
function S.raiden_lk_throw_startup()
  mk4.sound_indexed(25)
  mk4.native_release(14)
  mk4.open_peer_clip(149)
  mk4.open_clip(0, 33)
  mk4.playback_single(41)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_c(58)
  mk4.sound_roster(1)
  mk4.damage(0x35c2)
  mk4.native_step(111)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004de258 in MK4/MK-4.EXE.
function S.kai_lk_throw_startup()
  mk4.native_release(13)
  mk4.open_peer_clip(159)
  mk4.open_clip(0, 34)
  mk4.playback_single(72)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.damage(0x3333)
  mk4.native_step(110)
  mk4.native_word(0x1999)
  mk4.forward()
  mk4.tail(native.LK_PAUSE_SHORT)
  return S.fujin_lk_throw_startup()
end

-- Retail script 0x004de2a0 in MK4/MK-4.EXE.
function S.fujin_lk_throw_startup()
  mk4.native_release(7)
  mk4.open_peer_clip(155)
  mk4.open_clip(0, 36)
  mk4.playback_single(49)
  mk4.disarm_peer()
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.damage(0x2e14)
  mk4.playback_single(46)
  mk4.native_release(8)
  mk4.playback_single(86)
  mk4.clear_handler()
  mk4.native_step(91)
  mk4.forward_to(89)
  mk4.wait(12)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004de2f8 in MK4/MK-4.EXE.
function S.scorpion_lk_throw_startup()
  mk4.native_release(4)
  mk4.open_peer_clip(157)
  mk4.open_clip(0, 37)
  mk4.playback_single(18)
  mk4.tail(native.CAGE_LK_THROW_PAIRED_CONTINUATION)
  return S.cage_lk_throw_damage()
end

-- Retail script 0x004de318 in MK4/MK-4.EXE.
function S.cage_lk_throw_damage()
  mk4.playback_single(42)
  mk4.tail(native.CAGE_LK_THROW_RELEASE)
end

-- Retail script 0x004de328 in MK4/MK-4.EXE.
function S.cage_lk_throw_release()
  mk4.damage(0x2e14)
  mk4.disarm_peer()
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.spawn_effect(0)
  mk4.playback_single(65)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.native_step(77)
  mk4.tail(native.MMOVE_LK_OWNER_READY)
end

-- Retail script 0x004de360 in MK4/MK-4.EXE.
function S.sonya_lk_throw_startup()
  mk4.sound_indexed(19)
  mk4.open_peer_clip(163)
  mk4.open_clip(0, 43)
  mk4.playback_single(6)
  mk4.playback_single(24)
  mk4.native_release(3)
  mk4.playback_single(44)
  mk4.damage(0x2e14)
  mk4.spawn_effect(0)
  mk4.disarm_peer()
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.playback_single(75)
  mk4.native_step(78)
  mk4.tail(native.MMOVE_LK_BLEND_RETURN)
end

-- Retail script 0x004de3b0 in MK4/MK-4.EXE.
function S.liu_kang_lk_throw_startup()
  mk4.open_peer_clip(133)
  mk4.open_clip(0, 35)
  mk4.play_pair(10)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_LK_CONTINUE)
end

-- Retail script 0x004de3d0 in MK4/MK-4.EXE.
function S.cage_lk_throw_startup()
  mk4.native_release(39)
  mk4.sound_indexed(56)
  mk4.open_peer_clip(535)
  mk4.open_clip(0, 534)
  mk4.playback_single(23)
  mk4.damage(0x1999)
  mk4.native_step(174)
  mk4.forward_count(3)
  mk4.wait(16)
  mk4.forward()
  mk4.tail(native.CAGE_LK_THROW_OWNER_RETURN)
end

-- Retail script 0x004de410 in MK4/MK-4.EXE.
function S.liu_kang_lk_continue()
  mk4.native_release(1)
  mk4.play_pair(20)
  mk4.forward_to(38)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_LK_DAMAGE)
end

-- Retail script 0x004de428 in MK4/MK-4.EXE.
function S.liu_kang_lk_throw_damage_release()
  mk4.disarm_peer()
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.play_pair(2)
  mk4.native_return(80)
  mk4.forward_to(43)
  mk4.playback_pair(39)
  mk4.forward_to(43)
  mk4.playback_pair(39)
  mk4.forward_to(43)
  mk4.playback_pair(39)
  mk4.forward_to(43)
  mk4.playback_pair(39)
  mk4.playback_pair(0)
  return mk4.ready()
end

-- Retail script 0x004de468 in MK4/MK-4.EXE.
function S.liu_kang_lk_throw_retreat_entry()
  mk4.speed_negative(0x11eb)
  mk4.tail(native.LIU_KANG_LK_THROW_RETREAT)
  return S.liu_kang_lk_throw_retreat()
end

-- Retail script 0x004de478 in MK4/MK-4.EXE.
function S.liu_kang_lk_throw_retreat()
  mk4.open_clip(0, 182)
  mk4.double_to(28)
  mk4.landing()
  mk4.native_sound()
  return mk4.ready()
end

-- Retail script 0x004de490 in MK4/MK-4.EXE.
function S.sub_zero_lk_throw_startup()
  mk4.open_peer_clip(165)
  mk4.open_clip(0, 41)
  mk4.forward_count(10)
  mk4.native_release(2)
  mk4.sound_a(3)
  mk4.tail(native.SUB_ZERO_SCRIPT_LK_REBASE)
end

-- Retail script 0x004de4b8 in MK4/MK-4.EXE.
function S.sub_zero_lk_rebase()
  mk4.play_pair(29)
  mk4.disarm_peer()
  mk4.sound_a(36)
  mk4.play_pair(5)
  mk4.sound_roster(8)
  mk4.play_pair(8)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.tail(native.SUB_ZERO_SCRIPT_LK_DAMAGE)
  return S.sub_zero_lk_damage()
end

-- Retail script 0x004de4e0 in MK4/MK-4.EXE.
function S.sub_zero_lk_damage()
  mk4.play_pair(5)
  mk4.tail(native.SUB_ZERO_SCRIPT_LK_OWNER_RETURN)
end

-- Retail script 0x004de4f0 in MK4/MK-4.EXE.
function S.jax_lp_throw_first_slam()
  mk4.open_peer_clip(145)
  mk4.open_clip(0, 83)
  mk4.playback_single(55)
  mk4.spawn_effect(0)
  mk4.call(native.THROW_GRUNT)
  mk4.tail(native.JAX_LP_SLAM_FIRST)
  return S.slam_first_script()
end

-- Retail script 0x004de518 in MK4/MK-4.EXE.
function S.slam_first_script()
  mk4.playback_single(82)
  mk4.damage(0xccc)
  mk4.spawn_effect(0)
  mk4.call(native.THROW_GRUNT)
  mk4.tail(native.JAX_LP_SLAM_SECOND)
  return S.slam_second_script()
end

-- Retail script 0x004de538 in MK4/MK-4.EXE.
function S.slam_second_script()
  mk4.playback_single(116)
  mk4.spawn_effect(0)
  mk4.damage(0xccc)
  mk4.call(native.THROW_GRUNT)
  mk4.tail(native.JAX_LP_SLAM_THIRD)
  return S.slam_second_release_script()
end

-- Retail script 0x004de558 in MK4/MK-4.EXE.
function S.slam_second_release_script()
  mk4.native_return(125)
  return S.slam_first_release_script_native_word_1()
end

-- Retail script 0x004de568 in MK4/MK-4.EXE.
function S.slam_first_release_script()
  mk4.native_return(100)
  return S.slam_first_release_script_native_word_1()
end

-- Retail script 0x004de56c in MK4/MK-4.EXE.
function S.slam_first_release_script_native_word_1()
  mk4.native_word(0xf5c)
  mk4.damage(0xccc)
  mk4.tail(native.JAX_LP_THROW_RETURN)
end

-- Retail script 0x004de608 in MK4/MK-4.EXE.
function S.shared_tail_entry()
  mk4.tail(native.LP_SELECT_SHORT)
  return S.lp_select_short()
end

-- Retail script 0x004de610 in MK4/MK-4.EXE.
function S.lp_select_short()
  mk4.native_release(11)
  mk4.open_peer_clip(158)
  mk4.open_clip(0, 81)
  mk4.playback_single(46)
  mk4.native_return(109)
  mk4.damage(0x2e14)
  mk4.forward()
  mk4.tail(native.LP_THROW_OWNER_READY)
  return S.lp_select_long()
end

-- Retail script 0x004de640 in MK4/MK-4.EXE.
function S.lp_select_long()
  mk4.native_release(12)
  mk4.open_peer_clip(154)
  mk4.open_clip(0, 80)
  mk4.playback_single(57)
  mk4.native_return(107)
  mk4.tail(native.LP_END)
end

-- Retail script 0x004de668 in MK4/MK-4.EXE.
function S.lp_end()
  mk4.damage(0x2e14)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004de678 in MK4/MK-4.EXE.
function S.lp_end_native_release_1()
  mk4.native_release(36)
  mk4.open_peer_clip(151)
  mk4.open_clip(0, 79)
  mk4.playback_single(45)
  mk4.native_return(104)
  mk4.native_word(0xccc)
  mk4.damage(0x2666)
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

-- Retail script 0x004de6b8 in MK4/MK-4.EXE.
function S.lp_select()
  mk4.native_release(22)
  mk4.open_peer_clip(153)
  mk4.open_clip(0, 76)
  mk4.playback_single(25)
  mk4.native_return(102)
  mk4.native_word(0x1eb8)
  return S.shared_damage_entry()
end

-- Retail script 0x004de6e4 in MK4/MK-4.EXE.
function S.goro_lp_throw_final_damage()
  mk4.native_release(22)
  mk4.open_peer_clip(152)
  mk4.open_clip(0, 75)
  mk4.playback_single(42)
  mk4.native_return(100)
  mk4.native_word(0x1eb8)
  mk4.damage(0x2666)
  mk4.forward()
  mk4.tail(native.LP_THROW_VICTIM_ADJUST_RETURN)
end

-- Retail script 0x004de720 in MK4/MK-4.EXE.
function S.goro_lk_throw_startup()
  mk4.call(native.THROW_PAIR_START)
  mk4.native_release(82)
  mk4.open_peer_clip(610)
  mk4.open_clip(0, 609)
  mk4.playback_single(83)
  mk4.spawn_effect(0)
  mk4.sound_a(36)
  mk4.sound_roster(8)
  mk4.damage(0x3333)
  mk4.playback_single(140)
  mk4.tail(native.LP_ALIGN_AND_RELEASE)
  return S.goro_lk_throw_startup_call_1()
end

-- Retail script 0x004de760 in MK4/MK-4.EXE.
function S.goro_lk_throw_startup_call_1()
  mk4.call(native.THROW_PAIR_START)
  mk4.native_release(81)
  mk4.open_peer_clip(606)
  mk4.open_clip(0, 604)
  mk4.playback_single(47)
  mk4.sound_context(1)
  mk4.spawn_effect(1)
  mk4.effect(21)
  mk4.effect(21)
  mk4.tail(native.GORO_LP_THROW_FIVE_PHASE_SEQUENCE)
  return S.lp_throw_script()
end

-- Retail script 0x004de798 in MK4/MK-4.EXE.
function S.lp_throw_script()
  mk4.sound_b(7)
  return S.lp_throw_script_select_roster_1()
end

-- Retail script 0x004de79c in MK4/MK-4.EXE.
function S.lp_throw_script_select_roster_1()
  return mk4.select_roster {
    scorpion = S.goro_lp_throw_final_damage,
    raiden = S.shared_tail_entry_2,
    sonya = S.shared_tail_entry_2_native_release_1,
    liu_kang = S.lp_end_native_release_1,
    sub_zero = S.lp_select,
    fujin = S.shared_tail_entry,
    shinnok = S.goro_lp_throw_final_damage,
    reiko = S.shared_tail_entry_2,
    quan_chi = S.shared_tail_entry_2_native_release_1,
    tanya = S.lp_end_native_release_1,
    reptile = S.lp_select,
    kai = S.shared_tail_entry,
    jarek = S.shared_tail_entry_2,
    jax = S.lp_throw_script_select_roster_1_operand_16,
    cage = S.goro_lp_throw_final_damage,
    goro = S.goro_lk_throw_startup_call_1,
  }
end

function S.lp_throw_script_select_roster_1_operand_16()
  mk4.tail(native.JAX_LP_THROW_THROW_CONTINUATION)
  return S.goro_lp_throw_repeated_damage()
end

-- Retail script 0x004de7e8 in MK4/MK-4.EXE.
function S.goro_lp_throw_repeated_damage()
  mk4.open_peer_clip(607)
  mk4.open_clip(0, 605)
  mk4.playback_single(11)
  mk4.sound_context(1)
  mk4.spawn_effect(1)
  mk4.effect(21)
  mk4.effect(21)
  mk4.effect(21)
  mk4.damage(0x7ae)
  mk4.playback_single(23)
  mk4.sound_context(1)
  mk4.spawn_effect(1)
  mk4.effect(21)
  mk4.effect(21)
  mk4.damage(0x7ae)
  mk4.tail(native.GORO_SCRIPT_LP_PHASE_REPEAT)
end

-- Retail script 0x004de840 in MK4/MK-4.EXE.
function S.lp_shared_release()
  return S.goro_lp_throw_final_damage()
end

-- Retail script 0x004de858 in MK4/MK-4.EXE.
function S.shared_tail_entry_2()
  mk4.tail(native.LP_SELECT_LONG)
  return S.shared_tail_entry_2_native_release_1()
end

-- Retail script 0x004de860 in MK4/MK-4.EXE.
function S.shared_tail_entry_2_native_release_1()
  mk4.native_release(22)
  mk4.open_peer_clip(156)
  mk4.open_clip(0, 78)
  mk4.playback_single(21)
  mk4.native_return(103)
  mk4.damage(0x2666)
  mk4.forward_count(2)
  mk4.wait(10)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004de898 in MK4/MK-4.EXE.
function S.shared_damage_entry()
  mk4.damage(0x2666)
  mk4.forward()
  mk4.tail(native.SUB_ZERO_LP_THROW_OWNER_RETURN)
end

return S

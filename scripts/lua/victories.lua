-- Victory and native-return streams.
-- Translated from scripts/mk4/code/victories.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: raiden.victory (roster selector); sonya.victory (roster selector); sub_zero.fatality_2 (victory) (also shared by other catalog entries).
function S.raiden_victory()
  return mk4.select_roster {
    scorpion = S.raiden_victory_clip_115,
    raiden = S.raiden_victory_clip_123,
    sonya = S.raiden_victory_clip_114,
    liu_kang = S.raiden_victory_clip_119,
    sub_zero = S.raiden_victory_clip_115,
    fujin = S.raiden_victory_clip_120,
    shinnok = S.raiden_victory_clip_111,
    reiko = S.raiden_victory_clip_113,
    quan_chi = S.raiden_victory_clip_125,
    tanya = S.raiden_victory_clip_124,
    reptile = S.raiden_victory_clip_121,
    kai = S.raiden_victory_clip_122,
    jarek = S.raiden_victory_clip_125,
    jax = S.raiden_victory_clip_114,
    cage = S.raiden_victory_clip_111,
    goro = S.raiden_victory_clip_594,
  }
end

function S.raiden_victory_clip_594()
  mk4.native_release(83)
  mk4.prefixed_to(74, 594)
  mk4.sound_random()
  mk4.playback_pair(72)
  mk4.forward_to(74)
  mk4.playback_pair(72)
  mk4.forward_to(74)
  mk4.playback_pair(72)
  mk4.forward_to(74)
  mk4.forward()
  mk4.park()
  return S.raiden_victory_clip_111()
end

function S.raiden_victory_clip_111()
  mk4.prefixed_to(60, 111)
  mk4.native_release(15)
  mk4.forward()
  return S.raiden_victory_clip_111_cursor_set_1()
end

function S.raiden_victory_clip_111_cursor_set_1()
  mk4.cursor_set(42)
  mk4.forward()
  return S.raiden_victory_clip_111_cursor_set_1()
end

function S.raiden_victory_clip_115()
  mk4.prefixed_finish(115)
  mk4.open_clip(0, 116)
  mk4.prefixed_finish()
  mk4.native_release(6)
  mk4.prefixed_finish()
  mk4.prefixed_finish(117)
  mk4.bind_cursor(0, 118)
  mk4.park()
  return S.raiden_victory_clip_114()
end

function S.raiden_victory_clip_114()
  mk4.prefixed_finish(114)
  return S.raiden_victory_clip_114_playback_pair_1()
end

function S.raiden_victory_clip_114_playback_pair_1()
  mk4.playback_pair(69)
  mk4.forward_to(99)
  return S.raiden_victory_clip_114_playback_pair_1()
end

function S.raiden_victory_clip_113()
  mk4.prefixed_to(68, 113)
  return S.raiden_victory_clip_113_forward_1()
end

function S.raiden_victory_clip_113_forward_1()
  mk4.forward()
  mk4.cursor_set(69)
  return S.raiden_victory_clip_113_forward_1()
end

function S.raiden_victory_clip_125()
  mk4.native_release(35)
  mk4.prefixed_finish(125)
  return S.raiden_victory_clip_125_cursor_set_1()
end

function S.raiden_victory_clip_125_cursor_set_1()
  mk4.cursor_set(85)
  mk4.forward()
  return S.raiden_victory_clip_125_cursor_set_1()
end

function S.raiden_victory_clip_122()
  mk4.native_release(31)
  mk4.prefixed_finish(122)
  return S.raiden_victory_clip_122_cursor_set_1()
end

function S.raiden_victory_clip_122_cursor_set_1()
  mk4.cursor_set(96)
  mk4.forward()
  return S.raiden_victory_clip_122_cursor_set_1()
end

function S.raiden_victory_clip_119()
  mk4.native_release(30)
  mk4.prefixed_finish(119)
  return S.raiden_victory_clip_119_cursor_set_1()
end

function S.raiden_victory_clip_119_cursor_set_1()
  mk4.cursor_set(144)
  mk4.forward()
  return S.raiden_victory_clip_119_cursor_set_1()
end

function S.raiden_victory_clip_124()
  mk4.native_release(29)
  mk4.prefixed_finish(124)
  return S.raiden_victory_clip_124_cursor_set_1()
end

function S.raiden_victory_clip_124_cursor_set_1()
  mk4.cursor_set(88)
  mk4.forward()
  return S.raiden_victory_clip_124_cursor_set_1()
end

function S.raiden_victory_clip_123()
  mk4.native_release(34)
  mk4.play_clip(20, 123)
  mk4.call(native.RAIDEN_SCRIPT_VICTORY_EFFECT)
  mk4.forward()
  mk4.park()
  return S.raiden_victory_clip_121()
end

function S.raiden_victory_clip_121()
  mk4.native_release(27)
  mk4.prefixed_to(90, 121)
  mk4.native_release(28)
  return S.raiden_victory_clip_121_forward_1()
end

function S.raiden_victory_clip_121_forward_1()
  mk4.forward()
  mk4.cursor_set(160)
  return S.raiden_victory_clip_121_forward_1()
end

function S.raiden_victory_clip_120()
  mk4.native_release(25)
  mk4.prefixed_to(85, 120)
  mk4.spawn(0x26, native.SCENE_HOVER)
  mk4.native_release(26)
  mk4.forward()
  return S.raiden_victory_clip_120_cursor_set_1()
end

function S.raiden_victory_clip_120_cursor_set_1()
  mk4.cursor_set(93)
  mk4.forward()
  return S.raiden_victory_clip_120_cursor_set_1()
end

-- Catalog: liu_kang.high_kick (Liu Kang attacker).
function S.high_kick_clip_66()
  mk4.attack_selector(0x1)
  mk4.sound_a(1)
  mk4.play_clip(3, 66)
  mk4.double_count(17)
  mk4.tail(native.KICK_CONTACT_WIDE)
  return S.low_kick_clip_308()
end

function S.low_kick_clip_308()
  mk4.call(native.AVOID_CLEAR)
  mk4.attack_selector(0x5)
  mk4.sound_a(1)
  mk4.open_clip(0, 308)
  mk4.cursor_set(4)
  mk4.double_to(16)
  mk4.call(native.AVOID_SET)
  mk4.double_to(24)
  mk4.query_b(5)
  mk4.wait_branch(2, 4)
  mk4.action_recovery()
  mk4.backward_count(2)
  mk4.call(native.AVOID_CLEAR)
  mk4.release_attack()
  mk4.tail(native.LOW_KICK_REVERSE_RETURN)
end

-- Catalog: scorpion.low_kick (Scorpion attacker); liu_kang.low_kick (Liu Kang attacker).
function S.low_kick_clip_67()
  mk4.call(native.AVOID_CLEAR)
  mk4.attack_selector(0x6)
  mk4.sound_a(1)
  mk4.open_clip(0, 67)
  return S.low_kick_clip_67_prefixed_sample_1()
end

function S.low_kick_clip_67_prefixed_sample_1()
  mk4.prefixed_sample(0)
  mk4.wait(1)
  mk4.prefixed_sample(2)
  mk4.wait(1)
  mk4.prefixed_sample(4)
  mk4.wait(1)
  mk4.prefixed_sample(6)
  mk4.wait(1)
  mk4.prefixed_sample(8)
  mk4.wait(1)
  mk4.prefixed_sample(10)
  mk4.wait(1)
  mk4.prefixed_sample(12)
  mk4.wait(1)
  mk4.call(native.AVOID_SET)
  mk4.prefixed_sample(14)
  mk4.wait(1)
  mk4.prefixed_sample(16)
  mk4.wait(1)
  mk4.prefixed_sample(17)
  mk4.wait(1)
  mk4.call(native.AVOID_CLEAR)
  mk4.query_b(6)
  mk4.action_recovery()
  mk4.release_attack()
  mk4.wait_branch(5, 8)
  mk4.tail(native.LOW_KICK_NARROW_REVERSE_RETURN)
end

-- Catalog: sub_zero.low_kick (Sub-Zero attacker).
function S.low_kick_clip_70()
  mk4.call(native.AVOID_CLEAR)
  mk4.attack_selector(0x7)
  mk4.sound_a(1)
  mk4.open_clip(0, 70)
  mk4.prefixed_to(6)
  mk4.call(native.AVOID_SET)
  mk4.forward_to(11)
  mk4.call(native.AVOID_CLEAR)
  mk4.query_b(7)
  mk4.action_recovery()
  mk4.wait_branch(2, 4)
  mk4.forward_count(2)
  mk4.release_attack()
  mk4.forward()
  return mk4.ready()
end

function S.air_straight_kick_startup_clip_61()
  mk4.attack_selector(0x27)
  mk4.open_clip(0, 61)
  mk4.cursor_set(4)
  mk4.forward_to(7)
  mk4.sound_a(31)
  mk4.forward_to(10)
  mk4.tail(native.AIR_STRAIGHT_WIDE_TAIL)
  return S.air_straight_kick_startup_clip_61_clip_1()
end

function S.air_straight_kick_startup_clip_61_clip_1()
  mk4.clip(59)
  mk4.attack_selector(0x1c)
  mk4.tail(native.AIR_LK_ALT_TAIL)
  return S.air_straight_kick_startup_clip_61_clip_2()
end

function S.air_straight_kick_startup_clip_61_clip_2()
  mk4.clip(60)
  mk4.attack_selector(0x1d)
  mk4.tail(native.AIR_LK_WIDE_TAIL)
  return S.air_straight_kick_startup_clip_61_attack_selector_1()
end

function S.air_straight_kick_startup_clip_61_attack_selector_1()
  mk4.attack_selector(0x1f)
  mk4.tail(native.AIR_DIAGONAL_ALT_TAIL)
  return S.air_straight_kick_startup_clip_61_attack_selector_2()
end

function S.air_straight_kick_startup_clip_61_attack_selector_2()
  mk4.attack_selector(0x20)
  mk4.tail(native.AIR_DIAGONAL_WIDE_TAIL)
  return S.sweep_clip_63()
end

function S.sweep_clip_63()
  mk4.attack_selector(0x23)
  mk4.open_clip(0, 63)
  mk4.double_to(10)
  mk4.sound_a(175)
  mk4.forward_to(22)
  mk4.tail(native.SWEEP_STANDARD_TAIL)
end

-- Catalog: liu_kang.roundhouse (Liu Kang attacker).
function S.roundhouse_clip_64()
  mk4.attack_selector(0x22)
  mk4.open_clip(0, 64)
  mk4.tail(native.ROUNDHOUSE_WIDE_START)
end

return S

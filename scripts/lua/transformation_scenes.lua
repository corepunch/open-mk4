-- Transformation/second-finisher entry streams.
-- Translated from scripts/mk4/code/transformation_scenes.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: sub_zero.fatality_2 (victim).
function S.sub_zero_fatality_2_victim()
  mk4.prefixed_to(20, 206)
  mk4.sound_special()
  mk4.forward_to(90)
  mk4.sound_special()
  mk4.forward()
  mk4.tail(native.SUB_ZERO_SCRIPT_BREATH_VICTIM_END)
  return S.sub_zero_fatality_2_breath()
end

-- Catalog: sub_zero.fatality_2 (breath).
function S.sub_zero_fatality_2_breath()
  mk4.sound_indexed(4)
  mk4.play_clip(10, 552)
  mk4.spawn(0x0, native.BREATH_SCENE_DECORATION)
  mk4.sound_c(63)
  mk4.native_release(56)
  for _ = 1, 3 do
    mk4.cursor_set(10)
    mk4.forward_to(29)
  end
  mk4.native_release(57)
  mk4.sound_c(64)
  mk4.forward_to(83)
  mk4.scene_active()
  mk4.spawn(0x0, native.BREATH_SHATTER_DECORATION)
  for _ = 1, 5 do
    mk4.cursor_set(83)
    mk4.forward_to(93)
  end
  mk4.forward()
  mk4.tail(native.SUB_ZERO_SCRIPT_BREATH_APPROACH)
  return S.sub_zero_breath_approach()
end

-- Catalog: sub_zero.fatality_2 (approach).
function S.sub_zero_breath_approach()
  mk4.native_release(58)
  mk4.wait(20)
  mk4.speed_negative(0xffffd99a)
  mk4.sound_c(15)
  mk4.open_clip(0, 95)
  mk4.tail(native.SUB_ZERO_SCRIPT_BREATH_SLIDE)
end

-- Catalog: sub_zero.fatality_2 (uppercut).
function S.breath_resume()
  mk4.open_clip(0, 258)
  mk4.cursor_set(25)
  mk4.forward()
  mk4.sound_c(28)
  mk4.play_clip(5, 293)
  mk4.sound_c(59)
  mk4.tail(native.SUB_ZERO_SCRIPT_BREATH_SHATTER)
  return S.sub_zero_breath_shatter()
end

-- Catalog: sub_zero.fatality_2 (finish).
function S.sub_zero_breath_shatter()
  for _ = 1, 8 do
    mk4.forward_to(8)
    mk4.playback_pair(6)
  end
  mk4.forward()
  mk4.wait(60)
  mk4.tail(native.VICTORY_CONTINUE)
end

-- Catalog: reiko.fatality_2 (owner barrage).
function S.reiko_second_fatality()
  mk4.sound_indexed(62)
  mk4.native_release(64)
  mk4.wait(0)
  mk4.scene_active()
  mk4.play_clip(29, 553)
  mk4.spawn(0x0, native.REIKO_SECOND_STARS)
  for _ = 1, 9 do
    mk4.cursor_set(29)
    mk4.forward_to(38)
  end
  mk4.cursor_set(29)
  mk4.forward_to(33)
  mk4.wait(20)
  mk4.forward()
  mk4.tail(native.TASK_PARK_60_TICK_LOOP)
end

-- Catalog: reiko.fatality_2 (victim stagger).
function S.victim_stagger()
  mk4.blend(554)
  mk4.open_clip(0, 554)
  for _ = 1, 4 do
    mk4.cursor_set(0)
    mk4.forward_to(20)
  end
  mk4.native_release(65)
  mk4.forward_to(135)
  mk4.tail(native.REIKO_SECOND_VICTIM_FALL)
  return S.reiko_second_victim_fall()
end

-- Catalog: reiko.fatality_2 (victim fall).
function S.reiko_second_victim_fall()
  mk4.forward_to(145)
  mk4.sound_special()
  mk4.forward()
  mk4.sound_special()
  mk4.native_release(66)
  mk4.tail(native.REIKO_SECOND_VICTIM_END)
end

-- Catalog: jarek.fatality_2 (scene owner).
function S.jarek_fatality_2()
  mk4.native_release(59)
  mk4.play_clip(1, 577)
  mk4.scene_active()
  mk4.half_count(6)
  mk4.call(native.JAREK_SECOND_GLOW)
  mk4.half_count(19)
  mk4.sound_b(1)
  mk4.tail(native.JAREK_SECOND_HANDOFF)
end

-- Catalog: jarek.fatality_2 (laser owner).
function S.jarek_second_handoff()
  mk4.half_count(12)
  mk4.sound_c(11)
  mk4.half_count(20)
  mk4.sound_c(11)
  mk4.half_count(20)
  mk4.sound_c(11)
  mk4.half_count(20)
  mk4.sound_c(11)
  mk4.half_count(20)
  mk4.sound_c(11)
  mk4.tail(native.VICTORY_CONTINUE)
  return S.victim_start()
end

-- Catalog: jarek.fatality_2 (victim).
function S.victim_start()
  mk4.sound_c(11)
  mk4.play_clip(16, 182)
  mk4.backward_count(15)
  mk4.forward()
  mk4.sound_b(12)
  mk4.spawn(0x0, native.JAREK_SECOND_HEAD_FALL)
  mk4.prefixed_to(20, 206)
  mk4.sound_special()
  mk4.forward_to(90)
  mk4.forward()
  mk4.tail(native.JAREK_SECOND_VICTIM_HOLD)
end

-- Catalog: scorpion.fatality_2 (transformation).
function S.beast_start()
  mk4.native_release(60)
  mk4.play_clip(24, 12)
  mk4.sound_c(75)
  mk4.sound_c(74)
  mk4.spawn(0x0, native.SCORPION_SCRIPT_TRANSFORM_EFFECT)
  mk4.forward()
  mk4.tail(native.SCORPION_SCRIPT_BEAST_BEGIN)
end

-- Catalog: scorpion.fatality_2 (creature approach).
function S.scorpion_beast_begin()
  mk4.scene_active()
  mk4.clip(578)
  mk4.native_release(61)
  mk4.clip(579)
  mk4.sound_roster(1)
  mk4.tail(native.SCORPION_SCRIPT_BEAST_ATTACK)
end

-- Catalog: scorpion.fatality_2 (creature attack).
function S.scorpion_fatality_2_creature_attack()
  mk4.clip(580)
  for _ = 1, 5 do
    mk4.call(native.SCORPION_SCRIPT_BEAST_VOICE)
    mk4.backward_count(8)
    mk4.forward_count(8)
  end
  mk4.spawn_effect(6)
  mk4.sound_context(18)
  mk4.sound_roster(8)
  mk4.clip(581)
  mk4.native_release(62)
  mk4.bind_cursor(0, 578)
  mk4.tail(native.TASK_PARK_60_TICK_LOOP)
  return S.scorpion_fatality_2_victim()
end

-- Catalog: scorpion.fatality_2 (victim).
function S.scorpion_fatality_2_victim()
  mk4.play_clip(54, 584)
  mk4.sound_b(1)
  mk4.sound_context(7)
  mk4.tail(native.SCORPION_SCRIPT_BEAST_VICTIM_PIECES)
  return S.scorpion_beast_victim_pieces()
end

-- Catalog: scorpion.fatality_2 (victim release).
function S.scorpion_beast_victim_pieces()
  mk4.forward()
  for _ = 1, 5 do
    mk4.backward_count(8)
    mk4.forward_count(8)
  end
  mk4.spawn(0x0, native.SCORPION_SCRIPT_BEAST_GORE)
  mk4.clip(585)
  mk4.tail(native.TASK_PARK_60_TICK_LOOP)
end

return S

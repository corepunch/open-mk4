-- Projectiles.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004ed038 in MK4/MK-4.EXE.
function S.reiko_star()
  mk4.sound_c(100)
  mk4.prefixed_to(13, 437)
  mk4.spawn(0x52, native.REIKO_STAR_SPAWN)
  mk4.forward_to(18)
  mk4.playback_pair(16)
  mk4.forward_to(18)
  mk4.playback_pair(16)
  mk4.forward_to(18)
  mk4.playback_pair(16)
  mk4.forward_to(18)
  mk4.playback_pair(16)
  mk4.forward_to(18)
  mk4.playback_pair(16)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed090 in MK4/MK-4.EXE.
function S.jax_missile()
  mk4.prefixed_to(10, 29)
  mk4.sound_c(93)
  mk4.spawn(0x50, native.JAX_MISSILE_SPAWN)
  mk4.forward_to(25)
  mk4.half_finish()
  return mk4.ready()
end

-- Retail script 0x004ed0d8 in MK4/MK-4.EXE.
function S.cast_orb()
  mk4.sound_c(95)
  mk4.open_clip(0, 28)
  mk4.double_to(8)
  mk4.forward_to(20)
  mk4.spawn(0x4f, native.CAGE_ORB_SPAWN)
  mk4.forward()
  mk4.action_followup()
  mk4.wait(22)
  return mk4.ready()
end

-- Retail script 0x004ed118 in MK4/MK-4.EXE.
function S.jarek_blade()
  mk4.sound_c(103)
  if mk4.fast_start() then
    return S.shared_open_clip_entry()
  end
  mk4.prefixed_to(17, 27)
  return S.jarek_blade_spawn_1()
end

-- Retail script 0x004ed12c in MK4/MK-4.EXE.
function S.jarek_blade_spawn_1()
  mk4.spawn(0x4e, native.JAREK_BLADE_SPAWN)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward_to(17)
  mk4.playback_pair(15)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed190 in MK4/MK-4.EXE.
function S.boomerang_start()
  mk4.sound_c(33)
  mk4.play_clip(7, 229)
  mk4.tail(native.TANYA_BOOMERANG_RELEASE)
end

-- Retail script 0x004ed1e8 in MK4/MK-4.EXE.
function S.reptile_acid_spit()
  mk4.play_clip(11, 25)
  mk4.call(native.REPTILE_SCRIPT_HEAD_OPEN)
  mk4.forward_count(5)
  mk4.call(native.REPTILE_SCRIPT_HEAD_WIDE)
  mk4.forward_count(5)
  mk4.spawn(0x4c, native.REPTILE_ACID_SPAWN)
  mk4.call(native.REPTILE_SCRIPT_ACID_DROOL)
  mk4.forward_count(8)
  mk4.call(native.REPTILE_SCRIPT_HEAD_OPEN)
  mk4.action_followup()
  mk4.forward_to(30)
  mk4.playback_pair(27)
  mk4.forward_to(30)
  mk4.playback_pair(27)
  mk4.forward_to(30)
  mk4.playback_pair(27)
  mk4.call(native.REPTILE_SCRIPT_HEAD_CLOSE)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed268 in MK4/MK-4.EXE.
function S.tanya_fireball_caster()
  mk4.prefixed_finish(26)
  mk4.sound_a(33)
  mk4.block_sound()
  mk4.spawn(0x4b, native.FIREBALL_SPAWN)
  mk4.playback_pair(11)
  mk4.forward_to(13)
  mk4.action_followup()
  mk4.playback_pair(11)
  mk4.forward_to(13)
  mk4.playback_pair(11)
  mk4.forward_to(13)
  mk4.playback_pair(11)
  mk4.forward_to(13)
  mk4.playback_pair(0)
  return mk4.ready()
end

-- Retail script 0x004ed2c0 in MK4/MK-4.EXE.
function S.air_fireball()
  mk4.sound_a(33)
  mk4.block_sound_alt()
  mk4.prefixed_to(9, 24)
  mk4.spawn(0x4b, native.AIR_FIREBALL_SPAWN)
  mk4.backward_count(5)
  mk4.tail(native.FIREBALL_FALL)
end

-- Retail script 0x004ed2f8 in MK4/MK-4.EXE.
function S.kai_ground_skull()
  mk4.prefixed_finish(22)
  mk4.sound_c(51)
  mk4.motion(0x0, 0xffffcccd, 0x28f)
  mk4.tail(native.KAI_GROUND_SKULL_SPAWN)
end

-- Retail script 0x004ed330 in MK4/MK-4.EXE.
function S.kai_rising_skull()
  mk4.prefixed_to(7, 21)
  mk4.sound_c(51)
  mk4.tail(native.KAI_RISING_SKULL_SPAWN)
end

-- Retail script 0x004ed348 in MK4/MK-4.EXE.
function S.kai_rising_skull_spawn()
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.playback_pair(4)
  mk4.forward_to(6)
  mk4.tail(native.KAI_RISING_SKULL_FINISH)
end

-- Retail script 0x004ed3a0 in MK4/MK-4.EXE.
function S.quan_chi_skull()
  mk4.open_clip(0, 23)
  if mk4.fast_start() then
    return S.quan_chi_skull_release_cursor_set_1()
  end
  mk4.prefixed_to(17)
  mk4.tail(native.QUAN_CHI_SCRIPT_SKULL_RELEASE)
end

-- Retail script 0x004ed3c0 in MK4/MK-4.EXE.
function S.quan_chi_skull_release()
  mk4.sound_c(56)
  mk4.forward_to(23)
  mk4.playback_pair(19)
  mk4.forward_to(23)
  mk4.playback_pair(19)
  mk4.forward_to(23)
  mk4.playback_pair(19)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed3e4 in MK4/MK-4.EXE.
function S.quan_chi_skull_release_cursor_set_1()
  mk4.cursor_set(5)
  mk4.double_to(17)
  mk4.tail(native.QUAN_CHI_SCRIPT_SKULL_RELEASE)
end

-- Retail script 0x004ed408 in MK4/MK-4.EXE.
function S.fujin_crossbow_lp_shot_recovery()
  mk4.forward_to(8)
  mk4.playback_pair(4)
  mk4.forward_to(8)
  mk4.playback_pair(4)
  mk4.forward_to(8)
  mk4.playback_pair(4)
  mk4.tail(native.FUJIN_BOW_FINISH)
end

-- Retail script 0x004ed448 in MK4/MK-4.EXE.
function S.scorpion_spear()
  mk4.sound_c(5)
  if mk4.fast_start() then
    return S.scorpion_spear_open_clip_1()
  end
  mk4.open_clip(0, 14)
  mk4.double_to(20)
  mk4.spawn(0x43, native.SPEAR_TASK)
  mk4.double_to(30)
  mk4.action_followup()
  mk4.wait(45)
  return mk4.ready()
end

-- Retail script 0x004ed47c in MK4/MK-4.EXE.
function S.scorpion_spear_open_clip_1()
  mk4.open_clip(9, 14)
  mk4.double_count(16)
  mk4.spawn(0x43, native.SPEAR_TASK)
  mk4.double_to(30)
  mk4.action_followup()
  mk4.wait(45)
  return mk4.ready()
end

-- Retail script 0x004ed4a8 in MK4/MK-4.EXE.
function S.scorpion_spear_return()
  mk4.forward_count(19)
  mk4.native_return(71)
  mk4.sound_indexed(5)
  mk4.forward()
  mk4.tail(native.SCORPION_SCRIPT_SPEAR_RETURN)
  return S.sonya_ring()
end

-- Retail script 0x004ed4c0 in MK4/MK-4.EXE.
function S.sonya_ring()
  mk4.sound_indexed(17)
  mk4.open_clip(0, 15)
  if mk4.fast_start() then
    return S.sonya_ring_double_to_1()
  end
  mk4.double_to(10)
  mk4.forward_to(15)
  return S.sonya_ring_spawn_1()
end

-- Retail script 0x004ed4dc in MK4/MK-4.EXE.
function S.sonya_ring_spawn_1()
  mk4.spawn(0x44, native.SONYA_RING_OBJECT)
  mk4.spawn(0x26, native.SONYA_SCRIPT_RING_FLASH)
  mk4.sound_c(4)
  mk4.forward_to(19)
  mk4.action_followup()
  mk4.playback_pair(15)
  mk4.forward_to(19)
  mk4.playback_pair(15)
  mk4.forward_to(19)
  mk4.playback_pair(15)
  mk4.forward_to(19)
  mk4.playback_pair(0)
  return mk4.ready()
end

-- Retail script 0x004ed520 in MK4/MK-4.EXE.
function S.sonya_ring_double_to_1()
  mk4.double_to(15)
  return S.sonya_ring_spawn_1()
end

-- Retail script 0x004ed588 in MK4/MK-4.EXE.
function S.sub_zero_ice()
  mk4.sound_indexed(4)
  mk4.open_clip(0, 16)
  if mk4.fast_start() then
    return S.shared_double_to_entry()
  end
  mk4.forward_to(15)
  return S.sub_zero_ice_sound_c_1()
end

-- Retail script 0x004ed5a0 in MK4/MK-4.EXE.
function S.sub_zero_ice_sound_c_1()
  mk4.sound_c(0)
  mk4.spawn(0x42, native.ICE_PROJECTILE_TASK)
  mk4.wait(1)
  mk4.action_followup()
  for _ = 1, 2 do
    mk4.forward_to(23)
    mk4.playback_pair(18)
  end
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed5f0 in MK4/MK-4.EXE.
function S.raiden_lightning()
  mk4.sound_indexed(20)
  mk4.sound_c(43)
  if mk4.fast_start() then
    return S.raiden_lightning_open_clip_1()
  end
  mk4.play_clip(4, 17)
  mk4.forward()
  return S.raiden_lightning_spawn_1()
end

-- Retail script 0x004ed614 in MK4/MK-4.EXE.
function S.raiden_lightning_open_clip_1()
  mk4.open_clip(0, 17)
  mk4.double_to(4)
  mk4.double_count(10)
  return S.raiden_lightning_spawn_1()
end

-- Retail script 0x004ed624 in MK4/MK-4.EXE.
function S.raiden_lightning_spawn_1()
  mk4.spawn(0x41, native.RAIDEN_LIGHTNING_OBJECT)
  mk4.wait(1)
  mk4.action_followup()
  for _ = 1, 3 do
    mk4.clip(17)
  end
  mk4.clip_alt(4, 17)
  return mk4.ready()
end

-- Retail script 0x004ed678 in MK4/MK-4.EXE.
function S.liu_kang_low_fireball_low()
  mk4.open_clip(0, 19)
  mk4.double_to(20)
  mk4.sound_a(33)
  mk4.sound_indexed(2)
  mk4.spawn(0x40, native.LIU_LOW_FIREBALL_OBJECT)
  mk4.forward_to(23)
  mk4.action_followup()
  mk4.playback_pair(18)
  mk4.forward_to(23)
  mk4.playback_pair(18)
  mk4.forward_to(23)
  mk4.playback_pair(18)
  mk4.forward_to(23)
  mk4.playback_pair(0)
  return mk4.ready()
end

-- Retail script 0x004ed6c0 in MK4/MK-4.EXE.
function S.liu_kang_high_fireball_high()
  mk4.open_clip(0, 18)
  mk4.double_to(20)
  mk4.sound_a(33)
  mk4.sound_indexed(2)
  mk4.spawn(0x40, native.LIU_HIGH_FIREBALL_OBJECT)
  mk4.forward_to(27)
  mk4.action_followup()
  mk4.forward_to(18)
  mk4.forward_to(27)
  mk4.playback_pair(0)
  return mk4.ready()
end

-- Retail script 0x004ed6f8 in MK4/MK-4.EXE.
function S.liu_kang_air_fireball_air()
  mk4.clip(20)
  mk4.sound_a(33)
  mk4.sound_indexed(3)
  mk4.spawn(0x40, native.LIU_AIR_FIREBALL_OBJECT)
  mk4.wait(16)
  mk4.clip_operation(20)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_AIR_RETURN)
end

-- Retail script 0x004ed788 in MK4/MK-4.EXE.
function S.fujin_crossbow_followup_short_shot()
  mk4.forward_to(8)
  return mk4.finish()
end

-- Retail script 0x004ed790 in MK4/MK-4.EXE.
function S.goro_fireball()
  mk4.open_clip(0, 592)
  mk4.forward_to(14)
  mk4.sound_a(33)
  mk4.spawn(0x53, native.GORO_FIREBALL_SPAWN)
  mk4.playback_pair(12)
  mk4.forward_to(14)
  mk4.playback_pair(12)
  mk4.forward_to(14)
  mk4.playback_pair(12)
  mk4.forward_to(14)
  mk4.playback_pair(12)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ed7f0 in MK4/MK-4.EXE.
function S.shared_open_clip_entry()
  mk4.open_clip(0, 27)
  mk4.double_to(17)
  return S.jarek_blade_spawn_1()
end

-- Retail script 0x004ed818 in MK4/MK-4.EXE.
function S.shared_double_to_entry()
  mk4.double_to(15)
  return S.sub_zero_ice_sound_c_1()
end

return S

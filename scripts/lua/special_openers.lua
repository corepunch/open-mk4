-- Special openers.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004e5b58 in MK4/MK-4.EXE.
function S.fire_script_startup()
  mk4.open_clip(0, 448)
  mk4.cursor_set(1)
  mk4.forward_to(4)
  mk4.call(native.SCORPION_SCRIPT_FIRE_DECOR)
  mk4.forward_to(8)
  mk4.call(native.SCORPION_SCRIPT_FIRE_FLAME)
  mk4.forward_to(11)
  mk4.tail(native.FIRE_CONTACT)
end

-- Retail script 0x004e5b98 in MK4/MK-4.EXE.
function S.fire_script_recovery()
  mk4.forward_to(30)
  mk4.tail(native.FIRE_RECOVERY)
end

-- Retail script 0x004e5ba8 in MK4/MK-4.EXE.
function S.groin_punch_groin_recover()
  mk4.backward()
  return mk4.ready()
end

-- Retail script 0x004e5bb0 in MK4/MK-4.EXE.
function S.reptile_invisibility()
  mk4.play_clip(24, 12)
  mk4.sound_c(75)
  mk4.sound_c(74)
  mk4.tail(native.REPTILE_SCRIPT_INVISIBILITY)
end

-- Retail script 0x004e5be0 in MK4/MK-4.EXE.
function S.kai_second_victim_fall()
  mk4.prefixed_to(20, 206)
  mk4.sound_special()
  mk4.forward_to(90)
  mk4.sound_special()
  mk4.forward()
  mk4.park()
end

-- Retail script 0x004e5c00 in MK4/MK-4.EXE.
function S.fujin_wind_slam()
  mk4.native_return(97)
  mk4.prefixed_to(6, 99)
  mk4.forward()
  mk4.damage(0x147a)
  return mk4.ready()
end

-- Retail script 0x004e5c20 in MK4/MK-4.EXE.
function S.fujin_wind_recovery()
  mk4.forward()
  mk4.playback_pair(34)
  mk4.forward_to(40)
  mk4.playback_pair(34)
  mk4.forward_to(40)
  mk4.playback_pair(30)
  return mk4.ready()
end

-- Retail script 0x004e5c3c in MK4/MK-4.EXE.
function S.fujin_wind_recovery_open_clip_1()
  mk4.open_clip(0, 98)
  mk4.double_to(16)
  return S.fujin_wind_query_d_1()
end

-- Retail script 0x004e5c50 in MK4/MK-4.EXE.
function S.fujin_wind()
  mk4.sound_c(63)
  if mk4.fast_start() then
    return S.fujin_wind_recovery_open_clip_1()
  end
  mk4.prefixed_to(16, 98)
  return S.fujin_wind_query_d_1()
end

-- Retail script 0x004e5c64 in MK4/MK-4.EXE.
function S.fujin_wind_query_d_1()
  mk4.query_d(8)
  mk4.tail(native.FUJIN_WIND_RECOVERY)
  return S.jax_high_kick_recovery()
end

return S

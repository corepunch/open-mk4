-- Fire, invisibility and wind entry/recovery streams.
-- Translated from scripts/mk4/code/special_openers.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: scorpion.breathe_fire (attacker).
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

function S.fire_script_recovery()
  mk4.forward_to(30)
  mk4.tail(native.FIRE_RECOVERY)
end

-- Catalog: cage.groin_punch (owner reverse recovery).
function S.groin_punch_groin_recover()
  mk4.backward()
  return mk4.ready()
end

-- Catalog: reptile.invisibility (toggle startup).
function S.reptile_invisibility()
  mk4.play_clip(24, 12)
  mk4.sound_c(75)
  mk4.sound_c(74)
  mk4.tail(native.REPTILE_SCRIPT_INVISIBILITY)
end

-- Catalog: kai.fatality_2 (victim fall).
function S.kai_second_victim_fall()
  mk4.prefixed_to(20, 206)
  mk4.sound_special()
  mk4.forward_to(90)
  mk4.sound_special()
  mk4.forward()
  mk4.park()
end

-- Catalog: fujin.wind_slam (slam owner).
function S.fujin_wind_slam()
  mk4.native_return(97)
  mk4.prefixed_to(6, 99)
  mk4.forward()
  mk4.damage(0x147a)
  return mk4.ready()
end

-- Catalog: fujin.wind (caster recovery).
function S.fujin_wind_recovery()
  mk4.forward()
  mk4.playback_pair(34)
  mk4.forward_to(40)
  mk4.playback_pair(34)
  mk4.forward_to(40)
  mk4.playback_pair(30)
  return mk4.ready()
end

function S.fujin_wind_recovery_open_clip_1()
  mk4.open_clip(0, 98)
  mk4.double_to(16)
  return S.fujin_wind_query_d_1()
end

-- Catalog: fujin.wind (wind caster).
function S.fujin_wind()
  mk4.sound_c(63)
  if mk4.fast_start() then
    return S.fujin_wind_recovery_open_clip_1()
  end
  mk4.prefixed_to(16, 98)
  return S.fujin_wind_query_d_1()
end

function S.fujin_wind_query_d_1()
  mk4.query_d(8)
  mk4.tail(native.FUJIN_WIND_RECOVERY)
  return S.jax_high_kick_recovery()
end

return S

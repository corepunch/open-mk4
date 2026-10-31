-- Boss attacks.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004dfb60 in MK4/MK-4.EXE.
function S.goro_special_punch()
  mk4.prefixed_to(14, 599)
  mk4.tail(native.GORO_SCRIPT_PUNCH_TAIL)
  return S.goro_special_kick()
end

-- Retail script 0x004dfb70 in MK4/MK-4.EXE.
function S.goro_special_kick()
  mk4.prefixed_to(8, 598)
  mk4.tail(native.GORO_SCRIPT_KICK_TAIL)
end

-- Retail script 0x004dfbb8 in MK4/MK-4.EXE.
function S.goro_leap_stomp()
  mk4.open_clip(0, 601)
  mk4.double_to(23)
  mk4.tail(native.GORO_SCRIPT_LEAP_CONTINUE)
end

-- Retail script 0x004dfbd0 in MK4/MK-4.EXE.
function S.goro_leap_stomp_stomp_hit()
  mk4.handler(native.GORO_STOMP_HOLD_SEPARATION)
  mk4.prefixed_sample(46)
  mk4.native_context()
  mk4.forward()
  mk4.prefixed_to(25, 602)
  mk4.sound_special()
  mk4.forward()
  mk4.prefixed_to(25)
  mk4.sound_special()
  mk4.forward()
  mk4.prefixed_to(8)
  mk4.motion(0xffffe148, 0xffffe148, 0x28f)
  mk4.forward_to(25)
  mk4.native_context()
  mk4.call(native.DISTURB_RESTING_WEAPONS)
  mk4.sound_special()
  mk4.landing()
  mk4.forward()
  mk4.tail(native.GORO_SCRIPT_STOMP_RETURN)
  return S.goro_leap_stomp_miss_landing()
end

-- Retail script 0x004dfc38 in MK4/MK-4.EXE.
function S.goro_leap_stomp_miss_landing()
  mk4.sound_special()
  mk4.forward()
  mk4.prefixed_finish(603)
  return mk4.ready()
end

-- Retail script 0x004dfc50 in MK4/MK-4.EXE.
function S.quake_jump()
  mk4.prefixed_to(21, 602)
  mk4.tail(native.GORO_SCRIPT_QUAKE_LAND)
  return S.goro_quake_land()
end

-- Retail script 0x004dfc60 in MK4/MK-4.EXE.
function S.goro_quake_land()
  mk4.sound_special()
  mk4.forward()
  return mk4.ready()
end

return S

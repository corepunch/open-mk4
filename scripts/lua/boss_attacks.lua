-- Boss attack and landing streams.
-- Translated from scripts/mk4/code/boss_attacks.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: goro.special_punch (attacker).
function S.goro_special_punch()
  mk4.prefixed_to(14, 599)
  mk4.tail(native.GORO_SCRIPT_PUNCH_TAIL)
  return S.goro_special_kick()
end

-- Catalog: goro.special_kick (attacker).
function S.goro_special_kick()
  mk4.prefixed_to(8, 598)
  mk4.tail(native.GORO_SCRIPT_KICK_TAIL)
end

-- Catalog: goro.leap_stomp (rise).
function S.goro_leap_stomp()
  mk4.open_clip(0, 601)
  mk4.double_to(23)
  mk4.tail(native.GORO_SCRIPT_LEAP_CONTINUE)
end

-- Catalog: goro.leap_stomp (stomp hit).
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

-- Catalog: goro.leap_stomp (miss landing).
function S.goro_leap_stomp_miss_landing()
  mk4.sound_special()
  mk4.forward()
  mk4.prefixed_finish(603)
  return mk4.ready()
end

-- Catalog: goro.quake_stomp (jump).
function S.quake_jump()
  mk4.prefixed_to(21, 602)
  mk4.tail(native.GORO_SCRIPT_QUAKE_LAND)
  return S.goro_quake_land()
end

-- Catalog: goro.quake_stomp (landing).
function S.goro_quake_land()
  mk4.sound_special()
  mk4.forward()
  return mk4.ready()
end

return S

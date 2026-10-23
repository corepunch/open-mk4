-- Weapon attack playback and contact recovery.
-- Translated from scripts/mk4/code/weapon_attacks.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: jarek.weapon_back_hp (upper); cage.weapon_upper (upper strike).
function S.jarek_weapon_back_hp_upper()
  mk4.open_clip(0, 226)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.CAGE_WEAPON_UPPER_CONTACT)
end

-- Catalog: kai.draw_weapon (back strike).
function S.kai_draw_weapon_back_strike()
  mk4.open_clip(0, 226)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.KAI_WEAPON_BACK_STRIKE_CONTACT)
end

-- Catalog: reptile.weapon_back_hp (flurry startup).
function S.reptile_weapon_back_hp_flurry_startup()
  mk4.open_clip(0, 248)
  mk4.forward_to(8)
  mk4.tail(native.REPTILE_WEAPON_FLURRY)
end

-- Catalog: liu_kang.sword_hp (repeat program).
function S.liu_kang_sword_hp_repeat_program()
  mk4.attack_selector(0x30f)
  mk4.schedule_motion(6, 6, 0x11eb)
  mk4.open_clip(0, 233)
  mk4.double_to(9)
  mk4.sound_c(29)
  mk4.double_to(18)
  return S.liu_kang_sword_hp_repeat_program_prefixed_sample_1()
end

function S.liu_kang_sword_hp_repeat_program_prefixed_sample_1()
  mk4.prefixed_sample(0, 216)
  mk4.weapon_query(15)
  if mk4.predicate_flags(native.LIU_KANG_LIU_SCRIPT_SWORD_GATE) then
    return S.liu_kang_sword_hp_repeat_program_action_recovery_1()
  end
  mk4.schedule_motion(6, 6, 0x11eb)
  mk4.sound_c(31)
  mk4.forward_to(10)
  mk4.weapon_query(15)
  if mk4.predicate_flags(native.LIU_KANG_LIU_SCRIPT_SWORD_GATE) then
    return S.shared_action_recovery_entry()
  end
  mk4.schedule_motion(6, 6, 0x11eb)
  mk4.sound_c(29)
  mk4.forward_to(21)
  return S.liu_kang_sword_hp_repeat_program_prefixed_sample_1()
end

function S.liu_kang_sword_hp_repeat_program_action_recovery_1()
  mk4.action_recovery()
  mk4.forward_count(5)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_SWORD_BLEND)
end

-- Catalog: sonya.weapon_back_hp (attacker); kitana.weapon_back_hp (attacker).
function S.sonya_weapon_back_hp_attacker()
  mk4.open_clip(0, 228)
  mk4.forward_to(6)
  mk4.sound_c(30)
  mk4.forward_to(14)
  mk4.tail(native.LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT)
end

function S.sword_back_high_punch_hit_recovery()
  mk4.forward_to(24)
  mk4.playback_pair(15)
  mk4.forward_to(24)
  mk4.playback_pair(15)
  mk4.forward()
  return mk4.ready()
end

function S.sword_back_high_punch_block_recovery()
  mk4.forward()
  return mk4.ready()
end

-- Catalog: sonya.weapon_back_lp (attacker); liu_kang.sword_back_lp (attacker); cage.weapon_back_lp (back LP) (also shared by other catalog entries).
function S.sword_attack()
  mk4.open_clip(0, 227)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.CAGE_WEAPON_BACK_LP_CONTACT)
end

-- Catalog: scorpion.sword_back_lp (attacker); shinnok.weapon_followup (follow-up HP).
function S.sword_back_lp_script()
  mk4.play_clip(9, 237)
  mk4.sound_c(34)
  mk4.speed(0x1999)
  mk4.forward_count(8)
  mk4.weapon_query(2)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.forward_count(1)
  mk4.weapon_query(2)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.forward_count(1)
  mk4.weapon_query(2)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.landing()
  mk4.forward_count(2)
  mk4.wait(4)
  mk4.action_recovery()
  mk4.landing()
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

-- Catalog: scorpion.sword_back_hp (attacker).
function S.sword_back_hp_script()
  mk4.play_clip(8, 238)
  mk4.sound_c(34)
  mk4.forward_count(8)
  mk4.speed(0x1999)
  mk4.forward_to(25)
  mk4.weapon_query(3)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.forward_count(1)
  mk4.weapon_query(3)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.forward_count(1)
  mk4.weapon_query(3)
  if mk4.contact() then
    return S.sword_back_hp_script_action_recovery_1()
  end
  mk4.action_recovery()
  mk4.landing()
  mk4.forward_count(2)
  mk4.sound_c(19)
  mk4.wait(4)
  return S.sword_back_hp_script_action_recovery_1()
end

function S.sword_back_hp_script_action_recovery_1()
  mk4.action_recovery()
  mk4.landing()
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

function S.shared_action_recovery_entry()
  mk4.action_recovery()
  mk4.forward()
  return mk4.ready()
end

return S

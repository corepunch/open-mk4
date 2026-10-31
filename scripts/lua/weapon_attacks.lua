-- Weapon attacks.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004ec4f0 in MK4/MK-4.EXE.
function S.jarek_weapon_back_hp_upper()
  mk4.open_clip(0, 226)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.CAGE_WEAPON_UPPER_CONTACT)
end

-- Retail script 0x004ec5a8 in MK4/MK-4.EXE.
function S.kai_draw_weapon_back_strike()
  mk4.open_clip(0, 226)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.KAI_WEAPON_BACK_STRIKE_CONTACT)
end

-- Retail script 0x004ec640 in MK4/MK-4.EXE.
function S.reptile_weapon_back_hp_flurry_startup()
  mk4.open_clip(0, 248)
  mk4.forward_to(8)
  mk4.tail(native.REPTILE_WEAPON_FLURRY)
end

-- Retail script 0x004ec680 in MK4/MK-4.EXE.
function S.liu_kang_sword_hp_repeat_program()
  mk4.attack_selector(0x30f)
  mk4.schedule_motion(6, 6, 0x11eb)
  mk4.open_clip(0, 233)
  mk4.double_to(9)
  mk4.sound_c(29)
  mk4.double_to(18)
  return S.liu_kang_sword_hp_repeat_program_prefixed_sample_1()
end

-- Retail script 0x004ec6a4 in MK4/MK-4.EXE.
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

-- Retail script 0x004ec6f4 in MK4/MK-4.EXE.
function S.liu_kang_sword_hp_repeat_program_action_recovery_1()
  mk4.action_recovery()
  mk4.forward_count(5)
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_SWORD_BLEND)
end

-- Retail script 0x004ec7a0 in MK4/MK-4.EXE.
function S.sonya_weapon_back_hp_attacker()
  mk4.open_clip(0, 228)
  mk4.forward_to(6)
  mk4.sound_c(30)
  mk4.forward_to(14)
  mk4.tail(native.LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT)
end

-- Retail script 0x004ec7c0 in MK4/MK-4.EXE.
function S.sword_back_high_punch_hit_recovery()
  mk4.forward_to(24)
  mk4.playback_pair(15)
  mk4.forward_to(24)
  mk4.playback_pair(15)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ec7d8 in MK4/MK-4.EXE.
function S.sword_back_high_punch_block_recovery()
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004ec7f0 in MK4/MK-4.EXE.
function S.sword_attack()
  mk4.open_clip(0, 227)
  mk4.double_to(16)
  mk4.sound_c(31)
  mk4.double_to(26)
  mk4.forward_to(29)
  mk4.tail(native.CAGE_WEAPON_BACK_LP_CONTACT)
end

-- Retail script 0x004ec8b0 in MK4/MK-4.EXE.
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

-- Retail script 0x004ec918 in MK4/MK-4.EXE.
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

-- Retail script 0x004ec974 in MK4/MK-4.EXE.
function S.sword_back_hp_script_action_recovery_1()
  mk4.action_recovery()
  mk4.landing()
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

-- Retail script 0x004ecba8 in MK4/MK-4.EXE.
function S.shared_action_recovery_entry()
  mk4.action_recovery()
  mk4.forward()
  return mk4.ready()
end

return S

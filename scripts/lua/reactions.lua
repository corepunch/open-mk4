-- Reactions.
-- Generated from MK4/MK-4.EXE by tools/import_mk4_data.py --lua. Do not edit; regenerate with make scripts-export.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004e89a0 in MK4/MK-4.EXE.
function S.guard_airborne_fall()
  mk4.sound_context(5)
  mk4.sound_b(3)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.wait(2)
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.jax_dash_punch_airborne_victim_168()
end

-- Retail script 0x004e89d8 in MK4/MK-4.EXE.
function S.jax_dash_punch_airborne_victim_168()
  mk4.effect(9)
  mk4.sound_b(1)
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.effect(4)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
end

-- Retail script 0x004e8a18 in MK4/MK-4.EXE.
function S.react_kick_air()
  mk4.effect(4)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
end

-- Retail script 0x004e8a48 in MK4/MK-4.EXE.
function S.quan_chi_flip_reaction()
  mk4.sound_b(3)
  mk4.sound_context(11)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
  return S.reaction_fall_return()
end

-- Retail script 0x004e8a78 in MK4/MK-4.EXE.
function S.reaction_fall_return()
  mk4.motion(0xfffff0a4, 0xffffd47b, 0x28f)
  mk4.task_callback()
  mk4.open_clip(0, 131)
  mk4.double_to(13)
  mk4.forward_to(41)
  mk4.native_context()
  mk4.forward_count(4)
  mk4.prefixed_finish(185)
  mk4.tail(native.VM_RETURN)
  return S.impact_reaction()
end

-- Retail script 0x004e8ab8 in MK4/MK-4.EXE.
function S.impact_reaction()
  mk4.effect(12)
  mk4.sound_context(11)
  mk4.sound_b(3)
  mk4.peer_motion(0xfffff0a4, 0xffffd47b, 0x28f)
  mk4.task_callback()
  mk4.open_clip(0, 131)
  mk4.double_to(13)
  mk4.forward_to(41)
  mk4.native_context()
  mk4.forward_count(4)
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e8b00 in MK4/MK-4.EXE.
function S.react_jab_air()
  mk4.effect(6)
  mk4.sound_context(15)
  mk4.motion(0xfffff334, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.play_clip(30, 183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e8b38 in MK4/MK-4.EXE.
function S.sonya_weapon_hp_victim()
  mk4.sound_c(8)
  mk4.sound_b(1)
  mk4.effect(5)
  mk4.clip(136)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.kai_draw_weapon_hp_victim()
end

-- Retail script 0x004e8b78 in MK4/MK-4.EXE.
function S.kai_draw_weapon_hp_victim()
  mk4.sound_b(1)
  mk4.effect(5)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e8bb0 in MK4/MK-4.EXE.
function S.tanya_weapon_reaction_reaction_170_victim()
  mk4.effect(16)
  mk4.sound_context(1)
  return S.kai_draw_weapon_back_victim_spawn_effect_1()
end

-- Retail script 0x004e8bc0 in MK4/MK-4.EXE.
function S.kai_draw_weapon_back_victim()
  mk4.sound_context(6)
  mk4.effect(1)
  return S.kai_draw_weapon_back_victim_spawn_effect_1()
end

-- Retail script 0x004e8bc8 in MK4/MK-4.EXE.
function S.kai_draw_weapon_back_victim_spawn_effect_1()
  mk4.spawn_effect(0)
  mk4.sound_b(1)
  mk4.motion_block(0xfffff5c3, 0xccc, 0x0, 0x0)
  mk4.handler(native.FACE_PEER_ORIENT_GROUP)
  mk4.open_clip(0, 182)
  mk4.native_sound()
  mk4.landing()
  return mk4.ready()
end

-- Retail script 0x004e8c08 in MK4/MK-4.EXE.
function S.jarek_weapon_reaction_knife_victim()
  mk4.effect(1)
  mk4.spawn_effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.speed_negative(0xccc)
  mk4.prefixed_finish(130)
  return mk4.ready()
end

-- Retail script 0x004e8c30 in MK4/MK-4.EXE.
function S.weapon_clip_189_launch_back_getup()
  mk4.effect(2)
  mk4.sound_b(1)
  mk4.sound_context(6)
  mk4.motion(0xfffff99a, 0xffffc000, 0x28f)
  mk4.task_callback()
  mk4.effect(0)
  mk4.open_clip(0, 189)
  mk4.forward_to(9)
  mk4.half_finish()
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
end

-- Retail script 0x004e8c78 in MK4/MK-4.EXE.
function S.reaction_clip_311_short_slide()
  mk4.effect(16)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.clip(311)
  return mk4.ready()
end

-- Retail script 0x004e8c98 in MK4/MK-4.EXE.
function S.reptile_weapon_reaction_reaction_137()
  mk4.effect(8)
  mk4.effect(9)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.open_clip(0, 311)
  mk4.tail(native.REPTILE_WEAPON_STAGGER_TAIL)
end

-- Retail script 0x004e8cc0 in MK4/MK-4.EXE.
function S.reptile_weapon_reaction_reaction_144()
  mk4.effect(0)
  mk4.sound_b(1)
  mk4.sound_context(6)
  mk4.motion(0xfffff70b, 0xffffc000, 0x28f)
  mk4.task_callback()
  mk4.effect(0)
  mk4.open_clip(0, 189)
  mk4.forward_to(9)
  mk4.half_finish()
  mk4.native_context()
  mk4.forward_count(16)
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e8d08 in MK4/MK-4.EXE.
function S.shinnok_weapon_reaction_lp_victim()
  mk4.effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.speed_negative(0xa3d)
  mk4.open_clip(0, 132)
  mk4.double_to(14)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
  return S.cage_weapon_reaction_lp_victim()
end

-- Retail script 0x004e8d38 in MK4/MK-4.EXE.
function S.cage_weapon_reaction_lp_victim()
  mk4.effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.motion(0xfffff852, 0xffffc7af, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
end

-- Retail script 0x004e8d70 in MK4/MK-4.EXE.
function S.weapon_clip_189_launch_roll_getup()
  mk4.effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.motion(0xfffffc29, 0xffffc7af, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
end

-- Retail script 0x004e8da8 in MK4/MK-4.EXE.
function S.reaction_clip_326_short_slide()
  mk4.effect(18)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.clip(326)
  mk4.call(0x433c00)
  return mk4.ready()
end

-- Retail script 0x004e8dd0 in MK4/MK-4.EXE.
function S.goro_special_punch_189_victim()
  mk4.effect(16)
  mk4.spawn_effect(0)
  mk4.sound_b(1)
  mk4.sound_context(2)
  mk4.task_callback()
  mk4.motion_block(0x0, 0x1999, 0xffffdeb9, 0x20c)
  mk4.play_clip(34, 319)
  mk4.native_context()
  mk4.forward_count(8)
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e8e18 in MK4/MK-4.EXE.
function S.raiden_staff_hp_weapon_victim()
  mk4.spawn_effect(0)
  mk4.sound_context(2)
  mk4.effect(0)
  mk4.sound_b(1)
  mk4.speed_negative(0xa3d)
  mk4.open_clip(0, 132)
  mk4.double_to(14)
  mk4.landing()
  mk4.forward_count(10)
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e8e50 in MK4/MK-4.EXE.
function S.goro_special_uppercut_192_victim()
  mk4.effect(3)
  mk4.effect(8)
  mk4.effect(8)
  mk4.sound_b(1)
  mk4.sound_context(0)
  mk4.motion(0x0, 0xffffc7af, 0x20c)
  mk4.tail(native.GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE)
end

-- Retail script 0x004e8e80 in MK4/MK-4.EXE.
function S.goro_uppercut_victim_slide()
  mk4.sound_a(0)
  mk4.effect(0)
  mk4.play_clip(51, 181)
  mk4.native_context()
  mk4.sound_a(23)
  mk4.forward()
  mk4.tail(native.GORO_VICTIM_GETUP)
end

-- Retail script 0x004e8ea8 in MK4/MK-4.EXE.
function S.launch_clip_131_back_getup()
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.motion(0xfffff334, 0xffffdc29, 0x20c)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
end

-- Retail script 0x004e8ee0 in MK4/MK-4.EXE.
function S.reaction_clip_180_slide_hold()
  mk4.effect(8)
  mk4.effect(2)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.speed_negative(0x3333)
  mk4.bind_submit(0, 180)
  mk4.wait(6)
  mk4.clip(180)
  return mk4.ready()
end

-- Retail script 0x004e8f10 in MK4/MK-4.EXE.
function S.launch_clip_131_native_finish_back_getup()
  mk4.effect(8)
  mk4.effect(2)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.motion(0xfffff5c3, 0xffffdc29, 0x24d)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e8f50 in MK4/MK-4.EXE.
function S.sonya_weapon_lp_victim()
  mk4.effect(0)
  mk4.sound_b(1)
  mk4.speed_negative(0x11eb)
  mk4.play_clip(14, 132)
  mk4.native_context()
  mk4.sound_a(4)
  mk4.landing()
  mk4.forward_count(14)
  mk4.tail(native.REACTION_GETUP_BACK)
  return S.weapon_tumble_back_getup()
end

-- Retail script 0x004e8f80 in MK4/MK-4.EXE.
function S.weapon_tumble_back_getup()
  mk4.effect(8)
  mk4.effect(2)
  mk4.spawn_effect(6)
  mk4.sound_b(1)
  mk4.task_callback()
  mk4.motion_block(0xfffff5c3, 0xa3d, 0xffffeb86, 0x1ca)
  mk4.task_callback()
  mk4.handler(native.LANDING_MOTION)
  mk4.clip(142)
  mk4.bind_cursor(0, 141)
  mk4.native_context()
  mk4.clear_handler()
  mk4.open_clip(0, 143)
  mk4.cursor_set(12)
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
end

-- Retail script 0x004e8fe8 in MK4/MK-4.EXE.
function S.grounded_tumble_hold_return()
  mk4.effect(0)
  mk4.spawn_effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.task_callback()
  mk4.motion_block(0xfffff5c3, 0xa3d, 0xffffeb86, 0xde)
  mk4.handler(native.LANDING_MOTION)
  mk4.clip(142)
  mk4.clip(141)
  mk4.play_clip(12, 143)
  mk4.clear_handler()
  mk4.forward()
  mk4.wait(20)
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
  return S.tumble_native_finish_back_getup()
end

-- Retail script 0x004e9048 in MK4/MK-4.EXE.
function S.tumble_native_finish_back_getup()
  mk4.sound_c(8)
  mk4.sound_b(1)
  mk4.task_callback()
  mk4.motion_block(0xfffff5c3, 0xfffff5c3, 0xffffeb86, 0xde)
  mk4.effect(2)
  mk4.effect(8)
  mk4.effect(8)
  mk4.handler(native.LANDING_MOTION)
  mk4.clip(142)
  mk4.clip(141)
  mk4.play_clip(12, 143)
  mk4.clear_handler()
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e90b0 in MK4/MK-4.EXE.
function S.reaction_clip_132_native_finish()
  mk4.effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.speed_negative(0x11eb)
  mk4.play_clip(14, 132)
  mk4.sound_a(4)
  mk4.landing()
  mk4.tail(native.REACTION_GETUP_BACK)
end

-- Retail script 0x004e90e0 in MK4/MK-4.EXE.
function S.stagger_clip_311_hold()
  mk4.speed_negative(0x2666)
  mk4.sound_context(17)
  mk4.sound_b(3)
  mk4.bind_submit(4, 311)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9110 in MK4/MK-4.EXE.
function S.sonya_crouch_hk_victim()
  mk4.speed_negative(0x3333)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

-- Retail script 0x004e9130 in MK4/MK-4.EXE.
function S.stagger_clip_134_friction()
  mk4.speed_negative(0x1eb8)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

-- Retail script 0x004e9150 in MK4/MK-4.EXE.
function S.stagger_clip_134_friction_impact()
  mk4.sound_b(9)
  mk4.speed_negative(0x1eb8)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

-- Retail script 0x004e9170 in MK4/MK-4.EXE.
function S.crouch_stagger_clip_254()
  mk4.speed_negative(0x1eb8)
  mk4.sound_b(9)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.prefixed_finish(254)
  mk4.wait(4)
  mk4.playback_pair(0)
  mk4.tail(native.CROUCH_RETURN)
end

-- Retail script 0x004e91a0 in MK4/MK-4.EXE.
function S.rear_hit_launch_clip_129()
  mk4.sound_a(22)
  mk4.motion(0xffffd99a, 0xffffcccd, 0x418)
  mk4.tail(native.REACTION_CLIP_129_STARTUP)
end

-- Retail script 0x004e91c0 in MK4/MK-4.EXE.
function S.reaction_clip_129_startup()
  mk4.play_clip(34, 129)
  mk4.tail(native.REACTION_CLIP_129_WAIT_LANDING)
  return S.reaction_clip_129_wait_landing()
end

-- Retail script 0x004e91d0 in MK4/MK-4.EXE.
function S.reaction_clip_129_wait_landing()
  mk4.native_context()
  mk4.sound_a(23)
  mk4.landing()
  return S.reaction_clip_129_wait_landing_tail_1()
end

-- Retail script 0x004e91dc in MK4/MK-4.EXE.
function S.reaction_clip_129_wait_landing_tail_1()
  mk4.tail(native.REACTION_CLIP_129_GETUP)
  return S.reaction_clip_129_wait_landing_tail_1_operand_1()
end

function S.reaction_clip_129_wait_landing_tail_1_operand_1()
  mk4.forward()
  mk4.wait(16)
  mk4.call(native.FACE_PEER_ORIENT_GROUP)
  mk4.tail(native.GETUP_BACK)
end

-- Retail script 0x004e9208 in MK4/MK-4.EXE.
function S.kick_hit_standing_stagger()
  mk4.effect(4)
  mk4.sound_b(3)
  mk4.sound_context(1)
  mk4.speed_negative(0x2666)
  mk4.bind_submit(4, 311)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9238 in MK4/MK-4.EXE.
function S.kick_hit_crouching_stagger()
  mk4.no_guard()
  mk4.effect(4)
  mk4.spawn_effect(1)
  mk4.sound_b(3)
  mk4.bind_submit(4, 311)
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9260 in MK4/MK-4.EXE.
function S.stagger_clip_134_shake()
  mk4.no_guard()
  mk4.speed_negative(0x51e)
  mk4.spawn_effect(1)
  mk4.sound_context(3)
  mk4.sound_b(2)
  mk4.open_clip(0, 134)
  mk4.forward_count(6)
  mk4.sound_indexed(10)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9308 in MK4/MK-4.EXE.
function S.kai_handstand_reaction_standing_stagger()
  mk4.effect(10)
  mk4.sound_b(2)
  mk4.sound_context(3)
  mk4.speed_negative(0x2666)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.open_clip(0, 134)
  mk4.forward_count(6)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.playback_native()
  mk4.wait(2)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9360 in MK4/MK-4.EXE.
function S.crouch_reaction_script()
  mk4.sound_a(20)
  mk4.speed_negative(0x2b85)
  mk4.prefixed_to(4, 254)
  mk4.playback_pair(0)
  mk4.landing()
  mk4.tail(native.CROUCH_RETURN)
end

-- Retail script 0x004e9388 in MK4/MK-4.EXE.
function S.knockdown_stagger_clip_311()
  mk4.effect(4)
  mk4.no_guard()
  mk4.spawn_effect(1)
  mk4.bind_submit(4, 311)
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e93a8 in MK4/MK-4.EXE.
function S.forward_fall_grounded()
  mk4.effect(11)
  mk4.sound_b(3)
  mk4.peer_motion(0xffffe667, 0x0, 0x0)
  mk4.clip(186)
  mk4.sound_special()
  mk4.landing()
  mk4.tail(native.GETUP_FRONT)
  return S.weapon_clip_132_directional_fall()
end

-- Retail script 0x004e93d8 in MK4/MK-4.EXE.
function S.weapon_clip_132_directional_fall()
  mk4.sound_b(1)
  mk4.effect(3)
  mk4.effect(8)
  mk4.effect(8)
  if mk4.airborne() then
    return S.weapon_clip_132_directional_fall_task_callback_1()
  end
  mk4.peer_motion(0xffffe667, 0x0, 0x0)
  return S.weapon_clip_132_directional_fall_task_callback_1()
end

-- Retail script 0x004e9400 in MK4/MK-4.EXE.
function S.weapon_clip_132_directional_fall_task_callback_1()
  mk4.task_callback()
  mk4.peer_motion(0xffffe667, 0x666, 0x147)
  mk4.play_clip(13, 132)
  mk4.native_context()
  mk4.tail(0x482f90)
end

-- Retail script 0x004e9430 in MK4/MK-4.EXE.
function S.forward_fall_airborne()
  mk4.effect(10)
  mk4.motion(0xffffe667, 0x0, 0x28f)
  mk4.play_clip(14, 186)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.GETUP_FRONT)
end

-- Retail script 0x004e9460 in MK4/MK-4.EXE.
function S.shinnok_weapon_reaction_sweep_victim()
  mk4.sound_b(0)
  mk4.motion_block(0xfffff0a4, 0xfffff334, 0x0, 0x0)
  mk4.clip(195)
  mk4.sound_special()
  mk4.landing()
  mk4.tail(native.SHINNOK_WEAPON_SWEEP_GETUP)
end

-- Retail script 0x004e9498 in MK4/MK-4.EXE.
function S.shinnok_weapon_sweep_getup()
  if mk4.down() then
    return S.shared_clip_entry()
  end
  mk4.open_clip(0, 197)
  mk4.tail(native.SHINNOK_WEAPON_SWEEP_FINISH)
  return S.sweep_victim_recovery()
end

-- Retail script 0x004e94b0 in MK4/MK-4.EXE.
function S.sweep_victim_recovery()
  mk4.sound_b(0)
  mk4.motion_block(0xfffff0a4, 0xccc, 0x0, 0x0)
  mk4.play_clip(24, 192)
  mk4.sound_special()
  mk4.landing()
  mk4.forward()
  if mk4.down() then
    return S.sweep_victim_recovery_clip_1()
  end
  mk4.open_clip(0, 194)
  mk4.tail(native.SHINNOK_WEAPON_SWEEP_FINISH)
  return S.sweep_victim_recovery_clip_1()
end

-- Retail script 0x004e94f8 in MK4/MK-4.EXE.
function S.sweep_victim_recovery_clip_1()
  mk4.clip(193)
  mk4.tail(native.CROUCH_RETURN)
  return S.tumble_camera_side_positive()
end

-- Retail script 0x004e9508 in MK4/MK-4.EXE.
function S.tumble_camera_side_positive()
  mk4.sound_context(1)
  mk4.spawn_effect(0)
  mk4.sound_b(3)
  mk4.effect(0)
  mk4.task_callback()
  mk4.motion_block(0xfffff334, 0xccc, 0xffffeb86, 0xde)
  mk4.handler(native.LANDING_MOTION)
  mk4.clip(142)
  mk4.clip(141)
  mk4.play_clip(12, 143)
  mk4.clear_handler()
  mk4.forward()
  mk4.wait(8)
  mk4.tail(native.GETUP_BACK)
  return S.tumble_camera_side_negative()
end

-- Retail script 0x004e9568 in MK4/MK-4.EXE.
function S.tumble_camera_side_negative()
  mk4.sound_context(1)
  mk4.spawn_effect(0)
  mk4.sound_b(3)
  mk4.effect(0)
  mk4.task_callback()
  mk4.motion_block(0xfffff334, 0xfffff334, 0xffffeb86, 0xde)
  mk4.handler(native.LANDING_MOTION)
  mk4.clip(142)
  mk4.clip(141)
  mk4.play_clip(12, 143)
  mk4.clear_handler()
  mk4.forward()
  mk4.wait(8)
  mk4.tail(native.GETUP_BACK)
  return S.goro_quake_victim()
end

-- Retail script 0x004e95c8 in MK4/MK-4.EXE.
function S.goro_quake_victim()
  mk4.sound_b(1)
  mk4.speed_negative(0x1999)
  mk4.open_clip(0, 182)
  mk4.double_to(38)
  mk4.landing()
  mk4.double_to(50)
  return mk4.ready()
end

-- Retail script 0x004e95f0 in MK4/MK-4.EXE.
function S.reptile_dash_victim()
  mk4.sound_b(0)
  mk4.motion(0xa3d, 0xffffcf5d, 0x28f)
  mk4.task_callback()
  mk4.play_clip(39, 179)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.SUB_ZERO_LP_THROW_VICTIM_GETUP)
  return S.reptile_reaction_40()
end

-- Retail script 0x004e9620 in MK4/MK-4.EXE.
function S.reptile_reaction_40()
  mk4.effect(17)
  mk4.spawn_effect(0)
  mk4.sound_context(1)
  mk4.motion(0xffffeb86, 0xffffe667, 0x24d)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.jarek_ball_victim()
end

-- Retail script 0x004e9658 in MK4/MK-4.EXE.
function S.jarek_ball_victim()
  mk4.sound_context(0)
  mk4.spawn_effect(6)
  mk4.motion(0xffffe667, 0xffffe667, 0x24d)
  return S.jarek_ball_victim_task_callback_1()
end

-- Retail script 0x004e9670 in MK4/MK-4.EXE.
function S.jarek_ball_victim_task_callback_1()
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e9690 in MK4/MK-4.EXE.
function S.launch_clip_131_shake_impact()
  mk4.sound_context(2)
  mk4.spawn_effect(6)
  mk4.sound_b(3)
  mk4.motion(0xffffe667, 0xffffdc29, 0x24d)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e96c8 in MK4/MK-4.EXE.
function S.fall_reaction_effects()
  mk4.sound_context(6)
  mk4.spawn_effect(0)
  mk4.effect(8)
  mk4.tail(native.REACTION_FALL)
end

-- Retail script 0x004e96e0 in MK4/MK-4.EXE.
function S.reaction_fall()
  mk4.effect(12)
  mk4.sound_context(0)
  mk4.sound_b(3)
  mk4.motion(0xffffe667, 0xffffdc29, 0x24d)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.tail(native.REACTION_FINISH)
end

-- Retail script 0x004e9718 in MK4/MK-4.EXE.
function S.cage_shadow_victim()
  mk4.effect(17)
  mk4.sound_b(3)
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.motion(0xffffe667, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e9758 in MK4/MK-4.EXE.
function S.uppercut_victim_recovery()
  mk4.sound_b(1)
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.effect(0)
  mk4.effect(8)
  mk4.motion(0xfffff852, 0xffffcccd, 0x20c)
  mk4.play_clip(53, 128)
  mk4.native_context()
  mk4.sound_a(23)
  mk4.sound_special()
  mk4.tail(native.UPPERCUT_RECOVER)
  return S.jab_hit_short_recovery()
end

-- Retail script 0x004e9798 in MK4/MK-4.EXE.
function S.jab_hit_short_recovery()
  mk4.effect(6)
  mk4.open_clip(0, 311)
  mk4.cursor_set(4)
  mk4.forward_to(12)
  mk4.prefixed_sample(14)
  mk4.wait(1)
  mk4.prefixed_sample(16)
  mk4.wait(1)
  mk4.prefixed_sample(18)
  mk4.wait(1)
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

-- Retail script 0x004e97d0 in MK4/MK-4.EXE.
function S.jab_hit_damage_stagger()
  mk4.speed_negative(0x1999)
  mk4.open_clip(0, 182)
  mk4.double_to(38)
  mk4.landing()
  mk4.double_to(50)
  return mk4.ready()
end

-- Retail script 0x004e97f8 in MK4/MK-4.EXE.
function S.stagger_entry_2()
  mk4.sound_b(1)
  mk4.speed_negative(0x3333)
  mk4.open_clip(0, 182)
  mk4.tail(native.STAGGER_TAIL)
end

-- Retail script 0x004e9818 in MK4/MK-4.EXE.
function S.quan_chi_weapon_steal_grounded_victim()
  mk4.speed_negative(0xccc)
  mk4.prefixed_to(25, 182)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
  return S.quan_chi_weapon_steal_airborne_victim()
end

-- Retail script 0x004e9838 in MK4/MK-4.EXE.
function S.quan_chi_weapon_steal_airborne_victim()
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.wait(2)
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.quan_chi_stomp_reaction()
end

-- Retail script 0x004e9868 in MK4/MK-4.EXE.
function S.quan_chi_stomp_reaction()
  mk4.sound_context(13)
  mk4.sound_b(1)
  mk4.prefixed_finish(178)
  mk4.tail(native.QUAN_CHI_SCRIPT_STOMP_VICTIM)
  return S.slam_victim()
end

-- Retail script 0x004e9880 in MK4/MK-4.EXE.
function S.slam_victim()
  mk4.speed_negative(0xffffeb86)
  mk4.sound_special()
  mk4.prefixed_finish(175)
  mk4.wait(8)
  return mk4.ready()
end

-- Retail script 0x004e98a0 in MK4/MK-4.EXE.
function S.air_victim_flip()
  mk4.prefixed_sample(0, 198)
  mk4.motion(0x0, 0x1999, 0x28f)
  mk4.native_context()
  mk4.call(native.TOUCHABLE)
  mk4.call(native.DISARM_SELF)
  mk4.sound_special()
  mk4.prefixed_to(12, 198)
  mk4.speed_negative(0xccc)
  mk4.tail(native.AIR_FLIP_GETUP)
  return S.air_victim_lift()
end

-- Retail script 0x004e98e8 in MK4/MK-4.EXE.
function S.air_victim_lift()
  mk4.motion(0xffffe667, 0xffffd99a, 0x28f)
  mk4.task_callback()
  mk4.prefixed_to(35, 140)
  mk4.native_context()
  mk4.tail(native.AIR_LIFT_GETUP)
  return S.air_victim_default()
end

-- Retail script 0x004e9910 in MK4/MK-4.EXE.
function S.air_victim_default()
  mk4.motion(0x0, 0x3333, 0x28f)
  mk4.native_context()
  mk4.sound_special()
  mk4.forward()
  mk4.wait(4)
  mk4.tail(native.MMOVE_AIR_GETUP)
  return S.scorpion_teleport_punch_grounded_victim()
end

-- Retail script 0x004e9938 in MK4/MK-4.EXE.
function S.scorpion_teleport_punch_grounded_victim()
  mk4.effect(9)
  mk4.effect(8)
  mk4.spawn_effect(0)
  mk4.sound_context(1)
  mk4.sound_b(3)
  mk4.speed_negative(0x2666)
  mk4.bind_submit(4, 311)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.tail(native.SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL)
end

-- Retail script 0x004e9970 in MK4/MK-4.EXE.
function S.scorpion_teleport_victim_tail()
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9980 in MK4/MK-4.EXE.
function S.teleport_punch_airborne_victim()
  mk4.effect(9)
  mk4.effect(8)
  mk4.sound_context(1)
  mk4.sound_b(3)
  mk4.motion(0xfffff852, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e99c0 in MK4/MK-4.EXE.
function S.victim_disarm_fall()
  mk4.forward_count(8)
  mk4.speed_negative(0x147a)
  mk4.forward_count(23)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.THROW_VICTIM_STOP_AND_RETURN)
  return S.victim_friction_impact()
end

-- Retail script 0x004e99e0 in MK4/MK-4.EXE.
function S.victim_friction_impact()
  mk4.prefixed_finish(168)
  return mk4.ready()
end

-- Retail script 0x004e99f0 in MK4/MK-4.EXE.
function S.victim_saved_motion()
  mk4.spawn_effect(0)
  mk4.prefixed_to(11, 177)
  mk4.call(native.RESTORE_MOTION)
  mk4.forward_to(34)
  mk4.sound_special()
  mk4.landing()
  mk4.forward()
  mk4.wait(6)
  mk4.tail(native.JAX_GROUND_POUND_CONTROLLER_RETURN)
  return S.victim_ground_turn()
end

-- Retail script 0x004e9a20 in MK4/MK-4.EXE.
function S.victim_ground_turn()
  mk4.speed_negative(0x2666)
  mk4.spawn_effect(6)
  mk4.sound_a(3)
  mk4.prefixed_finish(170)
  mk4.call(native.IMPACT_VOICE)
  mk4.prefixed_finish(138)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.prefixed_finish(139)
  mk4.wait(10)
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e9a68 in MK4/MK-4.EXE.
function S.sonya_leg_grab_victim_recovery()
  mk4.motion(0x1c28, 0xffffe667, 0x24d)
  mk4.sound_special()
  mk4.task_callback()
  mk4.forward()
  mk4.tail(native.SONYA_SCRIPT_LEG_VICTIM_END)
end

-- Retail script 0x004e9a90 in MK4/MK-4.EXE.
function S.liu_kang_lk_throw_victim()
  mk4.sound_a(37)
  mk4.forward_to(35)
  for _ = 1, 3 do
    mk4.playback_pair(31)
    mk4.forward_to(35)
  end
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL)
end

-- Retail script 0x004e9ad0 in MK4/MK-4.EXE.
function S.raiden_torpedo_ground_release()
  mk4.speed_negative(0x4ccc)
  mk4.clip(182)
  return mk4.ready()
end

-- Retail script 0x004e9ae8 in MK4/MK-4.EXE.
function S.raiden_torpedo_air_release()
  mk4.motion(0xfffff334, 0xfffff0a4, 0x312)
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.wait(2)
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.lp_victim_slide_release()
end

-- Retail script 0x004e9b18 in MK4/MK-4.EXE.
function S.lp_victim_slide_release()
  mk4.prefixed_to(5, 171)
  mk4.speed_negative(0x1999)
  mk4.forward_to(24)
  mk4.landing()
  mk4.forward()
  mk4.tail(0x485a70)
end

-- Retail script 0x004e9b40 in MK4/MK-4.EXE.
function S.lp_victim_sliding_impact_release()
  mk4.speed_negative(0x2666)
  mk4.prefixed_to(12, 172)
  mk4.sound_special()
  mk4.forward_count(6)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.LP_SLIDING_IMPACT_FLIP_GETUP)
  return S.lp_victim_impact_release()
end

-- Retail script 0x004e9b68 in MK4/MK-4.EXE.
function S.lp_victim_impact_release()
  mk4.prefixed_to(17, 174)
  mk4.sound_special()
  mk4.forward()
  mk4.landing()
  mk4.tail(native.GETUP_ROLL_FLIP)
end

-- Retail script 0x004e9b88 in MK4/MK-4.EXE.
function S.lp_victim_two_speed_slide_release()
  mk4.prefixed_to(4, 176)
  mk4.speed_negative(0x3333)
  mk4.forward_count(8)
  mk4.speed_negative(0x1999)
  mk4.forward_count(8)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.LP_TWO_SPEED_SLIDE_DOUBLE_FLIP_ROLL_GETUP)
end

-- Retail script 0x004e9c28 in MK4/MK-4.EXE.
function S.hit_clip_324_recovery()
  mk4.effect(9)
  mk4.sound_a(20)
  mk4.clip(324)
  return mk4.ready()
end

-- Retail script 0x004e9c40 in MK4/MK-4.EXE.
function S.hit_clip_325_recovery()
  mk4.effect(9)
  mk4.sound_a(20)
  mk4.clip(325)
  return mk4.ready()
end

-- Retail script 0x004e9c58 in MK4/MK-4.EXE.
function S.launch_clip_131_no_impact()
  mk4.motion(0xffffe667, 0xffffdc29, 0x24d)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9c80 in MK4/MK-4.EXE.
function S.grounded_hit_clip_132_back_getup()
  mk4.sound_b(1)
  mk4.speed_negative(0xa3d)
  mk4.open_clip(0, 132)
  mk4.double_to(14)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
  return S.launch_clip_179_flip_getup()
end

-- Retail script 0x004e9ca8 in MK4/MK-4.EXE.
function S.launch_clip_179_flip_getup()
  mk4.sound_b(3)
  mk4.motion(0x51e, 0xffffb334, 0x353)
  mk4.task_callback()
  mk4.play_clip(39, 179)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.SUB_ZERO_LP_THROW_VICTIM_GETUP)
  return S.quan_chi_skull_reaction()
end

-- Retail script 0x004e9cd8 in MK4/MK-4.EXE.
function S.quan_chi_skull_reaction()
  mk4.sound_c(47)
  mk4.sound_b(3)
  mk4.speed_negative(0x11eb)
  mk4.open_clip(0, 182)
  mk4.native_sound()
  mk4.landing()
  return mk4.ready()
end

-- Retail script 0x004e9d00 in MK4/MK-4.EXE.
function S.scorpion_spear_victim_hit()
  mk4.effect(5)
  mk4.sound_b(3)
  mk4.sound_context(10)
  mk4.play_clip(5, 199)
  mk4.tail(native.SHAKE_STUN)
end

-- Retail script 0x004e9d20 in MK4/MK-4.EXE.
function S.guard_grounded_stagger()
  mk4.speed_negative(0xccc)
  mk4.prefixed_finish(182)
  return mk4.ready()
end

-- Retail script 0x004e9d38 in MK4/MK-4.EXE.
function S.airborne_thaw_script()
  mk4.gravity(0x147)
  mk4.native_context()
  mk4.tail(native.THAW_LANDING)
end

-- Retail script 0x004e9d50 in MK4/MK-4.EXE.
function S.raiden_lightning_victim()
  mk4.sound_a(45)
  mk4.speed_negative(0x5999)
  mk4.play_clip(27, 127)
  mk4.clear_handler()
  mk4.speed_negative(0xffffeb86)
  mk4.forward_count(5)
  mk4.landing()
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9d88 in MK4/MK-4.EXE.
function S.reaction_clip_127_friction()
  mk4.sound_a(38)
  mk4.speed_negative(0x5999)
  mk4.play_clip(27, 127)
  mk4.clear_handler()
  mk4.speed_negative(0xffffeb86)
  mk4.forward_count(5)
  mk4.landing()
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9db8 in MK4/MK-4.EXE.
function S.tanya_weapon_reaction_reaction_169_victim()
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.effect(4)
  mk4.spawn_effect(0)
  mk4.sound_context(1)
  mk4.sound_b(1)
  mk4.speed_negative(0x4000)
  mk4.clip(130)
  return mk4.ready()
end

-- Retail script 0x004e9de8 in MK4/MK-4.EXE.
function S.sonya_ring_victim()
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.sound_b(1)
  mk4.speed_negative(0x4000)
  mk4.clip(130)
  return mk4.ready()
end

-- Retail script 0x004e9e10 in MK4/MK-4.EXE.
function S.fujin_bolt_victim()
  mk4.speed_negative(0x2666)
  mk4.bind_submit(4, 311)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.wait(4)
  mk4.forward()
  return mk4.ready()
end

-- Retail script 0x004e9e38 in MK4/MK-4.EXE.
function S.jax_dash_punch_grounded_victim_168()
  mk4.effect(9)
  mk4.sound_b(1)
  mk4.speed_negative(0x6666)
  mk4.clip(130)
  return mk4.ready()
end

-- Retail script 0x004e9e68 in MK4/MK-4.EXE.
function S.block_unarmed_pose_cycle()
  mk4.bind_submit(3, 321)
  mk4.wait(1)
  mk4.bind_submit(5, 321)
  mk4.wait(1)
  mk4.bind_submit(7, 321)
  mk4.wait(1)
  mk4.bind_submit(9, 321)
  mk4.wait(1)
  mk4.bind_submit(7, 321)
  mk4.wait(1)
  mk4.bind_submit(5, 321)
  mk4.wait(1)
  mk4.bind_submit(3, 321)
  mk4.wait(1)
  mk4.tail(native.BLOCK_CYCLE)
end

-- Retail script 0x004e9ec8 in MK4/MK-4.EXE.
function S.block_armed_recovery()
  mk4.prefixed_finish(247)
  mk4.playback_pair(0)
  mk4.landing()
  mk4.tail(native.STANDING_BLOCK_HOLD)
  return S.lk_thrown_release()
end

-- Retail script 0x004e9ee0 in MK4/MK-4.EXE.
function S.lk_thrown_release()
  mk4.cursor_set(45)
  mk4.playback_pair(41)
  mk4.forward_to(46)
  mk4.playback_pair(41)
  mk4.forward_to(46)
  mk4.playback_pair(41)
  mk4.forward_to(46)
  mk4.tail(native.SUB_ZERO_SCRIPT_LK_VICTIM_RISE)
end

-- Retail script 0x004e9f08 in MK4/MK-4.EXE.
function S.snare_reaction()
  mk4.open_clip(6, 199)
  return S.snare_reaction_forward_to_1()
end

-- Retail script 0x004e9f10 in MK4/MK-4.EXE.
function S.snare_reaction_forward_to_1()
  mk4.forward_to(11)
  mk4.playback_pair(7)
  return S.snare_reaction_forward_to_1()
end

-- Retail script 0x004e9f20 in MK4/MK-4.EXE.
function S.spear_arrival()
  mk4.bind_cursor(0, 13)
  mk4.tail(native.SCORPION_SPEAR_STUN_SCRIPT)
end

-- Retail script 0x004e9f48 in MK4/MK-4.EXE.
function S.pair_release()
  mk4.motion(0xfffffae2, 0x0, 0x28f)
  mk4.task_callback()
  mk4.prefixed_finish(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e9f78 in MK4/MK-4.EXE.
function S.groin_punch_groin_victim()
  mk4.speed_negative(0x28f5)
  mk4.spawn_effect(0)
  mk4.sound_context(18)
  mk4.sound_b(3)
  mk4.prefixed_to(14, 126)
  mk4.playback_pair(10)
  mk4.forward_to(14)
  mk4.playback_pair(10)
  mk4.forward_to(14)
  mk4.playback_pair(10)
  mk4.forward_to(14)
  return mk4.ready()
end

-- Retail script 0x004e9fb0 in MK4/MK-4.EXE.
function S.orbit_release()
  mk4.motion(0xffffe667, 0xffffd99a, 0x353)
  mk4.task_callback()
  mk4.prefixed_finish(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004e9fe0 in MK4/MK-4.EXE.
function S.vortex_landed()
  mk4.sound_special()
  return S.shared_clip_entry()
end

-- Retail script 0x004e9ff0 in MK4/MK-4.EXE.
function S.slam_landed()
  mk4.sound_special()
  mk4.wait(4)
  mk4.motion(0x11eb, 0xffffcf5d, 0x28f)
  mk4.task_callback()
  mk4.play_clip(39, 179)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.GETUP_BACK)
end

-- Retail script 0x004ea028 in MK4/MK-4.EXE.
function S.jax_dash_victim_hit()
  mk4.effect(9)
  mk4.sound_context(1)
  mk4.prefixed_finish(311)
  return mk4.ready()
end

-- Retail script 0x004ea040 in MK4/MK-4.EXE.
function S.cage_power_victim()
  mk4.sound_context(3)
  mk4.play_clip(27, 127)
  return mk4.ready()
end

-- Retail script 0x004ea050 in MK4/MK-4.EXE.
function S.fujin_triple_victim()
  mk4.effect(10)
  mk4.sound_context(3)
  mk4.bind_submit(4, 311)
  mk4.wait(13)
  return mk4.ready()
end

-- Retail script 0x004ea068 in MK4/MK-4.EXE.
function S.raiden_staff_hp_staff_victim()
  mk4.sound_b(1)
  mk4.sound_context(2)
  mk4.prefixed_to(5, 205)
  mk4.tail(native.RAIDEN_SCRIPT_STAFF_VICTIM_TAIL)
end

-- Retail script 0x004ea0b8 in MK4/MK-4.EXE.
function S.sonya_cartwheel_victim()
  mk4.sound_context(1)
  mk4.sound_b(3)
  mk4.motion(0xffffe667, 0xffffd47b, 0x28f)
  mk4.task_callback()
  mk4.open_clip(0, 131)
  mk4.double_to(13)
  mk4.forward_to(41)
  mk4.native_context()
  mk4.forward_count(4)
  mk4.tail(native.MMOVE_SIDE_GETUP)
  return S.bicycle_capture()
end

-- Retail script 0x004ea0f8 in MK4/MK-4.EXE.
function S.bicycle_capture()
  mk4.clip(136)
  mk4.tail(native.BICYCLE_CYCLE)
  return S.raiden_torpedo_contact()
end

-- Retail script 0x004ea108 in MK4/MK-4.EXE.
function S.raiden_torpedo_contact()
  mk4.effect(19)
  mk4.handler(native.RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION)
  mk4.prefixed_finish(135)
  return S.raiden_torpedo_contact_playback_pair_1()
end

-- Retail script 0x004ea11c in MK4/MK-4.EXE.
function S.raiden_torpedo_contact_playback_pair_1()
  mk4.playback_pair(4)
  mk4.forward_to(9)
  return S.raiden_torpedo_contact_playback_pair_1()
end

-- Retail script 0x004ea130 in MK4/MK-4.EXE.
function S.sonya_bicycle_victim()
  mk4.effect(8)
  mk4.effect(9)
  mk4.sound_context(2)
  mk4.spawn_effect(0)
  mk4.task_callback()
  mk4.motion(0xffffe667, 0x666, 0x147)
  mk4.clip(186)
  mk4.native_context()
  mk4.sound_special()
  mk4.landing()
  mk4.tail(native.GETUP_FRONT)
  return S.kai_handstand_reaction_knockdown()
end

-- Retail script 0x004ea170 in MK4/MK-4.EXE.
function S.kai_handstand_reaction_knockdown()
  mk4.effect(4)
  mk4.sound_b(3)
  mk4.sound_context(1)
  mk4.spawn_effect(0)
  mk4.motion(0xffffe667, 0xffffd47b, 0x28f)
  mk4.task_callback()
  mk4.open_clip(0, 131)
  mk4.double_to(13)
  mk4.forward_to(41)
  mk4.native_context()
  mk4.forward_count(4)
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Retail script 0x004ea1c0 in MK4/MK-4.EXE.
function S.kai_handstand_reaction_rear_stagger()
  mk4.effect(10)
  mk4.sound_b(2)
  mk4.sound_context(3)
  mk4.speed_negative(0x3333)
  mk4.open_clip(0, 127)
  mk4.forward_count(20)
  mk4.tail(native.KAI_HANDSTAND_VICTIM_TAIL)
  return S.mmove_side_getup()
end

-- Retail script 0x004ea1e8 in MK4/MK-4.EXE.
function S.mmove_side_getup()
  mk4.open_clip(0, 131)
  mk4.cursor_set(45)
  mk4.forward()
  mk4.park()
end

-- Retail script 0x004ea240 in MK4/MK-4.EXE.
function S.kai_second_victim()
  mk4.effect(0)
  mk4.effect(8)
  mk4.sound_c(105)
  mk4.spawn_effect(6)
  mk4.sound_b(8)
  mk4.sound_c(52)
  mk4.prefixed_to(10, 130)
  mk4.half_finish()
  mk4.native_release(47)
  mk4.tail(native.KAI_SECOND_VICTIM_FALL)
  return S.shinnok_second_victim()
end

-- Retail script 0x004ea270 in MK4/MK-4.EXE.
function S.shinnok_second_victim()
  mk4.prefixed_finish(542)
  mk4.bind_cursor(0, 543)
  mk4.wait(50)
  mk4.sound_b(8)
  mk4.park()
end

-- Retail script 0x004ea290 in MK4/MK-4.EXE.
function S.reptile_fatality_2_victim_melt()
  mk4.bind_cursor(0, 546)
  mk4.speed_negative(0x28f)
  mk4.tail(native.REPTILE_SECOND_MELT)
  return S.fujin_fatality_2_victim_fall()
end

-- Retail script 0x004ea2a8 in MK4/MK-4.EXE.
function S.fujin_fatality_2_victim_fall()
  mk4.prefixed_to(40, 550)
  mk4.sound_special()
  mk4.forward()
  mk4.wait(10)
  mk4.face(0)
  mk4.scene_event(63)
  mk4.tail(native.SCENE_BODY_FINISH)
end

-- Retail script 0x004ea2d8 in MK4/MK-4.EXE.
function S.fujin_second_victim()
  mk4.speed_negative(0x28f)
  mk4.bind_cursor(0, 549)
  mk4.wait(90)
  mk4.sound_b(8)
  mk4.tail(native.FUJIN_SECOND_BURST)
  return S.reaction_park_task()
end

-- Retail script 0x004ea2f8 in MK4/MK-4.EXE.
function S.reaction_park_task()
  mk4.park()
end

-- Retail script 0x004ea300 in MK4/MK-4.EXE.
function S.stage_victim()
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.motion_block(0xfffffd71, 0x28f5, 0xffffcccd, 0x0)
  mk4.open_clip(0, 128)
  mk4.double_to(31)
  mk4.tail(native.STAGE_IMPALE)
  return S.stage_impaled()
end

-- Retail script 0x004ea330 in MK4/MK-4.EXE.
function S.stage_impaled()
  mk4.landing()
  mk4.effect(8)
  mk4.effect(8)
  mk4.effect(20)
  mk4.sound_context(7)
  mk4.spawn_effect(6)
  mk4.sound_b(12)
  mk4.wait(128)
  mk4.native_release(41)
  mk4.wait(50)
  mk4.tail(native.STAGE_COMPLETE)
end

-- Retail script 0x004ea398 in MK4/MK-4.EXE.
function S.victim_disarm_raise()
  mk4.speed_negative(0x1c28)
  mk4.sound_b(1)
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.open_clip(0, 536)
  mk4.half_count(7)
  mk4.tail(native.THROW_VICTIM_CONTINUE)
  return S.throw_victim_continue()
end

-- Retail script 0x004ea3c0 in MK4/MK-4.EXE.
function S.throw_victim_continue()
  mk4.forward_to(14)
  mk4.landing()
  mk4.tail(native.THROW_VICTIM_IMPACT)
  return S.throw_victim_impact()
end

-- Retail script 0x004ea3d0 in MK4/MK-4.EXE.
function S.throw_victim_impact()
  mk4.sound_special()
  mk4.native_sound()
  return mk4.ready()
end

-- Retail script 0x004ea3e0 in MK4/MK-4.EXE.
function S.goro_kick_victim()
  mk4.sound_context(0)
  mk4.spawn_effect(6)
  mk4.motion(0xffffe148, 0xffffd47b, 0x24d)
  return S.jarek_ball_victim_task_callback_1()
end

-- Retail script 0x004ea400 in MK4/MK-4.EXE.
function S.goro_stomp_victim()
  mk4.sound_context(13)
  mk4.sound_b(1)
  mk4.prefixed_finish(178)
  mk4.tail(native.GORO_SCRIPT_STOMP_VICTIM_HOLD)
end

-- Retail script 0x004ea4d0 in MK4/MK-4.EXE.
function S.shared_clip_entry()
  mk4.clip(196)
  mk4.tail(native.CROUCH_RETURN)
end

return S

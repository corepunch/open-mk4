-- Shared hit, block, fall, capture and get-up streams.
-- Translated from scripts/mk4/code/reactions.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

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

-- Catalog: jax.dash_punch (airborne victim 168).
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

function S.react_kick_air()
  mk4.effect(4)
  mk4.motion(0xffffee15, 0xffffdc29, 0x28f)
  mk4.task_callback()
  mk4.clip(189)
  mk4.native_context()
  mk4.tail(native.JAX_DASH_PUNCH_ROLL_GETUP)
end

-- Catalog: quan_chi.flip_kick (reaction 160 victim).
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

-- Catalog: sonya.weapon_hp (victim); kitana.weapon_hp (victim).
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

-- Catalog: kai.draw_weapon (HP victim).
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

-- Catalog: tanya.weapon_reaction (reaction 170 victim).
function S.tanya_weapon_reaction_reaction_170_victim()
  mk4.effect(16)
  mk4.sound_context(1)
  return S.kai_draw_weapon_back_victim_spawn_effect_1()
end

-- Catalog: reptile.weapon_reaction (reaction 96); kai.draw_weapon (back victim).
function S.kai_draw_weapon_back_victim()
  mk4.sound_context(6)
  mk4.effect(1)
  return S.kai_draw_weapon_back_victim_spawn_effect_1()
end

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

-- Catalog: jarek.weapon_reaction (knife victim).
function S.jarek_weapon_reaction_knife_victim()
  mk4.effect(1)
  mk4.spawn_effect(0)
  mk4.sound_context(6)
  mk4.sound_b(1)
  mk4.speed_negative(0xccc)
  mk4.prefixed_finish(130)
  return mk4.ready()
end

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

function S.reaction_clip_311_short_slide()
  mk4.effect(16)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.clip(311)
  return mk4.ready()
end

-- Catalog: reptile.weapon_reaction (reaction 137).
function S.reptile_weapon_reaction_reaction_137()
  mk4.effect(8)
  mk4.effect(9)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.open_clip(0, 311)
  mk4.tail(native.REPTILE_WEAPON_STAGGER_TAIL)
end

-- Catalog: reptile.weapon_reaction (reaction 144); cage.weapon_reaction (back HP victim).
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

-- Catalog: shinnok.weapon_reaction (LP victim).
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

-- Catalog: cage.weapon_reaction (LP victim).
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

function S.reaction_clip_326_short_slide()
  mk4.effect(18)
  mk4.sound_context(6)
  mk4.speed_negative(0xccc)
  mk4.clip(326)
  mk4.call(0x433c00)
  return mk4.ready()
end

-- Catalog: goro.special_punch (189 victim).
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

-- Catalog: raiden.staff_hp (weapon victim).
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

-- Catalog: goro.special_uppercut (192 victim).
function S.goro_special_uppercut_192_victim()
  mk4.effect(3)
  mk4.effect(8)
  mk4.effect(8)
  mk4.sound_b(1)
  mk4.sound_context(0)
  mk4.motion(0x0, 0xffffc7af, 0x20c)
  mk4.tail(native.GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE)
end

-- Catalog: goro.special_uppercut (192 victim slide).
function S.goro_uppercut_victim_slide()
  mk4.sound_a(0)
  mk4.effect(0)
  mk4.play_clip(51, 181)
  mk4.native_context()
  mk4.sound_a(23)
  mk4.forward()
  mk4.tail(native.GORO_VICTIM_GETUP)
end

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

-- Catalog: sonya.weapon_lp (victim); kai.draw_weapon (LP victim); kitana.weapon_lp (victim).
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

-- Catalog: sonya.crouch_hk (victim); kitana.crouch_hk (victim).
function S.sonya_crouch_hk_victim()
  mk4.speed_negative(0x3333)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

function S.stagger_clip_134_friction()
  mk4.speed_negative(0x1eb8)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

function S.stagger_clip_134_friction_impact()
  mk4.sound_b(9)
  mk4.speed_negative(0x1eb8)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.clip(134)
  return mk4.ready()
end

function S.crouch_stagger_clip_254()
  mk4.speed_negative(0x1eb8)
  mk4.sound_b(9)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.prefixed_finish(254)
  mk4.wait(4)
  mk4.playback_pair(0)
  mk4.tail(native.CROUCH_RETURN)
end

function S.rear_hit_launch_clip_129()
  mk4.sound_a(22)
  mk4.motion(0xffffd99a, 0xffffcccd, 0x418)
  mk4.tail(native.REACTION_CLIP_129_STARTUP)
end

function S.reaction_clip_129_startup()
  mk4.play_clip(34, 129)
  mk4.tail(native.REACTION_CLIP_129_WAIT_LANDING)
  return S.reaction_clip_129_wait_landing()
end

function S.reaction_clip_129_wait_landing()
  mk4.native_context()
  mk4.sound_a(23)
  mk4.landing()
  return S.reaction_clip_129_wait_landing_tail_1()
end

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

-- Catalog: kai.handstand_reaction (standing stagger).
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

-- Catalog: kai.handstand_reaction (crouching stagger).
function S.crouch_reaction_script()
  mk4.sound_a(20)
  mk4.speed_negative(0x2b85)
  mk4.prefixed_to(4, 254)
  mk4.playback_pair(0)
  mk4.landing()
  mk4.tail(native.CROUCH_RETURN)
end

function S.knockdown_stagger_clip_311()
  mk4.effect(4)
  mk4.no_guard()
  mk4.spawn_effect(1)
  mk4.bind_submit(4, 311)
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

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

function S.weapon_clip_132_directional_fall_task_callback_1()
  mk4.task_callback()
  mk4.peer_motion(0xffffe667, 0x666, 0x147)
  mk4.play_clip(13, 132)
  mk4.native_context()
  mk4.tail(0x482f90)
end

function S.forward_fall_airborne()
  mk4.effect(10)
  mk4.motion(0xffffe667, 0x0, 0x28f)
  mk4.play_clip(14, 186)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.GETUP_FRONT)
end

-- Catalog: shinnok.weapon_reaction (sweep victim).
function S.shinnok_weapon_reaction_sweep_victim()
  mk4.sound_b(0)
  mk4.motion_block(0xfffff0a4, 0xfffff334, 0x0, 0x0)
  mk4.clip(195)
  mk4.sound_special()
  mk4.landing()
  mk4.tail(native.SHINNOK_WEAPON_SWEEP_GETUP)
end

-- Catalog: shinnok.weapon_reaction (sweep getup).
function S.shinnok_weapon_sweep_getup()
  if mk4.down() then
    return S.shared_clip_entry()
  end
  mk4.open_clip(0, 197)
  mk4.tail(native.SHINNOK_WEAPON_SWEEP_FINISH)
  return S.sweep_victim_recovery()
end

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

function S.sweep_victim_recovery_clip_1()
  mk4.clip(193)
  mk4.tail(native.CROUCH_RETURN)
  return S.tumble_camera_side_positive()
end

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

-- Catalog: goro.quake_stomp (136 victim).
function S.goro_quake_victim()
  mk4.sound_b(1)
  mk4.speed_negative(0x1999)
  mk4.open_clip(0, 182)
  mk4.double_to(38)
  mk4.landing()
  mk4.double_to(50)
  return mk4.ready()
end

-- Catalog: reptile.input_47a690 (victim reaction 158).
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

-- Catalog: reptile.reaction_40 (reaction continuation).
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

-- Catalog: jarek.cannonball (victim knockdown).
function S.jarek_ball_victim()
  mk4.sound_context(0)
  mk4.spawn_effect(6)
  mk4.motion(0xffffe667, 0xffffe667, 0x24d)
  return S.jarek_ball_victim_task_callback_1()
end

function S.jarek_ball_victim_task_callback_1()
  mk4.task_callback()
  mk4.clip(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

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

function S.fall_reaction_effects()
  mk4.sound_context(6)
  mk4.spawn_effect(0)
  mk4.effect(8)
  mk4.tail(native.REACTION_FALL)
end

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

-- Catalog: cage.cpu_kick (victim).
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

function S.jab_hit_damage_stagger()
  mk4.speed_negative(0x1999)
  mk4.open_clip(0, 182)
  mk4.double_to(38)
  mk4.landing()
  mk4.double_to(50)
  return mk4.ready()
end

-- Catalog: scorpion.breathe_fire (grounded victim); reiko.star (grounded victim); reptile.acid_spit (acid victim).
function S.stagger_entry_2()
  mk4.sound_b(1)
  mk4.speed_negative(0x3333)
  mk4.open_clip(0, 182)
  mk4.tail(native.STAGGER_TAIL)
end

-- Catalog: quan_chi.weapon_steal (grounded victim).
function S.quan_chi_weapon_steal_grounded_victim()
  mk4.speed_negative(0xccc)
  mk4.prefixed_to(25, 182)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
  return S.quan_chi_weapon_steal_airborne_victim()
end

-- Catalog: quan_chi.weapon_steal (airborne victim).
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

-- Catalog: quan_chi.teleport_stomp (reaction 106 victim).
function S.quan_chi_stomp_reaction()
  mk4.sound_context(13)
  mk4.sound_b(1)
  mk4.prefixed_finish(178)
  mk4.tail(native.QUAN_CHI_SCRIPT_STOMP_VICTIM)
  return S.slam_victim()
end

-- Catalog: reiko.teleport (slam victim); quan_chi.air_throw (released victim); noob_saibot.teleport (slam victim) (also shared by other catalog entries).
function S.slam_victim()
  mk4.speed_negative(0xffffeb86)
  mk4.sound_special()
  mk4.prefixed_finish(175)
  mk4.wait(8)
  return mk4.ready()
end

-- Catalog: sonya.air_throw (victim release); kitana.air_throw (victim release).
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

-- Catalog: jax.air_throw (victim release).
function S.air_victim_lift()
  mk4.motion(0xffffe667, 0xffffd99a, 0x28f)
  mk4.task_callback()
  mk4.prefixed_to(35, 140)
  mk4.native_context()
  mk4.tail(native.AIR_LIFT_GETUP)
  return S.air_victim_default()
end

-- Catalog: scorpion.air_throw (victim release); raiden.air_throw (victim release); sub_zero.air_throw (victim release) (also shared by other catalog entries).
function S.air_victim_default()
  mk4.motion(0x0, 0x3333, 0x28f)
  mk4.native_context()
  mk4.sound_special()
  mk4.forward()
  mk4.wait(4)
  mk4.tail(native.MMOVE_AIR_GETUP)
  return S.scorpion_teleport_punch_grounded_victim()
end

-- Catalog: scorpion.teleport_punch (grounded victim).
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

-- Catalog: scorpion.teleport_punch (victim fall).
function S.scorpion_teleport_victim_tail()
  mk4.wait(13)
  mk4.forward()
  return mk4.ready()
end

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

-- Catalog: raiden.lk_throw (native continuation); jarek.lk_throw (native continuation); jax.lk_throw (native continuation).
function S.victim_disarm_fall()
  mk4.forward_count(8)
  mk4.speed_negative(0x147a)
  mk4.forward_count(23)
  mk4.landing()
  mk4.forward()
  mk4.tail(native.THROW_VICTIM_STOP_AND_RETURN)
  return S.victim_friction_impact()
end

-- Catalog: quan_chi.lk_throw (native continuation).
function S.victim_friction_impact()
  mk4.prefixed_finish(168)
  return mk4.ready()
end

-- Catalog: kai.lk_throw (native continuation).
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

-- Catalog: fujin.lk_throw (native continuation).
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

-- Catalog: sonya.leg_grab (victim recovery); kitana.leg_grab (victim recovery).
function S.sonya_leg_grab_victim_recovery()
  mk4.motion(0x1c28, 0xffffe667, 0x24d)
  mk4.sound_special()
  mk4.task_callback()
  mk4.forward()
  mk4.tail(native.SONYA_SCRIPT_LEG_VICTIM_END)
end

-- Catalog: liu_kang.lk_throw (native continuation).
function S.liu_kang_lk_throw_victim()
  mk4.sound_a(37)
  mk4.forward_to(35)
  for _ = 1, 3 do
    mk4.playback_pair(31)
    mk4.forward_to(35)
  end
  mk4.tail(native.LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL)
end

-- Catalog: raiden.torpedo (ground release); liu_kang.bicycle_kick (released ground victim).
function S.raiden_torpedo_ground_release()
  mk4.speed_negative(0x4ccc)
  mk4.clip(182)
  return mk4.ready()
end

-- Catalog: raiden.torpedo (air release); liu_kang.bicycle_kick (released air victim).
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

function S.lp_victim_slide_release()
  mk4.prefixed_to(5, 171)
  mk4.speed_negative(0x1999)
  mk4.forward_to(24)
  mk4.landing()
  mk4.forward()
  mk4.tail(0x485a70)
end

-- Catalog: raiden.lp_throw (extended victim); reiko.lp_throw (throw victim recovery); noob_saibot.lp_throw (throw victim recovery).
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

-- Catalog: liu_kang.lp_throw (victim release).
function S.lp_victim_impact_release()
  mk4.prefixed_to(17, 174)
  mk4.sound_special()
  mk4.forward()
  mk4.landing()
  mk4.tail(native.GETUP_ROLL_FLIP)
end

-- Catalog: sonya.lp_throw (victim release); kitana.lp_throw (victim release).
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

function S.hit_clip_324_recovery()
  mk4.effect(9)
  mk4.sound_a(20)
  mk4.clip(324)
  return mk4.ready()
end

function S.hit_clip_325_recovery()
  mk4.effect(9)
  mk4.sound_a(20)
  mk4.clip(325)
  return mk4.ready()
end

function S.launch_clip_131_no_impact()
  mk4.motion(0xffffe667, 0xffffdc29, 0x24d)
  mk4.task_callback()
  mk4.play_clip(40, 131)
  mk4.native_context()
  mk4.forward()
  return mk4.ready()
end

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

-- Catalog: quan_chi.skull (reaction 128 victim).
function S.quan_chi_skull_reaction()
  mk4.sound_c(47)
  mk4.sound_b(3)
  mk4.speed_negative(0x11eb)
  mk4.open_clip(0, 182)
  mk4.native_sound()
  mk4.landing()
  return mk4.ready()
end

-- Catalog: scorpion.spear (victim hit).
function S.scorpion_spear_victim_hit()
  mk4.effect(5)
  mk4.sound_b(3)
  mk4.sound_context(10)
  mk4.play_clip(5, 199)
  mk4.tail(native.SHAKE_STUN)
end

function S.guard_grounded_stagger()
  mk4.speed_negative(0xccc)
  mk4.prefixed_finish(182)
  return mk4.ready()
end

function S.airborne_thaw_script()
  mk4.gravity(0x147)
  mk4.native_context()
  mk4.tail(native.THAW_LANDING)
end

-- Catalog: raiden.lightning (victim).
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

-- Catalog: tanya.weapon_reaction (reaction 169 victim).
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

-- Catalog: sonya.ring (victim); liu_kang.fireball_reaction (native C0 victim); kitana.ring (victim).
function S.sonya_ring_victim()
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.sound_b(1)
  mk4.speed_negative(0x4000)
  mk4.clip(130)
  return mk4.ready()
end

-- Catalog: fujin.crossbow_reaction (bolt victim).
function S.fujin_bolt_victim()
  mk4.speed_negative(0x2666)
  mk4.bind_submit(4, 311)
  mk4.handler(native.VELOCITY_FRICTION)
  mk4.wait(4)
  mk4.forward()
  return mk4.ready()
end

-- Catalog: jax.dash_punch (grounded victim 168).
function S.jax_dash_punch_grounded_victim_168()
  mk4.effect(9)
  mk4.sound_b(1)
  mk4.speed_negative(0x6666)
  mk4.clip(130)
  return mk4.ready()
end

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

function S.block_armed_recovery()
  mk4.prefixed_finish(247)
  mk4.playback_pair(0)
  mk4.landing()
  mk4.tail(native.STANDING_BLOCK_HOLD)
  return S.lk_thrown_release()
end

-- Catalog: sub_zero.lk_throw (native continuation); noob_saibot.lk_throw (native continuation).
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

function S.snare_reaction()
  mk4.open_clip(6, 199)
  return S.snare_reaction_forward_to_1()
end

function S.snare_reaction_forward_to_1()
  mk4.forward_to(11)
  mk4.playback_pair(7)
  return S.snare_reaction_forward_to_1()
end

-- Catalog: scorpion.spear (victim stun).
function S.spear_arrival()
  mk4.bind_cursor(0, 13)
  mk4.tail(native.SCORPION_SPEAR_STUN_SCRIPT)
end

function S.pair_release()
  mk4.motion(0xfffffae2, 0x0, 0x28f)
  mk4.task_callback()
  mk4.prefixed_finish(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Catalog: cage.groin_punch (victim stun).
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

-- Catalog: fujin.wind_grab (victim release).
function S.orbit_release()
  mk4.motion(0xffffe667, 0xffffd99a, 0x353)
  mk4.task_callback()
  mk4.prefixed_finish(183)
  mk4.native_context()
  mk4.forward()
  mk4.tail(native.MMOVE_SIDE_GETUP)
end

-- Catalog: fujin.wind_victim (vortex landing).
function S.vortex_landed()
  mk4.sound_special()
  return S.shared_clip_entry()
end

-- Catalog: fujin.wind_slam (victim bounce).
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

-- Catalog: jax.dash_punch (hit victim 157).
function S.jax_dash_victim_hit()
  mk4.effect(9)
  mk4.sound_context(1)
  mk4.prefixed_finish(311)
  return mk4.ready()
end

-- Catalog: fujin.triple_kick (first victim); reiko.axe_back_lp (first victim); jax.axe_back_lp (first victim) (also shared by other catalog entries).
function S.cage_power_victim()
  mk4.sound_context(3)
  mk4.play_clip(27, 127)
  return mk4.ready()
end

-- Catalog: fujin.triple_kick (middle victim).
function S.fujin_triple_victim()
  mk4.effect(10)
  mk4.sound_context(3)
  mk4.bind_submit(4, 311)
  mk4.wait(13)
  return mk4.ready()
end

-- Catalog: raiden.staff_hp (staff victim).
function S.raiden_staff_hp_staff_victim()
  mk4.sound_b(1)
  mk4.sound_context(2)
  mk4.prefixed_to(5, 205)
  mk4.tail(native.RAIDEN_SCRIPT_STAFF_VICTIM_TAIL)
end

-- Catalog: sonya.cartwheel (victim); fujin.triple_kick (final victim); reiko.axe_back_lp (follow-up victim) (also shared by other catalog entries).
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

function S.bicycle_capture()
  mk4.clip(136)
  mk4.tail(native.BICYCLE_CYCLE)
  return S.raiden_torpedo_contact()
end

-- Catalog: raiden.torpedo (held victim).
function S.raiden_torpedo_contact()
  mk4.effect(19)
  mk4.handler(native.RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION)
  mk4.prefixed_finish(135)
  return S.raiden_torpedo_contact_playback_pair_1()
end

function S.raiden_torpedo_contact_playback_pair_1()
  mk4.playback_pair(4)
  mk4.forward_to(9)
  return S.raiden_torpedo_contact_playback_pair_1()
end

-- Catalog: sonya.bicycle (victim); kitana.bicycle (victim).
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

-- Catalog: kai.handstand_reaction (knockdown).
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

-- Catalog: kai.handstand_reaction (rear stagger).
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

function S.mmove_side_getup()
  mk4.open_clip(0, 131)
  mk4.cursor_set(45)
  mk4.forward()
  mk4.park()
end

-- Catalog: kai.fatality_2 (victim reaction 178).
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

-- Catalog: shinnok.fatality_2 (victim).
function S.shinnok_second_victim()
  mk4.prefixed_finish(542)
  mk4.bind_cursor(0, 543)
  mk4.wait(50)
  mk4.sound_b(8)
  mk4.park()
end

-- Catalog: reptile.fatality_2 (victim melt).
function S.reptile_fatality_2_victim_melt()
  mk4.bind_cursor(0, 546)
  mk4.speed_negative(0x28f)
  mk4.tail(native.REPTILE_SECOND_MELT)
  return S.fujin_fatality_2_victim_fall()
end

-- Catalog: fujin.fatality_2 (victim fall); reptile.fatality_2 (victim fall).
function S.fujin_fatality_2_victim_fall()
  mk4.prefixed_to(40, 550)
  mk4.sound_special()
  mk4.forward()
  mk4.wait(10)
  mk4.face(0)
  mk4.scene_event(63)
  mk4.tail(native.SCENE_BODY_FINISH)
end

-- Catalog: fujin.fatality_2 (victim wind hold).
function S.fujin_second_victim()
  mk4.speed_negative(0x28f)
  mk4.bind_cursor(0, 549)
  mk4.wait(90)
  mk4.sound_b(8)
  mk4.tail(native.FUJIN_SECOND_BURST)
  return S.reaction_park_task()
end

function S.reaction_park_task()
  mk4.park()
end

function S.stage_victim()
  mk4.spawn_effect(0)
  mk4.sound_context(0)
  mk4.motion_block(0xfffffd71, 0x28f5, 0xffffcccd, 0x0)
  mk4.open_clip(0, 128)
  mk4.double_to(31)
  mk4.tail(native.STAGE_IMPALE)
  return S.stage_impaled()
end

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

-- Catalog: cage.lk_throw (native continuation).
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

-- Catalog: cage.lk_throw (native continuation).
function S.throw_victim_continue()
  mk4.forward_to(14)
  mk4.landing()
  mk4.tail(native.THROW_VICTIM_IMPACT)
  return S.throw_victim_impact()
end

-- Catalog: cage.lk_throw (native continuation).
function S.throw_victim_impact()
  mk4.sound_special()
  mk4.native_sound()
  return mk4.ready()
end

-- Catalog: goro.special_kick (190 victim).
function S.goro_kick_victim()
  mk4.sound_context(0)
  mk4.spawn_effect(6)
  mk4.motion(0xffffe148, 0xffffd47b, 0x24d)
  return S.jarek_ball_victim_task_callback_1()
end

-- Catalog: goro.leap_stomp (191 victim).
function S.goro_stomp_victim()
  mk4.sound_context(13)
  mk4.sound_b(1)
  mk4.prefixed_finish(178)
  mk4.tail(native.GORO_SCRIPT_STOMP_VICTIM_HOLD)
end

function S.shared_clip_entry()
  mk4.clip(196)
  mk4.tail(native.CROUCH_RETURN)
end

return S

-- Special attack startup, contact branches and recovery.
-- Translated from scripts/mk4/code/special_attacks.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: cage.cpu_kick (attack).
function S.cage_cpu_kick()
  mk4.speed(0x2666)
  mk4.sound_c(81)
  mk4.open_clip(0, 308)
  mk4.double_to(22)
  mk4.tail(native.CAGE_SCRIPT_CPU_KICK_TAIL)
end

-- Catalog: cage.powered_punch (attack).
function S.cage_powered_punch()
  mk4.sound_c(81)
  mk4.open_clip(0, 85)
  mk4.double_to(14)
  mk4.speed(0x2b85)
  mk4.double_to(24)
  mk4.tail(native.CAGE_SCRIPT_POWERED_PUNCH_TAIL)
end

-- Catalog: quan_chi.flip_kick (startup).
function S.quan_chi_flip_kick()
  mk4.sound_c(86)
  mk4.open_clip(0, 105)
  mk4.prefixed_to(10)
  mk4.speed(0x547a)
  mk4.forward_to(15)
  mk4.tail(native.QUAN_CHI_SCRIPT_FLIP_KICK_TAIL)
end

-- Catalog: reptile.input_47a690 (selected stream).
function S.reptile_dash()
  mk4.prefixed_finish(302)
  mk4.bind_cursor(0, 303)
  mk4.speed(0x2e14)
  mk4.tail(native.REPTILE_SCRIPT_DASH_CONTACT)
end

-- Catalog: jax.dash_punch (dash).
function S.jax_dash_punch()
  mk4.sound_indexed(37)
  mk4.sound_c(78)
  mk4.speed(0x2666)
  mk4.open_clip(0, 299)
  mk4.double_to(18)
  mk4.tail(native.JAX_DASH_PUNCH_CONTACT)
end

-- Catalog: reptile.input_47afe0 (selected stream).
function S.reptile_roll()
  mk4.sound_indexed(35)
  mk4.sound_c(70)
  mk4.speed(0x1eb8)
  mk4.prefixed_to(13, 106)
  mk4.query_d(21)
  mk4.forward_to(17)
  mk4.landing()
  mk4.tail(native.REPTILE_SCRIPT_ROLL_TAIL)
end

-- Catalog: jarek.quake (stomp).
function S.jarek_quake()
  mk4.sound_indexed(33)
  mk4.prefixed_to(20, 107)
  mk4.sound_c(68)
  mk4.spawn_effect(3)
  mk4.tail(native.JAREK_QUAKE_TAIL)
end

-- Catalog: jarek.quake (recovery).
function S.jarek_quake_tail()
  mk4.forward()
  return mk4.ready()
end

-- Catalog: sonya.cartwheel (startup); kitana.cartwheel (startup).
function S.sonya_cartwheel()
  mk4.sound_indexed(32)
  mk4.sound_c(6)
  mk4.speed(0x1999)
  mk4.prefixed_to(14, 104)
  mk4.tail(native.SONYA_SCRIPT_CARTWHEEL_TAIL)
  return S.tanya_drill_kick()
end

-- Catalog: tanya.drill_kick (attacker).
function S.tanya_drill_kick()
  mk4.sound_indexed(31)
  mk4.speed(0x23d7)
  mk4.prefixed_to(12, 103)
  mk4.tail(native.TANYA_DRILL_KICK_CONTACT)
end

-- Catalog: kai.input_47bd10 (selected continuation).
function S.kai_aerial_punch()
  mk4.sound_c(60)
  mk4.motion(0x2e14, 0xffffe667, 0x41)
  mk4.prefixed_to(9, 298)
  mk4.tail(native.KAI_AERIAL_PUNCH_CONTACT)
end

-- Catalog: fujin.aerial_dive (startup).
function S.fujin_dive()
  mk4.sound_c(57)
  mk4.motion(0x1eb8, 0xf5c, 0x41)
  mk4.prefixed_finish(58)
  mk4.tail(native.FUJIN_DIVE_CONTACT)
end

-- Catalog: fujin.aerial_dive (recoil).
function S.dive_recoil()
  mk4.landing()
  mk4.wait(8)
  mk4.allow_commands()
  mk4.gravity(0x51e)
  mk4.native_context()
  mk4.tail(native.FUJIN_DIVE_LAND)
  return S.teleport()
end

-- Catalog: reiko.teleport (rise); noob_saibot.teleport (rise).
function S.teleport()
  mk4.sound_c(54)
  mk4.prefixed_to(3, 86)
  mk4.motion(0x0, 0xffffcccd, 0x0)
  mk4.forward()
  mk4.tail(native.TELEPORT_VANISH)
  return S.teleport_slam()
end

-- Catalog: reiko.teleport (slam opener); noob_saibot.teleport (slam opener).
function S.teleport_slam()
  mk4.sound_indexed(24)
  mk4.native_release(23)
  mk4.landing()
  mk4.open_peer_clip(150)
  mk4.open_clip(0, 74)
  mk4.playback_single(18)
  mk4.tail(native.TELEPORT_SLAM_FALL)
end

-- Catalog: reiko.teleport (slam fall); noob_saibot.teleport (slam fall).
function S.teleport_slam_fall()
  mk4.playback_single(27)
  mk4.tail(native.TELEPORT_SLAM_DROP)
end

-- Catalog: kai.input_47c520 (selected continuation).
function S.kai_handstand_enter()
  mk4.sound_indexed(23)
  mk4.prefixed_finish(1)
  mk4.tail(native.KAI_SCRIPT_STAGE_EVENT)
end

-- Catalog: kai.handstand_spin (spin startup).
function S.handstand_spin()
  mk4.prefixed_finish(2)
  mk4.bind_cursor(0, 3)
  mk4.tail(native.KAI_HANDSTAND_SPIN_BEGIN)
end

-- Catalog: kai.input_47cc10 (selected continuation).
function S.handstand_front()
  mk4.sound_a(1)
  mk4.prefixed_to(9, 0)
  mk4.tail(native.KAI_SCRIPT_CONTACT_B10_CONTINUATION)
end

-- Catalog: kai.input_47cc10 (scheduler continuation).
function S.kai_contact_b10_continuation()
  mk4.wait(8)
  mk4.playback_pair(0)
  mk4.tail(native.KAI_SCRIPT_STAGE_EVENT)
  return S.handstand_rear()
end

-- Catalog: kai.handstand_rear (rear startup).
function S.handstand_rear()
  mk4.sound_a(1)
  mk4.prefixed_to(9, 6)
  mk4.tail(native.KAI_HANDSTAND_REAR_CONTACT)
end

-- Catalog: kai.handstand_rear (rear recovery).
function S.kai_handstand_rear_contact()
  mk4.playback_pair(0)
  mk4.tail(native.KAI_SCRIPT_STAGE_EVENT)
end

-- Catalog: kai.handstand_exit (reverse intro).
function S.handstand_exit()
  mk4.sound_indexed(23)
  mk4.clip_operation(1)
  return mk4.ready()
end

-- Catalog: raiden.torpedo (fast startup).
function S.raiden_torpedo_fast_startup()
  mk4.open_clip(0, 94)
  mk4.playback_tail(14)
  return S.raiden_torpedo_ordinary_startup_call_1()
end

-- Catalog: raiden.torpedo (ordinary startup).
function S.raiden_torpedo_ordinary_startup()
  mk4.open_clip(0, 94)
  mk4.double_to(14)
  return S.raiden_torpedo_ordinary_startup_call_1()
end

function S.raiden_torpedo_ordinary_startup_call_1()
  mk4.call(native.RESTORE_MOTION)
  mk4.handler(native.RAIDEN_SCRIPT_TORPEDO_MOTION)
  mk4.sound_a(43)
  mk4.sound_c(11)
  for _ = 1, 29 do
    if mk4.predicate_argument(0, native.CONTACT_QUERY_D) then
      return S.shared_tail_entry_3()
    end
    mk4.call(native.RAIDEN_SCRIPT_TORPEDO_STEP)
    mk4.wait(1)
  end
  mk4.tail(native.RAIDEN_SCRIPT_TORPEDO_MISS)
end

-- Catalog: tanya.spin_kick (blocked rewind); jarek.cannonball (contact rewind); jarek.upward_ball (contact rewind) (also shared by other catalog entries).
function S.power_rewind()
  mk4.clip_operation(301)
  mk4.native_context()
  mk4.clear_handler()
  mk4.tail(native.RECOIL_LAND)
end

-- Catalog: sub_zero.slide (entry).
function S.sub_zero_slide()
  mk4.speed_negative(0xffffd99a)
  mk4.sound_c(15)
  mk4.open_clip(0, 95)
  mk4.tail(native.SUB_ZERO_SLIDE_SCRIPT)
end

-- Catalog: sub_zero.slide (hit).
function S.sub_zero_slide_hit()
  for _ = 1, 4 do
    mk4.playback_tail(10)
    mk4.wait(1)
    mk4.playback_tail(9)
    mk4.wait(1)
    mk4.playback_tail(8)
    mk4.wait(1)
    mk4.playback_tail(9)
    mk4.wait(1)
  end
  mk4.landing()
  mk4.tail(native.SUB_ZERO_SCRIPT_SLIDE_RETURN)
end

-- Catalog: sub_zero.slide (block).
function S.sub_zero_slide_block()
  mk4.no_guard()
  mk4.landing()
  for _ = 1, 6 do
    mk4.playback_tail(10)
    mk4.wait(1)
    mk4.playback_tail(9)
    mk4.wait(1)
    mk4.playback_tail(8)
    mk4.wait(1)
    mk4.playback_tail(9)
    mk4.wait(1)
  end
  mk4.tail(native.SUB_ZERO_SCRIPT_SLIDE_RETURN)
end

-- Catalog: sonya.leg_grab (paired grab); kitana.leg_grab (paired grab).
function S.sonya_leg_grab_paired_grab()
  mk4.native_release(37)
  mk4.sound_roster(1)
  mk4.open_peer_clip(201)
  mk4.playback_single(22)
  mk4.playback_pair_reverse(19)
  mk4.playback_single(22)
  mk4.playback_pair_reverse(19)
  mk4.playback_single(34)
  mk4.native_step(79)
  mk4.tail(native.SONYA_SCRIPT_LEG_GRAB_TAIL)
  return S.sonya_leg_grab_miss_or_block()
end

-- Catalog: sonya.leg_grab (miss or block); kitana.leg_grab (miss or block).
function S.sonya_leg_grab_miss_or_block()
  mk4.forward_to(22)
  mk4.playback_pair(19)
  mk4.forward_to(22)
  mk4.playback_pair(19)
  mk4.forward()
  return mk4.ready()
end

-- Catalog: sonya.square_wave (launch); kitana.square_wave (launch).
function S.sonya_square_wave()
  mk4.speed(0x1999)
  mk4.prefixed_finish(72)
  mk4.sound_indexed(14)
  mk4.bind_cursor(0, 73)
  mk4.motion_block(0x1999, 0x0, 0xffffe667, 0xffffff7d)
  mk4.tail(native.SONYA_SCRIPT_SQUARE_WAVE_TAIL)
end

-- Catalog: liu_kang.bicycle_kick (startup).
function S.liu_kang_bicycle_kick()
  mk4.motion(0x2147, 0xfffff5c3, 0x0)
  mk4.call(native.SAVE_MOTION)
  mk4.play_clip(6, 68)
  mk4.handler(native.LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION)
  mk4.sound_indexed(1)
  mk4.call(native.RESTORE_MOTION)
  mk4.gravity(0xc4)
  mk4.tail(native.LIU_KANG_BICYCLE_SCRIPT)
end

-- Catalog: liu_kang.bicycle_kick (finish).
function S.bicycle_land()
  mk4.clear_handler()
  mk4.clip_alt(6, 68)
  return mk4.ready()
end

-- Catalog: liu_kang.bicycle_kick (blocked).
function S.bicycle_contact_end()
  mk4.motion(0x0, 0xffffeb86, 0xe5)
  mk4.native_context()
  mk4.clear_handler()
  mk4.tail(native.LIU_KANG_BICYCLE_LAND)
  return S.liu_kang_flying_kick()
end

-- Catalog: liu_kang.flying_kick (launch).
function S.liu_kang_flying_kick()
  mk4.sound_indexed(0)
  mk4.open_clip(0, 92)
  mk4.motion(0x23d7, 0xffffe8f6, 0x106)
  mk4.tail(native.LIU_KANG_FLYING_KICK_SCRIPT)
end

-- Catalog: liu_kang.flying_kick (miss).
function S.liu_kang_flying_kick_miss()
  mk4.clip_operation(92)
  mk4.bind_submit(0, 92)
  return mk4.ready()
end

-- Catalog: liu_kang.flying_kick (hit).
function S.liu_kang_flying_kick_hit()
  mk4.landing()
  mk4.speed_negative(0xfffffae2)
  mk4.clip(93)
  mk4.wait(20)
  mk4.gravity(0x51e)
  mk4.clip_operation(92)
  mk4.native_context()
  return mk4.ready()
end

-- Catalog: liu_kang.flying_kick (block).
function S.liu_kang_flying_kick_block()
  mk4.landing()
  mk4.wait(12)
  mk4.landing()
  mk4.wait(12)
  mk4.gravity(0x28f)
  mk4.native_context()
  mk4.tail(native.LIU_KANG_FLYING_KICK_LAND)
end

-- Catalog: fujin.triple_kick (startup).
function S.fujin_triple_kick()
  if mk4.fast_start() then
    return S.fujin_triple_kick_tail_1()
  end
  mk4.prefixed_to(6, 100)
  mk4.speed(0x2b85)
  mk4.forward_to(13)
  mk4.tail(native.FUJIN_TRIPLE_CONTACT)
  return S.fujin_triple_kick_tail_1()
end

function S.fujin_triple_kick_tail_1()
  mk4.tail(native.FUJIN_TRIPLE_FAST)
end

-- Catalog: fujin.triple_kick (fast startup).
function S.fujin_triple_fast()
  mk4.open_clip(0, 100)
  mk4.double_to(6)
  mk4.speed(0x2b85)
  mk4.double_to(13)
  mk4.tail(native.FUJIN_TRIPLE_CONTACT)
end

-- Catalog: scorpion.teleport_punch (attacker).
function S.scorpion_teleport()
  mk4.motion(0xffffd99a, 0xffffe8f6, 0x1ca)
  mk4.open_clip(0, 312)
  mk4.sound_c(35)
  mk4.handler(native.TELEPORT_GROUP_CALLBACK)
  mk4.tail(native.SCORPION_TELEPORT_SCRIPT)
end

-- Catalog: scorpion.teleport_punch (landing); reiko.teleport (blocked landing); quan_chi.teleport_stomp (blocked landing) (also shared by other catalog entries).
function S.teleport_block_return()
  mk4.clip_operation(301)
  mk4.native_context()
  mk4.clear_handler()
  mk4.tail(native.LAND)
  return S.fujin_wind_grab()
end

-- Catalog: fujin.wind_grab (opening).
function S.fujin_wind_grab()
  mk4.sound_indexed(30)
  mk4.sound_c(61)
  mk4.prefixed_finish(10)
  mk4.bind_cursor(0, 11)
  mk4.tail(native.FUJIN_GRAB_CONTROL_START)
  return S.sword_spin_script()
end

-- Catalog: scorpion.sword_spin (startup).
function S.sword_spin_script()
  mk4.prefixed_finish(223)
  mk4.bind_cursor(0, 224)
  mk4.tail(native.SCORPION_SPIN_SCRIPT)
end

-- Catalog: kai.input_478df0 (selected continuation).
function S.kai_lunge_kick()
  mk4.sound_indexed(59)
  mk4.open_clip(0, 551)
  if mk4.fast_start() then
    return S.kai_armed_handstand_enter_cursor_set_1()
  end
  return S.kai_lunge_kick_double_to_1()
end

function S.kai_lunge_kick_double_to_1()
  mk4.double_to(26)
  mk4.sound_c(31)
  mk4.speed(0x2666)
  mk4.forward_to(29)
  mk4.tail(native.KAI_LUNGE_KICK_CONTACT)
end

-- Catalog: kai.input_47cbf0 (selected continuation).
function S.kai_armed_handstand_enter()
  mk4.sound_indexed(23)
  mk4.open_clip(0, 1)
  mk4.native_sound()
  mk4.tail(native.HANDSTAND_FRONT)
  return S.kai_armed_handstand_enter_cursor_set_1()
end

function S.kai_armed_handstand_enter_cursor_set_1()
  mk4.cursor_set(14)
  return S.kai_lunge_kick_double_to_1()
end

function S.shared_tail_entry_3()
  mk4.tail(native.RAIDEN_SCRIPT_TORPEDO_CONTACT)
end

return S

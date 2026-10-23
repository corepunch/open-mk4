-- Normal attacks, air attacks and shared roster selection.
-- Translated from scripts/mk4/code/normal_attacks.asm by tools/mk4_asm_to_lua.py; edit as Lua.
-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Catalog: scorpion.high_kick (recovery); liu_kang.high_kick (recovery); shinnok.high_kick (recovery) (also shared by other catalog entries).
function S.jax_high_kick_recovery()
  mk4.backward_count(2)
  mk4.release_attack()
  mk4.backward()
  return mk4.ready()
end

-- Catalog: sub_zero.high_kick (Sub-Zero attacker).
function S.high_kick_clip_71()
  mk4.attack_selector(0x2)
  mk4.sound_a(1)
  mk4.prefixed_to(10, 71)
  mk4.tail(native.KICK_CONTACT_NARROW)
end

-- Catalog: scorpion.high_kick (attacker); raiden.high_kick (roster selector); sonya.high_kick (roster selector) (also shared by other catalog entries).
function S.high_kick()
  return mk4.select_roster {
    scorpion = S.high_kick_clip_66,
    raiden = S.high_kick_clip_71,
    sonya = S.high_kick_sonya,
    liu_kang = S.high_kick_clip_66,
    sub_zero = S.high_kick_clip_71,
    fujin = S.high_kick_sonya,
    shinnok = S.high_kick_clip_66,
    reiko = S.high_kick_clip_66,
    quan_chi = S.high_kick_clip_71,
    tanya = S.high_kick_clip_66,
    reptile = S.high_kick_clip_66,
    kai = S.high_kick_clip_71,
    jarek = S.high_kick_sonya,
    jax = S.high_kick_clip_66,
    cage = S.high_kick_clip_71,
    goro = S.high_kick_clip_66,
  }
end

function S.high_kick_sonya()
  mk4.attack_selector(0x0)
  mk4.sound_a(1)
  mk4.prefixed_to(10, 65)
  mk4.tail(native.KICK_CONTACT_DEFAULT)
end

-- Catalog: sonya.high_kick (recovery); fujin.high_kick (recovery); jarek.high_kick (recovery) (also shared by other catalog entries).
function S.fujin_high_kick_recovery()
  mk4.backward_count(2)
  mk4.release_attack()
  mk4.tail(native.HIGH_KICK_REVERSE_RETURN)
end

-- Catalog: raiden.low_kick (character-selected attacker); sonya.low_kick (character-selected attacker); fujin.low_kick (character-selected attacker) (also shared by other catalog entries).
function S.low_kick()
  return mk4.select_roster {
    scorpion = S.low_kick_clip_67,
    raiden = S.low_kick_clip_70,
    sonya = S.low_kick_clip_308,
    liu_kang = S.low_kick_clip_67,
    sub_zero = S.low_kick_clip_70,
    fujin = S.low_kick_clip_308,
    shinnok = S.low_kick_clip_67,
    reiko = S.low_kick_clip_67,
    quan_chi = S.low_kick_clip_70,
    tanya = S.low_kick_clip_308,
    reptile = S.low_kick_clip_67,
    kai = S.low_kick_clip_70,
    jarek = S.low_kick_clip_308,
    jax = S.low_kick_clip_67,
    cage = S.low_kick_clip_70,
    goro = S.low_kick_clip_67,
  }
end

-- Catalog: jax.air_throw (paired capture).
function S.air_throw_lift()
  mk4.sound_indexed(54)
  mk4.open_peer_clip(144)
  mk4.open_clip(0, 84)
  mk4.playback_single(20)
  mk4.tail(native.AIR_LIFT_FALL)
  return S.air_lift_fall()
end

-- Catalog: jax.air_throw (falling pair).
function S.air_lift_fall()
  mk4.playback_single(32)
  mk4.sound_a(36)
  mk4.sound_roster(1)
  mk4.spawn_effect(6)
  mk4.playback_single(43)
  mk4.disarm_peer()
  mk4.tail(native.AIR_LIFT_RELEASE)
  return S.air_lift_release()
end

-- Catalog: jax.air_throw (owner release).
function S.air_lift_release()
  mk4.playback_single(48)
  mk4.forward()
  return mk4.ready()
end

-- Catalog: scorpion.air_throw (paired attacker/victim); raiden.air_throw (paired attacker/victim); sub_zero.air_throw (paired attacker/victim) (also shared by other catalog entries).
function S.air_throw()
  mk4.native_release(23)
  mk4.sound_indexed(13)
  mk4.open_peer_clip(202)
  mk4.open_clip(0, 82)
  mk4.playback_single(28)
  mk4.tail(native.AIR_THROW_RELEASE)
end

-- Catalog: scorpion.air_throw (release); raiden.air_throw (release); sub_zero.air_throw (release) (also shared by other catalog entries).
function S.air_throw_release()
  mk4.forward_count(1)
  mk4.motion(0x7ae, 0xffffe667, 0x28f)
  mk4.wait(10)
  mk4.forward()
  mk4.native_context()
  mk4.tail(native.AIR_THROW_LAND)
  return S.air_throw_slam()
end

-- Catalog: quan_chi.air_throw (capture); noob_saibot.air_throw (Noob air-throw opener).
function S.air_throw_slam()
  mk4.landing()
  mk4.native_release(23)
  mk4.sound_indexed(21)
  mk4.open_peer_clip(150)
  mk4.open_clip(0, 74)
  mk4.playback_single(18)
  mk4.tail(native.AIR_SLAM_FALL)
  return S.air_slam_fall()
end

-- Catalog: quan_chi.air_throw (paired fall); noob_saibot.air_throw (Noob pair continuation).
function S.air_slam_fall()
  mk4.playback_single(27)
  mk4.tail(native.NOOB_SAIBOT_AIR_THROW_LANDING_LINK)
end

-- Catalog: quan_chi.air_throw (landing); noob_saibot.air_throw (Noob landing continuation).
function S.quan_chi_air_throw_landing()
  mk4.native_context()
  mk4.tail(native.AIR_SLAM_END)
end

-- Catalog: sonya.air_throw (paired owner); kitana.air_throw (paired owner).
function S.air_throw_flip()
  mk4.native_release(23)
  mk4.sound_indexed(16)
  mk4.open_peer_clip(161)
  mk4.open_clip(0, 77)
  mk4.play_pair(24)
  mk4.tail(native.AIR_FLIP_RELEASE)
end

-- Catalog: scorpion.close_hk (follow-up crouch entry); raiden.close_hk (follow-up crouch entry); sonya.close_hk (follow-up crouch entry) (also shared by other catalog entries).
function S.follow_crouch_hk()
  mk4.speed(0x1999)
  return S.crouch_high_kick_clip_52()
end

-- Catalog: sub_zero.crouch_hk (attacker).
function S.crouch_high_kick_clip_52()
  mk4.attack_selector(0x1a)
  mk4.sound_a(1)
  mk4.play_clip(7, 52)
  mk4.query_b(26)
  mk4.action_recovery()
  mk4.landing()
  mk4.forward()
  for _ = 1, 5 do
    mk4.playback_tail(10)
    mk4.wait(2)
    mk4.playback_tail(9)
    mk4.wait(2)
  end
  mk4.clip_operation(52)
  return mk4.finish()
end

-- Catalog: raiden.crouch_hk (roster selector); sonya.crouch_hk (roster selector); kitana.crouch_hk (roster selector).
function S.crouch_high_kick()
  return mk4.select_roster {
    scorpion = S.crouch_high_kick_clip_54,
    raiden = S.crouch_high_kick_clip_52,
    sonya = S.crouch_high_kick_sonya,
    liu_kang = S.crouch_high_kick_clip_54,
    sub_zero = S.crouch_high_kick_clip_52,
    fujin = S.crouch_high_kick_sonya,
    shinnok = S.crouch_high_kick_clip_54,
    reiko = S.crouch_high_kick_clip_54,
    quan_chi = S.crouch_high_kick_clip_52,
    tanya = S.crouch_high_kick_clip_54,
    reptile = S.crouch_high_kick_clip_52,
    kai = S.crouch_high_kick_clip_52,
    jarek = S.crouch_high_kick_clip_54,
    jax = S.crouch_high_kick_clip_54,
    cage = S.crouch_high_kick_clip_52,
    goro = S.crouch_high_kick_clip_52,
  }
end

function S.crouch_high_kick_sonya()
  mk4.attack_selector(0x18)
  mk4.sound_a(1)
  mk4.play_clip(9, 53)
  mk4.query_b(24)
  mk4.action_recovery()
  mk4.forward()
  for _ = 1, 2 do
    mk4.playback_tail(11)
    mk4.wait(2)
    mk4.playback_tail(10)
    mk4.wait(2)
    mk4.playback_tail(11)
    mk4.wait(2)
    mk4.playback_tail(12)
    mk4.wait(2)
  end
  mk4.clip_operation(53)
  return mk4.finish()
end

-- Catalog: scorpion.crouch_hk (attacker); liu_kang.crouch_hk (attacker).
function S.crouch_high_kick_clip_54()
  mk4.attack_selector(0x19)
  mk4.sound_a(1)
  mk4.play_clip(9, 54)
  mk4.query_b(25)
  mk4.action_recovery()
  mk4.forward()
  return S.crouch_high_kick_clip_54_loop_1()
end

function S.crouch_high_kick_clip_54_loop_1()
  for _ = 1, 2 do
    mk4.playback_tail(11)
    mk4.wait(2)
    mk4.playback_tail(10)
    mk4.wait(2)
    mk4.playback_tail(11)
    mk4.wait(2)
    mk4.playback_tail(12)
    mk4.wait(2)
  end
  mk4.clip_operation(54)
  return mk4.finish()
end

-- Catalog: scorpion.crouch_lk (attacker); raiden.crouch_lk (attacker); sonya.crouch_lk (attacker) (also shared by other catalog entries).
function S.crouch_low_kick()
  mk4.attack_selector(0x17)
  mk4.sound_c(27)
  mk4.open_clip(0, 315)
  mk4.cursor_set(1)
  mk4.forward_to(9)
  mk4.tail(native.CROUCH_LOW_KICK_TAIL)
end

function S.crouch_low_kick_recover()
  mk4.cursor_set(9)
  mk4.playback_pair(1)
  return mk4.finish()
end

-- Catalog: scorpion.close_hk (attacker); raiden.close_hk (attacker); sonya.close_hk (attacker) (also shared by other catalog entries).
function S.knee_script()
  mk4.open_clip(0, 314)
  mk4.playback_tail(5)
  mk4.wait(1)
  mk4.playback_tail(6)
  mk4.wait(1)
  mk4.playback_tail(7)
  mk4.wait(1)
  mk4.playback_tail(8)
  mk4.wait(1)
  mk4.playback_tail(9)
  mk4.action_recovery()
  mk4.tail(native.CLOSE_HIGH_KICK_TAIL)
end

-- Catalog: scorpion.close_hk (miss recovery); raiden.close_hk (miss recovery); sonya.close_hk (miss recovery) (also shared by other catalog entries).
function S.knee_miss_finished()
  mk4.wait(4)
  mk4.backward()
  return mk4.ready()
end

-- Catalog: scorpion.close_hp (inherited animation); raiden.close_hp (inherited animation); sonya.close_hp (inherited animation) (also shared by other catalog entries).
function S.close_high_punch_2()
  mk4.attack_selector(0x110)
  mk4.forward_to(7)
  mk4.tail(native.CLOSE_HIGH_PUNCH_TAIL)
end

-- Catalog: scorpion.air_kick (straight kick); raiden.air_kick (roster selector); sonya.air_kick (roster selector) (also shared by other catalog entries).
function S.air_straight_kick_startup()
  return mk4.select_roster {
    scorpion = S.air_straight_kick_startup_scorpion,
    raiden = S.air_straight_kick_startup_clip_61,
    sonya = S.air_straight_kick_startup_scorpion,
    liu_kang = S.air_straight_kick_startup_clip_61,
    sub_zero = S.air_straight_kick_startup_scorpion,
    fujin = S.air_straight_kick_startup_clip_61,
    shinnok = S.air_straight_kick_startup_scorpion,
    reiko = S.air_straight_kick_startup_clip_61,
    quan_chi = S.air_straight_kick_startup_scorpion,
    tanya = S.air_straight_kick_startup_clip_61,
    reptile = S.air_straight_kick_startup_scorpion,
    kai = S.air_straight_kick_startup_clip_61,
    jarek = S.air_straight_kick_startup_scorpion,
    jax = S.air_straight_kick_startup_clip_61,
    cage = S.air_straight_kick_startup_scorpion,
    goro = S.air_straight_kick_startup_scorpion,
  }
end

function S.air_straight_kick_startup_scorpion()
  mk4.attack_selector(0x28)
  mk4.sound_a(31)
  mk4.clip(327)
  mk4.tail(native.AIR_STRAIGHT_TAIL)
end

-- Catalog: shinnok.air_kick (narrow miss reversal); quan_chi.air_kick (narrow miss reversal); reptile.air_kick (narrow miss reversal) (also shared by other catalog entries).
function S.cage_air_kick_narrow_miss_reversal()
  mk4.clip_operation(327)
  mk4.tail(native.AIR_STRAIGHT_LAND)
  return S.air_punch_contact()
end

function S.air_punch_contact()
  mk4.allow_commands()
  mk4.wait(12)
  mk4.gravity(0x28f)
  mk4.native_context()
  mk4.tail(native.AIR_ATTACK_LAND_OR_FINISH)
end

-- Catalog: scorpion.uppercut (attacker); raiden.uppercut (attacker); sonya.uppercut (attacker) (also shared by other catalog entries).
function S.uppercut()
  mk4.attack_selector(0x102)
  mk4.prefixed_to(5, 293)
  mk4.sound_c(28)
  mk4.tail(native.UPPERCUT_DEFAULT)
end

-- Catalog: scorpion.draw_weapon (draw); raiden.draw_weapon (draw); sonya.draw_weapon (draw) (also shared by other catalog entries).
function S.cage_draw_weapon_draw()
  mk4.sound_variant()
  mk4.play_clip(22, 244)
  mk4.tail(native.WEAPON_DRAW_EQUIP)
end

-- Catalog: scorpion.draw_weapon (return); raiden.draw_weapon (return); sonya.draw_weapon (return) (also shared by other catalog entries).
function S.shinnok_draw_weapon_equip()
  mk4.forward_count(13)
  mk4.sound_c(18)
  mk4.forward()
  mk4.tail(native.RETURN_TO_PLAYER_OR_CPU_CONTROL)
end

-- Catalog: scorpion.jab (attacker); raiden.jab (attacker); sonya.jab (attacker) (also shared by other catalog entries).
function S.jab()
  mk4.call(native.JAB_REPEAT_PREDICATE)
  mk4.sound_a(10)
  mk4.play_clip(8, 294)
  mk4.call(native.JAB_CONTACT)
  mk4.wait(1)
  if mk4.predicate_flags(native.JAB_REPEAT_PREDICATE) then
    return S.cage_jab_repeat_tail_1()
  end
  mk4.sound_a(10)
  mk4.open_clip(0, 295)
  mk4.tail(native.CAGE_JAB_SECOND_PUNCH)
  return S.cage_jab_second_punch()
end

-- Catalog: scorpion.jab (second punch); raiden.jab (second punch); sonya.jab (second punch) (also shared by other catalog entries).
function S.cage_jab_second_punch()
  mk4.cursor_set(0)
  mk4.forward_count(10)
  mk4.call(native.JAB_CONTACT)
  if mk4.predicate_flags(native.JAB_REPEAT_PREDICATE) then
    return S.cage_jab_recovery_action_recovery_1()
  end
  return S.cage_jab_second_punch_sound_a_1()
end

function S.cage_jab_second_punch_sound_a_1()
  mk4.sound_a(10)
  mk4.forward()
  mk4.tail(native.CAGE_JAB_REPEAT)
end

-- Catalog: scorpion.jab (repeat); raiden.jab (repeat); sonya.jab (repeat) (also shared by other catalog entries).
function S.cage_jab_repeat()
  mk4.wait(1)
  mk4.call(native.JAB_CONTACT)
  if mk4.predicate_flags(native.JAB_REPEAT_PREDICATE) then
    return S.cage_jab_repeat_tail_1()
  end
  mk4.sound_a(10)
  mk4.play_clip(10, 295)
  mk4.call(native.JAB_CONTACT)
  if mk4.predicate_flags(native.JAB_REPEAT_PREDICATE) then
    return S.cage_jab_recovery_action_recovery_1()
  end
  return S.cage_jab_second_punch_sound_a_1()
end

function S.cage_jab_repeat_tail_1()
  mk4.tail(native.CAGE_JAB_RECOVERY)
end

-- Catalog: scorpion.jab (recovery); raiden.jab (recovery); sonya.jab (recovery) (also shared by other catalog entries).
function S.cage_jab_recovery()
  mk4.action_recovery()
  mk4.open_clip(9, 294)
  mk4.double_to(17)
  mk4.native_sound()
  return mk4.ready()
end

function S.cage_jab_recovery_action_recovery_1()
  mk4.action_recovery()
  mk4.forward_count(3)
  return mk4.ready()
end

-- Catalog: scorpion.air_diagonal_lk (attacker); raiden.air_diagonal_lk (roster selector); sonya.air_diagonal_lk (roster selector) (also shared by other catalog entries).
function S.air_diagonal_low_kick_startup()
  mk4.sound_a(25)
  return S.air_diagonal_low_kick_startup_select_roster_1()
end

function S.air_diagonal_low_kick_startup_select_roster_1()
  return mk4.select_roster {
    scorpion = S.air_diagonal_low_kick_startup_select_roster_1_operand_16,
    raiden = S.air_straight_kick_startup_clip_61_clip_1,
    sonya = S.air_straight_kick_startup_clip_61_clip_2,
    liu_kang = S.air_diagonal_low_kick_startup_select_roster_1_operand_16,
    sub_zero = S.air_straight_kick_startup_clip_61_clip_1,
    fujin = S.air_straight_kick_startup_clip_61_clip_2,
    shinnok = S.air_diagonal_low_kick_startup_select_roster_1_operand_16,
    reiko = S.air_straight_kick_startup_clip_61_clip_1,
    quan_chi = S.air_straight_kick_startup_clip_61_clip_2,
    tanya = S.air_diagonal_low_kick_startup_select_roster_1_operand_16,
    reptile = S.air_straight_kick_startup_clip_61_clip_1,
    kai = S.air_straight_kick_startup_clip_61_clip_2,
    jarek = S.air_diagonal_low_kick_startup_select_roster_1_operand_16,
    jax = S.air_straight_kick_startup_clip_61_clip_1,
    cage = S.air_straight_kick_startup_clip_61_clip_2,
    goro = S.air_straight_kick_startup_clip_61_clip_2,
  }
end

function S.air_diagonal_low_kick_startup_select_roster_1_operand_16()
  mk4.clip(58)
  mk4.attack_selector(0x1b)
  mk4.tail(native.AIR_LK_TAIL)
end

function S.air_low_kick_clip_58_reverse()
  mk4.clip_operation(58)
  mk4.native_context()
  mk4.tail(native.AIR_LOW_KICK_LAND)
end

function S.air_low_kick_clip_59_reverse()
  mk4.clip_operation(59)
  mk4.native_context()
  mk4.tail(0x46e0d0)
end

function S.air_low_kick_clip_60_reverse()
  mk4.clip_operation(60)
  mk4.native_context()
  mk4.tail(0x46e1b0)
end

function S.air_low_kick_contact_hold()
  mk4.landing()
  mk4.wait(8)
  mk4.allow_commands()
  mk4.gravity(0x51e)
  mk4.native_context()
  mk4.tail(native.AIR_ATTACK_LAND_OR_FINISH)
  return S.air_diagonal_high_kick_startup()
end

-- Catalog: scorpion.air_diagonal_hk (attacker); raiden.air_diagonal_hk (roster selector); sonya.air_diagonal_hk (roster selector) (also shared by other catalog entries).
function S.air_diagonal_high_kick_startup()
  mk4.sound_a(25)
  return S.air_diagonal_high_kick_startup_select_roster_1()
end

function S.air_diagonal_high_kick_startup_select_roster_1()
  return mk4.select_roster {
    scorpion = S.air_diagonal_high_kick_startup_select_roster_1_operand_16,
    raiden = S.air_straight_kick_startup_clip_61_attack_selector_1,
    sonya = S.air_straight_kick_startup_clip_61_attack_selector_2,
    liu_kang = S.air_diagonal_high_kick_startup_select_roster_1_operand_16,
    sub_zero = S.air_straight_kick_startup_clip_61_attack_selector_1,
    fujin = S.air_straight_kick_startup_clip_61_attack_selector_2,
    shinnok = S.air_diagonal_high_kick_startup_select_roster_1_operand_16,
    reiko = S.air_straight_kick_startup_clip_61_attack_selector_1,
    quan_chi = S.air_straight_kick_startup_clip_61_attack_selector_2,
    tanya = S.air_diagonal_high_kick_startup_select_roster_1_operand_16,
    reptile = S.air_straight_kick_startup_clip_61_attack_selector_1,
    kai = S.air_straight_kick_startup_clip_61_attack_selector_2,
    jarek = S.air_diagonal_high_kick_startup_select_roster_1_operand_16,
    jax = S.air_straight_kick_startup_clip_61_attack_selector_1,
    cage = S.air_straight_kick_startup_clip_61_attack_selector_2,
    goro = S.air_straight_kick_startup_clip_61_attack_selector_1,
  }
end

function S.air_diagonal_high_kick_startup_select_roster_1_operand_16()
  mk4.attack_selector(0x1e)
  mk4.tail(native.AIR_DIAGONAL_TAIL)
end

function S.air_diagonal_kick_clip_55_reverse()
  mk4.clip_operation(55)
  mk4.native_context()
  mk4.tail(0x46e450)
end

function S.air_diagonal_kick_clip_56_reverse()
  mk4.clip_operation(56)
  mk4.native_context()
  mk4.tail(0x46e540)
end

function S.air_diagonal_kick_clip_57_reverse()
  mk4.clip_operation(57)
  mk4.native_context()
  mk4.tail(0x46e630)
end

function S.air_diagonal_kick_contact_hold()
  mk4.landing()
  mk4.wait(4)
  mk4.allow_commands()
  mk4.wait(4)
  mk4.gravity(0x51e)
  mk4.native_context()
  mk4.tail(native.AIR_ATTACK_LAND_OR_FINISH)
end

-- Catalog: scorpion.air_hp (attacker); raiden.air_hp (attacker); sonya.air_hp (attacker) (also shared by other catalog entries).
function S.air_high_punch_startup()
  mk4.attack_selector(0x104)
  mk4.sound_a(25)
  mk4.tail(native.AIR_HP_TAIL)
end

function S.air_high_punch_miss_reverse()
  mk4.clip_operation(97)
  mk4.tail(native.AIR_STRAIGHT_LAND)
end

function S.air_high_contact_end()
  mk4.gravity(0x28f)
  mk4.native_context()
  mk4.tail(native.AIR_ATTACK_LAND_OR_FINISH)
end

-- Catalog: scorpion.draw_weapon (air HP selector); raiden.draw_weapon (air HP selector); sonya.draw_weapon (air HP selector) (also shared by other catalog entries).
function S.jax_draw_weapon_air_hp()
  mk4.attack_selector(0x104)
  mk4.tail(native.WEAPON_AIR_HIGH_PUNCH_OPEN)
end

-- Catalog: scorpion.draw_weapon (air LP selector); raiden.draw_weapon (air LP selector); sonya.draw_weapon (air LP selector) (also shared by other catalog entries).
function S.jax_draw_weapon_air_lp()
  mk4.attack_selector(0x108)
  mk4.tail(native.WEAPON_AIR_LOW_PUNCH_OPEN)
end

-- Catalog: scorpion.air_lp (attacker); raiden.air_lp (attacker); sonya.air_lp (attacker) (also shared by other catalog entries).
function S.air_low_punch_startup()
  mk4.attack_selector(0x105)
  mk4.sound_a(25)
  mk4.tail(native.AIR_LP_TAIL)
end

function S.air_low_punch_miss_reverse()
  mk4.clip_operation(96)
  mk4.tail(native.AIR_STRAIGHT_LAND)
end

function S.air_low_punch_contact_hold()
  mk4.allow_commands()
  mk4.landing()
  mk4.wait(12)
  mk4.gravity(0x28f)
  mk4.native_context()
  mk4.tail(native.AIR_ATTACK_LAND_OR_FINISH)
end

-- Catalog: scorpion.sweep (attacker); raiden.sweep (roster selector); sonya.sweep (roster selector) (also shared by other catalog entries).
function S.sweep()
  return mk4.select_roster {
    scorpion = S.sweep_clip_63,
    raiden = S.sweep_raiden,
    sonya = S.sweep_clip_63,
    liu_kang = S.sweep_raiden,
    sub_zero = S.sweep_clip_63,
    fujin = S.sweep_raiden,
    shinnok = S.sweep_clip_63,
    reiko = S.sweep_raiden,
    quan_chi = S.sweep_clip_63,
    tanya = S.sweep_raiden,
    reptile = S.sweep_clip_63,
    kai = S.sweep_raiden,
    jarek = S.sweep_clip_63,
    jax = S.sweep_raiden,
    cage = S.sweep_clip_63,
    goro = S.sweep_clip_63,
  }
end

function S.sweep_raiden()
  mk4.attack_selector(0x24)
  mk4.open_clip(0, 62)
  mk4.double_to(10)
  mk4.sound_a(175)
  mk4.forward_to(22)
  mk4.tail(native.SWEEP_ADVANCING_TAIL)
  return S.roundhouse()
end

-- Catalog: raiden.roundhouse (roster selector); sonya.roundhouse (roster selector); sub_zero.roundhouse (character-selected attacker) (also shared by other catalog entries).
function S.roundhouse()
  return mk4.select_roster {
    scorpion = S.roundhouse_scorpion,
    raiden = S.roundhouse_clip_64,
    sonya = S.roundhouse_scorpion,
    liu_kang = S.roundhouse_clip_64,
    sub_zero = S.roundhouse_scorpion,
    fujin = S.roundhouse_clip_64,
    shinnok = S.roundhouse_scorpion,
    reiko = S.roundhouse_clip_64,
    quan_chi = S.roundhouse_scorpion,
    tanya = S.roundhouse_clip_64,
    reptile = S.roundhouse_scorpion,
    kai = S.roundhouse_clip_64,
    jarek = S.roundhouse_scorpion,
    jax = S.roundhouse_clip_64,
    cage = S.roundhouse_scorpion,
    goro = S.roundhouse_scorpion,
  }
end

-- Catalog: scorpion.roundhouse (attacker).
function S.scorpion_roundhouse_attacker()
  mk4.attack_selector(0x21)
  mk4.open_clip(0, 313)
  mk4.tail(native.ROUNDHOUSE_START)
end

-- Catalog: scorpion.roundhouse (fast start); sonya.roundhouse (fast start); sub_zero.roundhouse (fast start) (also shared by other catalog entries).
function S.cage_roundhouse_fast_start()
  mk4.double_to(12)
  mk4.sound_c(28)
  mk4.double_to(21)
  mk4.tail(native.ROUNDHOUSE_TAIL)
end

-- Catalog: scorpion.roundhouse (normal start); sonya.roundhouse (normal start); sub_zero.roundhouse (normal start) (also shared by other catalog entries).
function S.cage_roundhouse_normal_start()
  mk4.double_to(12)
  mk4.sound_c(28)
  mk4.forward_to(21)
  mk4.tail(native.ROUNDHOUSE_TAIL)
end

-- Catalog: raiden.roundhouse (fast start); liu_kang.roundhouse (fast startup); fujin.roundhouse (fast start) (also shared by other catalog entries).
function S.jax_roundhouse_fast_start()
  mk4.double_to(12)
  mk4.sound_c(28)
  mk4.double_to(20)
  mk4.tail(native.ROUNDHOUSE_WIDE_TAIL)
end

-- Catalog: raiden.roundhouse (normal start); liu_kang.roundhouse (normal startup); fujin.roundhouse (normal start) (also shared by other catalog entries).
function S.jax_roundhouse_normal_start()
  mk4.double_to(12)
  mk4.sound_c(28)
  mk4.forward_to(20)
  mk4.tail(native.ROUNDHOUSE_WIDE_TAIL)
end

-- Catalog: scorpion.spike_fatality (stage script); raiden.spike_fatality (stage script); sonya.spike_fatality (stage script) (also shared by other catalog entries).
function S.spike_enter()
  mk4.wait(28)
  mk4.prefixed_to(5, 293)
  mk4.sound_c(28)
  mk4.query_a(18)
  mk4.tail(native.STAGE_OWNER)
end

-- Catalog: raiden.high_kick (recovery); sub_zero.high_kick (recovery); quan_chi.high_kick (recovery) (also shared by other catalog entries).
function S.kai_high_kick_recovery()
  mk4.action_recovery()
  mk4.forward_count(2)
  mk4.release_attack()
  mk4.forward()
  return mk4.ready()
end

-- Catalog: scorpion.combo_hp (native-selected script); liu_kang.combo_hp (native-selected script); shinnok.combo_hp (native-selected script) (also shared by other catalog entries).
function S.jax_combo_hp_attacker()
  mk4.open_clip(2, 261)
  mk4.forward_to(11)
  mk4.tail(native.COMBO_HIGH_PUNCH_TAIL)
end

-- Catalog: raiden.combo_hp (native-selected script); sub_zero.combo_hp (native-selected script); reiko.combo_hp (native-selected script) (also shared by other catalog entries).
function S.sub_zero_combo_hp_attacker()
  mk4.open_clip(0, 532)
  mk4.prefixed_sample(0)
  mk4.wait(1)
  mk4.prefixed_sample(1)
  mk4.wait(1)
  mk4.prefixed_sample(2)
  mk4.wait(1)
  mk4.prefixed_sample(3)
  mk4.wait(1)
  mk4.prefixed_sample(4)
  mk4.wait(1)
  mk4.prefixed_sample(6)
  mk4.wait(1)
  mk4.prefixed_sample(8)
  mk4.wait(1)
  mk4.prefixed_sample(9)
  mk4.wait(1)
  mk4.prefixed_sample(10)
  mk4.wait(1)
  mk4.prefixed_sample(11)
  mk4.wait(1)
  mk4.tail(native.COMBO_HIGH_PUNCH_TAIL)
  return S.kitana_combo_high_punch_attacker()
end

-- Catalog: sonya.combo_hp (native-selected script); fujin.combo_hp (native-selected script); quan_chi.combo_hp (native-selected script) (also shared by other catalog entries).
function S.kitana_combo_high_punch_attacker()
  mk4.open_clip(2, 533)
  mk4.forward_to(11)
  mk4.tail(native.COMBO_HIGH_PUNCH_TAIL)
end

-- Entered one word into the stream above; the wordcode skips a no-op word.
function S.roundhouse_scorpion()
  return S.scorpion_roundhouse_attacker()
end

return S

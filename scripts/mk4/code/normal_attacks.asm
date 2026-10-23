; Normal attacks, air attacks and shared roster selection.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: scorpion.high_kick (recovery); liu_kang.high_kick (recovery); shinnok.high_kick (recovery) (also shared by other catalog entries).
jax_high_kick_recovery:
  backward_count 2
  release_attack 0
  backward 0
  ready 0

; Catalog: sub_zero.high_kick (Sub-Zero attacker).
high_kick_clip_71:
  attack_selector 0, 0x2
  sound_a 1
  prefixed_to WITH_CLIP + 10, clip(71)  ; bind clip prefix; inline sample/count 10
  tail 0, MK4_NATIVE_KICK_CONTACT_NARROW
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.high_kick (attacker); raiden.high_kick (roster selector); sonya.high_kick (roster selector) (also shared by other catalog entries).
high_kick:
  select_roster 0, \
    word(high_kick_clip_66), \  ; scorpion
    word(high_kick_clip_71), \  ; raiden
    word(high_kick_sonya), \  ; sonya
    word(high_kick_clip_66), \  ; liu_kang
    word(high_kick_clip_71), \  ; sub_zero
    word(high_kick_sonya), \  ; fujin
    word(high_kick_clip_66), \  ; shinnok
    word(high_kick_clip_66), \  ; reiko
    word(high_kick_clip_71), \  ; quan_chi
    word(high_kick_clip_66), \  ; tanya
    word(high_kick_clip_66), \  ; reptile
    word(high_kick_clip_71), \  ; kai
    word(high_kick_sonya), \  ; jarek
    word(high_kick_clip_66), \  ; jax
    word(high_kick_clip_71), \  ; cage
    word(high_kick_clip_66)  ; goro
  attack_selector 0, 0x0
  sound_a 1
  prefixed_to WITH_CLIP + 10, clip(65)  ; bind clip prefix; inline sample/count 10
  tail 0, MK4_NATIVE_KICK_CONTACT_DEFAULT
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.high_kick (recovery); fujin.high_kick (recovery); jarek.high_kick (recovery) (also shared by other catalog entries).
fujin_high_kick_recovery:
  backward_count 2
  release_attack 0
  tail 0, MK4_NATIVE_HIGH_KICK_REVERSE_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.low_kick (character-selected attacker); sonya.low_kick (character-selected attacker); fujin.low_kick (character-selected attacker) (also shared by other catalog entries).
low_kick:
  select_roster 0, \
    word(low_kick_clip_67), \  ; scorpion
    word(low_kick_clip_70), \  ; raiden
    word(low_kick_clip_308), \  ; sonya
    word(low_kick_clip_67), \  ; liu_kang
    word(low_kick_clip_70), \  ; sub_zero
    word(low_kick_clip_308), \  ; fujin
    word(low_kick_clip_67), \  ; shinnok
    word(low_kick_clip_67), \  ; reiko
    word(low_kick_clip_70), \  ; quan_chi
    word(low_kick_clip_308), \  ; tanya
    word(low_kick_clip_67), \  ; reptile
    word(low_kick_clip_70), \  ; kai
    word(low_kick_clip_308), \  ; jarek
    word(low_kick_clip_67), \  ; jax
    word(low_kick_clip_70), \  ; cage
    word(low_kick_clip_67)  ; goro
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.air_throw (paired capture).
air_throw_lift:
  sound_indexed 54
  open_peer_clip 0, clip(144)
  open_clip 0, clip(84)
  playback_single 20
  tail 0, MK4_NATIVE_AIR_LIFT_FALL

; Catalog: jax.air_throw (falling pair).
air_lift_fall:
  playback_single 32
  sound_a 36
  sound_roster 1
  spawn_effect 6
  playback_single 43
  disarm_peer 0
  tail 0, MK4_NATIVE_AIR_LIFT_RELEASE

; Catalog: jax.air_throw (owner release).
air_lift_release:
  playback_single 48
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_throw (paired attacker/victim); raiden.air_throw (paired attacker/victim); sub_zero.air_throw (paired attacker/victim) (also shared by other catalog entries).
air_throw:
  native_release 23
  sound_indexed 13
  open_peer_clip 0, clip(202)
  open_clip 0, clip(82)
  playback_single 28
  tail 0, MK4_NATIVE_AIR_THROW_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_throw (release); raiden.air_throw (release); sub_zero.air_throw (release) (also shared by other catalog entries).
air_throw_release:
  forward_count 1
  motion 0, 0x7ae, 0xffffe667, 0x28f
  wait 10
  forward 0
  native_context 0
  tail 0, MK4_NATIVE_AIR_THROW_LAND

; Catalog: quan_chi.air_throw (capture); noob_saibot.air_throw (Noob air-throw opener).
air_throw_slam:
  landing 0
  native_release 23
  sound_indexed 21
  open_peer_clip 0, clip(150)
  open_clip 0, clip(74)
  playback_single 18
  tail 0, MK4_NATIVE_AIR_SLAM_FALL

; Catalog: quan_chi.air_throw (paired fall); noob_saibot.air_throw (Noob pair continuation).
air_slam_fall:
  playback_single 27
  tail 0, MK4_NATIVE_NOOB_SAIBOT_AIR_THROW_LANDING_LINK
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.air_throw (landing); noob_saibot.air_throw (Noob landing continuation).
quan_chi_air_throw_landing:
  native_context 0
  tail 0, MK4_NATIVE_AIR_SLAM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.air_throw (paired owner); kitana.air_throw (paired owner).
air_throw_flip:
  native_release 23
  sound_indexed 16
  open_peer_clip 0, clip(161)
  open_clip 0, clip(77)
  play_pair 24
  tail 0, MK4_NATIVE_AIR_FLIP_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.close_hk (follow-up crouch entry); raiden.close_hk (follow-up crouch entry); sonya.close_hk (follow-up crouch entry) (also shared by other catalog entries).
follow_crouch_hk:
  speed 0, 0x1999

; Catalog: sub_zero.crouch_hk (attacker).
crouch_high_kick_clip_52:
  attack_selector 0, 0x1a
  sound_a 1
  play_clip 7, clip(52)
  query_b 26
  action_recovery 0
  landing 0
  forward 0
  loop 5
  playback_tail 10
  wait 2
  playback_tail 9
  wait 2
  repeat 0
  clip_operation 0, clip(52)
  return 0

; Catalog: raiden.crouch_hk (roster selector); sonya.crouch_hk (roster selector); kitana.crouch_hk (roster selector).
crouch_high_kick:
  select_roster 0, \
    word(crouch_high_kick_clip_54), \  ; scorpion
    word(crouch_high_kick_clip_52), \  ; raiden
    word(crouch_high_kick_sonya), \  ; sonya
    word(crouch_high_kick_clip_54), \  ; liu_kang
    word(crouch_high_kick_clip_52), \  ; sub_zero
    word(crouch_high_kick_sonya), \  ; fujin
    word(crouch_high_kick_clip_54), \  ; shinnok
    word(crouch_high_kick_clip_54), \  ; reiko
    word(crouch_high_kick_clip_52), \  ; quan_chi
    word(crouch_high_kick_clip_54), \  ; tanya
    word(crouch_high_kick_clip_52), \  ; reptile
    word(crouch_high_kick_clip_52), \  ; kai
    word(crouch_high_kick_clip_54), \  ; jarek
    word(crouch_high_kick_clip_54), \  ; jax
    word(crouch_high_kick_clip_52), \  ; cage
    word(crouch_high_kick_clip_52)  ; goro
  attack_selector 0, 0x18
  sound_a 1
  play_clip 9, clip(53)
  query_b 24
  action_recovery 0
  forward 0
  loop 2
  playback_tail 11
  wait 2
  playback_tail 10
  wait 2
  playback_tail 11
  wait 2
  playback_tail 12
  wait 2
  repeat 0
  clip_operation 0, clip(53)
  return 0

; Catalog: scorpion.crouch_hk (attacker); liu_kang.crouch_hk (attacker).
crouch_high_kick_clip_54:
  attack_selector 0, 0x19
  sound_a 1
  play_clip 9, clip(54)
  query_b 25
  action_recovery 0
  forward 0

crouch_high_kick_clip_54_loop_1:
  loop 2
  playback_tail 11
  wait 2
  playback_tail 10
  wait 2
  playback_tail 11
  wait 2
  playback_tail 12
  wait 2
  repeat 0
  clip_operation 0, clip(54)
  return 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.crouch_lk (attacker); raiden.crouch_lk (attacker); sonya.crouch_lk (attacker) (also shared by other catalog entries).
crouch_low_kick:
  attack_selector 0, 0x17
  sound_c 27
  open_clip 0, clip(315)
  cursor_set 1
  forward_to 9
  tail 0, MK4_NATIVE_CROUCH_LOW_KICK_TAIL
  .zero 4  ; non-executable guard between independent streams


crouch_low_kick_recover:
  cursor_set 9
  playback_pair 1
  return 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.close_hk (attacker); raiden.close_hk (attacker); sonya.close_hk (attacker) (also shared by other catalog entries).
knee_script:
  open_clip 0, clip(314)
  playback_tail 5
  wait 1
  playback_tail 6
  wait 1
  playback_tail 7
  wait 1
  playback_tail 8
  wait 1
  playback_tail 9
  action_recovery 0
  tail 0, MK4_NATIVE_CLOSE_HIGH_KICK_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.close_hk (miss recovery); raiden.close_hk (miss recovery); sonya.close_hk (miss recovery) (also shared by other catalog entries).
knee_miss_finished:
  wait 4
  backward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.close_hp (inherited animation); raiden.close_hp (inherited animation); sonya.close_hp (inherited animation) (also shared by other catalog entries).
close_high_punch_2:
  attack_selector 0, 0x110
  forward_to 7
  tail 0, MK4_NATIVE_CLOSE_HIGH_PUNCH_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_kick (straight kick); raiden.air_kick (roster selector); sonya.air_kick (roster selector) (also shared by other catalog entries).
air_straight_kick_startup:
  select_roster 0, \
    word(air_straight_kick_startup_scorpion), \  ; scorpion
    word(air_straight_kick_startup_clip_61), \  ; raiden
    word(air_straight_kick_startup_scorpion), \  ; sonya
    word(air_straight_kick_startup_clip_61), \  ; liu_kang
    word(air_straight_kick_startup_scorpion), \  ; sub_zero
    word(air_straight_kick_startup_clip_61), \  ; fujin
    word(air_straight_kick_startup_scorpion), \  ; shinnok
    word(air_straight_kick_startup_clip_61), \  ; reiko
    word(air_straight_kick_startup_scorpion), \  ; quan_chi
    word(air_straight_kick_startup_clip_61), \  ; tanya
    word(air_straight_kick_startup_scorpion), \  ; reptile
    word(air_straight_kick_startup_clip_61), \  ; kai
    word(air_straight_kick_startup_scorpion), \  ; jarek
    word(air_straight_kick_startup_clip_61), \  ; jax
    word(air_straight_kick_startup_scorpion), \  ; cage
    word(air_straight_kick_startup_scorpion)  ; goro
  attack_selector 0, 0x28
  sound_a 31
  clip 0, clip(327)
  tail 0, MK4_NATIVE_AIR_STRAIGHT_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.air_kick (narrow miss reversal); quan_chi.air_kick (narrow miss reversal); reptile.air_kick (narrow miss reversal) (also shared by other catalog entries).
cage_air_kick_narrow_miss_reversal:
  clip_operation 0, clip(327)
  tail 0, MK4_NATIVE_AIR_STRAIGHT_LAND

air_punch_contact:
  allow_commands 0
  wait 12
  gravity 0, 0x28f
  native_context 0
  tail 0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.uppercut (attacker); raiden.uppercut (attacker); sonya.uppercut (attacker) (also shared by other catalog entries).
uppercut:
  attack_selector 0, 0x102
  prefixed_to WITH_CLIP + 5, clip(293)  ; bind clip prefix; inline sample/count 5
  sound_c 28
  tail 0, MK4_NATIVE_UPPERCUT_DEFAULT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.draw_weapon (draw); raiden.draw_weapon (draw); sonya.draw_weapon (draw) (also shared by other catalog entries).
cage_draw_weapon_draw:
  sound_variant 0
  play_clip 22, clip(244)
  tail 0, MK4_NATIVE_WEAPON_DRAW_EQUIP
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.draw_weapon (return); raiden.draw_weapon (return); sonya.draw_weapon (return) (also shared by other catalog entries).
shinnok_draw_weapon_equip:
  forward_count 13
  sound_c 18
  forward 0
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.jab (attacker); raiden.jab (attacker); sonya.jab (attacker) (also shared by other catalog entries).
jab:
  call 0, MK4_NATIVE_JAB_REPEAT_PREDICATE
  sound_a 10
  play_clip 8, clip(294)
  call 0, MK4_NATIVE_JAB_CONTACT
  wait 1
  predicate_flags 0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_repeat_tail_1)
  sound_a 10
  open_clip 0, clip(295)
  tail 0, MK4_NATIVE_CAGE_JAB_SECOND_PUNCH

; Catalog: scorpion.jab (second punch); raiden.jab (second punch); sonya.jab (second punch) (also shared by other catalog entries).
cage_jab_second_punch:
  cursor_set 0
  forward_count 10
  call 0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_recovery_action_recovery_1)

cage_jab_second_punch_sound_a_1:
  sound_a 10
  forward 0
  tail 0, MK4_NATIVE_CAGE_JAB_REPEAT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.jab (repeat); raiden.jab (repeat); sonya.jab (repeat) (also shared by other catalog entries).
cage_jab_repeat:
  wait 1
  call 0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_repeat_tail_1)
  sound_a 10
  play_clip 10, clip(295)
  call 0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_recovery_action_recovery_1)
  jump 0, word(cage_jab_second_punch_sound_a_1)

cage_jab_repeat_tail_1:
  tail 0, MK4_NATIVE_CAGE_JAB_RECOVERY
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.jab (recovery); raiden.jab (recovery); sonya.jab (recovery) (also shared by other catalog entries).
cage_jab_recovery:
  action_recovery 0
  open_clip 9, clip(294)
  double_to 17
  native_sound 0
  ready 0

cage_jab_recovery_action_recovery_1:
  action_recovery 0
  forward_count 3
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_diagonal_lk (attacker); raiden.air_diagonal_lk (roster selector); sonya.air_diagonal_lk (roster selector) (also shared by other catalog entries).
air_diagonal_low_kick_startup:
  sound_a 25

air_diagonal_low_kick_startup_select_roster_1:
  select_roster 0, \
    word(air_diagonal_low_kick_startup_select_roster_1_operand_16), \  ; scorpion
    word(air_straight_kick_startup_clip_61_clip_1), \  ; raiden
    word(air_straight_kick_startup_clip_61_clip_2), \  ; sonya
    word(air_diagonal_low_kick_startup_select_roster_1_operand_16), \  ; liu_kang
    word(air_straight_kick_startup_clip_61_clip_1), \  ; sub_zero
    word(air_straight_kick_startup_clip_61_clip_2), \  ; fujin
    word(air_diagonal_low_kick_startup_select_roster_1_operand_16), \  ; shinnok
    word(air_straight_kick_startup_clip_61_clip_1), \  ; reiko
    word(air_straight_kick_startup_clip_61_clip_2), \  ; quan_chi
    word(air_diagonal_low_kick_startup_select_roster_1_operand_16), \  ; tanya
    word(air_straight_kick_startup_clip_61_clip_1), \  ; reptile
    word(air_straight_kick_startup_clip_61_clip_2), \  ; kai
    word(air_diagonal_low_kick_startup_select_roster_1_operand_16), \  ; jarek
    word(air_straight_kick_startup_clip_61_clip_1), \  ; jax
    word(air_straight_kick_startup_clip_61_clip_2), \  ; cage
    word(air_straight_kick_startup_clip_61_clip_2)  ; goro
  clip 0, clip(58)
  attack_selector 0, 0x1b
  tail 0, MK4_NATIVE_AIR_LK_TAIL
  .zero 4  ; non-executable guard between independent streams


air_low_kick_clip_58_reverse:
  clip_operation 0, clip(58)
  native_context 0
  tail 0, MK4_NATIVE_AIR_LOW_KICK_LAND
  .zero 4  ; non-executable guard between independent streams


air_low_kick_clip_59_reverse:
  clip_operation 0, clip(59)
  native_context 0
  tail 0, 0x46e0d0
  .zero 4  ; non-executable guard between independent streams


air_low_kick_clip_60_reverse:
  clip_operation 0, clip(60)
  native_context 0
  tail 0, 0x46e1b0
  .zero 4  ; non-executable guard between independent streams


air_low_kick_contact_hold:
  landing 0
  wait 8
  allow_commands 0
  gravity 0, 0x51e
  native_context 0
  tail 0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH

; Catalog: scorpion.air_diagonal_hk (attacker); raiden.air_diagonal_hk (roster selector); sonya.air_diagonal_hk (roster selector) (also shared by other catalog entries).
air_diagonal_high_kick_startup:
  sound_a 25

air_diagonal_high_kick_startup_select_roster_1:
  select_roster 0, \
    word(air_diagonal_high_kick_startup_select_roster_1_operand_16), \  ; scorpion
    word(air_straight_kick_startup_clip_61_attack_selector_1), \  ; raiden
    word(air_straight_kick_startup_clip_61_attack_selector_2), \  ; sonya
    word(air_diagonal_high_kick_startup_select_roster_1_operand_16), \  ; liu_kang
    word(air_straight_kick_startup_clip_61_attack_selector_1), \  ; sub_zero
    word(air_straight_kick_startup_clip_61_attack_selector_2), \  ; fujin
    word(air_diagonal_high_kick_startup_select_roster_1_operand_16), \  ; shinnok
    word(air_straight_kick_startup_clip_61_attack_selector_1), \  ; reiko
    word(air_straight_kick_startup_clip_61_attack_selector_2), \  ; quan_chi
    word(air_diagonal_high_kick_startup_select_roster_1_operand_16), \  ; tanya
    word(air_straight_kick_startup_clip_61_attack_selector_1), \  ; reptile
    word(air_straight_kick_startup_clip_61_attack_selector_2), \  ; kai
    word(air_diagonal_high_kick_startup_select_roster_1_operand_16), \  ; jarek
    word(air_straight_kick_startup_clip_61_attack_selector_1), \  ; jax
    word(air_straight_kick_startup_clip_61_attack_selector_2), \  ; cage
    word(air_straight_kick_startup_clip_61_attack_selector_1)  ; goro
  attack_selector 0, 0x1e
  tail 0, MK4_NATIVE_AIR_DIAGONAL_TAIL
  .zero 4  ; non-executable guard between independent streams


air_diagonal_kick_clip_55_reverse:
  clip_operation 0, clip(55)
  native_context 0
  tail 0, 0x46e450
  .zero 4  ; non-executable guard between independent streams


air_diagonal_kick_clip_56_reverse:
  clip_operation 0, clip(56)
  native_context 0
  tail 0, 0x46e540
  .zero 4  ; non-executable guard between independent streams


air_diagonal_kick_clip_57_reverse:
  clip_operation 0, clip(57)
  native_context 0
  tail 0, 0x46e630
  .zero 4  ; non-executable guard between independent streams


air_diagonal_kick_contact_hold:
  landing 0
  wait 4
  allow_commands 0
  wait 4
  gravity 0, 0x51e
  native_context 0
  tail 0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_hp (attacker); raiden.air_hp (attacker); sonya.air_hp (attacker) (also shared by other catalog entries).
air_high_punch_startup:
  attack_selector 0, 0x104
  sound_a 25
  tail 0, MK4_NATIVE_AIR_HP_TAIL
  .zero 4  ; non-executable guard between independent streams


air_high_punch_miss_reverse:
  clip_operation 0, clip(97)
  tail 0, MK4_NATIVE_AIR_STRAIGHT_LAND
  .zero 4  ; non-executable guard between independent streams


air_high_contact_end:
  gravity 0, 0x28f
  native_context 0
  tail 0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.draw_weapon (air HP selector); raiden.draw_weapon (air HP selector); sonya.draw_weapon (air HP selector) (also shared by other catalog entries).
jax_draw_weapon_air_hp:
  attack_selector 0, 0x104
  tail 0, MK4_NATIVE_WEAPON_AIR_HIGH_PUNCH_OPEN
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.draw_weapon (air LP selector); raiden.draw_weapon (air LP selector); sonya.draw_weapon (air LP selector) (also shared by other catalog entries).
jax_draw_weapon_air_lp:
  attack_selector 0, 0x108
  tail 0, MK4_NATIVE_WEAPON_AIR_LOW_PUNCH_OPEN
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.air_lp (attacker); raiden.air_lp (attacker); sonya.air_lp (attacker) (also shared by other catalog entries).
air_low_punch_startup:
  attack_selector 0, 0x105
  sound_a 25
  tail 0, MK4_NATIVE_AIR_LP_TAIL
  .zero 4  ; non-executable guard between independent streams


air_low_punch_miss_reverse:
  clip_operation 0, clip(96)
  tail 0, MK4_NATIVE_AIR_STRAIGHT_LAND
  .zero 4  ; non-executable guard between independent streams


air_low_punch_contact_hold:
  allow_commands 0
  landing 0
  wait 12
  gravity 0, 0x28f
  native_context 0
  tail 0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.sweep (attacker); raiden.sweep (roster selector); sonya.sweep (roster selector) (also shared by other catalog entries).
sweep:
  select_roster 0, \
    word(sweep_clip_63), \  ; scorpion
    word(sweep_raiden), \  ; raiden
    word(sweep_clip_63), \  ; sonya
    word(sweep_raiden), \  ; liu_kang
    word(sweep_clip_63), \  ; sub_zero
    word(sweep_raiden), \  ; fujin
    word(sweep_clip_63), \  ; shinnok
    word(sweep_raiden), \  ; reiko
    word(sweep_clip_63), \  ; quan_chi
    word(sweep_raiden), \  ; tanya
    word(sweep_clip_63), \  ; reptile
    word(sweep_raiden), \  ; kai
    word(sweep_clip_63), \  ; jarek
    word(sweep_raiden), \  ; jax
    word(sweep_clip_63), \  ; cage
    word(sweep_clip_63)  ; goro
  attack_selector 0, 0x24
  open_clip 0, clip(62)
  double_to 10
  sound_a 175
  forward_to 22
  tail 0, MK4_NATIVE_SWEEP_ADVANCING_TAIL

; Catalog: raiden.roundhouse (roster selector); sonya.roundhouse (roster selector); sub_zero.roundhouse (character-selected attacker) (also shared by other catalog entries).
roundhouse:
  select_roster 0, \
    word(roundhouse_scorpion), \  ; scorpion
    word(roundhouse_clip_64), \  ; raiden
    word(roundhouse_scorpion), \  ; sonya
    word(roundhouse_clip_64), \  ; liu_kang
    word(roundhouse_scorpion), \  ; sub_zero
    word(roundhouse_clip_64), \  ; fujin
    word(roundhouse_scorpion), \  ; shinnok
    word(roundhouse_clip_64), \  ; reiko
    word(roundhouse_scorpion), \  ; quan_chi
    word(roundhouse_clip_64), \  ; tanya
    word(roundhouse_scorpion), \  ; reptile
    word(roundhouse_clip_64), \  ; kai
    word(roundhouse_scorpion), \  ; jarek
    word(roundhouse_clip_64), \  ; jax
    word(roundhouse_scorpion), \  ; cage
    word(roundhouse_scorpion)  ; goro

; Catalog: scorpion.roundhouse (attacker).
scorpion_roundhouse_attacker:
  attack_selector 0, 0x21
  open_clip 0, clip(313)
  tail 0, MK4_NATIVE_ROUNDHOUSE_START
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.roundhouse (fast start); sonya.roundhouse (fast start); sub_zero.roundhouse (fast start) (also shared by other catalog entries).
cage_roundhouse_fast_start:
  double_to 12
  sound_c 28
  double_to 21
  tail 0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.roundhouse (normal start); sonya.roundhouse (normal start); sub_zero.roundhouse (normal start) (also shared by other catalog entries).
cage_roundhouse_normal_start:
  double_to 12
  sound_c 28
  forward_to 21
  tail 0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.roundhouse (fast start); liu_kang.roundhouse (fast startup); fujin.roundhouse (fast start) (also shared by other catalog entries).
jax_roundhouse_fast_start:
  double_to 12
  sound_c 28
  double_to 20
  tail 0, MK4_NATIVE_ROUNDHOUSE_WIDE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.roundhouse (normal start); liu_kang.roundhouse (normal startup); fujin.roundhouse (normal start) (also shared by other catalog entries).
jax_roundhouse_normal_start:
  double_to 12
  sound_c 28
  forward_to 20
  tail 0, MK4_NATIVE_ROUNDHOUSE_WIDE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.spike_fatality (stage script); raiden.spike_fatality (stage script); sonya.spike_fatality (stage script) (also shared by other catalog entries).
spike_enter:
  wait 28
  prefixed_to WITH_CLIP + 5, clip(293)  ; bind clip prefix; inline sample/count 5
  sound_c 28
  query_a 18
  tail 0, MK4_NATIVE_STAGE_OWNER
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.high_kick (recovery); sub_zero.high_kick (recovery); quan_chi.high_kick (recovery) (also shared by other catalog entries).
kai_high_kick_recovery:
  action_recovery 0
  forward_count 2
  release_attack 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.combo_hp (native-selected script); liu_kang.combo_hp (native-selected script); shinnok.combo_hp (native-selected script) (also shared by other catalog entries).
jax_combo_hp_attacker:
  open_clip 2, clip(261)
  forward_to 11
  tail 0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.combo_hp (native-selected script); sub_zero.combo_hp (native-selected script); reiko.combo_hp (native-selected script) (also shared by other catalog entries).
sub_zero_combo_hp_attacker:
  open_clip 0, clip(532)
  prefixed_sample 0
  wait 1
  prefixed_sample 1
  wait 1
  prefixed_sample 2
  wait 1
  prefixed_sample 3
  wait 1
  prefixed_sample 4
  wait 1
  prefixed_sample 6
  wait 1
  prefixed_sample 8
  wait 1
  prefixed_sample 9
  wait 1
  prefixed_sample 10
  wait 1
  prefixed_sample 11
  wait 1
  tail 0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL

; Catalog: sonya.combo_hp (native-selected script); fujin.combo_hp (native-selected script); quan_chi.combo_hp (native-selected script) (also shared by other catalog entries).
kitana_combo_high_punch_attacker:
  open_clip 2, clip(533)
  forward_to 11
  tail 0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  .zero 4  ; non-executable guard

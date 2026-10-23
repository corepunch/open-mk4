; Victory and native-return streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: raiden.victory (roster selector); sonya.victory (roster selector); sub_zero.fatality_2 (victory) (also shared by other catalog entries).
raiden_victory:
  select_roster 0, \
    word(raiden_victory_clip_115), \  ; scorpion
    word(raiden_victory_clip_123), \  ; raiden
    word(raiden_victory_clip_114), \  ; sonya
    word(raiden_victory_clip_119), \  ; liu_kang
    word(raiden_victory_clip_115), \  ; sub_zero
    word(raiden_victory_clip_120), \  ; fujin
    word(raiden_victory_clip_111), \  ; shinnok
    word(raiden_victory_clip_113), \  ; reiko
    word(raiden_victory_clip_125), \  ; quan_chi
    word(raiden_victory_clip_124), \  ; tanya
    word(raiden_victory_clip_121), \  ; reptile
    word(raiden_victory_clip_122), \  ; kai
    word(raiden_victory_clip_125), \  ; jarek
    word(raiden_victory_clip_114), \  ; jax
    word(raiden_victory_clip_111), \  ; cage
    word(raiden_victory_clip_594)  ; goro

raiden_victory_clip_594:
  native_release 83
  prefixed_to WITH_CLIP + 74, clip(594)  ; bind clip prefix; inline sample/count 74
  sound_random 0
  playback_pair 72
  forward_to 74
  playback_pair 72
  forward_to 74
  playback_pair 72
  forward_to 74
  forward 0
  park 0

raiden_victory_clip_111:
  prefixed_to WITH_CLIP + 60, clip(111)  ; bind clip prefix; inline sample/count 60
  native_release 15
  forward 0

raiden_victory_clip_111_cursor_set_1:
  cursor_set 42
  forward 0
  jump 0, word(raiden_victory_clip_111_cursor_set_1)

raiden_victory_clip_115:
  prefixed_finish WITH_CLIP + 0, clip(115)  ; bind clip prefix; inline sample/count 0
  open_clip 0, clip(116)
  prefixed_finish 0
  native_release 6
  prefixed_finish 0
  prefixed_finish WITH_CLIP + 0, clip(117)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(118)
  park 0

raiden_victory_clip_114:
  prefixed_finish WITH_CLIP + 0, clip(114)  ; bind clip prefix; inline sample/count 0

raiden_victory_clip_114_playback_pair_1:
  playback_pair 69
  forward_to 99
  jump 0, word(raiden_victory_clip_114_playback_pair_1)

raiden_victory_clip_113:
  prefixed_to WITH_CLIP + 68, clip(113)  ; bind clip prefix; inline sample/count 68

raiden_victory_clip_113_forward_1:
  forward 0
  cursor_set 69
  jump 0, word(raiden_victory_clip_113_forward_1)

raiden_victory_clip_125:
  native_release 35
  prefixed_finish WITH_CLIP + 0, clip(125)  ; bind clip prefix; inline sample/count 0

raiden_victory_clip_125_cursor_set_1:
  cursor_set 85
  forward 0
  jump 0, word(raiden_victory_clip_125_cursor_set_1)

raiden_victory_clip_122:
  native_release 31
  prefixed_finish WITH_CLIP + 0, clip(122)  ; bind clip prefix; inline sample/count 0

raiden_victory_clip_122_cursor_set_1:
  cursor_set 96
  forward 0
  jump 0, word(raiden_victory_clip_122_cursor_set_1)

raiden_victory_clip_119:
  native_release 30
  prefixed_finish WITH_CLIP + 0, clip(119)  ; bind clip prefix; inline sample/count 0

raiden_victory_clip_119_cursor_set_1:
  cursor_set 144
  forward 0
  jump 0, word(raiden_victory_clip_119_cursor_set_1)

raiden_victory_clip_124:
  native_release 29
  prefixed_finish WITH_CLIP + 0, clip(124)  ; bind clip prefix; inline sample/count 0

raiden_victory_clip_124_cursor_set_1:
  cursor_set 88
  forward 0
  jump 0, word(raiden_victory_clip_124_cursor_set_1)

raiden_victory_clip_123:
  native_release 34
  play_clip 20, clip(123)
  call 0, MK4_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT
  forward 0
  park 0

raiden_victory_clip_121:
  native_release 27
  prefixed_to WITH_CLIP + 90, clip(121)  ; bind clip prefix; inline sample/count 90
  native_release 28

raiden_victory_clip_121_forward_1:
  forward 0
  cursor_set 160
  jump 0, word(raiden_victory_clip_121_forward_1)

raiden_victory_clip_120:
  native_release 25
  prefixed_to WITH_CLIP + 85, clip(120)  ; bind clip prefix; inline sample/count 85
  spawn 0, 0x26, MK4_NATIVE_SCENE_HOVER
  native_release 26
  forward 0

raiden_victory_clip_120_cursor_set_1:
  cursor_set 93
  forward 0
  jump 0, word(raiden_victory_clip_120_cursor_set_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.high_kick (Liu Kang attacker).
high_kick_clip_66:
  attack_selector 0, 0x1
  sound_a 1
  play_clip 3, clip(66)
  double_count 17
  tail 0, MK4_NATIVE_KICK_CONTACT_WIDE

low_kick_clip_308:
  call 0, MK4_NATIVE_AVOID_CLEAR
  attack_selector 0, 0x5
  sound_a 1
  open_clip 0, clip(308)
  cursor_set 4
  double_to 16
  call 0, MK4_NATIVE_AVOID_SET
  double_to 24
  query_b 5
  wait_branch 0x4002
  action_recovery 0
  backward_count 2
  call 0, MK4_NATIVE_AVOID_CLEAR
  release_attack 0
  tail 0, MK4_NATIVE_LOW_KICK_REVERSE_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.low_kick (Scorpion attacker); liu_kang.low_kick (Liu Kang attacker).
low_kick_clip_67:
  call 0, MK4_NATIVE_AVOID_CLEAR
  attack_selector 0, 0x6
  sound_a 1
  open_clip 0, clip(67)

low_kick_clip_67_prefixed_sample_1:
  prefixed_sample 0
  wait 1
  prefixed_sample 2
  wait 1
  prefixed_sample 4
  wait 1
  prefixed_sample 6
  wait 1
  prefixed_sample 8
  wait 1
  prefixed_sample 10
  wait 1
  prefixed_sample 12
  wait 1
  call 0, MK4_NATIVE_AVOID_SET
  prefixed_sample 14
  wait 1
  prefixed_sample 16
  wait 1
  prefixed_sample 17
  wait 1
  call 0, MK4_NATIVE_AVOID_CLEAR
  query_b 6
  action_recovery 0
  release_attack 0
  wait_branch 0x8005
  tail 0, MK4_NATIVE_LOW_KICK_NARROW_REVERSE_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.low_kick (Sub-Zero attacker).
low_kick_clip_70:
  call 0, MK4_NATIVE_AVOID_CLEAR
  attack_selector 0, 0x7
  sound_a 1
  open_clip 0, clip(70)
  prefixed_to 6
  call 0, MK4_NATIVE_AVOID_SET
  forward_to 11
  call 0, MK4_NATIVE_AVOID_CLEAR
  query_b 7
  action_recovery 0
  wait_branch 0x4002
  forward_count 2
  release_attack 0
  forward 0
  ready 0

air_straight_kick_startup_clip_61:
  attack_selector 0, 0x27
  open_clip 0, clip(61)
  cursor_set 4
  forward_to 7
  sound_a 31
  forward_to 10
  tail 0, MK4_NATIVE_AIR_STRAIGHT_WIDE_TAIL

air_straight_kick_startup_clip_61_clip_1:
  clip 0, clip(59)
  attack_selector 0, 0x1c
  tail 0, MK4_NATIVE_AIR_LK_ALT_TAIL

air_straight_kick_startup_clip_61_clip_2:
  clip 0, clip(60)
  attack_selector 0, 0x1d
  tail 0, MK4_NATIVE_AIR_LK_WIDE_TAIL

air_straight_kick_startup_clip_61_attack_selector_1:
  attack_selector 0, 0x1f
  tail 0, MK4_NATIVE_AIR_DIAGONAL_ALT_TAIL

air_straight_kick_startup_clip_61_attack_selector_2:
  attack_selector 0, 0x20
  tail 0, MK4_NATIVE_AIR_DIAGONAL_WIDE_TAIL

sweep_clip_63:
  attack_selector 0, 0x23
  open_clip 0, clip(63)
  double_to 10
  sound_a 175
  forward_to 22
  tail 0, MK4_NATIVE_SWEEP_STANDARD_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.roundhouse (Liu Kang attacker).
roundhouse_clip_64:
  attack_selector 0, 0x22
  open_clip 0, clip(64)
  tail 0, MK4_NATIVE_ROUNDHOUSE_WIDE_START
  .zero 4  ; non-executable guard

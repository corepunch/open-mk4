; Generated raiden bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT 0x410380
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES 0x427b70
.export_equ MK4_NATIVE_RAIDEN_LK_THROW_ENTRY 0x42bce0
.export_equ MK4_NATIVE_RAIDEN_FIRST_FATALITY 0x440fc0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE 0x441140
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_FIRST_END 0x441180
.export_equ MK4_NATIVE_RAIDEN_CAN_TELEPORT 0x45ea10
.export_equ MK4_NATIVE_RAIDEN_SECOND_FATALITY 0x4647f0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND 0x464960
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT 0x464a60
.export_equ MK4_NATIVE_RAIDEN_FATALITY_2_HELD_POSE 0x464a90
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE 0x464aa0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_END 0x464bb0
.export_equ MK4_NATIVE_RAIDEN_FATALITY_2_SCENE_VICTIM 0x464e30
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END 0x464ee0
.export_equ MK4_NATIVE_RAIDEN_TELEPORT 0x47cd00
.export_equ MK4_NATIVE_RAIDEN_TORPEDO 0x47d5f0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS 0x47d680
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MOTION 0x47d6e0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP 0x47d720
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT 0x47d7f0
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL 0x488590
.export_equ MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION 0x4889b0
.export_equ MK4_NATIVE_RAIDEN_STAFF_HP_ENTRY 0x496a40
.export_equ MK4_NATIVE_RAIDEN_STAFF_LP_ENTRY 0x496ba0
.export_equ MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY 0x496be0
.export_equ MK4_NATIVE_RAIDEN_LIGHTNING 0x49bdd0
.export_equ MK4_NATIVE_RAIDEN_LIGHTNING_OBJECT 0x49be10

; Retail source 0x004de218; branches use linked names.
raiden_lk_throw_startup:
  sound_indexed 0x19
  native_release 0xe
  open_peer_clip 0x0, clip(149)
  open_clip 0x0, clip(33)
  playback_single 0x29
  disarm_peer 0x0
  spawn_effect 0x0
  sound_c 0x3a
  sound_roster 0x1
  damage 0x0, 0x35c2
  native_step 0x6f
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e0210; branches use linked names.
raiden_first_fatality:
  prefixed_finish 0x200, clip(392)
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE

; Retail source 0x004e0220; branches use linked names.
raiden_fatality_1_owner_release:
  playback_pair 0x14
  forward 0x0
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_END
  .zero 4 ; non-executable guard

; Retail source 0x004e56c8; branches use linked names.
raiden_second_fatality:
  native_release 0x4b
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND
  .zero 4 ; non-executable guard

; Retail source 0x004e56d8; branches use linked names.
raiden_fatality_2_first_playback:
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES
  playback_pair 0x5e
  forward 0x0
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT

; Retail source 0x004e56f0; branches use linked names.
raiden_fatality_2_repeated_playback:
  native_release 0x4c
  playback_pair 0x5e
  forward 0x0
  playback_pair 0x5e
  forward 0x0
  playback_pair 0x50
  scene_active 0x0
  blend 0x0, clip(570)
  native_return 0xbb
  clip 0x0, clip(570)
  tail 0x0, MK4_NATIVE_RAIDEN_FATALITY_2_HELD_POSE

; Retail source 0x004e5728; branches use linked names.
raiden_fatality_2_held_pose:
  bind_cursor 0x0, clip(571)
  wait 0xd3
  native_release 0x4d
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE

; Retail source 0x004e5740; branches use linked names.
raiden_fatality_2_ending:
  spawn 0x0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
  clip 0x0, clip(572)
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_END
  .zero 4 ; non-executable guard

; Retail source 0x004e5760; branches use linked names.
raiden_fatality_2_owner_hold:
  clip 0x0, clip(573)
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e5770; branches use linked names.
raiden_fatality_2_victim:
  clip 0x0, clip(574)
  bind_cursor 0x0, clip(575)
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END

; Retail source 0x004e5788; branches use linked names.
raiden_fatality_2_victim_hold:
  wait 0xd3
  clip 0x0, clip(576)
  park 0x0
  jump 0, word(tanya_fatality_2)
  .zero 4 ; non-executable guard

; Retail source 0x004e6a40; branches use linked names.
raiden_victory:
  select_roster 0x0, word(raiden_victory_clip_115), word(raiden_victory_clip_123), word(raiden_victory_clip_114), word(raiden_victory_clip_119), word(raiden_victory_clip_115), word(raiden_victory_clip_120), word(raiden_victory_clip_111), word(raiden_victory_clip_113), word(raiden_victory_clip_125), word(raiden_victory_clip_124), word(raiden_victory_clip_121), word(raiden_victory_clip_122), word(raiden_victory_clip_125), word(raiden_victory_clip_114), word(raiden_victory_clip_111), word(raiden_victory_clip_594)

; Retail source 0x004e6a84; branches use linked names.
raiden_victory_clip_594:
  native_release 0x53
  prefixed_to 0x24a, clip(594)
  sound_random 0x0
  playback_pair 0x48
  forward_to 0x4a
  playback_pair 0x48
  forward_to 0x4a
  playback_pair 0x48
  forward_to 0x4a
  forward 0x0
  park 0x0

; Retail source 0x004e6ab4; branches use linked names.
raiden_victory_clip_111:
  prefixed_to 0x23c, clip(111)
  native_release 0xf
  forward 0x0

; Retail source 0x004e6ac4; branches use linked names.
raiden_victory_clip_111_cursor_set_1:
  cursor_set 0x2a
  forward 0x0
  jump 0x0, word(raiden_victory_clip_111_cursor_set_1)

; Retail source 0x004e6ad4; branches use linked names.
raiden_victory_clip_115:
  prefixed_finish 0x200, clip(115)
  open_clip 0x0, clip(116)
  prefixed_finish 0x0
  native_release 0x6
  prefixed_finish 0x0
  prefixed_finish 0x200, clip(117)
  bind_cursor 0x0, clip(118)
  park 0x0

; Retail source 0x004e6b04; branches use linked names.
raiden_victory_clip_114:
  prefixed_finish 0x200, clip(114)

; Retail source 0x004e6b0c; branches use linked names.
raiden_victory_clip_114_playback_pair_1:
  playback_pair 0x45
  forward_to 0x63
  jump 0x0, word(raiden_victory_clip_114_playback_pair_1)

; Retail source 0x004e6b1c; branches use linked names.
raiden_victory_clip_113:
  prefixed_to 0x244, clip(113)

; Retail source 0x004e6b24; branches use linked names.
raiden_victory_clip_113_forward_1:
  forward 0x0
  cursor_set 0x45
  jump 0x0, word(raiden_victory_clip_113_forward_1)

; Retail source 0x004e6b34; branches use linked names.
raiden_victory_clip_125:
  native_release 0x23
  prefixed_finish 0x200, clip(125)

; Retail source 0x004e6b40; branches use linked names.
raiden_victory_clip_125_cursor_set_1:
  cursor_set 0x55
  forward 0x0
  jump 0x0, word(raiden_victory_clip_125_cursor_set_1)

; Retail source 0x004e6b50; branches use linked names.
raiden_victory_clip_122:
  native_release 0x1f
  prefixed_finish 0x200, clip(122)

; Retail source 0x004e6b5c; branches use linked names.
raiden_victory_clip_122_cursor_set_1:
  cursor_set 0x60
  forward 0x0
  jump 0x0, word(raiden_victory_clip_122_cursor_set_1)

; Retail source 0x004e6b6c; branches use linked names.
raiden_victory_clip_119:
  native_release 0x1e
  prefixed_finish 0x200, clip(119)

; Retail source 0x004e6b78; branches use linked names.
raiden_victory_clip_119_cursor_set_1:
  cursor_set 0x90
  forward 0x0
  jump 0x0, word(raiden_victory_clip_119_cursor_set_1)

; Retail source 0x004e6b88; branches use linked names.
raiden_victory_clip_124:
  native_release 0x1d
  prefixed_finish 0x200, clip(124)

; Retail source 0x004e6b94; branches use linked names.
raiden_victory_clip_124_cursor_set_1:
  cursor_set 0x58
  forward 0x0
  jump 0x0, word(raiden_victory_clip_124_cursor_set_1)

; Retail source 0x004e6ba4; branches use linked names.
raiden_victory_clip_123:
  native_release 0x22
  play_clip 0x14, clip(123)
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT
  forward 0x0
  park 0x0

; Retail source 0x004e6bc0; branches use linked names.
raiden_victory_clip_121:
  native_release 0x1b
  prefixed_to 0x25a, clip(121)
  native_release 0x1c

; Retail source 0x004e6bd0; branches use linked names.
raiden_victory_clip_121_forward_1:
  forward 0x0
  cursor_set 0xa0
  jump 0x0, word(raiden_victory_clip_121_forward_1)

; Retail source 0x004e6be0; branches use linked names.
raiden_victory_clip_120:
  native_release 0x19
  prefixed_to 0x255, clip(120)
  spawn 0x0, 0x26, MK4_NATIVE_SCENE_HOVER
  native_release 0x1a
  forward 0x0

; Retail source 0x004e6c00; branches use linked names.
raiden_victory_clip_120_cursor_set_1:
  cursor_set 0x5d
  forward 0x0
  jump 0x0, word(raiden_victory_clip_120_cursor_set_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e8440; branches use linked names.
raiden_torpedo_fast_startup:
  open_clip 0x0, clip(94)
  playback_tail 0xe
  jump 0x0, word(raiden_torpedo_ordinary_startup_call_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e8458; branches use linked names.
raiden_torpedo_ordinary_startup:
  open_clip 0x0, clip(94)
  double_to 0xe

; Retail source 0x004e8464; branches use linked names.
raiden_torpedo_ordinary_startup_call_1:
  call 0x0, MK4_NATIVE_RESTORE_MOTION
  handler 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MOTION
  sound_a 0x2b
  sound_c 0xb
  loop 0x1d
  predicate_argument 0x0, MK4_NATIVE_CONTACT_QUERY_D, word(shared_tail_entry_3)
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP
  wait 0x1
  repeat 0x0
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS
  .zero 4 ; non-executable guard

; Retail source 0x004e8908; branches use linked names.
shared_tail_entry_3:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004e8e18; branches use linked names.
raiden_staff_hp_weapon_victim:
  spawn_effect 0x0
  sound_context 0x2
  effect 0x0
  sound_b 0x1
  speed_negative 0x0, 0xa3d
  open_clip 0x0, clip(132)
  double_to 0xe
  landing 0x0
  forward_count 0xa
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4 ; non-executable guard

; Retail source 0x004e9ad0; branches use linked names.
raiden_torpedo_ground_release:
  speed_negative 0x0, 0x4ccc
  clip 0x0, clip(182)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e9ae8; branches use linked names.
raiden_torpedo_air_release:
  motion 0x0, 0xfffff334, 0xfffff0a4, 0x312
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  wait 0x2
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  jump 0, word(lp_victim_slide_release)
  .zero 4 ; non-executable guard

; Retail source 0x004e9d50; branches use linked names.
raiden_lightning_victim:
  sound_a 0x2d
  speed_negative 0x0, 0x5999
  play_clip 0x1b, clip(127)
  clear_handler 0x0
  speed_negative 0x0, 0xffffeb86
  forward_count 0x5
  landing 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea068; branches use linked names.
raiden_staff_hp_staff_victim:
  sound_b 0x1
  sound_context 0x2
  prefixed_to 0x205, clip(205)
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004ea108; branches use linked names.
raiden_torpedo_contact:
  effect 0x13
  handler 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION
  prefixed_finish 0x200, clip(135)

; Retail source 0x004ea11c; branches use linked names.
raiden_torpedo_contact_playback_pair_1:
  playback_pair 0x4
  forward_to 0x9
  jump 0x0, word(raiden_torpedo_contact_playback_pair_1)
  .zero 4 ; non-executable guard

; Retail source 0x004ed5f0; branches use linked names.
raiden_lightning:
  sound_indexed 0x14
  sound_c 0x2b
  fast_start 0x0, word(raiden_lightning_open_clip_1)
  play_clip 0x4, clip(17)
  forward 0x0
  jump 0x0, word(raiden_lightning_spawn_1)

; Retail source 0x004ed614; branches use linked names.
raiden_lightning_open_clip_1:
  open_clip 0x0, clip(17)
  double_to 0x4
  double_count 0xa

; Retail source 0x004ed624; branches use linked names.
raiden_lightning_spawn_1:
  spawn 0x0, 0x41, MK4_NATIVE_RAIDEN_LIGHTNING_OBJECT
  wait 0x1
  action_followup 0x0
  loop 0x3
  clip 0x4, clip(17)
  repeat 0x0
  clip_alt 0x4, clip(17)
  ready 0x0
  .zero 4 ; non-executable guard

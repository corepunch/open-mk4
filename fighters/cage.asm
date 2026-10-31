; Generated cage bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_CAGE_SECOND_CHARGE 0x413970
.export_equ MK4_NATIVE_CAGE_SECOND_FLASH 0x413a10
.export_equ MK4_NATIVE_CAGE_LK_THROW_ENTRY 0x42b700
.export_equ MK4_NATIVE_CAGE_LK_THROW_OWNER_RETURN 0x42b730
.export_equ MK4_NATIVE_CAGE_LK_THROW_PAIRED_CONTINUATION 0x42bdf0
.export_equ MK4_NATIVE_CAGE_LK_THROW_RELEASE 0x42be00
.export_equ MK4_NATIVE_CAGE_FATALITY_1 0x448160
.export_equ MK4_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL 0x448480
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_59 0x4487b0
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_131 0x4487f0
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_148 0x448830
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_166 0x448870
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_185 0x4488b0
.export_equ MK4_NATIVE_CAGE_FIRST_VICTIM_206 0x4488f0
.export_equ MK4_NATIVE_CAGE_FIRST_SEVER 0x448930
.export_equ MK4_NATIVE_CAGE_FIRST_TORSO_HALF 0x448c80
.export_equ MK4_NATIVE_CAGE_FIRST_TORSO_123 0x448d20
.export_equ MK4_NATIVE_CAGE_FIRST_TORSO_FINISH 0x448d70
.export_equ MK4_NATIVE_CAGE_CAN_GROIN_PUNCH 0x45f120
.export_equ MK4_NATIVE_CAGE_FATALITY_2 0x465790
.export_equ MK4_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE 0x4658a0
.export_equ MK4_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS 0x4658f0
.export_equ MK4_NATIVE_CAGE_SECOND_VICTIM_HIT 0x465a80
.export_equ MK4_NATIVE_CAGE_SECOND_VICTIM_FINISH 0x465b10
.export_equ MK4_NATIVE_CAGE_GROIN_PUNCH 0x468aa0
.export_equ MK4_NATIVE_CAGE_JAB_SECOND_PUNCH 0x46dd00
.export_equ MK4_NATIVE_CAGE_JAB_REPEAT 0x46dd10
.export_equ MK4_NATIVE_CAGE_JAB_RECOVERY 0x46dd20
.export_equ MK4_NATIVE_CAGE_CPU_KICK 0x479b20
.export_equ MK4_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL 0x479b70
.export_equ MK4_NATIVE_CAGE_POWERED_PUNCH 0x479ec0
.export_equ MK4_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL 0x479f00
.export_equ MK4_NATIVE_CAGE_WEAPON_REACTION_ENTRY 0x482300
.export_equ MK4_NATIVE_CAGE_SHADOW_VICTIM 0x4836b0
.export_equ MK4_NATIVE_CAGE_GROIN_PUNCH_VICTIM_REACTION 0x4879c0
.export_equ MK4_NATIVE_CAGE_POWER_VICTIM 0x488400
.export_equ MK4_NATIVE_CAGE_WEAPON_BACK_HP_ENTRY 0x4957b0
.export_equ MK4_NATIVE_CAGE_WEAPON_LP_ENTRY 0x495800
.export_equ MK4_NATIVE_CAGE_WEAPON_UPPER_ENTRY 0x495860
.export_equ MK4_NATIVE_CAGE_WEAPON_UPPER_CONTACT 0x4958b0
.export_equ MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY 0x496920
.export_equ MK4_NATIVE_CAGE_WEAPON_BACK_LP_CONTACT 0x496970
.export_equ MK4_NATIVE_CAGE_HIGH_ORB 0x497070
.export_equ MK4_NATIVE_CAGE_LOW_ORB 0x497080
.export_equ MK4_NATIVE_CAGE_ORB_SPAWN 0x4970d0

; Retail source 0x004de318; branches use linked names.
cage_lk_throw_damage:
  playback_single 0x2a
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_RELEASE
  .zero 4 ; non-executable guard

; Retail source 0x004de328; branches use linked names.
cage_lk_throw_release:
  damage 0x0, 0x2e14
  disarm_peer 0x0
  sound_a 0x24
  sound_roster 0x8
  spawn_effect 0x0
  playback_single 0x41
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  native_step 0x4d
  tail 0x0, MK4_NATIVE_MMOVE_LK_OWNER_READY
  .zero 4 ; non-executable guard

; Retail source 0x004de3d0; branches use linked names.
cage_lk_throw_startup:
  native_release 0x27
  sound_indexed 0x38
  open_peer_clip 0x0, clip(535)
  open_clip 0x0, clip(534)
  playback_single 0x17
  damage 0x0, 0x1999
  native_step 0xae
  forward_count 0x3
  wait 0x10
  forward 0x0
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_OWNER_RETURN
  .zero 4 ; non-executable guard

; Retail source 0x004e12a0; branches use linked names.
cage_fatality_1_scene_setup:
  forward 0x0
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL
  .zero 4 ; non-executable guard

; Retail source 0x004e12b0; branches use linked names.
cage_fatality_1_scene_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(cage_fatality_1_scene_loop)

; Retail source 0x004e12c0; branches use linked names.
cage_torso_rip_male_victim:
  prefixed_to 0x23a, clip(410)
  jump 0x0, word(cage_torso_rip_female_victim_tail_1)

; Retail source 0x004e12d0; branches use linked names.
cage_torso_rip_female_victim:
  prefixed_to 0x23a, clip(411)

; Retail source 0x004e12d8; branches use linked names.
cage_torso_rip_female_victim_tail_1:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_59

; Retail source 0x004e12e0; branches use linked names.
cage_first_victim_59:
  forward_to 0x82
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_131
  .zero 4 ; non-executable guard

; Retail source 0x004e12f0; branches use linked names.
cage_first_victim_131:
  forward_to 0x93
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_148
  .zero 4 ; non-executable guard

; Retail source 0x004e1300; branches use linked names.
cage_first_victim_148:
  forward_to 0xa5
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_166
  .zero 4 ; non-executable guard

; Retail source 0x004e1310; branches use linked names.
cage_first_victim_166:
  forward_to 0xb8
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_185
  .zero 4 ; non-executable guard

; Retail source 0x004e1320; branches use linked names.
cage_first_victim_185:
  forward_to 0xcd
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_206
  .zero 4 ; non-executable guard

; Retail source 0x004e1330; branches use linked names.
cage_first_victim_206:
  forward_to 0xe0
  tail 0x0, MK4_NATIVE_CAGE_FIRST_SEVER
  .zero 4 ; non-executable guard

; Retail source 0x004e1340; branches use linked names.
cage_torso_rip_lower_finish:
  forward 0x0
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4 ; non-executable guard

; Retail source 0x004e1350; branches use linked names.
cage_first_sever:
  prefixed_to 0x23a, clip(408)
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_HALF

; Retail source 0x004e1360; branches use linked names.
cage_first_torso_half:
  forward_to 0x7a
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_123
  .zero 4 ; non-executable guard

; Retail source 0x004e1370; branches use linked names.
cage_first_torso_123:
  forward_to 0x8a
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_FINISH
  .zero 4 ; non-executable guard

; Retail source 0x004e1380; branches use linked names.
cage_first_torso_finish:
  forward 0x0
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4 ; non-executable guard

; Retail source 0x004e5870; branches use linked names.
cage_fatality_2_scene_entry:
  native_release 0x44
  wait 0x1e
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE

; Retail source 0x004e5880; branches use linked names.
cage_fatality_2_uppercut:
  wait 0x5
  scene_active 0x0
  sound_c 0x2
  wait 0x6
  spawn 0x0, 0x26b, MK4_NATIVE_CAGE_SECOND_CHARGE
  wait 0x6
  play_clip 0x1, clip(293)
  half_count 0x4
  spawn_effect 0x6
  sound_context 0x2
  spawn 0x0, 0x26b, MK4_NATIVE_CAGE_SECOND_FLASH
  half_count 0x1
  sound_c 0x5d
  sound_b 0x1
  native_return 0xb9
  tail 0x0, MK4_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS

; Retail source 0x004e58d8; branches use linked names.
cage_second_repeated_uppercuts:
  prefixed_to 0x23c, clip(111)
  native_release 0x50
  forward 0x0
  cursor_set 0x2a
  forward 0x0
  jump 0x0, word(raiden_victory_clip_111_cursor_set_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e5900; branches use linked names.
cage_second_victim_hit:
  clip 0x0, clip(130)
  blend 0x0, clip(206)
  play_clip 0x14, clip(206)
  sound_special 0x0
  forward_to 0x5a
  sound_special 0x0
  tail 0x0, MK4_NATIVE_CAGE_SECOND_VICTIM_FINISH
  .zero 4 ; non-executable guard

; Retail source 0x004e5b38; branches use linked names.
cage_fatality_2_victim_start:
  forward 0x0
  tail 0x0, MK4_NATIVE_JAX_FATALITY_2_VICTIM_PAUSE
  .zero 4 ; non-executable guard

; Retail source 0x004e5ba8; branches use linked names.
groin_punch_groin_recover:
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e6270; branches use linked names.
cage_air_kick_narrow_miss_reversal:
  clip_operation 0x0, clip(327)
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_LAND
  jump 0, word(air_punch_contact)
  .zero 4 ; non-executable guard

; Retail source 0x004e62c0; branches use linked names.
cage_draw_weapon_draw:
  sound_variant 0x0
  play_clip 0x16, clip(244)
  tail 0x0, MK4_NATIVE_WEAPON_DRAW_EQUIP
  .zero 4 ; non-executable guard

; Retail source 0x004e6330; branches use linked names.
cage_jab_second_punch:
  cursor_set 0x0
  forward_count 0xa
  call 0x0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_recovery_action_recovery_1)

; Retail source 0x004e634c; branches use linked names.
cage_jab_second_punch_sound_a_1:
  sound_a 0xa
  forward 0x0
  tail 0x0, MK4_NATIVE_CAGE_JAB_REPEAT
  .zero 4 ; non-executable guard

; Retail source 0x004e6360; branches use linked names.
cage_jab_repeat:
  wait 0x1
  call 0x0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_repeat_tail_1)
  sound_a 0xa
  play_clip 0xa, clip(295)
  call 0x0, MK4_NATIVE_JAB_CONTACT
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(cage_jab_recovery_action_recovery_1)
  jump 0x0, word(cage_jab_second_punch_sound_a_1)

; Retail source 0x004e63a0; branches use linked names.
cage_jab_repeat_tail_1:
  tail 0x0, MK4_NATIVE_CAGE_JAB_RECOVERY
  .zero 4 ; non-executable guard

; Retail source 0x004e63b0; branches use linked names.
cage_jab_recovery:
  action_recovery 0x0
  open_clip 0x9, clip(294)
  double_to 0x11
  native_sound 0x0
  ready 0x0

; Retail source 0x004e63c8; branches use linked names.
cage_jab_recovery_action_recovery_1:
  action_recovery 0x0
  forward_count 0x3
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e6860; branches use linked names.
cage_roundhouse_fast_start:
  double_to 0xc
  sound_c 0x1c
  double_to 0x15
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e6878; branches use linked names.
cage_roundhouse_normal_start:
  double_to 0xc
  sound_c 0x1c
  forward_to 0x15
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e80a8; branches use linked names.
cage_cpu_kick:
  speed 0x0, 0x2666
  sound_c 0x51
  open_clip 0x0, clip(308)
  double_to 0x16
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e80d0; branches use linked names.
cage_powered_punch:
  sound_c 0x51
  open_clip 0x0, clip(85)
  double_to 0xe
  speed 0x0, 0x2b85
  double_to 0x18
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e8d38; branches use linked names.
cage_weapon_reaction_lp_victim:
  effect 0x0
  sound_context 0x6
  sound_b 0x1
  motion 0x0, 0xfffff852, 0xffffc7af, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4 ; non-executable guard

; Retail source 0x004e9718; branches use linked names.
cage_shadow_victim:
  effect 0x11
  sound_b 0x3
  spawn_effect 0x0
  sound_context 0x0
  motion 0x0, 0xffffe667, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4 ; non-executable guard

; Retail source 0x004e9f78; branches use linked names.
groin_punch_groin_victim:
  speed_negative 0x0, 0x28f5
  spawn_effect 0x0
  sound_context 0x12
  sound_b 0x3
  prefixed_to 0x20e, clip(126)
  playback_pair 0xa
  forward_to 0xe
  playback_pair 0xa
  forward_to 0xe
  playback_pair 0xa
  forward_to 0xe
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea040; branches use linked names.
cage_power_victim:
  sound_context 0x3
  play_clip 0x1b, clip(127)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea398; branches use linked names.
victim_disarm_raise:
  speed_negative 0x0, 0x1c28
  sound_b 0x1
  spawn_effect 0x0
  sound_context 0x0
  open_clip 0x0, clip(536)
  half_count 0x7
  tail 0x0, MK4_NATIVE_THROW_VICTIM_CONTINUE

; Retail source 0x004ea3c0; branches use linked names.
throw_victim_continue:
  forward_to 0xe
  landing 0x0
  tail 0x0, MK4_NATIVE_THROW_VICTIM_IMPACT

; Retail source 0x004ea3d0; branches use linked names.
throw_victim_impact:
  sound_special 0x0
  native_sound 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed0d8; branches use linked names.
cast_orb:
  sound_c 0x5f
  open_clip 0x0, clip(28)
  double_to 0x8
  forward_to 0x14
  spawn 0x0, 0x4f, MK4_NATIVE_CAGE_ORB_SPAWN
  forward 0x0
  action_followup 0x0
  wait 0x16
  ready 0x0
  .zero 4 ; non-executable guard

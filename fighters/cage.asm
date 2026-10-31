; Generated cage bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ native_cage_second_charge 0x413970
.export_equ native_cage_second_flash 0x413a10
.export_equ native_cage_lk_throw_entry 0x42b700
.export_equ native_cage_lk_throw_owner_return 0x42b730
.export_equ native_cage_lk_throw_paired_continuation 0x42bdf0
.export_equ native_cage_lk_throw_release 0x42be00
.export_equ native_cage_fatality_1 0x448160
.export_equ native_cage_script_fatality_1_signal 0x448480
.export_equ native_cage_first_victim_59 0x4487b0
.export_equ native_cage_first_victim_131 0x4487f0
.export_equ native_cage_first_victim_148 0x448830
.export_equ native_cage_first_victim_166 0x448870
.export_equ native_cage_first_victim_185 0x4488b0
.export_equ native_cage_first_victim_206 0x4488f0
.export_equ native_cage_first_sever 0x448930
.export_equ native_cage_first_torso_half 0x448c80
.export_equ native_cage_first_torso_123 0x448d20
.export_equ native_cage_first_torso_finish 0x448d70
.export_equ native_cage_can_groin_punch 0x45f120
.export_equ native_cage_fatality_2 0x465790
.export_equ native_cage_script_fatality_2_continue 0x4658a0
.export_equ native_cage_second_repeated_uppercuts 0x4658f0
.export_equ native_cage_second_victim_hit 0x465a80
.export_equ native_cage_second_victim_finish 0x465b10
.export_equ native_cage_groin_punch 0x468aa0
.export_equ native_cage_jab_second_punch 0x46dd00
.export_equ native_cage_jab_repeat 0x46dd10
.export_equ native_cage_jab_recovery 0x46dd20
.export_equ native_cage_cpu_kick 0x479b20
.export_equ native_cage_script_cpu_kick_tail 0x479b70
.export_equ native_cage_powered_punch 0x479ec0
.export_equ native_cage_script_powered_punch_tail 0x479f00
.export_equ native_cage_weapon_reaction_entry 0x482300
.export_equ native_cage_shadow_victim 0x4836b0
.export_equ native_cage_groin_punch_victim_reaction 0x4879c0
.export_equ native_cage_power_victim 0x488400
.export_equ native_cage_weapon_back_hp_entry 0x4957b0
.export_equ native_cage_weapon_lp_entry 0x495800
.export_equ native_cage_weapon_upper_entry 0x495860
.export_equ native_cage_weapon_upper_contact 0x4958b0
.export_equ native_cage_weapon_back_lp_entry 0x496920
.export_equ native_cage_weapon_back_lp_contact 0x496970
.export_equ native_cage_high_orb 0x497070
.export_equ native_cage_low_orb 0x497080
.export_equ native_cage_orb_spawn 0x4970d0

; Retail source 0x004de318; branches use linked names.
cage_lk_throw_damage:
  playback_single 0x2a
  tail 0x0, native_cage_lk_throw_release
  .zero 4 ; non-executable guard

; Retail source 0x004de328; branches use linked names.
cage_lk_throw_release:
  damage 0x0, 0x2e14
  disarm_peer 0x0
  sound_a 0x24
  sound_roster 0x8
  spawn_effect 0x0
  playback_single 0x41
  handler 0x0, native_velocity_friction
  native_step 0x4d
  tail 0x0, native_mmove_lk_owner_ready
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
  tail 0x0, native_cage_lk_throw_owner_return
  .zero 4 ; non-executable guard

; Retail source 0x004e12a0; branches use linked names.
cage_fatality_1_scene_setup:
  forward 0x0
  tail 0x0, native_cage_script_fatality_1_signal
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
  tail 0x0, native_cage_first_victim_59

; Retail source 0x004e12e0; branches use linked names.
cage_first_victim_59:
  forward_to 0x82
  tail 0x0, native_cage_first_victim_131
  .zero 4 ; non-executable guard

; Retail source 0x004e12f0; branches use linked names.
cage_first_victim_131:
  forward_to 0x93
  tail 0x0, native_cage_first_victim_148
  .zero 4 ; non-executable guard

; Retail source 0x004e1300; branches use linked names.
cage_first_victim_148:
  forward_to 0xa5
  tail 0x0, native_cage_first_victim_166
  .zero 4 ; non-executable guard

; Retail source 0x004e1310; branches use linked names.
cage_first_victim_166:
  forward_to 0xb8
  tail 0x0, native_cage_first_victim_185
  .zero 4 ; non-executable guard

; Retail source 0x004e1320; branches use linked names.
cage_first_victim_185:
  forward_to 0xcd
  tail 0x0, native_cage_first_victim_206
  .zero 4 ; non-executable guard

; Retail source 0x004e1330; branches use linked names.
cage_first_victim_206:
  forward_to 0xe0
  tail 0x0, native_cage_first_sever
  .zero 4 ; non-executable guard

; Retail source 0x004e1340; branches use linked names.
cage_torso_rip_lower_finish:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Retail source 0x004e1350; branches use linked names.
cage_first_sever:
  prefixed_to 0x23a, clip(408)
  tail 0x0, native_cage_first_torso_half

; Retail source 0x004e1360; branches use linked names.
cage_first_torso_half:
  forward_to 0x7a
  tail 0x0, native_cage_first_torso_123
  .zero 4 ; non-executable guard

; Retail source 0x004e1370; branches use linked names.
cage_first_torso_123:
  forward_to 0x8a
  tail 0x0, native_cage_first_torso_finish
  .zero 4 ; non-executable guard

; Retail source 0x004e1380; branches use linked names.
cage_first_torso_finish:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Retail source 0x004e5870; branches use linked names.
cage_fatality_2_scene_entry:
  native_release 0x44
  wait 0x1e
  tail 0x0, native_cage_script_fatality_2_continue

; Retail source 0x004e5880; branches use linked names.
cage_fatality_2_uppercut:
  wait 0x5
  scene_active 0x0
  sound_c 0x2
  wait 0x6
  spawn 0x0, 0x26b, native_cage_second_charge
  wait 0x6
  play_clip 0x1, clip(293)
  half_count 0x4
  spawn_effect 0x6
  sound_context 0x2
  spawn 0x0, 0x26b, native_cage_second_flash
  half_count 0x1
  sound_c 0x5d
  sound_b 0x1
  native_return 0xb9
  tail 0x0, native_cage_second_repeated_uppercuts

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
  tail 0x0, native_cage_second_victim_finish
  .zero 4 ; non-executable guard

; Retail source 0x004e5b38; branches use linked names.
cage_fatality_2_victim_start:
  forward 0x0
  tail 0x0, native_jax_fatality_2_victim_pause
  .zero 4 ; non-executable guard

; Retail source 0x004e5ba8; branches use linked names.
groin_punch_groin_recover:
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e6270; branches use linked names.
cage_air_kick_narrow_miss_reversal:
  clip_operation 0x0, clip(327)
  tail 0x0, native_air_straight_land
  jump 0, word(air_punch_contact)
  .zero 4 ; non-executable guard

; Retail source 0x004e62c0; branches use linked names.
cage_draw_weapon_draw:
  sound_variant 0x0
  play_clip 0x16, clip(244)
  tail 0x0, native_weapon_draw_equip
  .zero 4 ; non-executable guard

; Retail source 0x004e6330; branches use linked names.
cage_jab_second_punch:
  cursor_set 0x0
  forward_count 0xa
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_recovery_action_recovery_1)

; Retail source 0x004e634c; branches use linked names.
cage_jab_second_punch_sound_a_1:
  sound_a 0xa
  forward 0x0
  tail 0x0, native_cage_jab_repeat
  .zero 4 ; non-executable guard

; Retail source 0x004e6360; branches use linked names.
cage_jab_repeat:
  wait 0x1
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_repeat_tail_1)
  sound_a 0xa
  play_clip 0xa, clip(295)
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_recovery_action_recovery_1)
  jump 0x0, word(cage_jab_second_punch_sound_a_1)

; Retail source 0x004e63a0; branches use linked names.
cage_jab_repeat_tail_1:
  tail 0x0, native_cage_jab_recovery
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
  tail 0x0, native_roundhouse_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e6878; branches use linked names.
cage_roundhouse_normal_start:
  double_to 0xc
  sound_c 0x1c
  forward_to 0x15
  tail 0x0, native_roundhouse_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e80a8; branches use linked names.
cage_cpu_kick:
  speed 0x0, 0x2666
  sound_c 0x51
  open_clip 0x0, clip(308)
  double_to 0x16
  tail 0x0, native_cage_script_cpu_kick_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e80d0; branches use linked names.
cage_powered_punch:
  sound_c 0x51
  open_clip 0x0, clip(85)
  double_to 0xe
  speed 0x0, 0x2b85
  double_to 0x18
  tail 0x0, native_cage_script_powered_punch_tail
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
  tail 0x0, native_jax_dash_punch_roll_getup
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
  tail 0x0, native_mmove_side_getup
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
  tail 0x0, native_throw_victim_continue

; Retail source 0x004ea3c0; branches use linked names.
throw_victim_continue:
  forward_to 0xe
  landing 0x0
  tail 0x0, native_throw_victim_impact

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
  spawn 0x0, 0x4f, native_cage_orb_spawn
  forward 0x0
  action_followup 0x0
  wait 0x16
  ready 0x0
  .zero 4 ; non-executable guard

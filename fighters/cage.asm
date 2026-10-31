; Generated cage bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
cage_lk_throw_damage:
  playback_single 0x2a
  tail 0x0, native_cage_lk_throw_release
  .zero 4 ; non-executable guard

; Branches use linked names.
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

; Branches use linked names.
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

; Branches use linked names.
cage_fatality_1_scene_setup:
  forward 0x0
  tail 0x0, native_cage_script_fatality_1_signal
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_fatality_1_scene_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(cage_fatality_1_scene_loop)

; Branches use linked names.
cage_torso_rip_male_victim:
  prefixed_to 0x23a, clip(410)
  jump 0x0, word(cage_torso_rip_female_victim_tail_1)

; Branches use linked names.
cage_torso_rip_female_victim:
  prefixed_to 0x23a, clip(411)

; Branches use linked names.
cage_torso_rip_female_victim_tail_1:
  tail 0x0, native_cage_first_victim_59

; Branches use linked names.
cage_first_victim_59:
  forward_to 0x82
  tail 0x0, native_cage_first_victim_131
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_victim_131:
  forward_to 0x93
  tail 0x0, native_cage_first_victim_148
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_victim_148:
  forward_to 0xa5
  tail 0x0, native_cage_first_victim_166
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_victim_166:
  forward_to 0xb8
  tail 0x0, native_cage_first_victim_185
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_victim_185:
  forward_to 0xcd
  tail 0x0, native_cage_first_victim_206
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_victim_206:
  forward_to 0xe0
  tail 0x0, native_cage_first_sever
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_torso_rip_lower_finish:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_sever:
  prefixed_to 0x23a, clip(408)
  tail 0x0, native_cage_first_torso_half

; Branches use linked names.
cage_first_torso_half:
  forward_to 0x7a
  tail 0x0, native_cage_first_torso_123
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_torso_123:
  forward_to 0x8a
  tail 0x0, native_cage_first_torso_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_first_torso_finish:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_fatality_2_scene_entry:
  native_release 0x44
  wait 0x1e
  tail 0x0, native_cage_script_fatality_2_continue

; Branches use linked names.
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

; Branches use linked names.
cage_second_repeated_uppercuts:
  prefixed_to 0x23c, clip(111)
  native_release 0x50
  forward 0x0
  cursor_set 0x2a
  forward 0x0
  jump 0x0, word(raiden_victory_clip_111_cursor_set_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_second_victim_hit:
  clip 0x0, clip(130)
  blend 0x0, clip(206)
  play_clip 0x14, clip(206)
  sound_special 0x0
  forward_to 0x5a
  sound_special 0x0
  tail 0x0, native_cage_second_victim_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_fatality_2_victim_start:
  forward 0x0
  tail 0x0, native_jax_fatality_2_victim_pause
  .zero 4 ; non-executable guard

; Branches use linked names.
groin_punch_groin_recover:
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_air_kick_narrow_miss_reversal:
  clip_operation 0x0, clip(327)
  tail 0x0, native_air_straight_land
  jump 0, word(air_punch_contact)
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_draw_weapon_draw:
  sound_variant 0x0
  play_clip 0x16, clip(244)
  tail 0x0, native_weapon_draw_equip
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_jab_second_punch:
  cursor_set 0x0
  forward_count 0xa
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_recovery_action_recovery_1)

; Branches use linked names.
cage_jab_second_punch_sound_a_1:
  sound_a 0xa
  forward 0x0
  tail 0x0, native_cage_jab_repeat
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_jab_repeat:
  wait 0x1
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_repeat_tail_1)
  sound_a 0xa
  play_clip 0xa, clip(295)
  call 0x0, native_jab_contact
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_recovery_action_recovery_1)
  jump 0x0, word(cage_jab_second_punch_sound_a_1)

; Branches use linked names.
cage_jab_repeat_tail_1:
  tail 0x0, native_cage_jab_recovery
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_jab_recovery:
  action_recovery 0x0
  open_clip 0x9, clip(294)
  double_to 0x11
  native_sound 0x0
  ready 0x0

; Branches use linked names.
cage_jab_recovery_action_recovery_1:
  action_recovery 0x0
  forward_count 0x3
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_roundhouse_fast_start:
  double_to 0xc
  sound_c 0x1c
  double_to 0x15
  tail 0x0, native_roundhouse_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_roundhouse_normal_start:
  double_to 0xc
  sound_c 0x1c
  forward_to 0x15
  tail 0x0, native_roundhouse_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_cpu_kick:
  speed 0x0, 0x2666
  sound_c 0x51
  open_clip 0x0, clip(308)
  double_to 0x16
  tail 0x0, native_cage_script_cpu_kick_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
cage_powered_punch:
  sound_c 0x51
  open_clip 0x0, clip(85)
  double_to 0xe
  speed 0x0, 0x2b85
  double_to 0x18
  tail 0x0, native_cage_script_powered_punch_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
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

; Branches use linked names.
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

; Branches use linked names.
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

; Branches use linked names.
cage_power_victim:
  sound_context 0x3
  play_clip 0x1b, clip(127)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_disarm_raise:
  speed_negative 0x0, 0x1c28
  sound_b 0x1
  spawn_effect 0x0
  sound_context 0x0
  open_clip 0x0, clip(536)
  half_count 0x7
  tail 0x0, native_throw_victim_continue

; Branches use linked names.
throw_victim_continue:
  forward_to 0xe
  landing 0x0
  tail 0x0, native_throw_victim_impact

; Branches use linked names.
throw_victim_impact:
  sound_special 0x0
  native_sound 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
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

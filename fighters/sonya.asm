; Generated sonya bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
sonya_lk_throw_startup:
  sound_indexed 0x13
  open_peer_clip 0x0, clip(163)
  open_clip 0x0, clip(43)
  playback_single 0x6
  playback_single 0x18
  native_release 0x3
  playback_single 0x2c
  damage 0x0, 0x2e14
  spawn_effect 0x0
  disarm_peer 0x0
  sound_a 0x24
  sound_roster 0x8
  playback_single 0x4b
  native_step 0x4e
  tail 0x0, native_mmove_lk_blend_return
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_first_fatality:
  prefixed_to 0x22a, clip(388)
  tail 0x0, native_sonya_script_first_release

; Branches use linked names.
sonya_fatality_1_owner_loop:
  forward 0x0
  cursor_set 0x54
  jump 0x0, word(sonya_fatality_1_owner_loop)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_second_fatality:
  native_release 0x4f
  wait 0x28
  tail 0x0, native_sonya_script_second_open

; Branches use linked names.
sonya_fatality_2_opening:
  prefixed_finish 0x200, clip(558)
  native_return 0xb8
  scene_active 0x0
  clip 0x0, clip(559)
  tail 0x0, native_sonya_script_second_repeat

; Branches use linked names.
sonya_fatality_2_cycle:
  spawn 0x0, 0x26b, native_scene_keepalive_task
  play_clip 0x18, clip(560)
  wait 0x6
  tail 0x0, native_sonya_script_second_branch

; Branches use linked names.
sonya_fatality_2_first_cycle:
  playback_tail 0x0
  tail 0x0, native_sonya_script_second_pause
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_fatality_2_second_cycle:
  playback_tail 0x0
  wait 0xf
  clip 0x0, clip(560)
  tail 0x0, native_sonya_script_second_cycle_end

; Branches use linked names.
sonya_fatality_2_owner_ending:
  forward 0x0
  prefixed_finish 0x200, clip(114)

; Branches use linked names.
sonya_fatality_2_owner_ending_playback_pair_1:
  playback_pair 0x45
  forward_to 0x63
  jump 0x0, word(sonya_fatality_2_owner_ending_playback_pair_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_fatality_2_victim:
  clip 0x0, clip(561)
  tail 0x0, native_sonya_script_second_victim_end
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_fatality_2_victim_end:
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_cartwheel:
  sound_indexed 0x20
  sound_c 0x6
  speed 0x0, 0x1999
  prefixed_to 0x20e, clip(104)
  tail 0x0, native_sonya_script_cartwheel_tail
  jump 0, word(tanya_drill_kick)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_leg_grab_paired_grab:
  native_release 0x25
  sound_roster 0x1
  open_peer_clip 0x0, clip(201)
  playback_single 0x16
  playback_pair_reverse 0x13
  playback_single 0x16
  playback_pair_reverse 0x13
  playback_single 0x22
  native_step 0x4f
  tail 0x0, native_sonya_script_leg_grab_tail

; Branches use linked names.
sonya_leg_grab_miss_or_block:
  forward_to 0x16
  playback_pair 0x13
  forward_to 0x16
  playback_pair 0x13
  forward 0x0
  ready 0x0

; Branches use linked names.
sonya_square_wave:
  speed 0x0, 0x1999
  prefixed_finish 0x200, clip(72)
  sound_indexed 0xe
  bind_cursor 0x0, clip(73)
  motion_block 0x0, 0x1999, 0x0, 0xffffe667, 0xffffff7d
  tail 0x0, native_sonya_script_square_wave_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_weapon_hp_victim:
  sound_c 0x8
  sound_b 0x1
  effect 0x5
  clip 0x0, clip(136)
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  jump 0, word(kai_draw_weapon_hp_victim)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_weapon_lp_victim:
  effect 0x0
  sound_b 0x1
  speed_negative 0x0, 0x11eb
  play_clip 0xe, clip(132)
  native_context 0x0
  sound_a 0x4
  landing 0x0
  forward_count 0xe
  tail 0x0, native_reaction_getup_back
  jump 0, word(weapon_tumble_back_getup)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_crouch_hk_victim:
  speed_negative 0x0, 0x3333
  handler 0x0, native_velocity_friction
  clip 0x0, clip(134)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_leg_grab_victim_recovery:
  motion 0x0, 0x1c28, 0xffffe667, 0x24d
  sound_special 0x0
  task_callback 0x0
  forward 0x0
  tail 0x0, native_sonya_script_leg_victim_end
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_ring_victim:
  handler 0x0, native_velocity_friction
  sound_b 0x1
  speed_negative 0x0, 0x4000
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_cartwheel_victim:
  sound_context 0x1
  sound_b 0x3
  motion 0x0, 0xffffe667, 0xffffd47b, 0x28f
  task_callback 0x0
  open_clip 0x0, clip(131)
  double_to 0xd
  forward_to 0x29
  nop 0x13ece8
  native_context 0x0
  forward_count 0x4
  tail 0x0, native_mmove_side_getup
  jump 0, word(bicycle_capture)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_bicycle_victim:
  effect 0x8
  effect 0x9
  sound_context 0x2
  spawn_effect 0x0
  task_callback 0x0
  motion 0x0, 0xffffe667, 0x666, 0x147
  clip 0x0, clip(186)
  native_context 0x0
  sound_special 0x0
  landing 0x0
  tail 0x0, native_getup_front
  jump 0, word(kai_handstand_reaction_knockdown)
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_weapon_back_hp_attacker:
  open_clip 0x0, clip(228)
  forward_to 0x6
  sound_c 0x1e
  forward_to 0xe
  tail 0x0, native_liu_kang_sword_back_high_punch_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
sonya_ring:
  sound_indexed 0x11
  open_clip 0x0, clip(15)
  fast_start 0x0, word(sonya_ring_double_to_1)
  double_to 0xa
  forward_to 0xf

; Branches use linked names.
sonya_ring_spawn_1:
  spawn 0x0, 0x44, native_sonya_ring_object
  spawn 0x0, 0x26, native_sonya_script_ring_flash
  sound_c 0x4
  forward_to 0x13
  action_followup 0x0
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0x0
  ready 0x0

; Branches use linked names.
sonya_ring_double_to_1:
  double_to 0xf
  jump 0x0, word(sonya_ring_spawn_1)
  .zero 4 ; non-executable guard

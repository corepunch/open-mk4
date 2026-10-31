; Generated kai bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
kai_lk_throw_startup:
  native_release 0xd
  open_peer_clip 0x0, clip(159)
  open_clip 0x0, clip(34)
  playback_single 0x48
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x8
  damage 0x0, 0x3333
  native_step 0x6e
  native_word 0x0, 0x1999
  forward 0x0
  tail 0x0, native_lk_pause_short
  jump 0, word(fujin_lk_throw_startup)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_fatality_2:
  bind_cursor 0x0, clip(493)
  scene_event 0x2d
  wait 0x28
  spawn 0x0, 0x26, native_scene_hover
  native_release 0x2e
  wait 0x3c
  sound_indexed 0x3a
  scene_active 0x0
  wait 0x1e
  call 0x0, native_kai_second_child_walk
  prefixed_finish 0x200, clip(486)
  sound_c 0x33
  spawn 0x0, 0x26, native_kai_second_skull
  bind_cursor 0x0, clip(487)
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_fatality_1_owner_opening:
  prefixed_to 0x22d, clip(440)
  tail 0x0, native_kai_first_owner_grab

; Branches use linked names.
kai_first_owner_grab:
  forward_to 0x7b
  tail 0x0, native_kai_first_owner_spin
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_first_owner_spin:
  prefixed_finish 0x200, clip(445)
  prefixed_finish 0x200, clip(445)
  tail 0x0, native_kai_first_owner_tear

; Branches use linked names.
kai_fatality_1_owner_victory:
  forward 0x0

; Branches use linked names.
kai_fatality_1_owner_victory_prefixed_finish_1:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(kai_fatality_1_owner_victory_prefixed_finish_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_fatality_1_victim_opening:
  prefixed_to 0x246, clip(441)
  tail 0x0, native_kai_first_victim_71

; Branches use linked names.
kai_first_victim_71:
  forward_to 0x7b
  prefixed_finish 0x200, clip(444)
  tail 0x0, native_kai_first_victim_turn
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_first_victim_turn:
  prefixed_finish 0x200, clip(444)
  tail 0x0, native_kai_first_victim_sever

; Branches use linked names.
kai_fatality_1_victim_finish:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_fatality_1_torso_opening:
  prefixed_to 0x2ca, clip(442)
  tail 0x0, native_kai_first_torso_203

; Branches use linked names.
kai_first_torso_203:
  forward_to 0xf2
  tail 0x0, native_kai_first_torso_impact
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_first_torso_impact:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_second_victim_fall:
  prefixed_to 0x214, clip(206)
  sound_special 0x0
  forward_to 0x5a
  sound_special 0x0
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_high_kick_recovery:
  action_recovery 0x0
  forward_count 0x2
  release_attack 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_aerial_punch:
  sound_c 0x3c
  motion 0x0, 0x2e14, 0xffffe667, 0x41
  prefixed_to 0x209, clip(298)
  tail 0x0, native_kai_aerial_punch_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_handstand_enter:
  sound_indexed 0x17
  prefixed_finish 0x200, clip(1)
  tail 0x0, native_kai_script_stage_event
  .zero 4 ; non-executable guard

; Branches use linked names.
handstand_spin:
  prefixed_finish 0x200, clip(2)
  bind_cursor 0x0, clip(3)
  tail 0x0, native_kai_handstand_spin_begin
  .zero 4 ; non-executable guard

; Branches use linked names.
handstand_front:
  sound_a 0x1
  prefixed_to 0x209, clip(0)
  tail 0x0, native_kai_script_contact_b10_continuation
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_contact_b10_continuation:
  wait 0x8
  playback_pair 0x0
  tail 0x0, native_kai_script_stage_event

; Branches use linked names.
handstand_rear:
  sound_a 0x1
  prefixed_to 0x209, clip(6)
  tail 0x0, native_kai_handstand_rear_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_handstand_rear_contact:
  playback_pair 0x0
  tail 0x0, native_kai_script_stage_event
  .zero 4 ; non-executable guard

; Branches use linked names.
handstand_exit:
  sound_indexed 0x17
  clip_operation 0x0, clip(1)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_lunge_kick:
  sound_indexed 0x3b
  open_clip 0x0, clip(551)
  fast_start 0x0, word(kai_armed_handstand_enter_cursor_set_1)

; Branches use linked names.
kai_lunge_kick_double_to_1:
  double_to 0x1a
  sound_c 0x1f
  speed 0x0, 0x2666
  forward_to 0x1d
  tail 0x0, native_kai_lunge_kick_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_armed_handstand_enter:
  sound_indexed 0x17
  open_clip 0x0, clip(1)
  native_sound 0x0
  tail 0x0, native_handstand_front

; Branches use linked names.
kai_armed_handstand_enter_cursor_set_1:
  cursor_set 0xe
  jump 0x0, word(kai_lunge_kick_double_to_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_draw_weapon_hp_victim:
  sound_b 0x1
  effect 0x5
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_draw_weapon_back_victim:
  sound_context 0x6
  effect 0x1

; Branches use linked names.
kai_draw_weapon_back_victim_spawn_effect_1:
  spawn_effect 0x0
  sound_b 0x1
  reaction_context 0x0
  motion_block 0x0, 0xfffff5c3, 0xccc, 0x0, 0x0
  handler 0x0, native_face_peer_orient_group
  open_clip 0x0, clip(182)
  native_sound 0x0
  landing 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_handstand_reaction_standing_stagger:
  effect 0xa
  sound_b 0x2
  sound_context 0x3
  speed_negative 0x0, 0x2666
  handler 0x0, native_velocity_friction
  open_clip 0x0, clip(134)
  forward_count 0x6
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
crouch_reaction_script:
  sound_a 0x14
  speed_negative 0x0, 0x2b85
  prefixed_to 0x204, clip(254)
  playback_pair 0x0
  landing 0x0
  tail 0x0, native_crouch_return
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_saved_motion:
  spawn_effect 0x0
  prefixed_to 0x20b, clip(177)
  call 0x0, native_restore_motion
  forward_to 0x22
  sound_special 0x0
  landing 0x0
  forward 0x0
  wait 0x6
  tail 0x0, native_jax_ground_pound_controller_return
  jump 0, word(victim_ground_turn)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_handstand_reaction_knockdown:
  effect 0x4
  sound_b 0x3
  sound_context 0x1
  spawn_effect 0x0
  motion 0x0, 0xffffe667, 0xffffd47b, 0x28f
  task_callback 0x0
  open_clip 0x0, clip(131)
  double_to 0xd
  forward_to 0x29
  nop 0x13ece8
  native_context 0x0
  forward_count 0x4
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_handstand_reaction_rear_stagger:
  effect 0xa
  sound_b 0x2
  sound_context 0x3
  speed_negative 0x0, 0x3333
  open_clip 0x0, clip(127)
  forward_count 0x14
  tail 0x0, native_kai_handstand_victim_tail
  jump 0, word(mmove_side_getup)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_second_victim:
  effect 0x0
  effect 0x8
  sound_c 0x69
  spawn_effect 0x6
  sound_b 0x8
  sound_c 0x34
  prefixed_to 0x20a, clip(130)
  half_finish 0x0
  native_release 0x2f
  tail 0x0, native_kai_second_victim_fall
  jump 0, word(shinnok_second_victim)
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_draw_weapon_back_strike:
  open_clip 0x0, clip(226)
  double_to 0x10
  sound_c 0x1f
  double_to 0x1a
  forward_to 0x1d
  tail 0x0, native_kai_weapon_back_strike_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_ground_skull:
  prefixed_finish 0x200, clip(22)
  sound_c 0x33
  motion 0x0, 0x0, 0xffffcccd, 0x28f
  tail 0x0, native_kai_ground_skull_spawn
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_rising_skull:
  prefixed_to 0x207, clip(21)
  sound_c 0x33
  tail 0x0, native_kai_rising_skull_spawn
  .zero 4 ; non-executable guard

; Branches use linked names.
kai_rising_skull_spawn:
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  playback_pair 0x4
  forward_to 0x6
  tail 0x0, native_kai_rising_skull_finish
  .zero 4 ; non-executable guard

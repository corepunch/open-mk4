; Generated fujin bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
fujin_lk_throw_startup:
  native_release 0x7
  open_peer_clip 0x0, clip(155)
  open_clip 0x0, clip(36)
  playback_single 0x31
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x8
  damage 0x0, 0x2e14
  playback_single 0x2e
  native_release 0x8
  playback_single 0x56
  clear_handler 0x0
  native_step 0x5b
  forward_to 0x59
  wait 0xc
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_second_fatality:
  wait 0x3c
  sound_indexed 0x3c
  native_release 0x37
  prefixed_finish 0x200, clip(547)
  bind_cursor 0x0, clip(548)
  wait 0x8
  scene_active 0x0
  spawn 0x0, 0x26, native_fujin_second_blood
  native_return 0xb5
  park 0x0
  jump 0, word(reptile_fatality_2)
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_first_fatality:
  half_restart 0x200, clip(398)
  tail 0x0, native_fujin_first_loop_start

; Branches use linked names.
wind_finish_cycle_script:
  prefixed_finish 0x200, clip(399)
  tail 0x0, native_fujin_first_cycle
  .zero 4 ; non-executable guard

; Branches use linked names.
wind_finish_exit_script:
  prefixed_finish 0x200, clip(210)
  prefixed_finish 0x200, clip(212)
  tail 0x0, native_fujin_first_shoot

; Branches use linked names.
fujin_fatality_1_crossbow_shot:
  prefixed_finish 0x200, clip(213)
  tail 0x0, native_task_retire
  jump 0, word(dragon_victim_script)
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_wind_slam:
  native_return 0x61
  prefixed_to 0x206, clip(99)
  forward 0x0
  damage 0x0, 0x147a
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_wind_recovery:
  forward 0x0
  playback_pair 0x22
  forward_to 0x28
  playback_pair 0x22
  forward_to 0x28
  playback_pair 0x1e
  ready 0x0

; Branches use linked names.
fujin_wind_recovery_open_clip_1:
  open_clip 0x0, clip(98)
  double_to 0x10
  jump 0x0, word(fujin_wind_query_d_1)

; Branches use linked names.
fujin_wind:
  sound_c 0x3f
  fast_start 0x0, word(fujin_wind_recovery_open_clip_1)
  prefixed_to 0x210, clip(98)

; Branches use linked names.
fujin_wind_query_d_1:
  query_d 0x8
  tail 0x0, native_fujin_wind_recovery
  jump 0, word(jax_high_kick_recovery)
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_high_kick_recovery:
  backward_count 0x2
  release_attack 0x0
  tail 0x0, native_high_kick_reverse_return
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_dive:
  sound_c 0x39
  motion 0x0, 0x1eb8, 0xf5c, 0x41
  prefixed_finish 0x200, clip(58)
  tail 0x0, native_fujin_dive_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
dive_recoil:
  landing 0x0
  wait 0x8
  allow_commands 0x0
  gravity 0x0, 0x51e
  native_context 0x0
  tail 0x0, native_fujin_dive_land
  jump 0, word(teleport)
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_triple_kick:
  fast_start 0x0, word(fujin_triple_kick_tail_1)
  prefixed_to 0x206, clip(100)
  speed 0x0, 0x2b85
  forward_to 0xd
  tail 0x0, native_fujin_triple_contact

; Branches use linked names.
fujin_triple_kick_tail_1:
  tail 0x0, native_fujin_triple_fast
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_triple_fast:
  open_clip 0x0, clip(100)
  double_to 0x6
  speed 0x0, 0x2b85
  double_to 0xd
  tail 0x0, native_fujin_triple_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_wind_grab:
  sound_indexed 0x1e
  sound_c 0x3d
  prefixed_finish 0x200, clip(10)
  bind_cursor 0x0, clip(11)
  tail 0x0, native_fujin_grab_control_start
  jump 0, word(sword_spin_script)
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_ground_turn:
  speed_negative 0x0, 0x2666
  spawn_effect 0x6
  sound_a 0x3
  prefixed_finish 0x200, clip(170)
  call 0x0, native_impact_voice
  prefixed_finish 0x200, clip(138)
  handler 0x0, native_velocity_friction
  prefixed_finish 0x200, clip(139)
  wait 0xa
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_bolt_victim:
  speed_negative 0x0, 0x2666
  bind_submit 0x4, clip(311)
  handler 0x0, native_velocity_friction
  wait 0x4
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
orbit_release:
  motion 0x0, 0xffffe667, 0xffffd99a, 0x353
  task_callback 0x0
  prefixed_finish 0x200, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
vortex_landed:
  sound_special 0x0
  jump 0x0, word(shared_clip_entry)
  .zero 4 ; non-executable guard

; Branches use linked names.
slam_landed:
  sound_special 0x0
  wait 0x4
  motion 0x0, 0x11eb, 0xffffcf5d, 0x28f
  task_callback 0x0
  play_clip 0x27, clip(179)
  native_context 0x0
  forward 0x0
  tail 0x0, native_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_triple_victim:
  effect 0xa
  sound_context 0x3
  bind_submit 0x4, clip(311)
  wait 0xd
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_fatality_2_victim_fall:
  prefixed_to 0x228, clip(550)
  sound_special 0x0
  forward 0x0
  wait 0xa
  face 0x0
  scene_event 0x3f
  tail 0x0, native_scene_body_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_second_victim:
  speed_negative 0x0, 0x28f
  bind_cursor 0x0, clip(549)
  wait 0x5a
  sound_b 0x8
  tail 0x0, native_fujin_second_burst
  jump 0, word(reaction_park_task)
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_crossbow_lp_shot_recovery:
  forward_to 0x8
  playback_pair 0x4
  forward_to 0x8
  playback_pair 0x4
  forward_to 0x8
  playback_pair 0x4
  tail 0x0, native_fujin_bow_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
fujin_crossbow_followup_short_shot:
  forward_to 0x8
  return 0x0
  .zero 4 ; non-executable guard

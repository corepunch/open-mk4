; Generated goro bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
goro_lp_throw_final_damage:
  native_release 0x16
  open_peer_clip 0x0, clip(152)
  open_clip 0x0, clip(75)
  playback_single 0x2a
  native_return 0x64
  native_word 0x0, 0x1eb8
  damage 0x0, 0x2666
  forward 0x0
  tail 0x0, native_lp_throw_victim_adjust_return
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_lk_throw_startup:
  call 0x0, native_throw_pair_start
  native_release 0x52
  open_peer_clip 0x0, clip(610)
  open_clip 0x0, clip(609)
  playback_single 0x53
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x8
  damage 0x0, 0x3333
  playback_single 0x8c
  tail 0x0, native_lp_align_and_release

; Branches use linked names.
goro_lk_throw_startup_call_1:
  call 0x0, native_throw_pair_start
  native_release 0x51
  open_peer_clip 0x0, clip(606)
  open_clip 0x0, clip(604)
  playback_single 0x2f
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  tail 0x0, native_goro_lp_throw_five_phase_sequence
  jump 0, word(lp_throw_script)
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_lp_throw_repeated_damage:
  open_peer_clip 0x0, clip(607)
  open_clip 0x0, clip(605)
  playback_single 0xb
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  effect 0x15
  damage 0x0, 0x7ae
  playback_single 0x17
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  damage 0x0, 0x7ae
  tail 0x0, native_goro_script_lp_phase_repeat
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_shared_release:
  jump 0x0, word(goro_lp_throw_final_damage)
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_special_punch:
  prefixed_to 0x20e, clip(599)
  tail 0x0, native_goro_script_punch_tail

; Branches use linked names.
goro_special_kick:
  prefixed_to 0x208, clip(598)
  tail 0x0, native_goro_script_kick_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_leap_stomp:
  open_clip 0x0, clip(601)
  double_to 0x17
  tail 0x0, native_goro_script_leap_continue
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_leap_stomp_stomp_hit:
  handler 0x0, native_goro_stomp_hold_separation
  prefixed_sample 0x2e
  native_context 0x0
  forward 0x0
  prefixed_to 0x219, clip(602)
  sound_special 0x0
  forward 0x0
  prefixed_to 0x19
  sound_special 0x0
  forward 0x0
  prefixed_to 0x8
  motion 0x0, 0xffffe148, 0xffffe148, 0x28f
  forward_to 0x19
  native_context 0x0
  call 0x0, native_disturb_resting_weapons
  sound_special 0x0
  landing 0x0
  forward 0x0
  tail 0x0, native_goro_script_stomp_return

; Branches use linked names.
goro_leap_stomp_miss_landing:
  sound_special 0x0
  forward 0x0
  prefixed_finish 0x200, clip(603)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
quake_jump:
  prefixed_to 0x215, clip(602)
  tail 0x0, native_goro_script_quake_land

; Branches use linked names.
goro_quake_land:
  sound_special 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_special_punch_189_victim:
  effect 0x10
  spawn_effect 0x0
  sound_b 0x1
  sound_context 0x2
  reaction_context 0x0
  task_callback 0x0
  motion_block 0x0, 0x0, 0x1999, 0xffffdeb9, 0x20c
  play_clip 0x22, clip(319)
  native_context 0x0
  forward_count 0x8
  tail 0x0, native_reaction_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_special_uppercut_192_victim:
  effect 0x3
  effect 0x8
  effect 0x8
  sound_b 0x1
  sound_context 0x0
  motion 0x0, 0x0, 0xffffc7af, 0x20c
  tail 0x0, native_goro_script_uppercut_victim_slide
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_uppercut_victim_slide:
  sound_a 0x0
  effect 0x0
  play_clip 0x33, clip(181)
  native_context 0x0
  sound_a 0x17
  forward 0x0
  tail 0x0, native_goro_victim_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_quake_victim:
  sound_b 0x1
  speed_negative 0x0, 0x1999
  open_clip 0x0, clip(182)
  double_to 0x26
  landing 0x0
  double_to 0x32
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_kick_victim:
  sound_context 0x0
  spawn_effect 0x6
  motion 0x0, 0xffffe148, 0xffffd47b, 0x24d
  jump 0x0, word(jarek_ball_victim_task_callback_1)

; Branches use linked names.
goro_stomp_victim:
  sound_context 0xd
  sound_b 0x1
  prefixed_finish 0x200, clip(178)
  tail 0x0, native_goro_script_stomp_victim_hold
  .zero 4 ; non-executable guard

; Branches use linked names.
goro_fireball:
  open_clip 0x0, clip(592)
  forward_to 0xe
  sound_a 0x21
  spawn 0x0, 0x53, native_goro_fireball_spawn
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

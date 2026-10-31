; Generated jarek bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
jarek_fatality_2:
  native_release 0x3b
  play_clip 0x1, clip(577)
  scene_active 0x0
  half_count 0x6
  call 0x0, native_jarek_second_glow
  half_count 0x13
  sound_b 0x1
  tail 0x0, native_jarek_second_handoff
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_second_handoff:
  half_count 0xc
  sound_c 0xb
  half_count 0x14
  sound_c 0xb
  half_count 0x14
  sound_c 0xb
  half_count 0x14
  sound_c 0xb
  half_count 0x14
  sound_c 0xb
  tail 0x0, native_victory_continue

; Branches use linked names.
victim_start:
  sound_c 0xb
  play_clip 0x10, clip(182)
  backward_count 0xf
  forward 0x0
  sound_b 0xc
  spawn 0x0, 0x0, native_jarek_second_head_fall
  prefixed_to 0x214, clip(206)
  sound_special 0x0
  forward_to 0x5a
  forward 0x0
  tail 0x0, native_jarek_second_victim_hold
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_lk_throw_startup:
  native_release 0x20
  sound_indexed 0x7
  open_peer_clip 0x0, clip(164)
  open_clip 0x0, clip(30)
  playback_single 0x16
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x1999
  playback_single 0x3d
  native_step 0x99
  tail 0x0, native_jarek_lk_throw_owner_return
  jump 0, word(tanya_lk_throw_startup)
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_fan_fatality_male_victim_owner_cycle:
  forward_to 0x8
  cursor_set 0x0
  tail 0x0, native_fan_male_victim_owner_cycle
  jump 0, word(jax_fan_fatality_owner_cycle)
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_fatality_1_female_victim_owner:
  prefixed_to 0x22b, clip(403)
  jump 0x0, word(jarek_fatality_1_male_victim_owner_tail_1)

; Branches use linked names.
jarek_fatality_1_male_victim_owner:
  prefixed_to 0x22b, clip(402)

; Branches use linked names.
jarek_fatality_1_male_victim_owner_tail_1:
  tail 0x0, native_jarek_first_heart
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_first_heart:
  forward 0x0
  bind_cursor 0x200, clip(405)
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_reach:
  prefixed_to 0x22b, clip(404)
  tail 0x0, native_jarek_first_victim_rip

; Branches use linked names.
jarek_first_victim_rip:
  forward_to 0x93
  tail 0x0, native_jarek_first_victim_impact
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_first_victim_impact:
  forward_to 0xc2
  tail 0x0, native_jarek_first_victim_end
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_first_victim_end:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_quake:
  sound_indexed 0x21
  prefixed_to 0x214, clip(107)
  sound_c 0x44
  spawn_effect 0x3
  tail 0x0, native_jarek_quake_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_quake_tail:
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_weapon_reaction_knife_victim:
  effect 0x1
  spawn_effect 0x0
  sound_context 0x6
  sound_b 0x1
  speed_negative 0x0, 0xccc
  prefixed_finish 0x200, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_ball_victim:
  sound_context 0x0
  spawn_effect 0x6
  motion 0x0, 0xffffe667, 0xffffe667, 0x24d

; Branches use linked names.
jarek_ball_victim_task_callback_1:
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_weapon_back_hp_upper:
  open_clip 0x0, clip(226)
  double_to 0x10
  sound_c 0x1f
  double_to 0x1a
  forward_to 0x1d
  tail 0x0, native_cage_weapon_upper_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
jarek_blade:
  sound_c 0x67
  fast_start 0x0, word(shared_open_clip_entry)
  prefixed_to 0x211, clip(27)

; Branches use linked names.
jarek_blade_spawn_1:
  spawn 0x0, 0x4e, native_jarek_blade_spawn
  forward_to 0x11
  playback_pair 0xf
  forward_to 0x11
  playback_pair 0xf
  forward_to 0x11
  playback_pair 0xf
  forward_to 0x11
  playback_pair 0xf
  forward_to 0x11
  playback_pair 0xf
  forward_to 0x11
  playback_pair 0xf
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
shared_open_clip_entry:
  open_clip 0x0, clip(27)
  double_to 0x211
  jump 0x0, word(jarek_blade_spawn_1)
  .zero 4 ; non-executable guard

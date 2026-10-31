; Generated reptile bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
reptile_lk_throw_startup:
  native_release 0x12
  sound_indexed 0x1b
  open_peer_clip 0x0, clip(147)
  open_clip 0x0, clip(32)
  playback_single 0x55
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x35c2
  playback_single 0x69
  native_release 0x13
  playback_single 0x7b
  native_step 0x85
  tail 0x0, native_lk_pause_long
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_fatality_2:
  native_release 0x35
  spawn 0x0, 0x26, native_scene_hover
  bind_cursor 0x0, clip(493)
  wait 0x96
  scene_active 0x0
  native_return 0xb4
  spawn 0x0, 0x0, native_reptile_second_acid
  sound_indexed 0x3d
  prefixed_finish 0x200, clip(544)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  clip_operation 0x0, clip(544)
  bind_cursor 0x0, clip(493)
  park 0x0
  jump 0, word(kai_fatality_2)
  .zero 4 ; non-executable guard

; Branches use linked names.
owner_entry:
  prefixed_to 0x20a, clip(432)
  tail 0x0, native_reptile_first_owner_cycle

; Branches use linked names.
reptile_first_owner_cycle:
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0xa8
  tail 0x0, native_reptile_first_owner_bite
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_first_owner_bite:
  forward 0x0
  tail 0x0, native_reptile_first_owner_loop
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_first_owner_loop:
  jump 0x0, word(reptile_first_owner_bite)

; Branches use linked names.
victim_entry:
  prefixed_to 0x255, clip(431)
  tail 0x0, native_reptile_first_victim_bite

; Branches use linked names.
reptile_first_victim_bite:
  forward_to 0x7c
  tail 0x0, native_reptile_first_victim_shake
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_first_victim_shake:
  cursor_set 0x55
  forward_to 0x7c
  cursor_set 0x55
  forward_to 0x7c
  cursor_set 0x55
  tail 0x0, native_reptile_first_victim_collapse
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_invisibility:
  play_clip 0x18, clip(12)
  sound_c 0x4b
  sound_c 0x4a
  tail 0x0, native_reptile_script_invisibility
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_dash:
  prefixed_finish 0x200, clip(302)
  bind_cursor 0x0, clip(303)
  speed 0x0, 0x2e14
  tail 0x0, native_reptile_script_dash_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_roll:
  sound_indexed 0x23
  sound_c 0x46
  speed 0x0, 0x1eb8
  prefixed_to 0x20d, clip(106)
  query_d 0x15
  forward_to 0x11
  landing 0x0
  tail 0x0, native_reptile_script_roll_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_weapon_reaction_reaction_137:
  effect 0x8
  effect 0x9
  sound_context 0x6
  speed_negative 0x0, 0xccc
  open_clip 0x0, clip(311)
  tail 0x0, native_reptile_weapon_stagger_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_weapon_reaction_reaction_144:
  effect 0x0
  sound_b 0x1
  sound_context 0x6
  motion 0x0, 0xfffff70b, 0xffffc000, 0x28f
  task_callback 0x0
  effect 0x0
  open_clip 0x0, clip(189)
  forward_to 0x9
  half_finish 0x0
  native_context 0x0
  forward_count 0x10
  tail 0x0, native_reaction_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_dash_victim:
  sound_b 0x0
  motion 0x0, 0xa3d, 0xffffcf5d, 0x28f
  task_callback 0x0
  play_clip 0x27, clip(179)
  native_context 0x0
  forward 0x0
  tail 0x0, native_sub_zero_lp_throw_victim_getup

; Branches use linked names.
reptile_reaction_40:
  effect 0x11
  spawn_effect 0x0
  sound_context 0x1
  motion 0x0, 0xffffeb86, 0xffffe667, 0x24d
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  jump 0, word(jarek_ball_victim)
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_fatality_2_victim_melt:
  bind_cursor 0x0, clip(546)
  speed_negative 0x0, 0x28f
  tail 0x0, native_reptile_second_melt
  jump 0, word(fujin_fatality_2_victim_fall)
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_weapon_back_hp_flurry_startup:
  open_clip 0x0, clip(248)
  forward_to 0x8
  tail 0x0, native_reptile_weapon_flurry
  .zero 4 ; non-executable guard

; Branches use linked names.
reptile_acid_spit:
  play_clip 0xb, clip(25)
  call 0x0, native_reptile_script_head_open
  forward_count 0x5
  call 0x0, native_reptile_script_head_wide
  forward_count 0x5
  spawn 0x0, 0x4c, native_reptile_acid_spawn
  call 0x0, native_reptile_script_acid_drool
  forward_count 0x8
  call 0x0, native_reptile_script_head_open
  action_followup 0x0
  forward_to 0x1e
  playback_pair 0x1b
  forward_to 0x1e
  playback_pair 0x1b
  forward_to 0x1e
  playback_pair 0x1b
  call 0x0, native_reptile_script_head_close
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Generated tanya bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
tanya_lk_throw_startup:
  native_release 0x18
  sound_indexed 0x1d
  open_peer_clip 0x0, clip(162)
  open_clip 0x0, clip(42)
  playback_single 0x1e
  tail 0x0, native_lk_branch_continue
  .zero 4 ; non-executable guard

; Branches use linked names.
lk_branch_continue:
  playback_single 0x2f
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x1999
  playback_single 0x7f
  native_step 0x87
  tail 0x0, native_lk_recoil
  jump 0, word(shinnok_lk_throw_startup)
  .zero 4 ; non-executable guard

; Branches use linked names.
owner_kiss_start:
  prefixed_to 0x234, clip(395)
  tail 0x0, native_tanya_first_owner_53

; Branches use linked names.
tanya_first_owner_53:
  forward_to 0x68
  tail 0x0, native_tanya_first_owner_drift
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_first_owner_drift:
  forward 0x0
  prefixed_finish 0x200, clip(396)
  tail 0x0, native_tanya_first_owner_replays
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_first_owner_loop:
  prefixed_finish 0x0
  tail 0x0, native_tanya_first_owner_loop
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_scene:
  prefixed_finish 0x200, clip(397)
  tail 0x0, native_tanya_first_victim_swell

; Branches use linked names.
tanya_fatality_1_victim_pulse:
  playback_pair 0xbc
  forward 0x0
  tail 0x0, native_tanya_first_victim_repeat

; Branches use linked names.
tanya_fatality_1_victim_shake:
  prefixed_to 0x209, clip(409)
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x48
  forward_to 0x4a
  playback_pair 0x48
  forward_to 0x4a
  playback_pair 0x48
  tail 0x0, native_tanya_first_victim_burst
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_first_victim_burst:
  forward 0x0
  playback_pair 0x48
  jump 0x0, word(tanya_first_victim_burst)
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_fatality_2:
  native_release 0x4e
  wait 0x1e
  wait 0x1e
  scene_active 0x0
  sound_indexed 0x12
  native_return 0xba
  clip 0x0, clip(564)
  tail 0x0, native_tanya_second_height

; Branches use linked names.
tanya_second_height:
  spawn 0x0, 0x26b, native_tanya_second_owner_cue
  clip 0x0, clip(565)
  scene_event 0x45
  wait 0xa
  spawn 0x0, 0x26b, native_tanya_second_owner_cue
  clip 0x0, clip(565)
  scene_event 0x46
  wait 0xa
  spawn 0x0, 0x26b, native_tanya_second_owner_cue
  clip 0x0, clip(565)
  clip 0x0, clip(566)
  tail 0x0, native_tanya_second_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_fatality_2_victim_scene:
  clip 0x0, clip(567)
  clip 0x0, clip(568)
  wait 0xa
  clip 0x0, clip(568)
  wait 0xa
  clip 0x0, clip(568)
  spawn 0x0, 0x26b, native_tanya_second_victim_cue
  tail 0x0, native_tanya_fatality_2_victim_collapse
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_fatality_2_victim_collapse:
  native_release 0x47
  clip 0x0, clip(569)
  park 0x0
  jump 0, word(cage_fatality_2_scene_entry)
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_drill_kick:
  sound_indexed 0x1f
  speed 0x0, 0x23d7
  prefixed_to 0x20c, clip(103)
  tail 0x0, native_tanya_drill_kick_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_weapon_reaction_reaction_170_victim:
  effect 0x10
  sound_context 0x1
  jump 0x0, word(kai_draw_weapon_back_victim_spawn_effect_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_weapon_reaction_reaction_169_victim:
  handler 0x0, native_velocity_friction
  effect 0x4
  spawn_effect 0x0
  sound_context 0x1
  sound_b 0x1
  speed_negative 0x0, 0x4000
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
boomerang_start:
  sound_c 0x21
  play_clip 0x7, clip(229)
  tail 0x0, native_tanya_boomerang_release
  .zero 4 ; non-executable guard

; Branches use linked names.
tanya_fireball_caster:
  prefixed_finish 0x200, clip(26)
  sound_a 0x21
  block_sound 0x0
  spawn 0x0, 0x4b, native_fireball_spawn
  playback_pair 0xb
  forward_to 0xd
  action_followup 0x0
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0x0
  ready 0x0
  .zero 4 ; non-executable guard

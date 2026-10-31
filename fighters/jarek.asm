; Generated jarek bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_JAREK_SECOND_GLOW 0x41de30
.export_equ MK4_NATIVE_JAREK_FATALITY_2 0x41df20
.export_equ MK4_NATIVE_JAREK_SECOND_HANDOFF 0x41dfc0
.export_equ MK4_NATIVE_JAREK_SECOND_VICTIM_HOLD 0x41e0a0
.export_equ MK4_NATIVE_JAREK_SECOND_HEAD_FALL 0x41e0e0
.export_equ MK4_NATIVE_JAREK_LK_THROW_ENTRY 0x42b770
.export_equ MK4_NATIVE_JAREK_LK_THROW_OWNER_RETURN 0x42b7a0
.export_equ MK4_NATIVE_JAREK_FATALITY_1 0x446d00
.export_equ MK4_NATIVE_JAREK_FIRST_HEART 0x446f80
.export_equ MK4_NATIVE_JAREK_FIRST_VICTIM_RIP 0x447310
.export_equ MK4_NATIVE_JAREK_FIRST_VICTIM_IMPACT 0x4473b0
.export_equ MK4_NATIVE_JAREK_FIRST_VICTIM_END 0x447410
.export_equ MK4_NATIVE_JAREK_UPWARD_BALL 0x4797f0
.export_equ MK4_NATIVE_JAREK_CANNONBALL 0x47b310
.export_equ MK4_NATIVE_JAREK_QUAKE 0x47b5d0
.export_equ MK4_NATIVE_JAREK_QUAKE_TAIL 0x47b600
.export_equ MK4_NATIVE_JAREK_WEAPON_REACTION_ENTRY 0x4821e0
.export_equ MK4_NATIVE_JAREK_BALL_VICTIM 0x4835a0
.export_equ MK4_NATIVE_JAREK_WEAPON_HP_ENTRY 0x495830
.export_equ MK4_NATIVE_JAREK_WEAPON_LP_ENTRY 0x495980
.export_equ MK4_NATIVE_JAREK_WEAPON_BACK_LP_ENTRY 0x4959c0
.export_equ MK4_NATIVE_JAREK_BLADE 0x497540
.export_equ MK4_NATIVE_JAREK_BLADE_SPAWN 0x497580

; Retail source 0x004d2db0; branches use linked names.
jarek_fatality_2:
  native_release 0x3b
  play_clip 0x1, clip(577)
  scene_active 0x0
  half_count 0x6
  call 0x0, MK4_NATIVE_JAREK_SECOND_GLOW
  half_count 0x13
  sound_b 0x1
  tail 0x0, MK4_NATIVE_JAREK_SECOND_HANDOFF
  .zero 4 ; non-executable guard

; Retail source 0x004d2de0; branches use linked names.
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
  tail 0x0, MK4_NATIVE_VICTORY_CONTINUE

; Retail source 0x004d2e10; branches use linked names.
victim_start:
  sound_c 0xb
  play_clip 0x10, clip(182)
  backward_count 0xf
  forward 0x0
  sound_b 0xc
  spawn 0x0, 0x0, MK4_NATIVE_JAREK_SECOND_HEAD_FALL
  prefixed_to 0x214, clip(206)
  sound_special 0x0
  forward_to 0x5a
  forward 0x0
  tail 0x0, MK4_NATIVE_JAREK_SECOND_VICTIM_HOLD
  .zero 4 ; non-executable guard

; Retail source 0x004ddff8; branches use linked names.
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
  tail 0x0, MK4_NATIVE_JAREK_LK_THROW_OWNER_RETURN
  jump 0, word(tanya_lk_throw_startup)
  .zero 4 ; non-executable guard

; Retail source 0x004e0128; branches use linked names.
jarek_fan_fatality_male_victim_owner_cycle:
  forward_to 0x8
  cursor_set 0x0
  tail 0x0, MK4_NATIVE_FAN_MALE_VICTIM_OWNER_CYCLE
  jump 0, word(jax_fan_fatality_owner_cycle)
  .zero 4 ; non-executable guard

; Retail source 0x004e1220; branches use linked names.
jarek_fatality_1_female_victim_owner:
  prefixed_to 0x22b, clip(403)
  jump 0x0, word(jarek_fatality_1_male_victim_owner_tail_1)

; Retail source 0x004e1230; branches use linked names.
jarek_fatality_1_male_victim_owner:
  prefixed_to 0x22b, clip(402)

; Retail source 0x004e1238; branches use linked names.
jarek_fatality_1_male_victim_owner_tail_1:
  tail 0x0, MK4_NATIVE_JAREK_FIRST_HEART
  .zero 4 ; non-executable guard

; Retail source 0x004e1248; branches use linked names.
jarek_first_heart:
  forward 0x0
  bind_cursor 0x200, clip(405)
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e1260; branches use linked names.
victim_reach:
  prefixed_to 0x22b, clip(404)
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_RIP

; Retail source 0x004e1270; branches use linked names.
jarek_first_victim_rip:
  forward_to 0x93
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_IMPACT
  .zero 4 ; non-executable guard

; Retail source 0x004e1280; branches use linked names.
jarek_first_victim_impact:
  forward_to 0xc2
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_END
  .zero 4 ; non-executable guard

; Retail source 0x004e1290; branches use linked names.
jarek_first_victim_end:
  forward 0x0
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4 ; non-executable guard

; Retail source 0x004e81a8; branches use linked names.
jarek_quake:
  sound_indexed 0x21
  prefixed_to 0x214, clip(107)
  sound_c 0x44
  spawn_effect 0x3
  tail 0x0, MK4_NATIVE_JAREK_QUAKE_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e81c8; branches use linked names.
jarek_quake_tail:
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e8c08; branches use linked names.
jarek_weapon_reaction_knife_victim:
  effect 0x1
  spawn_effect 0x0
  sound_context 0x6
  sound_b 0x1
  speed_negative 0x0, 0xccc
  prefixed_finish 0x200, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e9658; branches use linked names.
jarek_ball_victim:
  sound_context 0x0
  spawn_effect 0x6
  motion 0x0, 0xffffe667, 0xffffe667, 0x24d

; Retail source 0x004e9670; branches use linked names.
jarek_ball_victim_task_callback_1:
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4 ; non-executable guard

; Retail source 0x004ec4f0; branches use linked names.
jarek_weapon_back_hp_upper:
  open_clip 0x0, clip(226)
  double_to 0x10
  sound_c 0x1f
  double_to 0x1a
  forward_to 0x1d
  tail 0x0, MK4_NATIVE_CAGE_WEAPON_UPPER_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004ed118; branches use linked names.
jarek_blade:
  sound_c 0x67
  fast_start 0x0, word(shared_open_clip_entry)
  prefixed_to 0x211, clip(27)

; Retail source 0x004ed12c; branches use linked names.
jarek_blade_spawn_1:
  spawn 0x0, 0x4e, MK4_NATIVE_JAREK_BLADE_SPAWN
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

; Retail source 0x004ed7f0; branches use linked names.
shared_open_clip_entry:
  open_clip 0x0, clip(27)
  double_to 0x211
  jump 0x0, word(jarek_blade_spawn_1)
  .zero 4 ; non-executable guard

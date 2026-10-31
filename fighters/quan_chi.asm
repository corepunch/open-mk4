; Generated quan_chi bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ native_quan_chi_lk_throw_entry 0x42bc40
.export_equ native_quan_chi_second_fatality 0x43be30
.export_equ native_quan_chi_first_fatality 0x44bcd0
.export_equ native_quan_chi_fatality_cue_40 0x44be10
.export_equ native_quan_chi_fatality_leg 0x44be40
.export_equ native_quan_chi_fatality_hit_179 0x44be60
.export_equ native_quan_chi_fatality_hit_241 0x44beb0
.export_equ native_quan_chi_fatality_1_owner_beat 0x44bf00
.export_equ native_quan_chi_fatality_beat 0x44bf10
.export_equ native_quan_chi_can_first_fatality 0x45e2f0
.export_equ native_quan_chi_weapon_steal 0x469c80
.export_equ native_quan_chi_flip_kick 0x47a370
.export_equ native_quan_chi_script_flip_kick_tail 0x47a3b0
.export_equ native_quan_chi_teleport_stomp 0x47cf30
.export_equ native_quan_chi_flip_reaction 0x481d90
.export_equ native_quan_chi_steal_reaction 0x484060
.export_equ native_quan_chi_stomp_reaction 0x4843b0
.export_equ native_quan_chi_script_stomp_victim 0x484420
.export_equ native_quan_chi_skull_reaction 0x485f90
.export_equ native_quan_chi_weapon_back_hp_entry 0x495780
.export_equ native_quan_chi_weapon_hp_entry 0x496a70
.export_equ native_quan_chi_skull 0x499740
.export_equ native_quan_chi_script_skull_release 0x499780

; Retail source 0x004de1b0; branches use linked names.
quan_chi_lk_throw_startup:
  native_release 0x10
  open_peer_clip 0x0, clip(160)
  open_clip 0x0, clip(38)
  playback_single 0x28
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x8
  damage 0x0, 0x35c2
  playback_single 0x63
  native_release 0x11
  native_step 0x83
  native_word 0x0, 0x8000
  motion 0x0, 0xfffff334, 0xffffe148, 0x28f
  tail 0x0, native_lk_victim_return
  .zero 4 ; non-executable guard

; Retail source 0x004e14f8; branches use linked names.
quan_chi_fatality_1_owner_opening:
  prefixed_to 0x228, clip(420)
  tail 0x0, native_quan_chi_fatality_cue_40

; Retail source 0x004e1508; branches use linked names.
quan_chi_fatality_cue_40:
  forward_to 0x75
  tail 0x0, native_quan_chi_fatality_leg
  .zero 4 ; non-executable guard

; Retail source 0x004e1518; branches use linked names.
quan_chi_fatality_leg:
  forward_to 0xb3
  tail 0x0, native_quan_chi_fatality_hit_179
  .zero 4 ; non-executable guard

; Retail source 0x004e1528; branches use linked names.
quan_chi_fatality_hit_179:
  forward_to 0xf1
  tail 0x0, native_quan_chi_fatality_hit_241
  .zero 4 ; non-executable guard

; Retail source 0x004e1538; branches use linked names.
quan_chi_fatality_hit_241:
  forward 0x0
  tail 0x0, native_quan_chi_fatality_1_owner_beat
  .zero 4 ; non-executable guard

; Retail source 0x004e1548; branches use linked names.
quan_chi_fatality_1_owner_beat:
  prefixed_to 0x212, clip(421)
  tail 0x0, native_quan_chi_fatality_beat

; Retail source 0x004e1558; branches use linked names.
quan_chi_fatality_beat:
  forward 0x0
  jump 0x0, word(quan_chi_fatality_1_owner_beat)
  .zero 4 ; non-executable guard

; Retail source 0x004e1568; branches use linked names.
victim_stream:
  prefixed_finish 0x200, clip(424)

; Retail source 0x004e1570; branches use linked names.
victim_stream_prefixed_finish_1:
  prefixed_finish 0x200, clip(417)
  jump 0x0, word(victim_stream_prefixed_finish_1)

; Retail source 0x004e1580; branches use linked names.
leg_entry:
  prefixed_finish 0x200, clip(422)

; Retail source 0x004e1588; branches use linked names.
leg_entry_prefixed_finish_1:
  prefixed_finish 0x200, clip(423)
  jump 0x0, word(leg_entry_prefixed_finish_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e5eb0; branches use linked names.
quan_chi_air_throw_landing:
  native_context 0x0
  tail 0x0, native_air_slam_end
  .zero 4 ; non-executable guard

; Retail source 0x004e8100; branches use linked names.
quan_chi_flip_kick:
  sound_c 0x56
  open_clip 0x0, clip(105)
  prefixed_to 0xa
  speed 0x0, 0x547a
  forward_to 0xf
  tail 0x0, native_quan_chi_script_flip_kick_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e8a48; branches use linked names.
quan_chi_flip_reaction:
  sound_b 0x3
  sound_context 0xb
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, native_jax_dash_punch_roll_getup
  jump 0, word(reaction_fall_return)
  .zero 4 ; non-executable guard

; Retail source 0x004e9818; branches use linked names.
quan_chi_weapon_steal_grounded_victim:
  speed_negative 0x0, 0xccc
  prefixed_to 0x219, clip(182)
  landing 0x0
  forward 0x0
  tail 0x0, native_return_to_player_or_cpu_control

; Retail source 0x004e9838; branches use linked names.
quan_chi_weapon_steal_airborne_victim:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  wait 0x2
  tail 0x0, native_mmove_side_getup

; Retail source 0x004e9868; branches use linked names.
quan_chi_stomp_reaction:
  sound_context 0xd
  sound_b 0x1
  prefixed_finish 0x200, clip(178)
  tail 0x0, native_quan_chi_script_stomp_victim
  jump 0, word(slam_victim)
  .zero 4 ; non-executable guard

; Retail source 0x004e99e0; branches use linked names.
victim_friction_impact:
  prefixed_finish 0x200, clip(168)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e9cd8; branches use linked names.
quan_chi_skull_reaction:
  sound_c 0x2f
  sound_b 0x3
  speed_negative 0x0, 0x11eb
  open_clip 0x0, clip(182)
  native_sound 0x0
  landing 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed3a0; branches use linked names.
quan_chi_skull:
  open_clip 0x0, clip(23)
  fast_start 0x0, word(quan_chi_skull_release_cursor_set_1)
  prefixed_to 0x11
  tail 0x0, native_quan_chi_script_skull_release
  .zero 4 ; non-executable guard

; Retail source 0x004ed3c0; branches use linked names.
quan_chi_skull_release:
  sound_c 0x38
  forward_to 0x17
  playback_pair 0x13
  forward_to 0x17
  playback_pair 0x13
  forward_to 0x17
  playback_pair 0x13
  forward 0x0
  ready 0x0

; Retail source 0x004ed3e4; branches use linked names.
quan_chi_skull_release_cursor_set_1:
  cursor_set 0x5
  double_to 0x11
  tail 0x0, native_quan_chi_script_skull_release
  .zero 4 ; non-executable guard

; Generated reiko bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ native_reiko_second_fatality 0x41c360
.export_equ native_reiko_second_victim_fall 0x41c4b0
.export_equ native_reiko_second_victim_end 0x41c4e0
.export_equ native_reiko_second_stars 0x41d260
.export_equ native_reiko_lk_throw_entry 0x42bbe0
.export_equ native_reiko_first_fatality 0x452730
.export_equ native_reiko_first_victory 0x4528a0
.export_equ native_reiko_first_tear 0x452ac0
.export_equ native_reiko_first_victim_118 0x452bb0
.export_equ native_reiko_first_victim_165 0x452c10
.export_equ native_reiko_leg_tear_victim_finish 0x452c50
.export_equ native_reiko_first_torso_29 0x452e50
.export_equ native_reiko_first_torso_end 0x452e90
.export_equ native_reiko_first_head_59 0x452f00
.export_equ native_reiko_first_head_83 0x452f40
.export_equ native_reiko_leg_tear_head_finish 0x452f90
.export_equ native_reiko_first_left_arm_56 0x452fe0
.export_equ native_reiko_first_left_arm_land 0x453020
.export_equ native_reiko_leg_tear_left_arm_finish 0x453040
.export_equ native_reiko_first_right_arm_44 0x453090
.export_equ native_reiko_first_right_arm_land 0x4530d0
.export_equ native_reiko_leg_tear_right_arm_finish 0x4530f0
.export_equ native_reiko_can_first_fatality 0x45e360
.export_equ native_reiko_can_star 0x45e6d0
.export_equ native_reiko_can_orbit 0x45e710
.export_equ native_reiko_can_flip_kick 0x45e7a0
.export_equ native_reiko_orbit 0x478ee0
.export_equ native_reiko_flip_kick 0x4791a0
.export_equ native_reiko_star_reaction_45 0x483d40
.export_equ native_reiko_star 0x496db0
.export_equ native_reiko_star_spawn 0x496e00

; Retail source 0x004d2d10; branches use linked names.
reiko_second_fatality:
  sound_indexed 0x3e
  native_release 0x40
  wait 0x0
  scene_active 0x0
  play_clip 0x1d, clip(553)
  spawn 0x0, 0x0, native_reiko_second_stars
  loop 0x9
  cursor_set 0x1d
  forward_to 0x26
  repeat 0x0
  cursor_set 0x1d
  forward_to 0x21
  wait 0x14
  forward 0x0
  tail 0x0, native_task_park_60_tick_loop
  .zero 4 ; non-executable guard

; Retail source 0x004d2d60; branches use linked names.
victim_stagger:
  blend 0x0, clip(554)
  open_clip 0x0, clip(554)
  loop 0x4
  cursor_set 0x0
  forward_to 0x14
  repeat 0x0
  native_release 0x41
  forward_to 0x87
  tail 0x0, native_reiko_second_victim_fall

; Retail source 0x004d2d90; branches use linked names.
reiko_second_victim_fall:
  forward_to 0x91
  sound_special 0x0
  forward 0x0
  sound_special 0x0
  native_release 0x42
  tail 0x0, native_reiko_second_victim_end
  .zero 4 ; non-executable guard

; Retail source 0x004de138; branches use linked names.
reiko_lk_throw_startup:
  open_peer_clip 0x0, clip(148)
  open_clip 0x0, clip(40)
  playback_single 0x26
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x35c2
  tail 0x0, native_lk_throw_rock_repeat_one
  .zero 4 ; non-executable guard

; Retail source 0x004de170; branches use linked names.
lk_throw_rock_repeat_one:
  playback_single 0x2b
  playback_pair_reverse 0x2a
  tail 0x0, native_lk_throw_rock_repeat_two

; Retail source 0x004de180; branches use linked names.
lk_throw_rock_repeat_two:
  playback_single 0x2b
  playback_pair_reverse 0x2a
  tail 0x0, native_lk_throw_rock_release

; Retail source 0x004de190; branches use linked names.
lk_throw_rock_release:
  playback_single 0x2b
  playback_pair_reverse 0x2a
  playback_single 0x2b
  playback_pair_reverse 0x2a
  playback_single 0x3b
  native_step 0x84
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e2cf0; branches use linked names.
owner_tear:
  half_count 0x220, clip(425)
  forward_to 0x2b
  playback_pair 0x29
  forward_to 0x2b
  playback_pair 0x29
  forward 0x0
  tail 0x0, native_reiko_first_victory
  .zero 4 ; non-executable guard

; Retail source 0x004e2d18; branches use linked names.
owner_victory_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(owner_victory_loop)

; Retail source 0x004e2d28; branches use linked names.
victim_place:
  prefixed_to 0x225, clip(428)
  tail 0x0, native_reiko_first_tear

; Retail source 0x004e2d38; branches use linked names.
reiko_first_tear:
  cursor_set 0x26
  forward_to 0x75
  tail 0x0, native_reiko_first_victim_118

; Retail source 0x004e2d48; branches use linked names.
reiko_first_victim_118:
  forward_to 0xa5
  tail 0x0, native_reiko_first_victim_165
  .zero 4 ; non-executable guard

; Retail source 0x004e2d58; branches use linked names.
reiko_first_victim_165:
  forward 0x0
  tail 0x0, native_reiko_leg_tear_victim_finish
  .zero 4 ; non-executable guard

; Retail source 0x004e2d68; branches use linked names.
torso_start:
  prefixed_to 0x21d, clip(429)
  tail 0x0, native_reiko_first_torso_29

; Retail source 0x004e2d78; branches use linked names.
reiko_first_torso_29:
  forward 0x0
  tail 0x0, native_reiko_first_torso_end
  .zero 4 ; non-executable guard

; Retail source 0x004e2d88; branches use linked names.
head_start:
  prefixed_to 0x229, clip(430)
  cursor_set 0x15
  forward_to 0x29
  cursor_set 0x15
  forward_to 0x29
  cursor_set 0x15
  forward_to 0x29
  cursor_set 0x15
  forward_to 0x29
  cursor_set 0x15
  forward_to 0x3a
  tail 0x0, native_reiko_first_head_59

; Retail source 0x004e2dc0; branches use linked names.
reiko_first_head_59:
  forward_to 0x52
  tail 0x0, native_reiko_first_head_83
  .zero 4 ; non-executable guard

; Retail source 0x004e2dd0; branches use linked names.
reiko_first_head_83:
  forward 0x0
  tail 0x0, native_reiko_leg_tear_head_finish
  .zero 4 ; non-executable guard

; Retail source 0x004e2de0; branches use linked names.
left_arm_start:
  prefixed_to 0x219, clip(427)
  cursor_set 0x4
  forward_to 0x37
  tail 0x0, native_reiko_first_left_arm_56

; Retail source 0x004e2df8; branches use linked names.
reiko_first_left_arm_56:
  forward_to 0x42
  tail 0x0, native_reiko_first_left_arm_land
  .zero 4 ; non-executable guard

; Retail source 0x004e2e08; branches use linked names.
reiko_first_left_arm_land:
  forward 0x0
  tail 0x0, native_reiko_leg_tear_left_arm_finish
  .zero 4 ; non-executable guard

; Retail source 0x004e2e18; branches use linked names.
right_arm_start:
  prefixed_to 0x21d, clip(426)
  cursor_set 0x4
  forward_to 0x1d
  cursor_set 0x4
  forward_to 0x1d
  cursor_set 0x4
  forward_to 0x2b
  tail 0x0, native_reiko_first_right_arm_44

; Retail source 0x004e2e40; branches use linked names.
reiko_first_right_arm_44:
  forward_to 0x44
  tail 0x0, native_reiko_first_right_arm_land
  .zero 4 ; non-executable guard

; Retail source 0x004e2e50; branches use linked names.
reiko_first_right_arm_land:
  forward 0x0
  tail 0x0, native_reiko_leg_tear_right_arm_finish
  .zero 4 ; non-executable guard

; Retail source 0x004ed038; branches use linked names.
reiko_star:
  sound_c 0x64
  prefixed_to 0x20d, clip(437)
  spawn 0x0, 0x52, native_reiko_star_spawn
  forward_to 0x12
  playback_pair 0x10
  forward_to 0x12
  playback_pair 0x10
  forward_to 0x12
  playback_pair 0x10
  forward_to 0x12
  playback_pair 0x10
  forward_to 0x12
  playback_pair 0x10
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

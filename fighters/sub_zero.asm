; Generated sub_zero bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ native_sub_zero_script_breath_victim_end 0x41b080
.export_equ native_sub_zero_second_fatality 0x41b3a0
.export_equ native_sub_zero_script_breath_approach 0x41b440
.export_equ native_sub_zero_script_breath_slide 0x41b450
.export_equ native_sub_zero_script_breath_shatter 0x41b570
.export_equ native_sub_zero_lk_throw_entry 0x42c180
.export_equ native_sub_zero_script_lk_rebase 0x42c200
.export_equ native_sub_zero_script_lk_damage 0x42c220
.export_equ native_sub_zero_script_lk_owner_return 0x42c300
.export_equ native_sub_zero_lp_throw_owner_return 0x42ce80
.export_equ native_sub_zero_first_fatality 0x445100
.export_equ native_sub_zero_script_first_begin 0x445150
.export_equ native_sub_zero_script_first_pull 0x445220
.export_equ native_sub_zero_script_first_cues1 0x445240
.export_equ native_sub_zero_script_first_cues2 0x445280
.export_equ native_sub_zero_script_first_cues3 0x4452b0
.export_equ native_sub_zero_script_first_cues4 0x4452e0
.export_equ native_sub_zero_script_first_head 0x445310
.export_equ native_sub_zero_script_first_victim_blends 0x445510
.export_equ native_sub_zero_script_first_victim_collapse 0x4456c0
.export_equ native_sub_zero_script_first_victim_end 0x445700
.export_equ native_sub_zero_can_first_fatality 0x45e460
.export_equ native_sub_zero_can_clone 0x45ece0
.export_equ native_sub_zero_can_ice 0x45f1f0
.export_equ native_sub_zero_can_slide 0x45f270
.export_equ native_sub_zero_slide 0x47dc00
.export_equ native_sub_zero_slide_script 0x47dc40
.export_equ native_sub_zero_script_slide_return 0x47de00
.export_equ native_sub_zero_clone 0x47f4d0
.export_equ native_sub_zero_script_lk_victim_rise 0x487220
.export_equ native_sub_zero_lp_throw_victim_getup 0x488b80
.export_equ native_sub_zero_scepter_hp_entry 0x4956c0
.export_equ native_sub_zero_scepter_back_hp_entry 0x4956f0
.export_equ native_sub_zero_draw_weapon_scepter_follow_up 0x495710
.export_equ native_sub_zero_scepter_lp_entry 0x495720
.export_equ native_sub_zero_scepter_back_lp_entry 0x495740
.export_equ native_sub_zero_ice 0x49bc60

; Retail source 0x004d2bf0; branches use linked names.
sub_zero_fatality_2_victim:
  prefixed_to 0x214, clip(206)
  sound_special 0x0
  forward_to 0x5a
  sound_special 0x0
  forward 0x0
  tail 0x0, native_sub_zero_script_breath_victim_end

; Retail source 0x004d2c10; branches use linked names.
sub_zero_fatality_2_breath:
  sound_indexed 0x4
  play_clip 0xa, clip(552)
  spawn 0x0, 0x0, native_breath_scene_decoration
  sound_c 0x3f
  native_release 0x38
  loop 0x3
  cursor_set 0xa
  forward_to 0x1d
  repeat 0x0
  native_release 0x39
  sound_c 0x40
  forward_to 0x53
  scene_active 0x0
  spawn 0x0, 0x0, native_breath_shatter_decoration
  loop 0x5
  cursor_set 0x53
  forward_to 0x5d
  repeat 0x0
  forward 0x0
  tail 0x0, native_sub_zero_script_breath_approach

; Retail source 0x004d2c78; branches use linked names.
sub_zero_breath_approach:
  native_release 0x3a
  wait 0x14
  speed_negative 0x0, 0xffffd99a
  sound_c 0xf
  open_clip 0x0, clip(95)
  tail 0x0, native_sub_zero_script_breath_slide
  .zero 4 ; non-executable guard

; Retail source 0x004d2ca0; branches use linked names.
breath_resume:
  open_clip 0x0, clip(258)
  cursor_set 0x19
  forward 0x0
  sound_c 0x1c
  play_clip 0x5, clip(293)
  sound_c 0x3b
  tail 0x0, native_sub_zero_script_breath_shatter

; Retail source 0x004d2cc8; branches use linked names.
sub_zero_breath_shatter:
  loop 0x8
  forward_to 0x8
  playback_pair 0x6
  repeat 0x0
  forward 0x0
  wait 0x3c
  tail 0x0, native_victory_continue
  .zero 4 ; non-executable guard

; Retail source 0x004de490; branches use linked names.
sub_zero_lk_throw_startup:
  open_peer_clip 0x0, clip(165)
  open_clip 0x0, clip(41)
  forward_count 0xa
  native_release 0x2
  sound_a 0x3
  tail 0x0, native_sub_zero_script_lk_rebase
  .zero 4 ; non-executable guard

; Retail source 0x004de4b8; branches use linked names.
sub_zero_lk_rebase:
  play_pair 0x1d
  disarm_peer 0x0
  sound_a 0x24
  play_pair 0x5
  sound_roster 0x8
  play_pair 0x8
  handler 0x0, native_velocity_friction
  tail 0x0, native_sub_zero_script_lk_damage

; Retail source 0x004de4e0; branches use linked names.
sub_zero_lk_damage:
  play_pair 0x5
  tail 0x0, native_sub_zero_script_lk_owner_return
  .zero 4 ; non-executable guard

; Retail source 0x004e0368; branches use linked names.
sub_zero_fatality_1_setup:
  wait 0x32
  tail 0x0, native_sub_zero_script_first_begin
  .zero 4 ; non-executable guard

; Retail source 0x004e0378; branches use linked names.
sub_zero_fatality_1_first_pull:
  prefixed_to 0x211, clip(385)
  tail 0x0, native_sub_zero_script_first_pull

; Retail source 0x004e0388; branches use linked names.
sub_zero_fatality_1_pull_forward:
  forward_to 0x2a
  cursor_set 0x29
  tail 0x0, native_sub_zero_script_first_cues1

; Retail source 0x004e0398; branches use linked names.
sub_zero_fatality_1_pull_reverse:
  playback_pair 0x1f
  tail 0x0, native_sub_zero_script_first_cues2
  .zero 4 ; non-executable guard

; Retail source 0x004e03a8; branches use linked names.
sub_zero_fatality_1_second_pull:
  forward_to 0x2a
  cursor_set 0x29
  tail 0x0, native_sub_zero_script_first_cues3

; Retail source 0x004e03b8; branches use linked names.
sub_zero_fatality_1_second_reverse:
  playback_pair 0x1f
  tail 0x0, native_sub_zero_script_first_cues4
  .zero 4 ; non-executable guard

; Retail source 0x004e03c8; branches use linked names.
sub_zero_fatality_1_head_release:
  forward_to 0x2c
  tail 0x0, native_sub_zero_script_first_head
  .zero 4 ; non-executable guard

; Retail source 0x004e03d8; branches use linked names.
sub_zero_fatality_1_owner_loop:
  forward_to 0x3c

; Retail source 0x004e03dc; branches use linked names.
sub_zero_fatality_1_owner_loop_forward_1:
  forward 0x0
  cursor_set 0x60
  jump 0x0, word(sub_zero_fatality_1_owner_loop_forward_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e03f0; branches use linked names.
sub_zero_fatality_1_victim:
  prefixed_to 0x20f, clip(387)
  cursor_set 0xe
  playback_pair 0x4
  forward_to 0xf
  cursor_set 0xe
  playback_pair 0x4
  forward_to 0x11
  forward_to 0x30
  tail 0x0, native_sub_zero_script_first_victim_blends
  .zero 4 ; non-executable guard

; Retail source 0x004e0420; branches use linked names.
sub_zero_fatality_1_victim_finish:
  forward_to 0x3f
  tail 0x0, native_sub_zero_script_first_victim_collapse
  .zero 4 ; non-executable guard

; Retail source 0x004e0430; branches use linked names.
sub_zero_fatality_1_victim_cleanup:
  forward 0x0
  tail 0x0, native_sub_zero_script_first_victim_end
  .zero 4 ; non-executable guard

; Retail source 0x004e69c8; branches use linked names.
sub_zero_combo_hp_attacker:
  open_clip 0x0, clip(532)
  prefixed_sample 0x0
  wait 0x1
  prefixed_sample 0x1
  wait 0x1
  prefixed_sample 0x2
  wait 0x1
  prefixed_sample 0x3
  wait 0x1
  prefixed_sample 0x4
  wait 0x1
  prefixed_sample 0x6
  wait 0x1
  prefixed_sample 0x8
  wait 0x1
  prefixed_sample 0x9
  wait 0x1
  prefixed_sample 0xa
  wait 0x1
  prefixed_sample 0xb
  wait 0x1
  tail 0x0, native_combo_high_punch_tail
  jump 0, word(kitana_combo_high_punch_attacker)
  .zero 4 ; non-executable guard

; Retail source 0x004e84d8; branches use linked names.
sub_zero_slide:
  speed_negative 0x0, 0xffffd99a
  sound_c 0xf
  open_clip 0x0, clip(95)
  tail 0x0, native_sub_zero_slide_script
  .zero 4 ; non-executable guard

; Retail source 0x004e84f8; branches use linked names.
sub_zero_slide_hit:
  loop 0x4
  playback_tail 0xa
  wait 0x1
  playback_tail 0x9
  wait 0x1
  playback_tail 0x8
  wait 0x1
  playback_tail 0x9
  wait 0x1
  repeat 0x0
  landing 0x0
  tail 0x0, native_sub_zero_script_slide_return
  .zero 4 ; non-executable guard

; Retail source 0x004e8530; branches use linked names.
sub_zero_slide_block:
  no_guard 0x0
  landing 0x0
  loop 0x6
  playback_tail 0xa
  wait 0x1
  playback_tail 0x9
  wait 0x1
  playback_tail 0x8
  wait 0x1
  playback_tail 0x9
  wait 0x1
  repeat 0x0
  tail 0x0, native_sub_zero_script_slide_return
  .zero 4 ; non-executable guard

; Retail source 0x004ed588; branches use linked names.
sub_zero_ice:
  sound_indexed 0x4
  open_clip 0x0, clip(16)
  fast_start 0x0, word(shared_double_to_entry)
  forward_to 0xf

; Retail source 0x004ed5a0; branches use linked names.
sub_zero_ice_sound_c_1:
  sound_c 0x0
  spawn 0x0, 0x42, native_ice_projectile_task
  wait 0x1
  action_followup 0x0
  loop 0x2
  forward_to 0x17
  playback_pair 0x12
  repeat 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed818; branches use linked names.
shared_double_to_entry:
  double_to 0xf
  jump 0x0, word(sub_zero_ice_sound_c_1)
  .zero 4 ; non-executable guard

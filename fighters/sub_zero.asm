; Generated sub_zero bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END 0x41b080
.export_equ MK4_NATIVE_SUB_ZERO_SECOND_FATALITY 0x41b3a0
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH 0x41b440
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE 0x41b450
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER 0x41b570
.export_equ MK4_NATIVE_SUB_ZERO_LK_THROW_ENTRY 0x42c180
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE 0x42c200
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE 0x42c220
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN 0x42c300
.export_equ MK4_NATIVE_SUB_ZERO_LP_THROW_OWNER_RETURN 0x42ce80
.export_equ MK4_NATIVE_SUB_ZERO_FIRST_FATALITY 0x445100
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN 0x445150
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL 0x445220
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1 0x445240
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2 0x445280
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3 0x4452b0
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4 0x4452e0
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD 0x445310
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS 0x445510
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE 0x4456c0
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END 0x445700
.export_equ MK4_NATIVE_SUB_ZERO_CAN_FIRST_FATALITY 0x45e460
.export_equ MK4_NATIVE_SUB_ZERO_CAN_CLONE 0x45ece0
.export_equ MK4_NATIVE_SUB_ZERO_CAN_ICE 0x45f1f0
.export_equ MK4_NATIVE_SUB_ZERO_CAN_SLIDE 0x45f270
.export_equ MK4_NATIVE_SUB_ZERO_SLIDE 0x47dc00
.export_equ MK4_NATIVE_SUB_ZERO_SLIDE_SCRIPT 0x47dc40
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN 0x47de00
.export_equ MK4_NATIVE_SUB_ZERO_CLONE 0x47f4d0
.export_equ MK4_NATIVE_SUB_ZERO_SCRIPT_LK_VICTIM_RISE 0x487220
.export_equ MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP 0x488b80
.export_equ MK4_NATIVE_SUB_ZERO_SCEPTER_HP_ENTRY 0x4956c0
.export_equ MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_HP_ENTRY 0x4956f0
.export_equ MK4_NATIVE_SUB_ZERO_DRAW_WEAPON_SCEPTER_FOLLOW_UP 0x495710
.export_equ MK4_NATIVE_SUB_ZERO_SCEPTER_LP_ENTRY 0x495720
.export_equ MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_LP_ENTRY 0x495740
.export_equ MK4_NATIVE_SUB_ZERO_ICE 0x49bc60

; Retail source 0x004d2bf0; branches use linked names.
sub_zero_fatality_2_victim:
  prefixed_to 0x214, clip(206)
  sound_special 0x0
  forward_to 0x5a
  sound_special 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END

; Retail source 0x004d2c10; branches use linked names.
sub_zero_fatality_2_breath:
  sound_indexed 0x4
  play_clip 0xa, clip(552)
  spawn 0x0, 0x0, MK4_NATIVE_BREATH_SCENE_DECORATION
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
  spawn 0x0, 0x0, MK4_NATIVE_BREATH_SHATTER_DECORATION
  loop 0x5
  cursor_set 0x53
  forward_to 0x5d
  repeat 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH

; Retail source 0x004d2c78; branches use linked names.
sub_zero_breath_approach:
  native_release 0x3a
  wait 0x14
  speed_negative 0x0, 0xffffd99a
  sound_c 0xf
  open_clip 0x0, clip(95)
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE
  .zero 4 ; non-executable guard

; Retail source 0x004d2ca0; branches use linked names.
breath_resume:
  open_clip 0x0, clip(258)
  cursor_set 0x19
  forward 0x0
  sound_c 0x1c
  play_clip 0x5, clip(293)
  sound_c 0x3b
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER

; Retail source 0x004d2cc8; branches use linked names.
sub_zero_breath_shatter:
  loop 0x8
  forward_to 0x8
  playback_pair 0x6
  repeat 0x0
  forward 0x0
  wait 0x3c
  tail 0x0, MK4_NATIVE_VICTORY_CONTINUE
  .zero 4 ; non-executable guard

; Retail source 0x004de490; branches use linked names.
sub_zero_lk_throw_startup:
  open_peer_clip 0x0, clip(165)
  open_clip 0x0, clip(41)
  forward_count 0xa
  native_release 0x2
  sound_a 0x3
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE
  .zero 4 ; non-executable guard

; Retail source 0x004de4b8; branches use linked names.
sub_zero_lk_rebase:
  play_pair 0x1d
  disarm_peer 0x0
  sound_a 0x24
  play_pair 0x5
  sound_roster 0x8
  play_pair 0x8
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE

; Retail source 0x004de4e0; branches use linked names.
sub_zero_lk_damage:
  play_pair 0x5
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN
  .zero 4 ; non-executable guard

; Retail source 0x004e0368; branches use linked names.
sub_zero_fatality_1_setup:
  wait 0x32
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN
  .zero 4 ; non-executable guard

; Retail source 0x004e0378; branches use linked names.
sub_zero_fatality_1_first_pull:
  prefixed_to 0x211, clip(385)
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL

; Retail source 0x004e0388; branches use linked names.
sub_zero_fatality_1_pull_forward:
  forward_to 0x2a
  cursor_set 0x29
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1

; Retail source 0x004e0398; branches use linked names.
sub_zero_fatality_1_pull_reverse:
  playback_pair 0x1f
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2
  .zero 4 ; non-executable guard

; Retail source 0x004e03a8; branches use linked names.
sub_zero_fatality_1_second_pull:
  forward_to 0x2a
  cursor_set 0x29
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3

; Retail source 0x004e03b8; branches use linked names.
sub_zero_fatality_1_second_reverse:
  playback_pair 0x1f
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4
  .zero 4 ; non-executable guard

; Retail source 0x004e03c8; branches use linked names.
sub_zero_fatality_1_head_release:
  forward_to 0x2c
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD
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
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS
  .zero 4 ; non-executable guard

; Retail source 0x004e0420; branches use linked names.
sub_zero_fatality_1_victim_finish:
  forward_to 0x3f
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE
  .zero 4 ; non-executable guard

; Retail source 0x004e0430; branches use linked names.
sub_zero_fatality_1_victim_cleanup:
  forward 0x0
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END
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
  tail 0x0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  jump 0, word(kitana_combo_high_punch_attacker)
  .zero 4 ; non-executable guard

; Retail source 0x004e84d8; branches use linked names.
sub_zero_slide:
  speed_negative 0x0, 0xffffd99a
  sound_c 0xf
  open_clip 0x0, clip(95)
  tail 0x0, MK4_NATIVE_SUB_ZERO_SLIDE_SCRIPT
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
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
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
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
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
  spawn 0x0, 0x42, MK4_NATIVE_ICE_PROJECTILE_TASK
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

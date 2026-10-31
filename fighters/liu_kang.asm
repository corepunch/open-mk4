; Generated liu_kang bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_LIU_KANG_LK_THROW_ENTRY 0x42c040
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE 0x42c0b0
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE 0x42c0d0
.export_equ MK4_NATIVE_LIU_KANG_LK_THROW_RETREAT 0x42c170
.export_equ MK4_NATIVE_LIU_KANG_SECOND_FATALITY 0x43c3d0
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP 0x43c500
.export_equ MK4_NATIVE_LIU_KANG_FIRST_FATALITY 0x450930
.export_equ MK4_NATIVE_LIU_KANG_CAN_FIRST_FATALITY 0x45e410
.export_equ MK4_NATIVE_LIU_KANG_CAN_BICYCLE 0x45eab0
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE 0x46db00
.export_equ MK4_NATIVE_LIU_KANG_BICYCLE_KICK 0x47eaf0
.export_equ MK4_NATIVE_LIU_KANG_BICYCLE_SCRIPT 0x47eb30
.export_equ MK4_NATIVE_LIU_KANG_BICYCLE_LAND 0x47ed40
.export_equ MK4_NATIVE_LIU_KANG_FLYING_KICK 0x47edc0
.export_equ MK4_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT 0x47ee00
.export_equ MK4_NATIVE_LIU_KANG_FLYING_KICK_LAND 0x47ef10
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION 0x480e90
.export_equ MK4_NATIVE_LIU_KANG_LK_THROW_VICTIM_REACTION_80 0x485910
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL 0x485930
.export_equ MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY 0x486730
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN 0x48f9f0
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND 0x496430
.export_equ MK4_NATIVE_LIU_KANG_SWORD_BACK_HP_ENTRY 0x496670
.export_equ MK4_NATIVE_LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT 0x496770
.export_equ MK4_NATIVE_LIU_KANG_LOW_FIREBALL 0x49bf50
.export_equ MK4_NATIVE_LIU_KANG_HIGH_FIREBALL 0x49bf90
.export_equ MK4_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN 0x49c010

; Retail source 0x004de3b0; branches use linked names.
liu_kang_lk_throw_startup:
  open_peer_clip 0x0, clip(133)
  open_clip 0x0, clip(35)
  play_pair 0xa
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE
  .zero 4 ; non-executable guard

; Retail source 0x004de410; branches use linked names.
liu_kang_lk_continue:
  native_release 0x1
  play_pair 0x14
  forward_to 0x26
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE
  .zero 4 ; non-executable guard

; Retail source 0x004de428; branches use linked names.
liu_kang_lk_throw_damage_release:
  disarm_peer 0x0
  sound_a 0x24
  sound_roster 0x8
  play_pair 0x2
  native_return 0x50
  forward_to 0x2b
  playback_pair 0x27
  forward_to 0x2b
  playback_pair 0x27
  forward_to 0x2b
  playback_pair 0x27
  forward_to 0x2b
  playback_pair 0x27
  playback_pair 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004de468; branches use linked names.
liu_kang_lk_throw_retreat_entry:
  speed_negative 0x0, 0x11eb
  tail 0x0, MK4_NATIVE_LIU_KANG_LK_THROW_RETREAT

; Retail source 0x004de478; branches use linked names.
liu_kang_lk_throw_retreat:
  open_clip 0x0, clip(182)
  double_to 0x1c
  landing 0x0
  native_sound 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004dfff8; branches use linked names.
liu_kang_start_fatality:
  scene_event 0x2a
  wait 0xf
  scene_event 0x2b
  wait 0x14
  scene_event 0x2c
  wait 0x3c
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP
  .zero 4 ; non-executable guard

; Retail source 0x004e0020; branches use linked names.
liu_kang_fireball_fatality_paired_opening:
  call 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN
  sound_b 0xa
  open_peer_clip 0x0, clip(539)
  open_clip 0x0, clip(537)
  play_pair 0x25
  scene_active 0x0
  native_return 0xb0
  forward 0x0
  wait 0x12
  sound_a 0x21
  sound_indexed 0x39
  prefixed_finish 0x200, clip(538)
  spawn 0x0, 0x26, MK4_NATIVE_LIU_FINISHER_PROJECTILE
  playback_pair 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e8610; branches use linked names.
liu_kang_bicycle_kick:
  motion 0x0, 0x2147, 0xfffff5c3, 0x0
  call 0x0, MK4_NATIVE_SAVE_MOTION
  play_clip 0x6, clip(68)
  handler 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION
  sound_indexed 0x1
  call 0x0, MK4_NATIVE_RESTORE_MOTION
  gravity 0x0, 0xc4
  tail 0x0, MK4_NATIVE_LIU_KANG_BICYCLE_SCRIPT
  .zero 4 ; non-executable guard

; Retail source 0x004e8678; branches use linked names.
bicycle_land:
  clear_handler 0x0
  clip_alt 0x6, clip(68)
  ready 0x0

; Retail source 0x004e8688; branches use linked names.
bicycle_contact_end:
  motion 0x0, 0x0, 0xffffeb86, 0xe5
  native_context 0x0
  clear_handler 0x0
  tail 0x0, MK4_NATIVE_LIU_KANG_BICYCLE_LAND

; Retail source 0x004e86a8; branches use linked names.
liu_kang_flying_kick:
  sound_indexed 0x0
  open_clip 0x0, clip(92)
  motion 0x0, 0x23d7, 0xffffe8f6, 0x106
  tail 0x0, MK4_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT
  .zero 4 ; non-executable guard

; Retail source 0x004e86d0; branches use linked names.
liu_kang_flying_kick_miss:
  clip_operation 0x0, clip(92)
  bind_submit 0x0, clip(92)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e86e8; branches use linked names.
liu_kang_flying_kick_hit:
  landing 0x0
  speed_negative 0x0, 0xfffffae2
  clip 0x0, clip(93)
  wait 0x14
  gravity 0x0, 0x51e
  clip_operation 0x0, clip(92)
  native_context 0x0
  ready 0x0

; Retail source 0x004e8718; branches use linked names.
liu_kang_flying_kick_block:
  landing 0x0
  wait 0xc
  landing 0x0
  wait 0xc
  gravity 0x0, 0x28f
  native_context 0x0
  tail 0x0, MK4_NATIVE_LIU_KANG_FLYING_KICK_LAND
  .zero 4 ; non-executable guard

; Retail source 0x004e9a90; branches use linked names.
liu_kang_lk_throw_victim:
  sound_a 0x25
  forward_to 0x23
  loop 0x3
  playback_pair 0x1f
  forward_to 0x23
  repeat 0x0
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e9b68; branches use linked names.
lp_victim_impact_release:
  prefixed_to 0x211, clip(174)
  sound_special 0x0
  forward 0x0
  landing 0x0
  tail 0x0, MK4_NATIVE_GETUP_ROLL_FLIP
  .zero 4 ; non-executable guard

; Retail source 0x004ec680; branches use linked names.
liu_kang_sword_hp_repeat_program:
  attack_selector 0x0, 0x30f
  schedule_motion 0x6006, 0x11eb
  open_clip 0x0, clip(233)
  double_to 0x9
  sound_c 0x1d
  double_to 0x12

; Retail source 0x004ec6a4; branches use linked names.
liu_kang_sword_hp_repeat_program_prefixed_sample_1:
  prefixed_sample 0x200, clip(216)
  weapon_query 0xf
  predicate_flags 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(liu_kang_sword_hp_repeat_program_action_recovery_1)
  schedule_motion 0x6006, 0x11eb
  sound_c 0x1f
  forward_to 0xa
  weapon_query 0xf
  predicate_flags 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(shared_action_recovery_entry)
  schedule_motion 0x6006, 0x11eb
  sound_c 0x1d
  forward_to 0x15
  jump 0x0, word(liu_kang_sword_hp_repeat_program_prefixed_sample_1)

; Retail source 0x004ec6f4; branches use linked names.
liu_kang_sword_hp_repeat_program_action_recovery_1:
  action_recovery 0x0
  forward_count 0x5
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND
  .zero 4 ; non-executable guard

; Retail source 0x004ecba8; branches use linked names.
shared_action_recovery_entry:
  action_recovery 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed678; branches use linked names.
liu_kang_low_fireball_low:
  open_clip 0x0, clip(19)
  double_to 0x14
  sound_a 0x21
  sound_indexed 0x2
  spawn 0x0, 0x40, MK4_NATIVE_LIU_LOW_FIREBALL_OBJECT
  forward_to 0x17
  action_followup 0x0
  playback_pair 0x12
  forward_to 0x17
  playback_pair 0x12
  forward_to 0x17
  playback_pair 0x12
  forward_to 0x17
  playback_pair 0x0
  ready 0x0

; Retail source 0x004ed6c0; branches use linked names.
liu_kang_high_fireball_high:
  open_clip 0x0, clip(18)
  double_to 0x14
  sound_a 0x21
  sound_indexed 0x2
  spawn 0x0, 0x40, MK4_NATIVE_LIU_HIGH_FIREBALL_OBJECT
  forward_to 0x1b
  action_followup 0x0
  forward_to 0x12
  forward_to 0x1b
  playback_pair 0x0
  ready 0x0

; Retail source 0x004ed6f8; branches use linked names.
liu_kang_air_fireball_air:
  clip 0x0, clip(20)
  sound_a 0x21
  sound_indexed 0x3
  spawn 0x0, 0x40, MK4_NATIVE_LIU_AIR_FIREBALL_OBJECT
  wait 0x10
  clip_operation 0x0, clip(20)
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN
  .zero 4 ; non-executable guard

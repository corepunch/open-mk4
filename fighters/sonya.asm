; Generated sonya bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_SONYA_LK_THROW_ENTRY 0x42bed0
.export_equ MK4_NATIVE_SONYA_FIRST_FATALITY 0x43f420
.export_equ MK4_NATIVE_SONYA_SCRIPT_FIRST_RELEASE 0x43f510
.export_equ MK4_NATIVE_SONYA_CAN_FIRST_FATALITY 0x45e4b0
.export_equ MK4_NATIVE_SONYA_CAN_CARTWHEEL 0x45e990
.export_equ MK4_NATIVE_SONYA_CAN_LEG_GRAB 0x45f1b0
.export_equ MK4_NATIVE_SONYA_SECOND_FATALITY 0x465cb0
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_OPEN 0x465e10
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_REPEAT 0x465e70
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_BRANCH 0x465ea0
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_PAUSE 0x465ed0
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END 0x466000
.export_equ MK4_NATIVE_SONYA_FATALITY_2_SCENE_VICTIM 0x466110
.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END 0x466190
.export_equ MK4_NATIVE_SONYA_CARTWHEEL 0x47b690
.export_equ MK4_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL 0x47b6c0
.export_equ MK4_NATIVE_SONYA_LEG_GRAB 0x47dee0
.export_equ MK4_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL 0x47e0a0
.export_equ MK4_NATIVE_SONYA_SQUARE_WAVE 0x47e180
.export_equ MK4_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL 0x47e1d0
.export_equ MK4_NATIVE_SONYA_BICYCLE 0x47e650
.export_equ MK4_NATIVE_SONYA_WEAPON_HIGH_PUNCH_VICTIM_POSITION 0x4820d0
.export_equ MK4_NATIVE_SONYA_WEAPON_LOW_PUNCH_VICTIM_CONTEXT_SOUND 0x4825e0
.export_equ MK4_NATIVE_SONYA_WEAPON_LOW_PUNCH_VICTIM_IMPACT_SOUND 0x482600
.export_equ MK4_NATIVE_SONYA_CROUCH_HIGH_KICK_VICTIM_KEEP_WEAPON 0x482850
.export_equ MK4_NATIVE_SONYA_LEG_GRAB_VICTIM_RECOVERY 0x4858a0
.export_equ MK4_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END 0x4858d0
.export_equ MK4_NATIVE_SONYA_WEAPON_HP_ENTRY 0x4966b0
.export_equ MK4_NATIVE_SONYA_WEAPON_LP_ENTRY 0x4966e0
.export_equ MK4_NATIVE_SONYA_WEAPON_BACK_HP_ENTRY 0x496720
.export_equ MK4_NATIVE_SONYA_DRAW_WEAPON_HP_SLASH 0x496c20
.export_equ MK4_NATIVE_SONYA_DRAW_WEAPON_LP_SLASH 0x496c30
.export_equ MK4_NATIVE_SONYA_RING 0x49b9a0
.export_equ MK4_NATIVE_SONYA_SCRIPT_RING_FLASH 0x49b9e0
.export_equ MK4_NATIVE_SONYA_RING_OBJECT 0x49ba50

; Retail source 0x004de360; branches use linked names.
sonya_lk_throw_startup:
  sound_indexed 0x13
  open_peer_clip 0x0, clip(163)
  open_clip 0x0, clip(43)
  playback_single 0x6
  playback_single 0x18
  native_release 0x3
  playback_single 0x2c
  damage 0x0, 0x2e14
  spawn_effect 0x0
  disarm_peer 0x0
  sound_a 0x24
  sound_roster 0x8
  playback_single 0x4b
  native_step 0x4e
  tail 0x0, MK4_NATIVE_MMOVE_LK_BLEND_RETURN
  .zero 4 ; non-executable guard

; Retail source 0x004e01c0; branches use linked names.
sonya_first_fatality:
  prefixed_to 0x22a, clip(388)
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_FIRST_RELEASE

; Retail source 0x004e01d0; branches use linked names.
sonya_fatality_1_owner_loop:
  forward 0x0
  cursor_set 0x54
  jump 0x0, word(sonya_fatality_1_owner_loop)
  .zero 4 ; non-executable guard

; Retail source 0x004e5930; branches use linked names.
sonya_second_fatality:
  native_release 0x4f
  wait 0x28
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_OPEN

; Retail source 0x004e5940; branches use linked names.
sonya_fatality_2_opening:
  prefixed_finish 0x200, clip(558)
  native_return 0xb8
  scene_active 0x0
  clip 0x0, clip(559)
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_REPEAT

; Retail source 0x004e5960; branches use linked names.
sonya_fatality_2_cycle:
  spawn 0x0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
  play_clip 0x18, clip(560)
  wait 0x6
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_BRANCH

; Retail source 0x004e5980; branches use linked names.
sonya_fatality_2_first_cycle:
  playback_tail 0x0
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_PAUSE
  .zero 4 ; non-executable guard

; Retail source 0x004e5990; branches use linked names.
sonya_fatality_2_second_cycle:
  playback_tail 0x0
  wait 0xf
  clip 0x0, clip(560)
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END

; Retail source 0x004e59a8; branches use linked names.
sonya_fatality_2_owner_ending:
  forward 0x0
  prefixed_finish 0x200, clip(114)

; Retail source 0x004e59b4; branches use linked names.
sonya_fatality_2_owner_ending_playback_pair_1:
  playback_pair 0x45
  forward_to 0x63
  jump 0x0, word(sonya_fatality_2_owner_ending_playback_pair_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e59c8; branches use linked names.
sonya_fatality_2_victim:
  clip 0x0, clip(561)
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END
  .zero 4 ; non-executable guard

; Retail source 0x004e5a20; branches use linked names.
sonya_fatality_2_victim_end:
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e81d0; branches use linked names.
sonya_cartwheel:
  sound_indexed 0x20
  sound_c 0x6
  speed 0x0, 0x1999
  prefixed_to 0x20e, clip(104)
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL
  jump 0, word(tanya_drill_kick)
  .zero 4 ; non-executable guard

; Retail source 0x004e8578; branches use linked names.
sonya_leg_grab_paired_grab:
  native_release 0x25
  sound_roster 0x1
  open_peer_clip 0x0, clip(201)
  playback_single 0x16
  playback_pair_reverse 0x13
  playback_single 0x16
  playback_pair_reverse 0x13
  playback_single 0x22
  native_step 0x4f
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL

; Retail source 0x004e85a8; branches use linked names.
sonya_leg_grab_miss_or_block:
  forward_to 0x16
  playback_pair 0x13
  forward_to 0x16
  playback_pair 0x13
  forward 0x0
  ready 0x0

; Retail source 0x004e85c0; branches use linked names.
sonya_square_wave:
  speed 0x0, 0x1999
  prefixed_finish 0x200, clip(72)
  sound_indexed 0xe
  bind_cursor 0x0, clip(73)
  motion_block 0x0, 0x1999, 0x0, 0xffffe667, 0xffffff7d
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e8b38; branches use linked names.
sonya_weapon_hp_victim:
  sound_c 0x8
  sound_b 0x1
  effect 0x5
  clip 0x0, clip(136)
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  jump 0, word(kai_draw_weapon_hp_victim)
  .zero 4 ; non-executable guard

; Retail source 0x004e8f50; branches use linked names.
sonya_weapon_lp_victim:
  effect 0x0
  sound_b 0x1
  speed_negative 0x0, 0x11eb
  play_clip 0xe, clip(132)
  native_context 0x0
  sound_a 0x4
  landing 0x0
  forward_count 0xe
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  jump 0, word(weapon_tumble_back_getup)
  .zero 4 ; non-executable guard

; Retail source 0x004e9110; branches use linked names.
sonya_crouch_hk_victim:
  speed_negative 0x0, 0x3333
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  clip 0x0, clip(134)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e9a68; branches use linked names.
sonya_leg_grab_victim_recovery:
  motion 0x0, 0x1c28, 0xffffe667, 0x24d
  sound_special 0x0
  task_callback 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END
  .zero 4 ; non-executable guard

; Retail source 0x004e9de8; branches use linked names.
sonya_ring_victim:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  sound_b 0x1
  speed_negative 0x0, 0x4000
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea0b8; branches use linked names.
sonya_cartwheel_victim:
  sound_context 0x1
  sound_b 0x3
  motion 0x0, 0xffffe667, 0xffffd47b, 0x28f
  task_callback 0x0
  open_clip 0x0, clip(131)
  double_to 0xd
  forward_to 0x29
  nop 0x13ece8
  native_context 0x0
  forward_count 0x4
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  jump 0, word(bicycle_capture)
  .zero 4 ; non-executable guard

; Retail source 0x004ea130; branches use linked names.
sonya_bicycle_victim:
  effect 0x8
  effect 0x9
  sound_context 0x2
  spawn_effect 0x0
  task_callback 0x0
  motion 0x0, 0xffffe667, 0x666, 0x147
  clip 0x0, clip(186)
  native_context 0x0
  sound_special 0x0
  landing 0x0
  tail 0x0, MK4_NATIVE_GETUP_FRONT
  jump 0, word(kai_handstand_reaction_knockdown)
  .zero 4 ; non-executable guard

; Retail source 0x004ec7a0; branches use linked names.
sonya_weapon_back_hp_attacker:
  open_clip 0x0, clip(228)
  forward_to 0x6
  sound_c 0x1e
  forward_to 0xe
  tail 0x0, MK4_NATIVE_LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004ed4c0; branches use linked names.
sonya_ring:
  sound_indexed 0x11
  open_clip 0x0, clip(15)
  fast_start 0x0, word(sonya_ring_double_to_1)
  double_to 0xa
  forward_to 0xf

; Retail source 0x004ed4dc; branches use linked names.
sonya_ring_spawn_1:
  spawn 0x0, 0x44, MK4_NATIVE_SONYA_RING_OBJECT
  spawn 0x0, 0x26, MK4_NATIVE_SONYA_SCRIPT_RING_FLASH
  sound_c 0x4
  forward_to 0x13
  action_followup 0x0
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0xf
  forward_to 0x13
  playback_pair 0x0
  ready 0x0

; Retail source 0x004ed520; branches use linked names.
sonya_ring_double_to_1:
  double_to 0xf
  jump 0x0, word(sonya_ring_spawn_1)
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_LK_THROW_STARTUP sonya_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FIRST_FATALITY sonya_first_fatality
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_1_OWNER_LOOP sonya_fatality_1_owner_loop
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_SECOND_FATALITY sonya_second_fatality
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_OPENING sonya_fatality_2_opening
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_CYCLE sonya_fatality_2_cycle
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_FIRST_CYCLE sonya_fatality_2_first_cycle
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_SECOND_CYCLE sonya_fatality_2_second_cycle
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_OWNER_ENDING sonya_fatality_2_owner_ending
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_VICTIM sonya_fatality_2_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_VICTIM_END sonya_fatality_2_victim_end
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_CARTWHEEL sonya_cartwheel
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_LEG_GRAB_PAIRED_GRAB sonya_leg_grab_paired_grab
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_LEG_GRAB_MISS_OR_BLOCK sonya_leg_grab_miss_or_block
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_SQUARE_WAVE sonya_square_wave
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_WEAPON_HP_VICTIM sonya_weapon_hp_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_WEAPON_LP_VICTIM sonya_weapon_lp_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_CROUCH_HK_VICTIM sonya_crouch_hk_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_LEG_GRAB_VICTIM_RECOVERY sonya_leg_grab_victim_recovery
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_RING_VICTIM sonya_ring_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_CARTWHEEL_VICTIM sonya_cartwheel_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_BICYCLE_VICTIM sonya_bicycle_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_WEAPON_BACK_HP_ATTACKER sonya_weapon_back_hp_attacker
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_RING sonya_ring

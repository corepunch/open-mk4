; Generated scorpion bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_SCORPION_SCRIPT_FIRE_DECOR 0x416ae0
.export_equ MK4_NATIVE_SCORPION_SCRIPT_FIRE_FLAME 0x416d40
.export_equ MK4_NATIVE_SCORPION_BEAST_FATALITY 0x41e960
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN 0x41ea50
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK 0x41ebd0
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES 0x41ec70
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BEAST_GORE 0x41ed80
.export_equ MK4_NATIVE_SCORPION_LK_THROW_ENTRY 0x42bdd0
.export_equ MK4_NATIVE_SCORPION_BURN_FATALITY 0x443840
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_BEGIN 0x443930
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START 0x443a30
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1 0x443a70
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2 0x443b40
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTORY 0x443c10
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE 0x444000
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT 0x444030
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND 0x444060
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT 0x444100
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE 0x444150
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END 0x444190
.export_equ MK4_NATIVE_SCORPION_SCRIPT_TRANSFORM_EFFECT 0x450d40
.export_equ MK4_NATIVE_SCORPION_CAN_SPEAR 0x45ea60
.export_equ MK4_NATIVE_SCORPION_CAN_TELEPORT 0x45f0c0
.export_equ MK4_NATIVE_SCORPION_FIRE 0x468610
.export_equ MK4_NATIVE_SCORPION_TELEPORT 0x47fc90
.export_equ MK4_NATIVE_SCORPION_TELEPORT_SCRIPT 0x47fcf0
.export_equ MK4_NATIVE_SCORPION_SPIN_SCRIPT 0x4807b0
.export_equ MK4_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL 0x484c00
.export_equ MK4_NATIVE_SCORPION_SPEAR_STUN_SCRIPT 0x487420
.export_equ MK4_NATIVE_SCORPION_SCRIPT_BEAST_VOICE 0x48a1f0
.export_equ MK4_NATIVE_SCORPION_SPEAR 0x49a630
.export_equ MK4_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN 0x49a680

; Retail source 0x004d2e58; branches use linked names.
beast_start:
  native_release 0x3c
  play_clip 0x18, clip(12)
  sound_c 0x4b
  sound_c 0x4a
  spawn 0x0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_TRANSFORM_EFFECT
  forward 0x0
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN
  .zero 4 ; non-executable guard

; Retail source 0x004d2e88; branches use linked names.
scorpion_beast_begin:
  scene_active 0x0
  clip 0x0, clip(578)
  native_release 0x3d
  clip 0x0, clip(579)
  sound_roster 0x1
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK
  .zero 4 ; non-executable guard

; Retail source 0x004d2eb0; branches use linked names.
scorpion_fatality_2_creature_attack:
  clip 0x0, clip(580)
  loop 0x5
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VOICE
  backward_count 0x8
  forward_count 0x8
  repeat 0x0
  spawn_effect 0x6
  sound_context 0x12
  sound_roster 0x8
  clip 0x0, clip(581)
  native_release 0x3e
  bind_cursor 0x0, clip(578)
  tail 0x0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP

; Retail source 0x004d2ef8; branches use linked names.
scorpion_fatality_2_victim:
  play_clip 0x36, clip(584)
  sound_b 0x1
  sound_context 0x7
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES

; Retail source 0x004d2f10; branches use linked names.
scorpion_beast_victim_pieces:
  forward 0x0
  loop 0x5
  backward_count 0x8
  forward_count 0x8
  repeat 0x0
  spawn 0x0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_GORE
  clip 0x0, clip(585)
  tail 0x0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
  .zero 4 ; non-executable guard

; Retail source 0x004de2f8; branches use linked names.
scorpion_lk_throw_startup:
  native_release 0x4
  open_peer_clip 0x0, clip(157)
  open_clip 0x0, clip(37)
  playback_single 0x12
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_PAIRED_CONTINUATION
  jump 0, word(cage_lk_throw_damage)
  .zero 4 ; non-executable guard

; Retail source 0x004e0278; branches use linked names.
burn_start:
  prefixed_to 0x22b, clip(390)
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_BEGIN

; Retail source 0x004e0288; branches use linked names.
scorpion_burn_begin:
  forward_to 0x69
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START
  .zero 4 ; non-executable guard

; Retail source 0x004e0298; branches use linked names.
scorpion_fatality_1_owner_flame:
  forward_to 0x7b
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1
  .zero 4 ; non-executable guard

; Retail source 0x004e02a8; branches use linked names.
scorpion_burn_repeat1:
  cursor_set 0x69
  forward_to 0x7b
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2

; Retail source 0x004e02b8; branches use linked names.
scorpion_burn_repeat2:
  cursor_set 0x69
  forward_to 0x7b
  forward 0x0
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTORY
  .zero 4 ; non-executable guard

; Retail source 0x004e02d0; branches use linked names.
scorpion_burn_victory:
  prefixed_finish 0x200, clip(115)
  prefixed_finish 0x200, clip(116)
  prefixed_finish 0x200, clip(117)
  bind_cursor 0x200, clip(118)
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e0300; branches use linked names.
scorpion_fatality_1_victim:
  prefixed_to 0x20c, clip(391)
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE

; Retail source 0x004e0310; branches use linked names.
scorpion_burn_victim_cue:
  forward_to 0xe
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT
  .zero 4 ; non-executable guard

; Retail source 0x004e0320; branches use linked names.
scorpion_burn_victim_repeat:
  forward 0x0
  cursor_set 0x3f
  forward 0x0
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND
  .zero 4 ; non-executable guard

; Retail source 0x004e0338; branches use linked names.
scorpion_burn_victim_blend:
  prefixed_to 0x213, clip(206)
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT

; Retail source 0x004e0348; branches use linked names.
scorpion_burn_victim_impact:
  forward_to 0x59
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE
  .zero 4 ; non-executable guard

; Retail source 0x004e0358; branches use linked names.
scorpion_burn_victim_settle:
  forward 0x0
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END
  .zero 4 ; non-executable guard

; Retail source 0x004e5b58; branches use linked names.
fire_script_startup:
  open_clip 0x0, clip(448)
  cursor_set 0x1
  forward_to 0x4
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_DECOR
  forward_to 0x8
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_FLAME
  forward_to 0xb
  tail 0x0, MK4_NATIVE_FIRE_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004e6844; branches use linked names.
scorpion_roundhouse_attacker:
  attack_selector 0x0, 0x21
  open_clip 0x0, clip(313)
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_START
  .zero 4 ; non-executable guard

; Retail source 0x004e87b0; branches use linked names.
scorpion_teleport:
  motion 0x0, 0xffffd99a, 0xffffe8f6, 0x1ca
  open_clip 0x0, clip(312)
  sound_c 0x23
  handler 0x0, MK4_NATIVE_TELEPORT_GROUP_CALLBACK
  tail 0x0, MK4_NATIVE_SCORPION_TELEPORT_SCRIPT
  .zero 4 ; non-executable guard

; Retail source 0x004e8850; branches use linked names.
sword_spin_script:
  prefixed_finish 0x200, clip(223)
  bind_cursor 0x0, clip(224)
  tail 0x0, MK4_NATIVE_SCORPION_SPIN_SCRIPT
  .zero 4 ; non-executable guard

; Retail source 0x004e9938; branches use linked names.
scorpion_teleport_punch_grounded_victim:
  effect 0x9
  effect 0x8
  spawn_effect 0x0
  sound_context 0x1
  sound_b 0x3
  speed_negative 0x0, 0x2666
  bind_submit 0x4, clip(311)
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e9970; branches use linked names.
scorpion_teleport_victim_tail:
  wait 0xd
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e9d00; branches use linked names.
scorpion_spear_victim_hit:
  effect 0x5
  sound_b 0x3
  sound_context 0xa
  play_clip 0x5, clip(199)
  tail 0x0, MK4_NATIVE_SHAKE_STUN
  .zero 4 ; non-executable guard

; Retail source 0x004e9f20; branches use linked names.
spear_arrival:
  bind_cursor 0x0, clip(13)
  tail 0x0, MK4_NATIVE_SCORPION_SPEAR_STUN_SCRIPT
  .zero 4 ; non-executable guard

; Retail source 0x004ec918; branches use linked names.
sword_back_hp_script:
  play_clip 0x8, clip(238)
  sound_c 0x22
  forward_count 0x8
  speed 0x0, 0x1999
  forward_to 0x19
  weapon_query 0x3
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  forward_count 0x1
  weapon_query 0x3
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  forward_count 0x1
  weapon_query 0x3
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  action_recovery 0x0
  landing 0x0
  forward_count 0x2
  sound_c 0x13
  wait 0x4
  jump 0, word(sword_back_hp_script_action_recovery_1)
  .zero 4 ; non-executable guard

; Retail source 0x004ed448; branches use linked names.
scorpion_spear:
  sound_c 0x5
  fast_start 0x0, word(scorpion_spear_open_clip_1)
  open_clip 0x0, clip(14)
  double_to 0x14
  spawn 0x0, 0x43, MK4_NATIVE_SPEAR_TASK
  double_to 0x1e
  action_followup 0x0
  wait 0x2d
  ready 0x0

; Retail source 0x004ed47c; branches use linked names.
scorpion_spear_open_clip_1:
  open_clip 0x9, clip(14)
  double_count 0x10
  spawn 0x0, 0x43, MK4_NATIVE_SPEAR_TASK
  double_to 0x1e
  action_followup 0x0
  wait 0x2d
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed4a8; branches use linked names.
scorpion_spear_return:
  forward_count 0x13
  native_return 0x47
  sound_indexed 0x5
  forward 0x0
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN
  jump 0, word(sonya_ring)
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_BEAST_START beast_start
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BEAST_BEGIN scorpion_beast_begin
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_FATALITY_2_CREATURE_ATTACK scorpion_fatality_2_creature_attack
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_FATALITY_2_VICTIM scorpion_fatality_2_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BEAST_VICTIM_PIECES scorpion_beast_victim_pieces
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_LK_THROW_STARTUP scorpion_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_BURN_START burn_start
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_BEGIN scorpion_burn_begin
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_FATALITY_1_OWNER_FLAME scorpion_fatality_1_owner_flame
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_REPEAT1 scorpion_burn_repeat1
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_REPEAT2 scorpion_burn_repeat2
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTORY scorpion_burn_victory
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_FATALITY_1_VICTIM scorpion_fatality_1_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTIM_CUE scorpion_burn_victim_cue
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTIM_REPEAT scorpion_burn_victim_repeat
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTIM_BLEND scorpion_burn_victim_blend
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTIM_IMPACT scorpion_burn_victim_impact
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_BURN_VICTIM_SETTLE scorpion_burn_victim_settle
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_TELEPORT scorpion_teleport
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_TELEPORT_PUNCH_GROUNDED_VICTIM scorpion_teleport_punch_grounded_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_TELEPORT_VICTIM_TAIL scorpion_teleport_victim_tail
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_SPEAR_VICTIM_HIT scorpion_spear_victim_hit
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SPEAR_ARRIVAL spear_arrival
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_SPEAR scorpion_spear
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SCORPION_SPEAR_RETURN scorpion_spear_return
.export MK4_LINK_PROJECTILES_FIRE_SCRIPT_STARTUP fire_script_startup
.export MK4_LINK_WEAPON_ATTACKS_SWORD_BACK_HP_SCRIPT sword_back_hp_script
.export MK4_LINK_WEAPON_ATTACKS_SWORD_SPIN_SCRIPT sword_spin_script

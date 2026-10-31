; Generated scorpion bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
beast_start:
  native_release 0x3c
  play_clip 0x18, clip(12)
  sound_c 0x4b
  sound_c 0x4a
  spawn 0x0, 0x0, native_scorpion_script_transform_effect
  forward 0x0
  tail 0x0, native_scorpion_script_beast_begin
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_beast_begin:
  scene_active 0x0
  clip 0x0, clip(578)
  native_release 0x3d
  clip 0x0, clip(579)
  sound_roster 0x1
  tail 0x0, native_scorpion_script_beast_attack
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_fatality_2_creature_attack:
  clip 0x0, clip(580)
  loop 0x5
  call 0x0, native_scorpion_script_beast_voice
  backward_count 0x8
  forward_count 0x8
  repeat 0x0
  spawn_effect 0x6
  sound_context 0x12
  sound_roster 0x8
  clip 0x0, clip(581)
  native_release 0x3e
  bind_cursor 0x0, clip(578)
  tail 0x0, native_task_park_60_tick_loop

; Branches use linked names.
scorpion_fatality_2_victim:
  play_clip 0x36, clip(584)
  sound_b 0x1
  sound_context 0x7
  tail 0x0, native_scorpion_script_beast_victim_pieces

; Branches use linked names.
scorpion_beast_victim_pieces:
  forward 0x0
  loop 0x5
  backward_count 0x8
  forward_count 0x8
  repeat 0x0
  spawn 0x0, 0x0, native_scorpion_script_beast_gore
  clip 0x0, clip(585)
  tail 0x0, native_task_park_60_tick_loop
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_lk_throw_startup:
  native_release 0x4
  open_peer_clip 0x0, clip(157)
  open_clip 0x0, clip(37)
  playback_single 0x12
  tail 0x0, native_cage_lk_throw_paired_continuation
  jump 0, word(cage_lk_throw_damage)
  .zero 4 ; non-executable guard

; Branches use linked names.
burn_start:
  prefixed_to 0x22b, clip(390)
  tail 0x0, native_scorpion_script_burn_begin

; Branches use linked names.
scorpion_burn_begin:
  forward_to 0x69
  tail 0x0, native_scorpion_script_burn_victim_start
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_fatality_1_owner_flame:
  forward_to 0x7b
  tail 0x0, native_scorpion_script_burn_repeat1
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_burn_repeat1:
  cursor_set 0x69
  forward_to 0x7b
  tail 0x0, native_scorpion_script_burn_repeat2

; Branches use linked names.
scorpion_burn_repeat2:
  cursor_set 0x69
  forward_to 0x7b
  forward 0x0
  tail 0x0, native_scorpion_script_burn_victory
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_burn_victory:
  prefixed_finish 0x200, clip(115)
  prefixed_finish 0x200, clip(116)
  prefixed_finish 0x200, clip(117)
  bind_cursor 0x200, clip(118)
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_fatality_1_victim:
  prefixed_to 0x20c, clip(391)
  tail 0x0, native_scorpion_script_burn_victim_cue

; Branches use linked names.
scorpion_burn_victim_cue:
  forward_to 0xe
  tail 0x0, native_scorpion_script_burn_victim_repeat
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_burn_victim_repeat:
  forward 0x0
  cursor_set 0x3f
  forward 0x0
  tail 0x0, native_scorpion_script_burn_victim_blend
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_burn_victim_blend:
  prefixed_to 0x213, clip(206)
  tail 0x0, native_scorpion_script_burn_victim_impact

; Branches use linked names.
scorpion_burn_victim_impact:
  forward_to 0x59
  tail 0x0, native_scorpion_script_burn_victim_settle
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_burn_victim_settle:
  forward 0x0
  tail 0x0, native_scorpion_script_burn_victim_end
  .zero 4 ; non-executable guard

; Branches use linked names.
fire_script_startup:
  open_clip 0x0, clip(448)
  cursor_set 0x1
  forward_to 0x4
  call 0x0, native_scorpion_script_fire_decor
  forward_to 0x8
  call 0x0, native_scorpion_script_fire_flame
  forward_to 0xb
  tail 0x0, native_fire_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_roundhouse_attacker:
  attack_selector 0x0, 0x21
  open_clip 0x0, clip(313)
  tail 0x0, native_roundhouse_start
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_teleport:
  motion 0x0, 0xffffd99a, 0xffffe8f6, 0x1ca
  open_clip 0x0, clip(312)
  sound_c 0x23
  handler 0x0, native_teleport_group_callback
  tail 0x0, native_scorpion_teleport_script
  .zero 4 ; non-executable guard

; Branches use linked names.
sword_spin_script:
  prefixed_finish 0x200, clip(223)
  bind_cursor 0x0, clip(224)
  tail 0x0, native_scorpion_spin_script
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_teleport_punch_grounded_victim:
  effect 0x9
  effect 0x8
  spawn_effect 0x0
  sound_context 0x1
  sound_b 0x3
  speed_negative 0x0, 0x2666
  bind_submit 0x4, clip(311)
  handler 0x0, native_velocity_friction
  tail 0x0, native_scorpion_script_teleport_victim_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_teleport_victim_tail:
  wait 0xd
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_spear_victim_hit:
  effect 0x5
  sound_b 0x3
  sound_context 0xa
  play_clip 0x5, clip(199)
  tail 0x0, native_shake_stun
  .zero 4 ; non-executable guard

; Branches use linked names.
spear_arrival:
  bind_cursor 0x0, clip(13)
  tail 0x0, native_scorpion_spear_stun_script
  .zero 4 ; non-executable guard

; Branches use linked names.
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

; Branches use linked names.
scorpion_spear:
  sound_c 0x5
  fast_start 0x0, word(scorpion_spear_open_clip_1)
  open_clip 0x0, clip(14)
  double_to 0x14
  spawn 0x0, 0x43, native_spear_task
  double_to 0x1e
  action_followup 0x0
  wait 0x2d
  ready 0x0

; Branches use linked names.
scorpion_spear_open_clip_1:
  open_clip 0x9, clip(14)
  double_count 0x10
  spawn 0x0, 0x43, native_spear_task
  double_to 0x1e
  action_followup 0x0
  wait 0x2d
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
scorpion_spear_return:
  forward_count 0x13
  native_return 0x47
  sound_indexed 0x5
  forward 0x0
  tail 0x0, native_scorpion_script_spear_return
  jump 0, word(sonya_ring)
  .zero 4 ; non-executable guard

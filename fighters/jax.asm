; Generated jax bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
jax_lp_throw_first_slam:
  open_peer_clip 0x0, clip(145)
  open_clip 0x0, clip(83)
  playback_single 0x37
  spawn_effect 0x0
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_first

; Branches use linked names.
slam_first_script:
  playback_single 0x52
  damage 0x0, 0xccc
  spawn_effect 0x0
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_second

; Branches use linked names.
slam_second_script:
  playback_single 0x74
  spawn_effect 0x0
  damage 0x0, 0xccc
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_third

; Branches use linked names.
slam_second_release_script:
  native_return 0x7d
  jump 0x0, word(slam_first_release_script_native_word_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
slam_first_release_script:
  native_return 0x64

; Branches use linked names.
slam_first_release_script_native_word_1:
  native_word 0x0, 0xf5c
  damage 0x0, 0xccc
  tail 0x0, native_jax_lp_throw_return
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fan_fatality_performer:
  prefixed_to 0x245, clip(415)
  tail 0x0, native_fan_tail
  jump 0, word(jarek_fan_fatality_male_victim_owner_cycle)
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fan_fatality_owner_cycle:
  forward_to 0x4d
  cursor_set 0x45
  tail 0x0, native_fan_female_victim_owner_cycle

; Branches use linked names.
jax_fan_fatality_owner_release:
  cursor_set 0x4e
  forward_to 0x6b
  tail 0x0, native_fan_victory
  jump 0, word(fan_victory)
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fan_fatality_victim:
  prefixed_to 0x245, clip(416)

; Branches use linked names.
jax_fan_fatality_victim_forward_to_1:
  forward_to 0x4d
  cursor_set 0x45
  jump 0x0, word(jax_fan_fatality_victim_forward_to_1)

; Branches use linked names.
jax_fan_fatality_victim_release:
  cursor_set 0x4e
  forward_to 0x6b
  tail 0x0, native_fan_victim_end
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fatality_1_owner_opening:
  prefixed_finish 0x200, clip(384)
  tail 0x0, native_jax_fatality_1_victory

; Branches use linked names.
jax_fatality_1_owner_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(jax_fatality_1_owner_loop)

; Branches use linked names.
arm_tear_victim_clip_204:
  prefixed_to 0x231, clip(204)
  jump 0x0, word(arm_tear_victim_clip_400_tail_1)

; Branches use linked names.
arm_tear_victim_clip_400:
  prefixed_to 0x231, clip(400)

; Branches use linked names.
arm_tear_victim_clip_400_tail_1:
  tail 0x0, native_jax_fatality_1_grab

; Branches use linked names.
jax_fatality_1_grab:
  forward_to 0x4e
  tail 0x0, native_jax_fatality_1_first_arm
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fatality_1_first_arm:
  forward_to 0x80
  tail 0x0, native_jax_fatality_1_second_arm
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fatality_1_second_arm:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fatality_2:
  native_release 0x43
  bind_cursor 0x0, clip(284)
  wait 0x14
  tail 0x0, native_jax_fatality_2_approach

; Branches use linked names.
jax_fatality_2_approach:
  spawn 0x0, 0x26b, native_jax_fatality_2_sparks
  native_release 0x43
  sound_c 0x46
  wait 0xf
  scene_active 0x0
  sound_c 0x46
  wait 0xf
  sound_c 0x2
  prefixed_finish 0x200, clip(555)
  spawn 0x0, 0x26b, native_jax_fatality_2_flash
  spawn_effect 0x6
  sound_c 0x69
  sound_c 0x2c
  native_return 0xb7
  wait 0xf
  clip 0x0, clip(556)
  native_release 0x50
  prefixed_finish 0x200, clip(114)

; Branches use linked names.
jax_fatality_2_approach_playback_pair_1:
  playback_pair 0x45
  forward_to 0x63
  jump 0x0, word(jax_fatality_2_approach_playback_pair_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_fatality_2_head:
  sound_b 0x8
  clip 0x0, clip(557)
  spawn 0x0, 0x26b, native_scene_keepalive_task
  clip 0x0, clip(206)
  tail 0x0, native_jax_fatality_2_finish
  jump 0, word(cage_fatality_2_victim_start)
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_high_kick_recovery:
  backward_count 0x2
  release_attack 0x0
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
air_throw_lift:
  sound_indexed 0x36
  open_peer_clip 0x0, clip(144)
  open_clip 0x0, clip(84)
  playback_single 0x14
  tail 0x0, native_air_lift_fall

; Branches use linked names.
air_lift_fall:
  playback_single 0x20
  sound_a 0x24
  sound_roster 0x1
  spawn_effect 0x6
  playback_single 0x2b
  disarm_peer 0x0
  tail 0x0, native_air_lift_release

; Branches use linked names.
air_lift_release:
  playback_single 0x30
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_draw_weapon_air_hp:
  attack_selector 0x0, 0x104
  tail 0x0, native_weapon_air_high_punch_open
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_draw_weapon_air_lp:
  attack_selector 0x0, 0x108
  tail 0x0, native_weapon_air_low_punch_open
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_roundhouse_fast_start:
  double_to 0xc
  sound_c 0x1c
  double_to 0x14
  tail 0x0, native_roundhouse_wide_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_roundhouse_normal_start:
  double_to 0xc
  sound_c 0x1c
  forward_to 0x14
  tail 0x0, native_roundhouse_wide_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_combo_hp_attacker:
  open_clip 0x2, clip(261)
  forward_to 0xb
  tail 0x0, native_combo_high_punch_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_dash_punch:
  sound_indexed 0x25
  sound_c 0x4e
  speed 0x0, 0x2666
  open_clip 0x0, clip(299)
  double_to 0x12
  tail 0x0, native_jax_dash_punch_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_dash_punch_airborne_victim_168:
  effect 0x9
  sound_b 0x1
  spawn_effect 0x0
  sound_context 0x0
  effect 0x4
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, native_jax_dash_punch_roll_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
air_victim_lift:
  motion 0x0, 0xffffe667, 0xffffd99a, 0x28f
  task_callback 0x0
  prefixed_to 0x223, clip(140)
  native_context 0x0
  tail 0x0, native_air_lift_getup
  jump 0, word(air_victim_default)
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_dash_punch_grounded_victim_168:
  effect 0x9
  sound_b 0x1
  speed_negative 0x0, 0x6666
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_dash_victim_hit:
  effect 0x9
  sound_context 0x1
  prefixed_finish 0x200, clip(311)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
jax_missile:
  prefixed_to 0x20a, clip(29)
  sound_c 0x5d
  spawn 0x0, 0x50, native_jax_missile_spawn
  forward_to 0x19
  half_finish 0x0
  ready 0x0
  .zero 4 ; non-executable guard
